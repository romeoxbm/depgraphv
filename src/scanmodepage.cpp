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
 * scanmodepage.cpp
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
#include "scanmodepage.h"
#include "ui_scanmodepage.h"
#include "handlerootsdialog.h"
#include "selectfilesdialog.h"

namespace depgraphV
{
	ScanModePage::ScanModePage( MainWindow* w, SettingsDialog* parent )
		: SettingsPage( w, parent ),
		_ui( new Ui::ScanModePage )
	{
		_ui->setupUi( this );
	}
	//-------------------------------------------------------------------------
	ScanModePage::~ScanModePage()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	QString ScanModePage::iconPath() const
	{
		return ":/settingsDlgIcons/folder-scan_96x96.png";
	}
	//-------------------------------------------------------------------------
	void ScanModePage::onGraphChanged()
	{
		_updateSelectionCount();
	}
	//-------------------------------------------------------------------------
	bool ScanModePage::event( QEvent* evt )
	{
		if( evt )
		{
			if( evt->type() == QEvent::LanguageChange )
				_ui->retranslateUi( this );

			if( evt->type() == QEvent::Show )
				_updateSelectionCount();
		}

		return QWidget::event( evt );
	}
	//-------------------------------------------------------------------------
	void ScanModePage::_modifySelection()
	{
		if( _ui->selectRootFoldersRadio->isChecked() )
			Singleton<HandleRootsDialog>::instancePtr()->exec();
		else
			Singleton<SelectFilesDialog>::instancePtr()->exec();

		_updateSelectionCount();
	}
	//-------------------------------------------------------------------------
	void ScanModePage::onProjectOpened( Project* p )
	{
		SettingsPage::onProjectOpened( p );

		QRadioButton* radios[] = { _ui->selectRootFoldersRadio, _ui->selectFilesRadio };
		p->addMapping( radios, "scanByFolders" );
		p->addMapping( _ui->recursiveScanCheckBox, "scanRecursively" );
		p->addMapping( _ui->hiddenFoldersCheckbox, "includeHiddenFolders" );
	}
	//-------------------------------------------------------------------------
	void ScanModePage::_updateSelectionCount()
	{
		Project* p = Singleton<Project>::instancePtr();
		int c;
		if( _ui->selectRootFoldersRadio->isChecked() )
		{
			QStringList sel = p->currentValue( "selectedFolders" ).toStringList();
			c = sel.count();
		}
		else
			c = 27; //TODO

		_ui->selectionCount->setText( tr( "Current selection: %1 item(s)" ).arg( c ) );
	}
} // end of depgraphV namespace
