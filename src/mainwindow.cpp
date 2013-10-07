/**
 * mainwindow.cpp
 *
 * This source file is part of dep-graph - An useful tool to analize header
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
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace depgraph
{
	MainWindow::MainWindow( QWidget* parent )
		: QMainWindow( parent ),
		ui( new Ui::MainWindow )
	{
		ui->setupUi( this );
		this->setWindowTitle( QApplication::applicationName() );
		ui->actionHigh_Quality_Antialiasing->setChecked( ui->graph->highQualityAntialiasing() );

		//Action groups
		QActionGroup* rendererGroup = new QActionGroup( this );
		rendererGroup->addAction( ui->actionNative );
		rendererGroup->addAction( ui->actionOpenGL );
		rendererGroup->addAction( ui->actionImage );

		ui->actionNative->setData( QVariant( Graph::Native ) );
		ui->actionOpenGL->setData( QVariant( Graph::OpenGL ) );
		ui->actionImage->setData( QVariant( Graph::Image ) );

#ifndef USE_OPENGL
		ui->actionOpenGL->setEnabled( false );
#endif

		QRegExp rx( "(\\s*\\*\\.\\w+\\s*;?)+" );
		QValidator* validator = new QRegExpValidator( rx );
		ui->headersFilter->setValidator( validator );
		ui->sourcesFilter->setValidator( validator );

		connect( ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
		connect( rendererGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( rendererTypeChanged( QAction* ) ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete ui;
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
					"Clear graph",
					"Are you sure?",
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
				qWarning() << "Cannot read about informations.";
				return;
			}

			QTextStream in( &f );
			_aboutText = in.readAll();
			f.close();
		}

		QMessageBox::about( this, "About...", _aboutText.arg( QApplication::applicationName(), QApplication::applicationVersion() ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void MainWindow::rendererTypeChanged( QAction* action )
	{
		ui->graph->setRenderer( (Graph::RendererType)action->data().toInt() );
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
					"Select path and name of the image file",
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
					"Select path and name of the dot file",
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
} // end of depgraph namespace
