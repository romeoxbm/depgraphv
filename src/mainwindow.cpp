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

namespace depgraphV
{
	MainWindow::MainWindow( QWidget* parent )
		: QMainWindow( parent ),
		_ui( new Ui::MainWindow ),
		_imageFiltersUpdated( false )
	{
		_ui->setupUi( this );
		this->setWindowTitle( APP_NAME );

		//Dialogs
		_config			= new AppConfig( _ui->graph, this );
		_aboutDlg		= new AboutDialog( this );
		_settingsDlg	= new SettingsDialog( this );
		_rootsDlg		= new HandleRootsDialog( this );
		_filesDlg		= new SelectFilesDialog( this );
		_scanDlg		= new ScanDialog( this );

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
	}
	//-------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete _ui;
		delete _settingsDlg;
		delete _aboutDlg;
		delete _scanDlg;
		delete _rootsDlg;
		delete _filesDlg;
		delete _config;
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
		if( _config->scanByFolders() )
			_scanDlg->scanFolders();
		else
			_scanDlg->scanFiles( _filesDlg->selectedFiles() );

		//TODO scandialog exec?
		/*if( _scanDlg->exec() == QDialog::Accepted )
			_setButtonsAndActionsEnabled( true );
		else
			_doClearGraph();*/
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
			_doClearGraph();
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

		//TODO There are still two task to achieve:
		//1) Handle orientation only when the toolbar is floating
		//2) Resize toolbar when changing orientation
	}
	//-------------------------------------------------------------------------
	void MainWindow::_doClearGraph() const
	{
		_ui->graph->clear();
		_setButtonsAndActionsEnabled( false );
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
	void MainWindow::exitApp()
	{
		this->close();
		qApp->quit();
	}
	//-------------------------------------------------------------------------
	void MainWindow::_setButtonsAndActionsEnabled( bool value ) const
	{
		_ui->actionDraw->setEnabled( !value );
		_ui->actionClear->setEnabled( value );
		_ui->actionSave_as_Image->setEnabled( value );
		_ui->actionSave_as_dot->setEnabled( value );
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
} // end of depgraphV namespace
