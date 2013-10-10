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
#include "mainwindow.h"
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
		ui( new Ui::MainWindow )
	{
		ui->setupUi( this );
		ui->actionHigh_Quality_Antialiasing->setChecked( ui->graph->highQualityAntialiasing() );

		//Renderer action group
		QActionGroup* rendererGroup = new QActionGroup( ui->menuRenderer );
		rendererGroup->addAction( ui->actionNative );
		rendererGroup->addAction( ui->actionOpenGL );
		rendererGroup->addAction( ui->actionImage );

		ui->actionNative->setData( Graph::Native );
		ui->actionOpenGL->setData( Graph::OpenGL );
		ui->actionImage->setData( Graph::Image );

#ifndef USE_OPENGL
		ui->actionOpenGL->setEnabled( false );
#endif

		QRegExp rx( "(\\s*\\*\\.\\w+\\s*;?)+" );
		QValidator* validator = new QRegExpValidator( rx );
		ui->headersFilter->setValidator( validator );
		ui->sourcesFilter->setValidator( validator );

		//Create language action group and setting up actionSystem_language
		_langGroup = new QActionGroup( ui->menu_Language );
		ui->actionSystem_language->setData( QLocale::system().name() );
		_langGroup->addAction( ui->actionSystem_language );

		_lookForTranslations( QApplication::applicationDirPath() );
		_lookForTranslations( LANG_PATH );

		if( !_availableLanguages.contains( QLocale::system().name() ) )
		{
			_currentLocale = "en_US";
			ui->actionSystem_language->setEnabled( false );
			QLocale::setDefault( QLocale( _currentLocale ) );
		}
		else
		{
			ui->actionSystem_language->setObjectName( _availableLanguages[ QLocale::system().name() ] );
			ui->actionSystem_language->setIcon( QIcon( ":/flags/" + QLocale::system().name() ) );
			languageChanged( ui->actionSystem_language );
		}

		//Connect signals and slots
		connect( _langGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( languageChanged( QAction* )  ) );
		connect( ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
		connect( rendererGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( rendererTypeChanged( QAction* ) ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete ui;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::changeEvent( QEvent* event )
	{
		if( event )
		{
			if( event->type() == QEvent::LanguageChange )
			{
				ui->retranslateUi( this );
				_aboutText.clear();
				this->setWindowTitle( APP_NAME );
			}

			else if( event->type() == QEvent::LocaleChange )
			{}
		}

		QMainWindow::changeEvent( event );
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

		ui->drawButton->setEnabled( _isValidDirSelected &&
									( ui->parseHeadersCheckbox->isChecked() || ui->parseSourcesCheckbox->isChecked() )
		);
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::selectRootFolder()
	{
		QString path = QFileDialog::getExistingDirectory( this, tr( "Select root folder" ), QDir::currentPath() );
		if( !path.isNull() )
			ui->selectedRootFolder->setText( path );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::on_drawButton_clicked()
	{
		this->_setGraphAttributes();
		//Disable related ui widgets
		_setGroupBoxesEnabled( false );

		ui->graph->prepare();
		_scanFolder( ui->selectedRootFolder->text() );

		//Layout and draw graph
		ui->graph->applyLayout();
		_setButtonsAndActionsEnabled( true );
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

		if( answer == QMessageBox::No )
			return;

		ui->graph->clear();
		_setGroupBoxesEnabled( true );
		_setButtonsAndActionsEnabled( false );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::about()
	{
		if( _aboutText.isEmpty() )
		{
			QFile f( ":/text/about.html" );
			if( !f.open( QFile::ReadOnly | QFile::Text ) )
			{
				qWarning() << tr( "Cannot read about informations." );
				return;
			}

			QTextStream in( &f );
			in.setCodec( "UTF-8" );
			_aboutText = in.readAll();
			f.close();
		}

		QMessageBox::about( this, tr( "About..." ),
							_aboutText.arg( QApplication::applicationName(),
											QApplication::applicationVersion() )
		);
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::rendererTypeChanged( QAction* action )
	{
		Q_ASSERT( action );
		ui->graph->setRenderer( (Graph::RendererType)action->data().toInt() );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::languageChanged( QAction* action )
	{
		Q_ASSERT( action );
		QString locale = action->data().toString();

		if( _currentLocale == locale )
			return;

		QString appFileName = QString( "%1_%2" ).arg( QApplication::applicationName(), locale );
		QString qtFileName = QString( "qt_%1" ).arg( locale );

		_currentLocale = locale;

		//Changing translators
		_switchTranslator( &_appTranslator, appFileName, action->objectName() );
		_switchTranslator( &_qtTranslator, qtFileName, QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );

		//Changing locale
		QLocale::setDefault( QLocale( locale ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::parseOptionsChanged()
	{
		ui->drawButton->setEnabled( _isValidDirSelected &&
				( ui->parseHeadersCheckbox->isChecked() || ui->parseSourcesCheckbox->isChecked() )
		);
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::saveAsImage() const
	{
		QString path = QFileDialog::getSaveFileName(
					const_cast<MainWindow*>( this ),
					tr( "Select path and name of the image file" ),
					QDir::currentPath(), "PNG(*.png);;SVG(*.svg);;JPEG(*.jpg)" );

		if( path.isEmpty() )
			return;

		QStringList split = path.split( "." );
		QString format = split[ split.size() - 1 ];
		ui->graph->saveImage( path, format );
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

		ui->graph->saveDot( path );
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
		ui->graph->setGraphAttribute( "splines", "spline" );
		ui->graph->setGraphAttribute( "nodesep", "0.4" );

		ui->graph->setVerticesAttribute( "shape", "box" );
		ui->graph->setVerticesAttribute( "style", "rounded" );

		ui->graph->setEdgesAttribute( "minlen", "3" );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_setGroupBoxesEnabled( bool value ) const
	{
		ui->rootFolderGroupBox->setEnabled( value );
		ui->parseGroupbox->setEnabled( value );
		ui->headersFilterGroupBox->setEnabled( value );
		ui->sourcesFilterGroupBox->setEnabled( value );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_setButtonsAndActionsEnabled( bool value ) const
	{
		ui->drawButton->setEnabled( !value );
		ui->clearButton->setEnabled( value );
		ui->actionSave_as_Image->setEnabled( value );
		ui->actionSave_as_dot->setEnabled( value );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_scanFolder( const QString& dirName ) const
	{
		QDir dir( dirName );

		//Iterating subfolders, if required
		if( ui->recursiveCheckBox->isChecked() )
		{
			foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs ) )
				_scanFolder( entry.filePath() );
		}

		//Iterating files
		dir.setNameFilters( _getNameFilters() );
		foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files ) )
		{
			ui->graph->createOrRetrieveVertex( entry.fileName() );
			ui->graph->createEdges( entry.absolutePath(), entry.fileName() );
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	QStringList MainWindow::_getNameFilters() const
	{
		QStringList nameFilters;

		//Headers
		if( ui->parseHeadersCheckbox->isChecked() )
		{
			if( ui->standardHeaderFilterRadio->isChecked() )
				nameFilters << ui->headersFilterComboBox->currentText();
			else
			{
				QString filters = ui->headersFilter->text().replace( ' ', "" );
				nameFilters = filters.split( ";" );
			}
		}

		//Sources
		if( ui->parseSourcesCheckbox->isChecked() )
		{
			if( ui->standardSourcesFilterRadio->isChecked() )
				nameFilters << ui->sourcesFilterComboBox->currentText();
			else
			{
				QString filters = ui->sourcesFilter->text().replace( ' ', "" );
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
			QString locale( entry.fileName().mid( startPos, 5 ) );
			if( _availableLanguages.contains( locale ) )
				continue;

			_availableLanguages.insert( locale, path ) ;
			qDebug() << "Found locale " << locale << " in " << path;

			//Creating the action
			QAction* newLang = new QAction( this );
			newLang->setObjectName( path );
			newLang->setText( QLocale::languageToString( QLocale( locale ).language() ) );
			newLang->setData( locale );
			newLang->setCheckable( true );
			newLang->setIcon( QIcon( ":/flags/" + locale ) );
			ui->menu_Language->addAction( newLang );
			_langGroup->addAction( newLang );
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::_switchTranslator( QTranslator* t, const QString& fileName, const QString& directory )
	{
		//remove the old one
		QApplication::removeTranslator( t );

		//and then..load the new one
		if( t->load( fileName, directory ) )
		{
			QApplication::installTranslator( t );
			qDebug() << tr( "Switched to translation file " ) << fileName << " in " << directory;
		}
	}
} // end of depgraph namespace
