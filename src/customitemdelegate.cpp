/**
 * customitemdelegate.cpp
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
#include "customitemdelegate.h"
#include "binaryradiowidget.h"

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
			_disconnect( editor );

		QComboBox* combo = qobject_cast<QComboBox*>( editor );
		if( combo )
			combo->setCurrentIndex( combo->findText( index.data().toString() ) );
		else
			QItemDelegate::setEditorData( editor, index );

		_connect( editor );
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::setModelData( QWidget* editor, QAbstractItemModel* model,
										 const QModelIndex& index ) const
	{
		QComboBox* combo = qobject_cast<QComboBox*>( editor );
		if( combo )
			model->setData( index, combo->currentText() );
		else
			QItemDelegate::setModelData( editor, model, index );
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::_connect( QWidget* widget ) const
	{
		if( widget->property( "editingStartedConnected" ).isValid() )
			return;

		bool ok = true;
		if( QLineEdit* w = qobject_cast<QLineEdit*>( widget ) )
		{
			ok = connect( w, SIGNAL( textEdited( const QString& ) ),
						  this, SIGNAL( editingStarted() )
			);
		}
		else if( QAbstractSpinBox* w = qobject_cast<QAbstractSpinBox*>( widget ) )
		{
			ok = connect( w, SIGNAL( editingFinished() ),
						  this, SIGNAL( editingStarted() )
			);
		}
		else if( QComboBox* w = qobject_cast<QComboBox*>( widget ) )
		{
			ok = connect( w, SIGNAL( currentIndexChanged( int ) ),
						  this, SIGNAL( editingStarted() )
			);
		}
		else if( QCheckBox* w = qobject_cast<QCheckBox*>( widget ) )
		{
			ok = connect( w, SIGNAL( stateChanged( int ) ),
						  this, SIGNAL( editingStarted() )
			);
		}
		else if( BinaryRadioWidget* w = qobject_cast<BinaryRadioWidget*>( widget ) )
		{
			ok = connect( w, SIGNAL( toggled( bool ) ),
						  this, SIGNAL( editingStarted() )
			);
		}

		Q_ASSERT( ok );
		widget->setProperty( "editingStartedConnected", true );
	}
	//-------------------------------------------------------------------------
	void CustomItemDelegate::_disconnect( QWidget* widget ) const
	{
		if( !widget->property( "editingStartedConnected" ).isValid() )
			return;

		widget->disconnect( this, SIGNAL( editingStarted() ) );
		widget->setProperty( "editingStartedConnected", QVariant() );
	}
}
