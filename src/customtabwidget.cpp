/**
 * customtabwidget.cpp
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
#include "customtabwidget.h"
#include "project.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>
#ifndef QT_USE_QT5
#	include <QTabBar>
#endif

namespace depgraphV
{
	CustomTabWidget::CustomTabWidget( QWidget* parent )
		: QTabWidget( parent ),
		  _newGraphCount( 0 ),
		  _dataConnected( false )
	{
		connect( this, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeTab( int ) ) );

#ifdef QT_USE_QT5
		connect( this, SIGNAL( tabBarDoubleClicked( int ) ), this, SLOT( renameTab( int ) ) );
#else
		tabBar()->installEventFilter( this );
#endif
	}
	//-------------------------------------------------------------------------
	Graph* CustomTabWidget::currentGraph() const
	{
		return graph( currentIndex() );
	}
	//-------------------------------------------------------------------------
	Graph* CustomTabWidget::graph( int index ) const
	{
		Q_ASSERT( index >= 0 && index < count() );
		return static_cast<Graph*>( widget( index ) );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::loadTabs()
	{
		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* m = p->tableModel( "graphSettings" );
		for( int i = 0; i < m->rowCount(); i ++ )
		{
			QSqlRecord r = m->record( i );
			Graph* g = new Graph( this );
			g->setAttributes( r );
			addTab( g, r.value( "name" ).toString() );
		}

		if( !_dataConnected )
		{
			connect( m, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
					 this, SLOT( onDataChanged( QModelIndex, QModelIndex ) )
			);
			_dataConnected = true;
		}
	}
	//-------------------------------------------------------------------------
#ifndef QT_USE_QT5
	bool CustomTabWidget::eventFilter( QObject* o, QEvent* evt )
	{
		bool result = QTabWidget::eventFilter( o, evt );

		if( o == tabBar() && evt->type() == QEvent::MouseButtonDblClick )
		{
			QMouseEvent* mouseEvt = static_cast<QMouseEvent*>( evt );
			int index = tabBar()->tabAt( mouseEvt->pos() );
			if( index == -1 )
				return result;

			renameTab( index );
			return true;
		}

		return result;
	}
#endif
	//-------------------------------------------------------------------------
	void CustomTabWidget::newGraph()
	{
		QString graphName = "New Graph " + QString::number( ++_newGraphCount );

		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* m = p->tableModel( "graphSettings" );
		QSqlField f( "name", QVariant::String );
		f.setValue( graphName );
		QSqlRecord r;
		r.append( f );
		if( !m->insertRecord( -1, r ) )
		{
			QMessageBox::critical(
						this,
						tr( "New Graph" ),
						tr( "Unable to add a new graph:\n"
							"There's something wrong with your project file." )
			);
			return;
		}

		Graph* g = new Graph( this );
		g->setDefaultAttributes();
		addTab( g, graphName );

		if( !_dataConnected )
		{
			connect( m, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
					 this, SLOT( onDataChanged( QModelIndex, QModelIndex ) )
			);
			_dataConnected = true;
		}
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::closeTab( int index )
	{
		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* model = p->tableModel( "graphSettings" );
		if( !model->removeRow( index ) )
		{
			QMessageBox::critical(
						this,
						tr( "Remove Graph" ),
						tr( "Unable to remove selected graph:\n"
							"There's something wrong with your project file." )
			);
			return;
		}

		delete widget( index );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::renameTab( int index )
	{
		bool ok;
		QString newName = QInputDialog::getText(
							  this,
							  tr( "Change graph name" ),
							  tr( "Type a new graph name" ),
							  QLineEdit::Normal,
							  tabText( index ),
							  &ok
		);

		if( ok && !newName.isEmpty() )
		{
			Project* p = Singleton<Project>::instancePtr();
			QSqlTableModel* model = p->tableModel( "graphSettings" );
			QSqlRecord r = model->record( index );
			r.setValue( "name", newName );
			if( !model->setRecord( index, r ) )
			{
				QMessageBox::critical(
							this,
							tr( "Rename Graph" ),
							tr( "Unable to rename selected graph:\n"
								"There's something wrong with your project file." )
				);
				return;
			}

			setTabText( index, newName );
		}
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::onDataChanged( QModelIndex i, QModelIndex )
	{
		QSqlTableModel* m = static_cast<QSqlTableModel*>( sender() );
		QSqlRecord r = m->record( i.row() );
		Graph* g = graph( i.row() );
		g->setAttributes( r );
	}
}
