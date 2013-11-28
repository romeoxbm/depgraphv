/**
 * mainwindow.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 Francesco Guastella aka romeoxbm
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
#include "appconfig.h"
#include "ui_mainwindow.h"
#include "buildsettings.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QLibraryInfo>

namespace depgraphV
{
	MainWindow::MainWindow( QWidget* parent )
		: QMainWindow( parent ),
		_ui( new Ui::MainWindow ),
		_aboutDlg( 0 ),
		_isValidDirSelected( false )
	{
		_ui->setupUi( this );
		this->setWindowTitle( APP_NAME );

		_ui->actionHigh_Quality_Antialiasing->setChecked( _ui->graph->highQualityAntialiasing() );

		//Renderer action group
		QActionGroup* rendererGroup = new QActionGroup( _ui->menuRenderer );
		rendererGroup->addAction( _ui->actionNative );
		rendererGroup->addAction( _ui->actionOpenGL );

		_ui->actionNative->setData( Graph::Native );
		_ui->actionOpenGL->setData( Graph::OpenGL );

#ifndef QT_USE_OPENGL
		ui->actionOpenGL->setEnabled( false );
#endif

		QRegExp rx( "(\\s*\\*\\.\\w+\\s*;?)+" );
		QValidator* validator = new QRegExpValidator( rx );
		_ui->headersFilter->setValidator( validator );
		_ui->sourcesFilter->setValidator( validator );

		//Create language action group and setting up actionSystem_language
		_langGroup = new QActionGroup( _ui->menu_Language );
		_ui->actionSystem_language->setData( QLocale::system().name().mid( 0, 2 ) );
		_langGroup->addAction( _ui->actionSystem_language );
		_createLanguageAction( "en", "" );

		_lookForTranslations( QApplication::applicationDirPath() );

		//Read translations path from tPath and look for translation files
		QFile f( QApplication::applicationDirPath() + "/tPath" );
		if( f.open( QFile::ReadOnly | QFile::Text ) )
		{
			QTextStream in( &f );
			in.setCodec( "UTF-8" );

			//TODO Check if it's a valid path
			_lookForTranslations( in.readLine() );
			f.close();
		}

		//Connect signals and slots
		connect( _langGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( languageChanged( QAction* )  ) );
		connect( _ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
		connect( rendererGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( rendererTypeChanged( QAction* ) ) );

		_config = new AppConfig( this, _ui->graph );

		//Save default settings, if this is the first time we launch this application
		_config->saveDefault();

		//Restore last settings
		_config->restore();

		_ui->statusBar->showMessage( QString( "%1 %2" ).arg( APP_NAME, tr( "ready" ) ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete _ui;

		if( _aboutDlg )
			delete _aboutDlg;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	QString MainWindow::rootPath() const
	{
		return _isValidDirSelected ? _ui->selectedRootFolder->text() : QDir::currentPath();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::translateUi( const QString& locale )
	{
		QString l = locale;
		if( !_availableLanguages.contains( locale ) )
		{
			_currentLocale = "en";
			QLocale::setDefault( QLocale( _currentLocale ) );
			l = _currentLocale;
		}
		else
			languageChanged( _availableLanguages[ locale ] );

		if( _availableLanguages.contains( l ) )
			_availableLanguages[ l ]->setChecked( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::closeEvent( QCloseEvent* event )
	{
		_config->save();
		event->accept();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::on_selectedRootFolder_textChanged( const QString& value )
	{
		_isValidDirSelected = false;
		if( !value.isEmpty() )
		{
			QFileInfo info( value );
			_isValidDirSelected = info.isDir();
		}

		_ui->drawButton->setEnabled( _isValidDirSelected &&
									( _ui->parseHeadersCheckbox->isChecked() || _ui->parseSourcesCheckbox->isChecked() )
		);
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::selectRootFolder()
	{
		QString path = QFileDialog::getExistingDirectory( this, tr( "Select root folder" ), rootPath() );
		if( !path.isNull() )
			_ui->selectedRootFolder->setText( path );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::on_drawButton_clicked()
	{
		//Disable related ui widgets
		_setGroupBoxesEnabled( false );

		_ui->graph->prepare();
		_setGraphAttributes();
		uint filesFound = _scanFolder( _ui->selectedRootFolder->text() );

		if( filesFound > 0 )
		{
			if( _ui->graph->applyLayout() )
			{
				_setButtonsAndActionsEnabled( true );
				return;
			}
			else
				QMessageBox::information( this, tr( "Draw" ), tr( "Unable to render; Plugin not found." ) );
		}
		else
			QMessageBox::information( this, tr( "Draw" ), tr( "No file has been found in selected folder; nothing to draw." ) );

		_doClearGraph();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::on_clearButton_clicked()
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
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_doClearGraph() const
	{
		_ui->graph->clear();
		_setGroupBoxesEnabled( true );
		_setButtonsAndActionsEnabled( false );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::about()
	{
		if( !_aboutDlg )
			_aboutDlg = new AboutDialog( APP_NAME, APP_VER, this );

		_aboutDlg->show();
		_aboutDlg->move( geometry().center() - _aboutDlg->rect().center() );
	}
	//--------------------------------------------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::rendererTypeChanged( QAction* action )
	{
		Q_ASSERT( action );
		_ui->graph->setRenderer( (Graph::RendererType)action->data().toInt() );
		_ui->statusBar->showMessage( tr( "Renderer method changed" ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::languageChanged( QAction* action )
	{
		Q_ASSERT( action );
		QString locale = action->data().toString();

		if( _currentLocale == locale )
			return;

		QString appFileName = QString( "%1_%2.qm" ).arg( QApplication::applicationName(), locale );
		QString qtFileName = QString( "qt_%1.qm" ).arg( locale );

		_currentLocale = locale;

		//Changing translators
		_switchTranslator( &_appTranslator, appFileName, action->objectName(), locale == "en" );
		_switchTranslator( &_qtTranslator, qtFileName, QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );

		//Changing locale
		QLocale::setDefault( QLocale( locale ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::parseOptionsChanged()
	{
		_ui->drawButton->setEnabled( _isValidDirSelected &&
				( _ui->parseHeadersCheckbox->isChecked() || _ui->parseSourcesCheckbox->isChecked() )
		);
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::saveAsImage() const
	{
		QStringList* list = _ui->graph->pluginsListByKind( "loadimage" );
		if( !list )
		{
			QMessageBox::critical( const_cast<MainWindow*>( this ),
								   tr( "Cannot save image" ), tr( "Unable to save graph as image; Plugin not found." ) );
			return;
		}

		QString filter;
		for( int i = 0; i < list->count(); ++i )
		{
			QString p = list->at( i );
			filter += QString( "%1(*.%2)" ).arg( p.toUpper(), p );
			if( i < list->count() - 1 )
				filter += ";;";
		}

		QString path = QFileDialog::getSaveFileName(
					const_cast<MainWindow*>( this ),
					tr( "Select path and name of the image file" ),
					QDir::currentPath(), filter );

		if( path.isEmpty() )
			return;

		QStringList split = path.split( "." );
		QString format = split[ split.size() - 1 ];
		if( _ui->graph->saveImage( path, format ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
		else
			QMessageBox::critical( 0, tr( "Save as image" ), tr( "Unable to save file" ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::saveAsDot() const
	{
		QString path = QFileDialog::getSaveFileName(
					const_cast<MainWindow*>( this ),
					tr( "Select path and name of the dot file" ),
					QDir::currentPath(), "DOT(*.dot)" );

		if( path.isEmpty() )
			return;

		if( _ui->graph->saveDot( path ) )
			_ui->statusBar->showMessage( tr( "File successfully saved." ) );
		else
			QMessageBox::critical( 0, tr( "Save as dot" ), tr( "Unable to save file" ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::exitApp()
	{
		this->close();
		qApp->quit();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_setGraphAttributes() const
	{
		//Configuring graph attributes
		//http://www.graphviz.org/doc/info/attrs.html
		_ui->graph->setGraphAttribute( "splines", "spline" );
		_ui->graph->setGraphAttribute( "nodesep", "0.4" );

		_ui->graph->setVerticesAttribute( "shape", "box" );
		_ui->graph->setVerticesAttribute( "style", "rounded" );

		_ui->graph->setEdgesAttribute( "minlen", "3" );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_setGroupBoxesEnabled( bool value ) const
	{
		_ui->rootFolderGroupBox->setEnabled( value );
		_ui->parseGroupbox->setEnabled( value );
		_ui->headersFilterGroupBox->setEnabled( value && _ui->parseHeadersCheckbox->isChecked() );
		_ui->sourcesFilterGroupBox->setEnabled( value && _ui->parseSourcesCheckbox->isChecked() );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_setButtonsAndActionsEnabled( bool value ) const
	{
		_ui->drawButton->setEnabled( !value );
		_ui->clearButton->setEnabled( value );
		_ui->actionSave_as_Image->setEnabled( value );
		_ui->actionSave_as_dot->setEnabled( value );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	uint MainWindow::_scanFolder( const QString& dirName ) const
	{
		QDir dir( dirName );
		uint filesCount = 0;

		//Iterating subfolders, if required
		if( _ui->recursiveCheckBox->isChecked() )
		{
			foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs ) )
				filesCount += _scanFolder( entry.filePath() );
		}

		//Iterating files
		dir.setNameFilters( _getNameFilters() );
		foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files ) )
		{
			_ui->graph->createOrRetrieveVertex( entry.fileName() );
			_ui->graph->createEdges( entry.absolutePath(), entry.fileName() );
			filesCount++;
		}

		return filesCount;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	QStringList MainWindow::_getNameFilters() const
	{
		QStringList nameFilters;

		//Headers
		if( _ui->parseHeadersCheckbox->isChecked() )
		{
			if( _ui->standardHeaderFilterRadio->isChecked() )
				nameFilters << _ui->headersFilterComboBox->currentText();
			else
			{
				QString filters = _ui->headersFilter->text().replace( ' ', "" );
				nameFilters = filters.split( ";" );
			}
		}

		//Sources
		if( _ui->parseSourcesCheckbox->isChecked() )
		{
			if( _ui->standardSourcesFilterRadio->isChecked() )
				nameFilters << _ui->sourcesFilterComboBox->currentText();
			else
			{
				QString filters = _ui->sourcesFilter->text().replace( ' ', "" );
				foreach( QString f, filters.split( ";" ) )
					nameFilters << f;
			}
		}

		return nameFilters;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_lookForTranslations( const QString& path )
	{
		QDir dir( path );
		dir.setNameFilters( QStringList( QApplication::applicationName() + "*.qm" ) );
		foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files ) )
		{
			int startPos = QApplication::applicationName().length() + 1;
			QString locale( entry.fileName().mid( startPos, 2 ) );
			if( !_createLanguageAction( locale, path ) )
				continue;

			qDebug() << qPrintable( tr( "Found locale %1 in %2" ).arg( locale, path ) );

			//Also update system language QAction..
			if( locale == QLocale::system().name().mid( 0, 2 ) )
			{
				_ui->actionSystem_language->setObjectName( path );
				_ui->actionSystem_language->setIcon( QIcon( ":/flags/" + locale ) );
				_ui->actionSystem_language->setEnabled( true );
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool MainWindow::_createLanguageAction( const QString& locale, const QString& langFilePath )
	{
		if( _availableLanguages.contains( locale ) )
			return false;

		QAction* newLang = new QAction( this );
		newLang->setObjectName( langFilePath );
		newLang->setText( QLocale::languageToString( QLocale( locale ).language() ) );
		newLang->setData( locale );
		newLang->setCheckable( true );
		newLang->setIcon( QIcon( ":/flags/" + locale ) );
		_ui->menu_Language->addAction( newLang );
		_langGroup->addAction( newLang );
		_availableLanguages.insert( locale, newLang );

		return true;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_switchTranslator( QTranslator* t, const QString& fileName, const QString& directory, bool justRemoveTranslator )
	{
		//remove the old one
		QApplication::removeTranslator( t );

		if( justRemoveTranslator )
			return;

		QString fName = QString( "%1/%2" ).arg( directory, fileName );
		QFile file( fName );
		if( !file.exists() )
		{
			qDebug() << qPrintable( tr( "Translation file \"%1\" does not exists" ).arg( fName ) );
			return;
		}

		//and then..load the new one
		if( t->load( fileName, directory ) )
		{
			QApplication::installTranslator( t );
			qDebug() << qPrintable( tr( "Switched to translation file %1 in %2" ).arg( fileName, directory ) );
		}
	}
} // end of depgraphV namespace
