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
		  _settings( APP_VENDOR, APP_NAME ),
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
		if( _settings.childGroups().contains( "default" ) )
			return;

		//Move the main window to its default position (center of the screen) before saving default settings
		_win->move( QApplication::desktop()->screenGeometry().center() - _win->rect().center() );
		_doSave( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::restoreDefault()
	{
		if( !_settings.childGroups().contains( "default" ) )
		{
			qCritical() << qPrintable( tr( "Missing default settings" ) );
			return;
		}

		_doRestore( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	const QString& AppConfig::installPrefix()
	{
		if( _instPrefix.isEmpty() )
		{
#ifdef WIN32
			QString regKey = QString( "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1" ).arg( APP_NAME );
			QSettings s( regKey, QSettings::NativeFormat );
			QFileInfo info( s.value( "UninstallString" ).toString() );
			_instPrefix = info.absolutePath();
#else
#	ifndef NDEBUG
			_instPrefix = ".";
#	else
			_instPrefix = QFileInfo( QApplication::applicationDirPath() ).absolutePath();
			_instPrefix.replace( APP_BIN_PATH, "" );
#	endif //NDEBUG
#endif
		}

		return _instPrefix;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	const QString& AppConfig::translationsPath()
	{
		if( _trPath.isEmpty() )
			_trPath = QString( "%1/%2" ).arg( installPrefix(), APP_TR_PATH );

		return _trPath;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::_doSave( bool def )
	{
		QString group = def ? "default/" : "current/";
		qDebug() << qPrintable( def ? tr( "Saving default settings..." ) : tr( "Saving settings..." ) );
		_settings.beginGroup( group + "MainWindow" );
		{
			_settings.setValue( "size", _win->size() );
			_settings.setValue( "pos", _win->pos() );
			_settings.setValue( "maximized", _win->isMaximized() );
			_settings.setValue( "recur", _win->isRecursiveScanEnabled() );
			_settings.setValue( "showDonate", _win->showDonateOnExit() );
			
			bool parseHdr, hCustomFiltersEnabled;
			int selectedHdrFilter;
			QString customHdrFilter;
			_win->headersInfo( &parseHdr, &hCustomFiltersEnabled, &selectedHdrFilter, &customHdrFilter );
			_settings.setValue( "pHdr", parseHdr );
			_settings.setValue( "hCustomFiltersEnabled", hCustomFiltersEnabled );
			_settings.setValue( "hIndex", selectedHdrFilter );
			_settings.setValue( "hCustomFilters", customHdrFilter );

			bool parseSrc, sCustomFiltersEnabled;
			int selectedSrcFilter;
			QString customSrcFilter;
			_win->sourcesInfo( &parseSrc, &sCustomFiltersEnabled, &selectedSrcFilter, &customSrcFilter ); 
			_settings.setValue( "pSrc", parseSrc );
			_settings.setValue( "sCustomFiltersEnabled", sCustomFiltersEnabled );
			_settings.setValue( "sIndex", selectedSrcFilter );
			_settings.setValue( "sCustomFilters", customSrcFilter );
			
			_settings.setValue( "lastRootPath", _win->rootPath() );
			_settings.setValue( "locale", _win->selectedLocaleData() );
		}
		_settings.endGroup();

		_settings.beginGroup( group + "Graph" );
		{
			_settings.setValue( "renderer", _graph->renderer() );
			_settings.setValue( "antialiasing", _graph->highQualityAA() );
		}
		_settings.endGroup();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::_doRestore( bool def )
	{
		QString group = def ? "default/" : "current/";
		qDebug() << qPrintable( def ? tr( "Restoring default settings..." ) : tr( "Restoring settings..." ) );
		_settings.beginGroup( group + "MainWindow" );
		{
			QPoint defaultPos( QApplication::desktop()->screenGeometry().center() - _win->rect().center() );
			_win->resize( _settings.value( "size", _win->rect().size() ).toSize() );
			_win->move( _settings.value( "pos", defaultPos ).toPoint() );
			if( _settings.value( "maximized", false ).toBool() )
				_win->showMaximized();

			_win->setRecursiveScanEnabled( _settings.value( "recur", false ).toBool() );
			_win->setShowDonateOnExit( _settings.value( "showDonate", true ).toBool() );

			_win->setHeadersInfo(
				_settings.value( "pHdr", true ).toBool(),
				_settings.value( "hCustomFiltersEnabled", false ).toBool(),
				_settings.value( "hIndex", 0 ).toInt(),
				_settings.value( "hCustomFilters", "*.h; *.hh; *.hxx; *.hpp; *.hp" ).toString()
				);

			_win->setSourcesInfo(
				_settings.value( "pSrc", false ).toBool(),
				_settings.value( "sCustomFiltersEnabled", false ).toBool(),
				_settings.value( "sIndex", 0 ).toInt(),
				_settings.value( "sCustomFilters", "*.cpp; *.cc; *.cp; *.cxx; *.c++; *.C" ).toString()
			);

			_win->setRootPath( _settings.value( "lastRootPath", QDir::currentPath() ).toString() );
			_win->translateUi( _settings.value( "locale", "en" ).toString() );
		}
		_settings.endGroup();

		_settings.beginGroup( group + "Graph" );
		{
			//renderer
			Graph::RendererType r = (Graph::RendererType)_settings.value( "renderer", Graph::Native ).toInt();
			_graph->setRenderer( r );
			_win->setRendererActionCheckedByType( r, true );

			//antialiasing
			bool aa = _settings.value( "antialiasing", false ).toBool();
			_graph->setHighQualityAntialiasing( aa );
			_win->setHighQualityAA( aa );
		}
		_settings.endGroup();
	}
} // end of depgraphV namespace
