/**
 * mainwindow.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2014 Francesco Guastella aka romeoxbm
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "depgraphv_pch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appconfig.h"
#include "helpers.h"
#include "project.h"

//Settings Dialog pages
#include "generalpage.h"
#include "scanmodepage.h"
#include "filterpage.h"
#include "graphpage.h"

namespace depgraphV
{
	MainWindow::MainWindow( QWidget* parent )
		: QMainWindow( parent ),
		_ui( new Ui::MainWindow ),
		_project( 0 ),
		_progressBar( new QProgressBar( this ) ),
		_netManager( new QNetworkAccessManager() ),
		_currentRecentDocument( 0 ),
		_imageFiltersUpdated( false )
	{
		_ui->setupUi( this );
		_ui->tabWidget->setMainWindow( this );
		_updateWindowTitle();

		//Title bar progress bar
		_progressBar->setMaximumHeight( 16 );
		_progressBar->setMaximumWidth( 200 );
		_progressBar->setVisible( false );
		_ui->statusBar->addPermanentWidget( _progressBar, 0 );
		_progressBar->setValue( 0 );
		_progressBar->setMinimum( 0 );
		_progressBar->setMaximum( 0 );

		//Dialogs
		_config			= new AppConfig( this );
		_aboutDlg		= new AboutDialog( this );
		_settingsDlg	= new SettingsDialog( this );
		_rootsDlg		= new HandleRootsDialog( this );
		_filesDlg		= new SelectFilesDialog( this );

		//Clear recent projects action
		_actionClearRecentList = new QAction( this );
		_actionClearRecentList->setText( tr( "Clear list" ) );
		connect( _actionClearRecentList, SIGNAL( triggered() ),
				 this, SLOT( clearRecentDocs() )
		);

		//Check for available image formats
		if( !_lookForRequiredImageFormats() )
		{
			qWarning() << qPrintable( tr( "Missing one or more required image formats; "
										  "you could meet unespected issues." ) );
		}

		//Create language action group and setting up actionSystem_language
		_langGroup = new QActionGroup( _ui->menu_Language );
		_ui->actionSystem_language->setData( QLocale::system().name().mid( 0, 2 ) );
		_langGroup->addAction( _ui->actionSystem_language );
		onTranslationFound( "en", "" );

		connect( _config, SIGNAL( translationFound( QString, QString ) ),
				 this, SLOT( onTranslationFound( QString, QString ) )
		);

		_config->lookForTranslations();

		//Settings dialog pages
		_settingsDlg->addPage(
					"General Settings",
					new GeneralPage( this, _settingsDlg )
		);
		_settingsDlg->addPage(
					"Scan Mode",
					new ScanModePage( this, _settingsDlg )
		);
		_settingsDlg->addPage(
					"Header Filters",
					new FilterPage( this, _settingsDlg )
		);
		_settingsDlg->addPage(
					"Source Filters",
					new FilterPage( this, _settingsDlg, false )
		);
		_settingsDlg->addPage(
					"Graph Settings",
					new GraphPage( this, _settingsDlg )
		);

		//Register serializable objects
		_config->registerSerializable( this );

		//Connect other signals and slots
		connect( _langGroup, SIGNAL( triggered( QAction* ) ),
				 this, SLOT( onLanguageActionTriggered( QAction* ) )
		);
		connect( _ui->actionAbout_Qt, SIGNAL( triggered() ),
				 qApp, SLOT( aboutQt() )
		);
		connect( _config, SIGNAL( configRestored() ),
				 this, SLOT( onConfigRestored() )
		);

		//Save default settings, if this is the first time we launch this application
		_config->saveDefault();

		//Restore last settings
		_config->restore();

		_ui->statusBar->showMessage( QString( "%1 %2" ).arg( APP_NAME, tr( "ready" ) ) );

		QObject::connect( _netManager, SIGNAL( finished( QNetworkReply* ) ),
						  this, SLOT( onUpdateReply( QNetworkReply* ) )
		);
	}
	//-------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete _ui;
		delete _settingsDlg;
		delete _aboutDlg;
		delete _rootsDlg;
		delete _filesDlg;
		delete _config;
		delete _netManager;
	}
	//------------------------------------------------------------------------
	void MainWindow::show( const QString& fileName )
	{
		if( !fileName.isEmpty() )
		{
			//Check if file exists
			QFile f( fileName );
			if( f.exists() )
			{
				_project = Project::open( this, fileName );
				if( _project )
					_onProjectOpened( tr( "Project \"%1\" successfully opened." ) );
			}
			else
				QMessageBox::critical( this, tr( "Open Project" ), tr( "File doesn't exist!" ) );
		}

		QMainWindow::show();
	}
	//------------------------------------------------------------------------
	void MainWindow::changeEvent( QEvent* event )
	{
		if( event )
		{
			if( event->type() == QEvent::LanguageChange )
				_ui->retranslateUi( this );

			else if( event->type() == QEvent::LocaleChange )
			{}
		}

		QMainWindow::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void MainWindow::closeEvent( QCloseEvent* event )
	{
		if( !_discardProjectChanges() )
		{
			event->ignore();
			return;
		}

		_showAboutDialog( true );
		_config->save();
		event->accept();
		qApp->quit();
	}
	//-------------------------------------------------------------------------
	void MainWindow::newProject()
	{
		//Close open project first( if open )
		closeProject();

		if( _project )
			return;

		_project = Project::create( this );
		if( _project )
			_onProjectOpened( tr( "Project \"%1\" successfully created." ) );
	}
	//-------------------------------------------------------------------------
	void MainWindow::openProject()
	{
		//Close open project first( if open )
		closeProject();

		if( _project )
			return;

		_project = Project::open( this );
		if( _project )
		{
			_onProjectOpened( tr( "Project \"%1\" successfully opened." ) );
			if( _project->load() )
				emit projectLoaded();
		}
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveProject()
	{
		_doSaveProject( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveAsProject()
	{
		_doSaveProject( true );
	}
	//-------------------------------------------------------------------------
	void MainWindow::closeProject()
	{
		if( !_project )
			return;

		//Check for unsaved chages
		if( !_discardProjectChanges() )
			return;

		QString pName = _project->name();

		//prevent onCurrentTabChanged slot from being called
		_ui->tabWidget->blockSignals( true );
		delete _project;
		_project = 0;

		_ui->actionNew_Graph->setEnabled( false );
		_ui->menuProject->setEnabled( false );
		_ui->actionClose->setEnabled( false );
		_ui->actionSave_As->setEnabled( false );
		_ui->statusBar->showMessage(
					tr( "Project \"%1\" has been closed." ).arg( pName )
		);

		if( _currentRecentDocument )
		{
			_currentRecentDocument->setEnabled( true );
			_currentRecentDocument = 0;
		}

		_updateWindowTitle();
		emit projectClosed();
		_ui->tabWidget->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveAsDot()
	{
		QString path = QFileDialog::getSaveFileName(
						   this,
						   tr( "Select path and name of the dot file" ),
						   QDir::currentPath(),
						   "DOT (*.dot)"
		);

		if( !Helpers::addExtension( path, ".dot" ) )
			return;

		if( _project->currentGraph()->saveDot( path ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
		else
			QMessageBox::critical( this, tr( "Save as dot" ), tr( "Unable to save file" ) );
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveAsImage()
	{
		if( !_imageFiltersUpdated )
		{
			QStringList* list = Graph::pluginsListByKind( "loadimage" );
			if( !list )
			{
				QMessageBox::critical(
							this,
							tr( "Cannot save image" ),
							tr( "Unable to save graph as image; No plugin found." )
				);
				return;
			}

			for( int i = 0; i < list->count(); ++i )
			{
				QString f = list->at( i );
				QString currentFilter = QString( "%1 (*.%2)" ).arg( f.toUpper(), f );
				_imageFiltersByExt[ currentFilter ] = f;
				_imageFilters += currentFilter;
				if( i < list->count() - 1 )
					_imageFilters += ";;";
			}

			_imageFiltersUpdated = true;
		}

		QString selectedFilter;
		QString path = QFileDialog::getSaveFileName(
			this,
			tr( "Select path and name of the image file" ),
			QDir::currentPath(), _imageFilters, &selectedFilter );

		QString format = _imageFiltersByExt[ selectedFilter ];
		if( !Helpers::addExtension( path, format ) )
			return;

		if( _project->currentGraph()->saveImage( path, format ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
	}
	//-------------------------------------------------------------------------
	void MainWindow::clearRecentDocs()
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr( "Clear recent projects list" ),
			tr( "Are you sure?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer == QMessageBox::No )
			return;

		_config->setRecentDocuments( QStringList() );
		_ui->menuOpen_Recent->clear();
		_actionClearRecentList->setEnabled( false );
		_actionClearRecentList->setText( tr( "Empty list" ) );
		_ui->menuOpen_Recent->addAction( _actionClearRecentList ) ;
	}
	//-------------------------------------------------------------------------
	void MainWindow::onDraw()
	{
		//TODO Warn when no file/folder has been selected
		_ui->toolBar->setEnabled( false );
		_ui->menuBar->setEnabled( false );

		if( _project->currentValue( "scanByFolders" ).toBool() )
			_scanFolders();
		else
			_scanFiles( _project->currentGraph()->model()->checkedFiles() );

		if( !_applyGraphLayout() )
			_project->currentGraph()->clearGraph();

		_ui->toolBar->setEnabled( true );
		_ui->menuBar->setEnabled( true );
		_progressBar->setVisible( false );
		_ui->statusBar->showMessage( tr( "All done" ) );
		//Force toolbar buttons update
		onCurrentTabChanged();
	}
	//-------------------------------------------------------------------------
	void MainWindow::onClear()
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr( "Clear graph" ),
			tr( "Are you sure?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer == QMessageBox::No )
			return;

		_project->currentGraph()->clearGraph();
		//Force toolbar buttons update
		onCurrentTabChanged();
		//_doClearGraph();
	}
	//-------------------------------------------------------------------------
	void MainWindow::onConfigRestored()
	{
		_langActions[ _config->language() ]->setChecked( true );

		//Setup Open Recent menu
		if( !_config->recentDocuments().isEmpty() )
		{
			foreach( QString doc, _config->recentDocuments() )
			{
				int c = _ui->menuOpen_Recent->actions().count();
				QAction* aDoc = _newRecentDocument( doc );
				QString shortcut = QString( "CTRL+%1" ).arg( c );
				aDoc->setShortcut( QKeySequence( shortcut ) );
				_ui->menuOpen_Recent->addAction( aDoc );
			}

			_ui->menuOpen_Recent->addSeparator();
		}
		else
		{
			_actionClearRecentList->setEnabled( false );
			_actionClearRecentList->setText( tr( "Empty list" ) );
		}
		_ui->menuOpen_Recent->addAction( _actionClearRecentList ) ;
	}
	//-------------------------------------------------------------------------
	void MainWindow::onCurrentTabChanged( int idx )
	{
		if( idx == -1 )
			return;

		Graph* g = _project->currentGraph();

		_ui->actionDraw->setEnabled( !g->drawn() );
		_ui->actionClear->setEnabled( g->drawn() );
		_ui->actionSave_as_dot->setEnabled( g->drawn() );
		_ui->actionSave_as_Image->setEnabled( g->drawn() );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onGraphCountChanged( int count )
	{
		Graph* g = _project->currentGraph();
		_ui->actionDraw->setEnabled( g && !g->drawn() );

		_ui->actionSelect_FilesFolders->setEnabled( count > 0 );
		_ui->actionSettings->setEnabled( count > 0 );
	}
	//-------------------------------------------------------------------------
	void MainWindow::_doSaveProject( bool saveAs )
	{
		Q_ASSERT( _project );
		bool saveRes = saveAs ? _project->saveAs() : _project->save();
		if( saveRes )
		{
			_ui->statusBar->showMessage(
						tr( "Project \"%1\" saved." ).arg( _project->name() )
			);
		}

		_updateRecentDocumentsList();
		_updateWindowTitle( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::_scanFolder( const QFlags<QDir::Filter>& flags,
								  QStringList* filesList,
								  QFileInfo& dirInfo )
	{
		bool isGuiThread = QThread::currentThread() ==
						   QCoreApplication::instance()->thread();

		QStack<QFileInfo> stack;
		stack.push( dirInfo );
		QStringList nameFilters = _project->nameFilters();

		while( !stack.isEmpty() )
		{
			if( isGuiThread )
				QCoreApplication::processEvents();

			QFileInfo folderInfo = stack.pop();
			QString folder = folderInfo.filePath();
			QDir d( folder );
			//TODO emit folderFound signal?
			foreach( QFileInfo childFolderInfo, d.entryInfoList( flags ) )
				stack.push( childFolderInfo );

			//Look for files in current folder
			d.setNameFilters( nameFilters );
			QFileInfoList fList = d.entryInfoList( QDir::NoDotAndDotDot | QDir::Files );
			foreach( QFileInfo fileEntry, fList )
				filesList->append( fileEntry.filePath() );
		}
	}
	//-------------------------------------------------------------------------
	void MainWindow::_scanFolders() const
	{
		//TODO MISSING selectedFolders
		/*QStringList filesList;
		_startSlowOperation(
					tr( "Scanning folders..." ),
					_config->selectedFolders().count()
		);
		QFlags<QDir::Filter> flags = QDir::NoDotAndDotDot | QDir::Dirs;

		if( _project->currentValue( "includeHiddenFolders" ).toBool() )
			flags |= QDir::Hidden;

		QStringList::const_iterator it = _config->selectedFolders().begin();
		for( ; it != _config->selectedFolders().end(); it++ )
		{
			QFileInfoList infos = QDir( *it ).entryInfoList( flags );
			auto memberFuncPtr = std::bind(
									 &MainWindow::_scanFolder,
									 const_cast<MainWindow*>( this ),
									 flags,
									 &filesList,
									 std::placeholders::_1
			);

			//TODO Cancel...
			QtConcurrent::blockingMap( infos, memberFuncPtr );
			_progressBar->setValue( _progressBar->value() + 1 );
		}

		_scanFiles( filesList );*/
	}
	//-------------------------------------------------------------------------
	void MainWindow::_scanFiles( const QStringList& files ) const
	{
		_startSlowOperation( tr( "Analyzing files..." ), files.count() );

		//TODO blockingMap here instead of the following "simple" foreach loop?
		foreach( QString path, files )
		{
			QFileInfo f( path );
			_project->currentGraph()->createEdges( f.absolutePath(), f.fileName() );
			_progressBar->setValue( _progressBar->value() + 1 );
		}
	}
	//-------------------------------------------------------------------------
	bool MainWindow::_applyGraphLayout() const
	{
		//TODO
		_startSlowOperation( tr( "Applying layout..." ), 0 );
		return _project->currentGraph()->applyLayout();
			//qDebug() << "Unable to render; Plugin not found.";
	}
	//-------------------------------------------------------------------------
	void MainWindow::_doClearGraph() const
	{
		//_ui->graph->clear();
		//_setButtonsAndActionsEnabled( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::about()
	{
		_showAboutDialog( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::settings()
	{
		_settingsDlg->move( geometry().center() - _settingsDlg->rect().center() );
		_settingsDlg->exec();
	}
	//-------------------------------------------------------------------------
	void MainWindow::restoreDefaultSettings()
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			0,
			tr( "Restore default settings" ),
			tr( "Are you sure?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer == QMessageBox::No )
			return;

		_config->restoreDefault();
	}
	//-------------------------------------------------------------------------
	void MainWindow::checkForUpdates() const
	{
		QNetworkRequest request(
					QUrl( "http://depgraphv.sourceforge.net/update.php" )
		);

		request.setHeader( QNetworkRequest::ContentTypeHeader,
						   "application/x-www-form-urlencoded"
		);

		request.setRawHeader( "User-Agent", APP_NAME );

		QNetworkReply* reply = _netManager->post( request, _postData() );

		connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ),
					this, SLOT( onUpdateReplyProgress( qint64, qint64 ) ) );

		_startSlowOperation( tr( "Downloading response..." ), 0 );
		_ui->action_Check_for_updates->setEnabled( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onUpdateReplyProgress( qint64 bytesReceived, qint64 bytesTotal )
	{
		_progressBar->setMaximum( bytesTotal );
		_progressBar->setValue( bytesReceived );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onUpdateReply( QNetworkReply* reply )
	{
		_ui->statusBar->showMessage( tr( "Response received" ) );
		_progressBar->setMaximum( 0 );
		_progressBar->setVisible( false );

		bool error = false;
		QString message;
		if( reply->error() )
		{
			error = true;
			message = reply->errorString();
		}
		else
		{
			QStringList res = QString( reply->readAll() ).
							  split( '\n', QString::SkipEmptyParts );

			if( res[ 0 ] == "OK" )
			{
				if( res[ 1 ] == "NO" )
					message = tr( "There's no update available.\n"
								  "You're using the latest version of " ) + APP_NAME;
				else
				{
					message = tr( "A new version of " ) + APP_NAME +
							  " (v" + res[ 1 ] + tr ( ") is available for download." );
				}
			}
			else
			{
				error = true;
				message = tr( "The server returned an error message:" ) + " \n" + res[ 1 ];
			}
		}

		if( error )
			QMessageBox::critical( this, tr( "Check for updates" ), message );
		else
			QMessageBox::information( this, tr( "Check for updates" ), message );

		_ui->action_Check_for_updates->setEnabled( true );
	}
	//-------------------------------------------------------------------------
	/*void MainWindow::_setButtonsAndActionsEnabled( bool value ) const
	{
		_ui->actionDraw->setEnabled( !value );
		_ui->actionClear->setEnabled( value );
		_ui->actionSave_as_dot->setEnabled( value );
		_ui->actionSave_as_Image->setEnabled( value );
	}*/
	//-------------------------------------------------------------------------
	void MainWindow::_onProjectOpened( const QString& statusBarMessage )
	{
		emit projectOpened();
		_ui->actionNew_Graph->setEnabled( true );
		_ui->actionClose->setEnabled( true );
		_ui->actionSave_As->setEnabled( true );
		_ui->menuProject->setEnabled( true );
		_updateWindowTitle( false );

		connect( _ui->actionNew_Graph, SIGNAL( triggered() ),
				 _project, SLOT( createGraph() )
		);
		connect( _project, SIGNAL( modified( bool ) ),
				 _ui->actionSave, SLOT( setEnabled( bool ) )
		);
		connect( _project, SIGNAL( graphCountChanged( int ) ),
				 this, SLOT( onGraphCountChanged( int ) )
		);

		_ui->statusBar->showMessage( statusBarMessage.arg( _project->name() ) );
		_updateRecentDocumentsList();
	}
	//-------------------------------------------------------------------------
	QList<const char*> MainWindow::propList() const
	{
		QList<const char*> props;
		props << "windowState"
			  << "geometryState";

		return props;
	}
	//-------------------------------------------------------------------------
	QByteArray MainWindow::windowState() const
	{
		return this->saveState();
	}
	//-------------------------------------------------------------------------
	QByteArray MainWindow::geometryState() const
	{
		return this->saveGeometry();
	}
	//-------------------------------------------------------------------------
	void MainWindow::setWindowState( const QByteArray& value )
	{
		if( !this->restoreState( value ) )
		{
			qCritical() << qPrintable(
							   tr( "Unable to restore window state!" )
			);
		}
	}
	//-------------------------------------------------------------------------
	void MainWindow::setGeometryState( const QByteArray& value )
	{
		if( !this->restoreGeometry( value ) )
		{
			qCritical() << qPrintable(
							   tr( "Unable to restore geometry state!" )
			);
		}
	}
	//-------------------------------------------------------------------------
	bool MainWindow::_lookForRequiredImageFormats()
	{
		QList<QByteArray> formats;
		formats << "ico" << "png";

		foreach( QByteArray f, formats )
		{
			if( !QImageReader::supportedImageFormats().contains( f ) )
				return false;
		}

		return true;
	}
	//-------------------------------------------------------------------------
	QAction* MainWindow::_newRecentDocument( const QString& filePath )
	{
		QFileInfo f( filePath );
		QAction* aDoc = new QAction( this );
		aDoc->setData( filePath );
		aDoc->setText( f.fileName() );
		if( _project && filePath == _project->fullPath() )
			aDoc->setEnabled( false );

		connect( aDoc, SIGNAL( triggered() ),
				 this, SLOT( onRecentDocumentTriggered() )
		);

		return aDoc;
	}
	//-------------------------------------------------------------------------
	void MainWindow::_updateRecentDocumentsList()
	{
		if( _project->fullPath().isEmpty() )
			return;

		if( !_config->recentDocuments().contains( _project->fullPath() ) )
		{
			QList<QAction*> actionList = _ui->menuOpen_Recent->actions();
			QAction* before = actionList.first();
			if( actionList.count() == 1 )
			{
				before = _ui->menuOpen_Recent->insertSeparator( before );
				_actionClearRecentList->setEnabled( true );
				_actionClearRecentList->setText( tr( "Clear list" ) );
			}

			_currentRecentDocument = _newRecentDocument( _project->fullPath() );
			_ui->menuOpen_Recent->insertAction(
						before,
						_currentRecentDocument
			);

			QStringList d = _config->recentDocuments();
			d.insert( 0, _project->fullPath() );
			if( d.count() > 10 )
			{
				d.removeLast();
				QAction* removedAction = actionList[ actionList.count() - 3 ];
				_ui->menuOpen_Recent->removeAction( removedAction );
			}
			_config->setRecentDocuments( d );
			actionList = _ui->menuOpen_Recent->actions();

			//Update shortcuts
			for( int i = 0; i < 10 && i < actionList.count() - 2; i++ )
			{
				QString shortcut = QString( "CTRL+%1" ).arg( i );
				actionList[ i ]->setShortcut( QKeySequence( shortcut ) );
			}
		}
		else if( !_currentRecentDocument )
		{
			foreach( QAction* a, _ui->menuOpen_Recent->actions() )
			{
				if( a->data().toString() == _project->fullPath() )
				{
					_currentRecentDocument = a;
					_currentRecentDocument->setEnabled( false );
				}
			}
		}
	}
	//-------------------------------------------------------------------------
	void MainWindow::_updateWindowTitle( bool closingProject )
	{
		if( closingProject )
			setWindowTitle( APP_NAME );
		else
		{
			Q_ASSERT( _project );
			setWindowTitle( "[" + _project->name() + "] - " + APP_NAME );
		}
	}
	//-------------------------------------------------------------------------
	bool MainWindow::_discardProjectChanges()
	{
		if( _project && _project->isModified() )
		{
			QMessageBox::StandardButton answer = QMessageBox::question(
				this,
				tr( "Modified project" ),
				tr( "Would you like to discard unsaved changes?" ),
				QMessageBox::Yes | QMessageBox::No,
				QMessageBox::No
			);

			bool result = answer == QMessageBox::Yes;
			_ui->actionSave->setEnabled( !result );
			return result;
		}

		return true;
	}
	//-------------------------------------------------------------------------
	void MainWindow::_startSlowOperation( const QString& message, int maxValue ) const
	{
		_ui->statusBar->showMessage( message );
		_progressBar->setValue( 0 );
		_progressBar->setMaximum( maxValue );
		_progressBar->setVisible( true );
	}
	//-------------------------------------------------------------------------
	void MainWindow::_showAboutDialog( bool showDonations )
	{
		if( !_config->showDonateOnExit() && showDonations )
			return;

		_aboutDlg->move( geometry().center() - _aboutDlg->rect().center() );
		_aboutDlg->exec( showDonations );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onSelectFilesOrFolders()
	{
		_project->setCurrentMapper( "tabMapper" );
		if( _project->currentValue( "scanByFolders" ).toBool() )
			_rootsDlg->exec();
		else
			_filesDlg->exec( _project->currentGraph()->model() );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onLanguageActionTriggered( QAction* action )
	{
		_config->setLanguage( action->data().toString() );
		action->setChecked( true );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onTranslationFound( QString lang, QString path )
	{
		//Double tab check
		if( _langActions.contains( lang ) )
			return;

		//Create a new QAction for lang
		QAction* newLang = new QAction( this );
		newLang->setObjectName( path );
		newLang->setText( Helpers::ucFirst( QLocale( lang ).nativeLanguageName() ) );
		newLang->setData( lang );
		newLang->setCheckable( true );
		newLang->setIcon( QIcon( ":/flags/" + lang ) );
		_ui->menu_Language->addAction( newLang );
		_langGroup->addAction( newLang );

		if( lang == "en" )
			newLang->setChecked( true );

		//Also update system language QAction..
		if( lang == QLocale::system().name().mid( 0, 2 ) )
		{
			_ui->actionSystem_language->setObjectName( path );
			_ui->actionSystem_language->setIcon( QIcon( ":/flags/" + lang ) );
			_ui->actionSystem_language->setEnabled( true );
			qDebug() << qPrintable( tr( "\"%1\" is the system language." ).arg( lang ) );
		}

		_langActions.insert( lang, newLang );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onRecentDocumentTriggered()
	{
		//Close open project first( if open )
		closeProject();

		if( _project )
			return;

		QAction* s = qobject_cast<QAction*>( sender () );
		_project = Project::open( this, s->data().toString() );

		if( _project )
		{
			_currentRecentDocument = s;
			_currentRecentDocument->setEnabled( false );
			_onProjectOpened( tr( "Project \"%1\" successfully opened." ) );
			if( _project->load() )
				emit projectLoaded();
		}
	}
	//-------------------------------------------------------------------------
	QByteArray MainWindow::_postData()
	{
		static QByteArray postData;

		if( postData.isEmpty() )
		{
			QString d = "ver=" APP_VER "&os="
#ifdef WIN32
			"Windows"
#else
			"Linux"
#endif
			"&arch=" ARCH "&qt="
#ifdef DEPGRAPHV_USE_QT5
			"5&gl="
#else
			"4&gl="
#endif
#ifdef DEPGRAPHV_USE_OPENGL
			"yes";
#else
			"no";
#endif
			postData = d.toUtf8();
		}

		return postData;
	}
} // end of depgraphV namespace
