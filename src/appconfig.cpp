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
#include "buildsettings.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

namespace depgraphV
{
	AppConfig::AppConfig( MainWindow* win )
		: _settings( "Francesco Guastella", APP_NAME ),
		  _win( win )
	{
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::save()
	{
		qDebug() << "Saving settings...";
		_settings.beginGroup( "MainWindow" );
		{
			_settings.setValue( "size", _win->size() );
			_settings.setValue( "pos", _win->pos() );
			_settings.setValue( "maximized", _win->isMaximized() );
		}
		_settings.endGroup();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AppConfig::restore()
	{
		qDebug() << "Restoring settings...";
		QPoint p( QApplication::desktop()->screenGeometry().center() - _win->rect().center() );

		_settings.beginGroup( "MainWindow" );
		{
			_win->resize( _settings.value( "size", _win->rect().size() ).toSize() );
			_win->move( _settings.value( "pos", p ).toPoint() );
			if( _settings.value( "maximized", false ).toBool() )
				_win->showMaximized();
		}
		_settings.endGroup();
	}
} // end of depgraphV namespace
