/**
 * removerootdialog.cpp
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
#include "removerootdialog.h"
#include "ui_removerootdialog.h"

#include <QPushButton>

namespace depgraphV
{
	RemoveRootDialog::RemoveRootDialog( QWidget* parent )
		: QDialog( parent ),
		_ui( new Ui::RemoveRootDialog )
	{
		_ui->setupUi( this );

		_ui->rootFolders->addAction( _ui->actionSelect_All );
		_ui->rootFolders->addAction( _ui->actionSelect_None );
		_ui->rootFolders->addAction( _ui->actionInvert_Selection );
	}
	//-------------------------------------------------------------------------
	RemoveRootDialog::~RemoveRootDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	QStringList RemoveRootDialog::exec( const QStringList& folders )
	{
		QStringList res;
		_ui->rootFolders->clear();
		_ui->rootFolders->addItems( folders );
		_foldersCount = folders.count();
		QDialog::DialogCode code;
		_updateEnabledFlags();
		code = static_cast<QDialog::DialogCode>( QDialog::exec() );
		if( code == QDialog::Accepted )
		{
			foreach( QListWidgetItem* item, _ui->rootFolders->selectedItems() )
				res << item->text();
		}

		return res;
	}
	//-------------------------------------------------------------------------
	void RemoveRootDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void RemoveRootDialog::_invertSelection()
	{
		_ui->rootFolders->blockSignals( true );
		for( int i = 0; i < _foldersCount; i++ )
		{
			QListWidgetItem* item = _ui->rootFolders->item( i );
			item->setSelected( !item->isSelected() );
		}
		_ui->rootFolders->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void RemoveRootDialog::_updateEnabledFlags()
	{
		_ui->actionSelect_None->setEnabled(
					!_ui->rootFolders->selectedItems().isEmpty() &&
					_foldersCount > 0
		);

		_ui->actionSelect_All->setEnabled(
					_ui->rootFolders->selectedItems().count() != _foldersCount &&
					_foldersCount > 0
		);

		_ui->actionInvert_Selection->setEnabled( _foldersCount > 0 );

		_ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled(
					!_ui->rootFolders->selectedItems().isEmpty()
		);
	}
}
