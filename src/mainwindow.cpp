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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appconfig.h"
#include "buildsettings.h"

//Settings Dialog pages
#include "scanmodepage.h"
#include "filterpage.h"
#include "graphpage.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QImageReader>

#ifndef QT_NO_CONCURRENT
#	ifdef QT_USE_QT5
#		include <QtConcurrent>
#	else
#		include <QtCore>
#	endif
#else
//TODO Warn on missing QtConcurrent support
#endif // QT_NO_CONCURRENT

#include <functional>

namespace depgraphV
{
	MainWindow::MainWindow( QWidget* parent )
		: QMainWindow( parent ),
		_ui( new Ui::MainWindow ),
		_progressBar( new QProgressBar( this ) ),
		_netManager( new QNetworkAccessManager() ),
		_imageFiltersUpdated( false )
	{
		_ui->setupUi( this );
		this->setWindowTitle( APP_NAME );

		//Title bar progress bar
		_progressBar->setMaximumHeight( 16 );
		_progressBar->setMaximumWidth( 200 );
		_progressBar->setVisible( false );
		_ui->statusBar->addPermanentWidget( _progressBar, 0 );
		_progressBar->setValue( 0 );
		_progressBar->setMinimum( 0 );
		_progressBar->setMaximum( 0 );

		//Dialogs
		_config			= new AppConfig( _ui->graph, this );
		_aboutDlg		= new AboutDialog( this );
		_settingsDlg	= new SettingsDialog( this );
		_rootsDlg		= new HandleRootsDialog( this );
		_filesDlg		= new SelectFilesDialog( this );

		//Renderer action group
		QActionGroup* rendererGroup = new QActionGroup( _ui->menuRenderer );
		rendererGroup->addAction( _ui->actionNative );
		rendererGroup->addAction( _ui->actionOpenGL );

		_ui->actionNative->setData( Graph::Native );
		_ui->actionOpenGL->setData( Graph::OpenGL );

#ifndef QT_USE_OPENGL
		_ui->actionOpenGL->setEnabled( false );
		_ui->actionHigh_Quality_Antialiasing->setEnabled( false );
#endif

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
				 this, SLOT( onTranslationFound( QString, QString ) ) );

		_config->lookForTranslations();

		//Settings dialog pages
		_settingsDlg->addPage( tr( "Scan Mode" ), new ScanModePage( _settingsDlg ) );
		_settingsDlg->addPage( tr( "Header Filters" ), new FilterPage( _settingsDlg ) );
		_settingsDlg->addPage( tr( "Source Filters" ), new FilterPage( _settingsDlg, false ) );
		_settingsDlg->addPage( tr( "Graph Settings" ), new GraphPage( _settingsDlg ) );

		//Register serializable objects
		_config->registerSerializable( this );
		_config->registerSerializable( _ui->graph );

