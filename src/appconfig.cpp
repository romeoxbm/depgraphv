/**
 * appconfig.cpp
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
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QDir>

namespace depgraphV
{
	AppConfig::AppConfig( MainWindow* win , Graph* g )
		: QObject( win ),
		  _settings( "romeoxbm", APP_NAME ),
		  _win( win ),
		  _graph( g )
	{
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::save()
	{
		qDebug() << tr( "Saving settings..." );
		_settings.beginGroup( "MainWindow" );
		{
			_settings.setValue( "size", _win->size() );
			_settings.setValue( "pos", _win->pos() );
			_settings.setValue( "maximized", _win->isMaximized() );
			_settings.setValue( "recur", _win->ui->recursiveCheckBox->isChecked() );
			_settings.setValue( "pHdr", _win->ui->parseHeadersCheckbox->isChecked() );
			_settings.setValue( "pSrc", _win->ui->parseSourcesCheckbox->isChecked() );
			_settings.setValue( "lastRootPath", _win->rootPath() );
		}
		_settings.endGroup();

		_settings.beginGroup( "Graph" );
		{
			_settings.setValue( "renderer", _graph->renderer() );
			_settings.setValue( "antialiasing", _graph->highQualityAntialiasing() );
		}
		_settings.endGroup();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::restore()
	{
		qDebug() << tr( "Restoring settings..." );
		_settings.beginGroup( "MainWindow" );
		{
			QPoint p( QApplication::desktop()->screenGeometry().center() - _win->rect().center() );
			_win->resize( _settings.value( "size", _win->rect().size() ).toSize() );
			_win->move( _settings.value( "pos", p ).toPoint() );
			if( _settings.value( "maximized", false ).toBool() )
				_win->showMaximized();

			_win->ui->recursiveCheckBox->setChecked( _settings.value( "recur", false ).toBool() );
			_win->ui->parseHeadersCheckbox->setChecked( _settings.value( "pHdr", true ).toBool() );
			_win->ui->parseSourcesCheckbox->setChecked( _settings.value( "pSrc", false ).toBool() );
			_win->ui->selectedRootFolder->setText( _settings.value( "lastRootPath", QDir::currentPath() ).toString() );
		}
		_settings.endGroup();

		_settings.beginGroup( "Graph" );
		{
			//renderer
			Graph::RendererType r = (Graph::RendererType)_settings.value( "renderer", Graph::Native ).toInt();
			_graph->setRenderer( r );
			QAction* a = r == Graph::Native ? _win->ui->actionNative : _win->ui->actionOpenGL;
			a->setChecked( true );

			//antialiasing
			bool aa = _settings.value( "antialiasing", false ).toBool();
			_graph->setHighQualityAntialiasing( aa );
			_win->ui->actionHigh_Quality_Antialiasing->setChecked( aa );
		}
		_settings.endGroup();
	}
} // end of depgraphV namespace
