/**
 ******************************************************************************
 *                _                                        _
 *             __| | ___ _ __         __ _ _ __ __ _ _ __ | |__/\   /\
 *            / _` |/ _ \ '_ \ _____ / _` | '__/ _` | '_ \| '_ \ \ / /
 *           | (_| |  __/ |_) |_____| (_| | | | (_| | |_) | | | \ V /
 *            \__,_|\___| .__/       \__, |_|  \__,_| .__/|_| |_|\_/
 *                      |_|          |___/          |_|
 *
 ******************************************************************************
 *
 * generalpage.cpp
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
#include "generalpage.h"
#include "ui_generalpage.h"

namespace depgraphV
{
	GeneralPage::GeneralPage( MainWindow* w, SettingsDialog* parent )
		: SettingsPage( w, parent ),
		_ui( new Ui::GeneralPage )
	{
		_ui->setupUi( this );

		AppConfig* c = Singleton<AppConfig>::instancePtr();
		connect( _ui->warnOnGraphRemoval, SIGNAL( toggled( bool ) ),
				 c, SLOT( setWarnOnGraphRemoval( bool ) )
		);
	}
	//-------------------------------------------------------------------------
	GeneralPage::~GeneralPage()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	QString GeneralPage::iconPath() const
	{
		//TODO Icon for General Settings
		return ":/settingsDlgIcons/folder-scan_96x96.png";
	}
	//-------------------------------------------------------------------------
	bool GeneralPage::event( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		return QWidget::event( evt );
	}
} // end of depgraphV namespace