		//Connect other signals and slots
		connect( _langGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( onLanguageActionTriggered( QAction* ) ) );
		connect( _ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
		connect( rendererGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( rendererTypeChanged( QAction* ) ) );
		connect( _config, SIGNAL( configRestored() ), this, SLOT( onConfigRestored() ) );

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
		_showAboutDialog( true );
		_config->save();
		event->accept();
	}
	//-------------------------------------------------------------------------
	void MainWindow::onDraw()
	{
		//TODO Warn when no file/folder has been selected
		_setButtonsAndActionsEnabled( false );
		_progressBar->setVisible( true );

		if( _config->scanByFolders() )
			_scanFolders();
		else
			_scanFiles( _filesDlg->selectedFiles() );

		if( !_applyGraphLayout() )
			_ui->graph->clear();

		_setButtonsAndActionsEnabled( true );
		_progressBar->setVisible( false );
		_ui->statusBar->showMessage( tr( "All done" ) );
		_ui->actionDraw->setEnabled( false );
		_ui->actionClear->setEnabled( true );
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

		if( answer != QMessageBox::No )
		{
			_ui->graph->clear();
			_ui->actionDraw->setEnabled( true );
			_ui->actionClear->setEnabled( false );
			//_doClearGraph();
		}
	}
	//-------------------------------------------------------------------------
	void MainWindow::onConfigRestored()
	{
		_ui->actionHigh_Quality_Antialiasing->setChecked(
					_ui->graph->highQualityAA()
		);

		QAction* a = _ui->graph->renderer() == Graph::Native ?
						 _ui->actionNative : _ui->actionOpenGL;

		a->setChecked( true );
		_langActions[ _config->language() ]->setChecked( true );
	}
	//-------------------------------------------------------------------------
	void MainWindow::changeToolbarOrientation()
	{
		Qt::Orientation o = _ui->toolBar->orientation();
		o = static_cast<Qt::Orientation>( ( o % 2 ) + 1 );
		_ui->toolBar->setOrientation( o );

		//Adjust toolbar size
		QSize hint = _ui->toolBar->sizeHint();
		bool h = o == Qt::Horizontal;

		_ui->toolBar->resize(
					h ? hint.width() : _ui->toolBar->minimumWidth(),
					h ? _ui->toolBar->minimumHeight() : hint.height()
		);
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
		QStringList nameFilters = _config->headerNameFilters() +
								  _config->sourceNameFilters();

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
		QStringList filesList;
		_progressBar->setValue( 0 );
		_progressBar->setMaximum( _config->selectedFolders().count() );
		_ui->statusBar->showMessage( tr( "Scanning folders..." ) );
		QFlags<QDir::Filter> flags = QDir::NoDotAndDotDot | QDir::Dirs;

		if( _config->hiddenFoldersIncluded() )
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

		_scanFiles( filesList );
	}
	//-------------------------------------------------------------------------
	void MainWindow::_scanFiles( const QStringList& files ) const
	{
		_progressBar->setValue( 0 );
		_progressBar->setMaximum( files.count() );
		_ui->statusBar->showMessage( tr( "Analyzing files..." ) );
		//TODO blockingMap here instead of the following "simple" foreach loop?
		foreach( QString path, files )
		{
			QFileInfo f( path );
			_ui->graph->createOrRetrieveVertex( f.fileName() );
			_ui->graph->createEdges( f.absolutePath(), f.fileName() );
			_progressBar->setValue( _progressBar->value() + 1 );
		}
	}
	//-------------------------------------------------------------------------
	bool MainWindow::_applyGraphLayout() const
	{
		_progressBar->setValue( 0 );
		_progressBar->setMaximum( 0 );
		_ui->statusBar->showMessage( tr( "Applying layout..." ) );
		return _ui->graph->applyLayout();
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
	void MainWindow::rendererTypeChanged( QAction* action )
	{
		Q_ASSERT( action );
		_ui->graph->setRenderer( (Graph::RendererType)action->data().toInt() );
		_ui->statusBar->showMessage( tr( "Renderer method changed" ) );
	}
	//-------------------------------------------------------------------------
	void MainWindow::parseOptionsChanged()
	{
		/*_ui->actionDraw->setEnabled( _ui->folderWidget->validDirSelected() &&
			( _ui->headerFilter->parseEnabled() || _ui->parseSourcesCheckbox->isChecked() )
		);*/
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveAsImage()
	{
		if( !_imageFiltersUpdated )
		{
			QStringList* list = _ui->graph->pluginsListByKind( "loadimage" );
			if( !list )
			{
				QMessageBox::critical(
							const_cast<MainWindow*>( this ),
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
			const_cast<MainWindow*>( this ),
			tr( "Select path and name of the image file" ),
			QDir::currentPath(), _imageFilters, &selectedFilter );

		if( path.isEmpty() )
			return;

		QFileInfo info( path );
		QString format = info.suffix();

		if( format.isEmpty() )
		{
			format = _imageFiltersByExt[ selectedFilter ];
			path += "." + format;
		}

		if( _ui->graph->saveImage( path, format ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
	}
	//-------------------------------------------------------------------------
	void MainWindow::saveAsDot() const
	{
		QString path = QFileDialog::getSaveFileName(
						   const_cast<MainWindow*>( this ),
						   tr( "Select path and name of the dot file" ),
						   QDir::currentPath(),
						   "DOT (*.dot)"
		);

		if( path.isEmpty() )
			return;

		QFileInfo info( path );
		QString format = info.suffix();

		if( format.isEmpty() )
			path += ".dot";

		if( _ui->graph->saveDot( path ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
		else
			QMessageBox::critical( 0, tr( "Save as dot" ), tr( "Unable to save file" ) );
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

		request.setHeader( QNetworkRequest::UserAgentHeader, APP_NAME );

		QNetworkReply* reply = _netManager->post( request, _postData() );

		connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ),
					this, SLOT( onUpdateReplyProgress( qint64, qint64 ) ) );

		_ui->statusBar->showMessage( tr( "Downloading response..." ) );
		_progressBar->setVisible( true );

		_ui->action_Check_for_updates->setEnabled( false );
	}
	//-------------------------------------------------------------------------
	void MainWindow::onUpdateReplyProgress( qint64 bytesReceived, qint64 bytesTotal )
	{
		if( _progressBar->maximum() == 0 )
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
			QMessageBox::critical( 0, tr( "Check for updates" ), message );
		else
			QMessageBox::information( 0, tr( "Check for updates" ), message );

		_ui->action_Check_for_updates->setEnabled( true );
	}
	//-------------------------------------------------------------------------
	void MainWindow::exitApp()
	{
		this->close();
		qApp->quit();
	}
	//-------------------------------------------------------------------------
	void MainWindow::_setButtonsAndActionsEnabled( bool value ) const
	{
		_ui->toolBar->setEnabled( value );
		_ui->menuBar->setEnabled( value );
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
		if( _config->scanByFolders() )
			_rootsDlg->exec();
		else
			_filesDlg->exec();
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
		newLang->setText( _ucFirst( QLocale( lang ).nativeLanguageName() ) );
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
	QString MainWindow::_ucFirst( const QString& value ) const
	{
		QStringList split = value.split( " " );
		for( int i = 0; i < split.count(); ++i )
		{
			QString& s = split[ i ];
			s[ 0 ] = s[ 0 ].toUpper();
		}

		return split.join( " " );
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
#ifdef QT_USE_QT5
			"5&gl="
#else
			"4&gl="
#endif
#ifdef QT_USE_OPENGL
			"yes";
#else
			"no";
#endif
			postData = d.toUtf8();
		}

		return postData;
	}
} // end of depgraphV namespace
