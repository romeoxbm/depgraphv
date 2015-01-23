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
 * settingspage.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2015 Francesco Guastella aka romeoxbm
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
#include "settingspage.h"
#include "settingsdialog.h"

namespace depgraphV
{
	SettingsPage::SettingsPage( MainWindow* w, SettingsDialog* parent )
		: QWidget( parent ),
		  _dialog( parent )
	{
		Q_ASSERT( w );
		connect( w, SIGNAL( projectOpened( Project* ) ),
				 this, SLOT( onProjectOpened( Project* ) )
		);
		connect( w, SIGNAL( projectLoaded( Project* ) ),
				 this, SLOT( onProjectLoaded( Project* ) )
		);
		connect( w, SIGNAL( projectClosed() ),
				 this, SLOT( onProjectClosed() )
		);
	}
	//-------------------------------------------------------------------------
	SettingsPage::~SettingsPage()
	{
	}
	//-------------------------------------------------------------------------
	void SettingsPage::onProjectOpened( Project* p )
	{
		Q_UNUSED( p );
	}
	//-------------------------------------------------------------------------
	void SettingsPage::onProjectLoaded( Project* p )
	{
		Q_UNUSED( p );
	}
	//-------------------------------------------------------------------------
	void SettingsPage::onProjectClosed()
	{
	}
}
