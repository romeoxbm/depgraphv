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
		_doSave();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::restore()
	{
		_doRestore();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::saveDefault()
	{
		if( !_settings.childGroups().contains( "default" ) )
			_doSave( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::restoreDefault()
	{
		if( !_settings.childGroups().contains( "default" ) )
		{
			qCritical() << tr( "Missing default settings" );
			return;
		}

		_doRestore( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::_doSave( bool def )
	{
		QString group = def ? "default/" : "current/";
		qDebug() << ( def ? tr( "Saving default settings..." ) : tr( "Saving settings..." ) );
		_settings.beginGroup( group + "MainWindow" );
		{
			_settings.setValue( "size", _win->size() );
			_settings.setValue( "pos", _win->pos() );
			_settings.setValue( "maximized", _win->isMaximized() );
			_settings.setValue( "recur", _win->ui->recursiveCheckBox->isChecked() );
			_settings.setValue( "pHdr", _win->ui->parseHeadersCheckbox->isChecked() );
			_settings.setValue( "hIndex", _win->ui->headersFilterComboBox->currentIndex() );
			_settings.setValue( "hCustomFilters", _win->ui->headersFilter->text() );
			_settings.setValue( "hCustomFiltersEnabled", _win->ui->customHeadersFilterRadio->isChecked() );
			_settings.setValue( "pSrc", _win->ui->parseSourcesCheckbox->isChecked() );
			_settings.setValue( "sIndex", _win->ui->sourcesFilterComboBox->currentIndex() );
			_settings.setValue( "sCustomFilters", _win->ui->sourcesFilter->text() );
			_settings.setValue( "sCustomFiltersEnabled", _win->ui->customSourcesFilterRadio->isChecked() );
			_settings.setValue( "lastRootPath", _win->rootPath() );
			_settings.setValue( "locale", _win->_langGroup->checkedAction()->data() );
		}
		_settings.endGroup();

		_settings.beginGroup( group + "Graph" );
		{
			_settings.setValue( "renderer", _graph->renderer() );
			_settings.setValue( "antialiasing", _graph->highQualityAntialiasing() );
		}
		_settings.endGroup();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::_doRestore( bool def )
	{
		QString group = def ? "default/" : "current/";
		qDebug() << ( def ? tr( "Restoring default settings..." ) : tr( "Restoring settings..." ) );
		_settings.beginGroup( group + "MainWindow" );
		{
			QPoint p( QApplication::desktop()->screenGeometry().center() - _win->rect().center() );
			_win->resize( _settings.value( "size", _win->rect().size() ).toSize() );
			_win->move( _settings.value( "pos", p ).toPoint() );
			if( _settings.value( "maximized", false ).toBool() )
				_win->showMaximized();

			_win->ui->recursiveCheckBox->setChecked( _settings.value( "recur", false ).toBool() );
			_win->ui->parseHeadersCheckbox->setChecked( _settings.value( "pHdr", true ).toBool() );
			_win->ui->headersFilterComboBox->setCurrentIndex( _settings.value( "hIndex", 0 ).toInt() );
			_win->ui->headersFilter->setText( _settings.value( "hCustomFilters", "*.h; *.hh; *.hxx; *.hpp; *.hp").toString() );
			_win->ui->customHeadersFilterRadio->setChecked( _settings.value( "hCustomFiltersEnabled", false).toBool() );
			_win->ui->parseSourcesCheckbox->setChecked( _settings.value( "pSrc", false ).toBool() );
			_win->ui->sourcesFilterComboBox->setCurrentIndex( _settings.value( "sIndex", 0 ).toInt() );
			_win->ui->sourcesFilter->setText( _settings.value( "sCustomFilters", "*.cpp; *.cc; *.cp; *.cxx; *.c++; *.C").toString() );
			_win->ui->customSourcesFilterRadio->setChecked( _settings.value( "sCustomFiltersEnabled", false).toBool() );
			_win->ui->selectedRootFolder->setText( _settings.value( "lastRootPath", QDir::currentPath() ).toString() );
			_win->translateUi( _settings.value( "locale", "en_US" ).toString() );
		}
		_settings.endGroup();

		_settings.beginGroup( group + "Graph" );
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
