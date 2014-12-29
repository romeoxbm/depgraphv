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
 * handlerootsdialog.cpp
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
#include "handlerootsdialog.h"
#include "ui_handlerootsdialog.h"
#include "helpers.h"
#include "mainwindow.h"

namespace depgraphV
{
	HandleRootsDialog::HandleRootsDialog( MainWindow* parent )
		: QDialog( parent ),
		_ui( new Ui::HandleRootsDialog )
	{
		_ui->setupUi( this );

		//Finishing ui setup
		_ui->rootFolders->addAction( _ui->actionSelect_All );
		_ui->rootFolders->addAction( _ui->actionSelect_None );
		_ui->rootFolders->addAction( _ui->actionInvert_Selection );
		Helpers::insertSeparator( _ui->rootFolders );
		_ui->rootFolders->addAction( _ui->actionAdd );
		_ui->rootFolders->addAction( _ui->actionRemove_Selection );
		_ui->rootFolders->viewport()->installEventFilter( this );

		connect( parent, SIGNAL( projectOpened( Project* ) ),
				 this, SLOT( _onProjectOpened( Project* ) )
		);
	}
	//-------------------------------------------------------------------------
	HandleRootsDialog::~HandleRootsDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	bool HandleRootsDialog::event( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		return QDialog::event( evt );
	}
	//-------------------------------------------------------------------------
	bool HandleRootsDialog::eventFilter( QObject* obj, QEvent* evt )
	{
		if( obj == _ui->rootFolders->viewport() && evt->type() == QEvent::ContextMenu )
			_updateEnabledFlags();

		return QDialog::eventFilter( obj, evt );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_invertSelection()
	{
		for( int i = 0; i < _ui->rootFolders->count(); i++ )
		{
			QListWidgetItem* item = _ui->rootFolders->item( i );
			item->setSelected( !item->isSelected() );
		}
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_updateEnabledFlags()
	{
		int selectedCount = _ui->rootFolders->selectedItems().count();
		int count = _ui->rootFolders->count();

		_ui->actionSelect_All->setEnabled( count && selectedCount < count );
		_ui->actionSelect_None->setEnabled( count && selectedCount );
		_ui->actionInvert_Selection->setEnabled( count );

		_ui->actionRemove_Selection->setEnabled( selectedCount );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_onRootFolderAdded()
	{
		Project* p = Singleton<Project>::instancePtr();

		QString root = QFileDialog::getExistingDirectory(
					_ui->rootFolders,
					tr( "Add root folder" ),
					p->currentValue( "rootFolder" ).toString()
		);

		if( root.isNull() )
			return;

		//TODO Check if root is a subfolder of an already added folder
		//and if recursive scan is enabled; If so, do not add root.
		if( p->currentValue( "scanRecursively" ).toBool() )
		{
		}

		_ui->rootFolders->addItem( root );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_onRemoveSelection()
	{
		QString count = QString::number( _ui->rootFolders->selectedItems().count() );

		QMessageBox::StandardButton answer = QMessageBox::question(
			parentWidget(),
			tr( "Remove selected root folders" ),
			tr( "Are you sure you want to remove %1 selected folders?" ).arg( count ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer != QMessageBox::No )
			qDeleteAll( _ui->rootFolders->selectedItems() );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_onClose( int result )
	{
		Project* p = Singleton<Project>::instancePtr();
		if( result )
			p->submitChanges();
		else
			p->revertChanges();
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_onProjectOpened( Project* p )
	{
		p->addMapping( _ui->rootFolders, "selectedFolders" );
	}
}
