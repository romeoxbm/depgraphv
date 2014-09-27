/**
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

namespace depgraphV
{
	HandleRootsDialog::HandleRootsDialog( QWidget* parent )
		: QDialog( parent ),
		_ui( new Ui::HandleRootsDialog ),
		_selectedFolders( 0 )
	{
		_ui->setupUi( this );

		QAction* sep = new QAction( this );
		sep->setSeparator( true );

		_ui->rootFolders->addAction( _ui->actionSelect_All );
		_ui->rootFolders->addAction( _ui->actionSelect_None );
		_ui->rootFolders->addAction( _ui->actionInvert_Selection );
		_ui->rootFolders->addAction( sep );
		_ui->rootFolders->addAction( _ui->actionAdd );
		_ui->rootFolders->addAction( _ui->actionRemove_Selection );
		_ui->rootFolders->viewport()->installEventFilter( this );

		connect( Singleton<AppConfig>::instancePtr(), SIGNAL( configRestored() ),
				 this, SLOT( onConfigRestored() ) );
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
		if( obj == _ui->rootFolders->viewport() &&evt->type() == QEvent::ContextMenu )
			_updateEnabledFlags();

		return QDialog::eventFilter( obj, evt );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_invertSelection()
	{
		_ui->rootFolders->blockSignals( true );
		for( int i = 0; i < _selectedFolders->count(); i++ )
		{
			QListWidgetItem* item = _ui->rootFolders->item( i );
			item->setSelected( !item->isSelected() );
		}
		_ui->rootFolders->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::_updateEnabledFlags()
	{
		int selectedCount = _ui->rootFolders->selectedItems().count();
		int count = _selectedFolders->count();

		_ui->actionSelect_None->setEnabled( selectedCount > 0 && count );

		_ui->actionSelect_All->setEnabled( selectedCount != count && count > 0 );

		_ui->actionInvert_Selection->setEnabled( count > 0 );
		_ui->actionRemove_Selection->setEnabled( selectedCount > 0 );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::on_actionAdd_triggered()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		QString root = QFileDialog::getExistingDirectory(
					_ui->rootFolders,
					tr( "Add root folder" ),
					c->rootFolder()
		);

		if( root.isNull() )
			return;

		//TODO Check if root is a subfolder of an already added folder
		//and if recursive scan is enabled; If so, do not add root.
		if( c->isRecursiveScanEnabled() )
		{

		}
		_selectedFolders->append( root );
		_ui->rootFolders->addItem( root );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::on_actionRemove_Selection_triggered()
	{
		QString count = QString::number( _ui->rootFolders->selectedItems().count() );

		QMessageBox::StandardButton answer = QMessageBox::question(
			0,
			tr( "Remove selected root folders" ),
			tr( "Are you sure you want to remove " )
												 + count
												 + tr( " selected folders?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer == QMessageBox::No )
			return;

		foreach( QListWidgetItem* i, _ui->rootFolders->selectedItems() )
			_selectedFolders->removeAll( i->text() );

		_ui->rootFolders->clear();
		_ui->rootFolders->addItems( *_selectedFolders );
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::onClose( int result )
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		Memento<QStringList>* m = c->selectedFoldersMemento();

		if( result )
			m->commit();
		else
		{
			m->revert();
			_ui->rootFolders->clear();
			_ui->rootFolders->addItems( m->state() );
		}
	}
	//-------------------------------------------------------------------------
	void HandleRootsDialog::onConfigRestored()
	{
		if( !_selectedFolders )
			_selectedFolders = &Singleton<AppConfig>::instance().selectedFolders();

		_ui->rootFolders->clear();
		_ui->rootFolders->addItems( *_selectedFolders );
	}
}
