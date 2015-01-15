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
 * customitemdelegate.cpp
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
#include "customitemdelegate.h"
#include "binaryradiowidget.h"
#include "selectfilesdialog.h"

namespace depgraphV
{
	CustomItemDelegate::CustomItemDelegate( QObject* parent )
		: QItemDelegate( parent )
	{
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::setEditorData( QWidget* editor,
										  const QModelIndex& index ) const
	{
		if( _disableConnections )
			editor->disconnect( this, SIGNAL( editingStarted() ) );

		if( QComboBox* c = qobject_cast<QComboBox*>( editor ) )
		{
			c->setCurrentIndex( c->findText( index.data().toString() ) );
			_connect( c );
		}
		else if( QListWidget* l = qobject_cast<QListWidget*>( editor ) )
		{
			QStringList list = index.data().toString().split( ';', QString::SkipEmptyParts );

			//Disable signals (prevent firing of both rowsInserted and rowsRemoved signals)
			l->model()->blockSignals( true );
			l->clear();
			if( !list.isEmpty() )
				l->addItems( list );

			l->model()->blockSignals( false );
			_connect( l );
		}
		else if( SelectFilesDialog* d = qobject_cast<SelectFilesDialog*>( editor ) )
		{
			connect( d, SIGNAL( selectionChanged() ),
					 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
			);
		}
		else
		{
			QItemDelegate::setEditorData( editor, index );
			_connect( editor );
		}
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::setModelData( QWidget* editor, QAbstractItemModel* model,
										 const QModelIndex& index ) const
	{
		if( QComboBox* c = qobject_cast<QComboBox*>( editor ) )
			model->setData( index, c->currentText() );

		else if( QListWidget* l = qobject_cast<QListWidget*>( editor ) )
		{
			QString data;
			for( int i = 0; i < l->count(); i++ )
				data += l->item( i )->text() + ";";

			model->setData( index, data );
		}
		else
			QItemDelegate::setModelData( editor, model, index );
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::_connect( QComboBox* combo ) const
	{
		connect( combo, SIGNAL( activated( int ) ),
				 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
		);
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::_connect( QListWidget* list ) const
	{
		connect( list->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ),
				 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
		);
		connect( list->model(), SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ),
				 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
		);
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::_connect( QWidget* widget ) const
	{
		//TODO Qt::UniqueConnection prevent signals being connected more than one time,
		//but every call to setEditorData does require 3 qobjects casts (7 in the worst case)...

		if( QLineEdit* w = qobject_cast<QLineEdit*>( widget ) )
		{
			connect( w, SIGNAL( textEdited( const QString& ) ),
					 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
			);
		}
		else if( QAbstractSpinBox* w = qobject_cast<QAbstractSpinBox*>( widget ) )
		{
			connect( w, SIGNAL( editingFinished() ),
					 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
			);
		}
		else if( QCheckBox* w = qobject_cast<QCheckBox*>( widget ) )
		{
			connect( w, SIGNAL( stateChanged( int ) ),
					 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
			);
		}
		else if( BinaryRadioWidget* w = qobject_cast<BinaryRadioWidget*>( widget ) )
		{
			connect( w, SIGNAL( toggled() ),
					 this, SIGNAL( editingStarted() ), Qt::UniqueConnection
			);
		}
	}
}
