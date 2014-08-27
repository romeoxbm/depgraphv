/**
 * scanmodepage.cpp
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
#include "scanmodepage.h"
#include "ui_scanmodepage.h"
#include "scandialog.h"
#include "appconfig.h"
#include "handlerootsdialog.h"
#include "selectfilesdialog.h"

namespace depgraphV
{
	ScanModePage::ScanModePage( QWidget* parent )
		: SettingsPage( parent ),
		_ui( new Ui::ScanModePage )
	{
		_ui->setupUi( this );

		AppConfig* c = Singleton<AppConfig>::instancePtr();
		connect( c, SIGNAL( configRestored() ), this, SLOT( onConfigRestored() ) );

		connect( _ui->selectRootFoldersRadio, SIGNAL( toggled( bool ) ),
				 c, SLOT( setScanByFolders( bool ) ) );

		connect( _ui->recursiveScanCheckBox, SIGNAL( toggled( bool ) ),
				 c, SLOT( setRecursiveScanEnabled( bool ) ) );

		connect( _ui->hiddenFoldersCheckbox, SIGNAL( toggled( bool ) ),
				 c, SLOT( setHiddenFoldersIncluded( bool ) ) );
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
	void ScanModePage::changeEvent( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QWidget::changeEvent( evt );
	}
	//-------------------------------------------------------------------------
	void ScanModePage::on_modifySelectionButton_clicked()
	{
		if( _ui->selectRootFoldersRadio->isChecked() )
			Singleton<HandleRootsDialog>::instancePtr()->exec();
		else
			Singleton<SelectFilesDialog>::instancePtr()->exec();
	}
	//-------------------------------------------------------------------------
	void ScanModePage::onConfigRestored()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		QRadioButton* radio = c->scanByFolders() ?
							  _ui->selectRootFoldersRadio : _ui->selectFilesRadio;
		radio->setChecked( true );
		_ui->recursiveScanCheckBox->setChecked( c->isRecursiveScanEnabled() );
		_ui->hiddenFoldersCheckbox->setChecked( c->hiddenFoldersIncluded() );
	}
} // end of depgraphV namespace
