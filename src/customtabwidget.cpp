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
#include <QMetaEnum>
#include <QAction>
#ifndef QT_USE_QT5
#	include <QTabBar>
#endif

#define C_STR( s ) s.toStdString().c_str()

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

		tabBar()->setContextMenuPolicy( Qt::ActionsContextMenu );
		QAction* closeButCurrent = new QAction( this );
		closeButCurrent->setText( tr( "Close all but current selected" ) );
		tabBar()->addAction( closeButCurrent );

		connect( closeButCurrent, SIGNAL( triggered() ),
				 this, SLOT( closeAllButCurrentTab() )
		);
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
	bool CustomTabWidget::loadTabs()
	{
		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* m = p->tableModel( "graphSettings" );
		if( !m )
			return false;

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

		return true;
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::closeAllTabs()
	{
		while( tabBar()->count() > 0 )
			delete widget( 0 );
	}
	//-------------------------------------------------------------------------
#ifndef QT_USE_QT5
	bool CustomTabWidget::eventFilter( QObject* o, QEvent* evt )
	{
		bool result = QTabWidget::eventFilter( o, evt );

		if( o == tabBar() && evt->type() == QEvent::MouseButtonDblClick )
		{
			QMouseEvent* mouseEvt = static_cast<QMouseEvent*>( evt );
			if( mouseEvt->buttons() & Qt::LeftButton )
			{
				int index = tabBar()->tabAt( mouseEvt->pos() );
				if( index == -1 )
					return result;

				renameTab( index );
				return true;
			}
		}

		return result;
	}
#endif
	//-------------------------------------------------------------------------
	void CustomTabWidget::newGraph()
	{
		QString graphName = "New Graph " + QString::number( ++_newGraphCount );

		//The new tab must be created before inserting the new row
		//in the project file, or connected dataChanged slots will crash
		Graph* g = new Graph( this );
		g->setDefaultAttributes();
		addTab( g, graphName );

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
		if( !model )
			return;

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

		//QSqlTableModel does no longer implicitly select() once a row has been removed
		if( model->select() )
			delete widget( index );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::closeAllButCurrentTab()
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr( "Close all tabs but selected" ),
			tr( "Are you sure?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		if( answer == QMessageBox::No )
			return;

		//Close all tabs *after* current selected
		for( int i = tabBar()->count() - 1; i > currentIndex(); i-- )
			closeTab( i );

		//Now remove all tabs *before*
		while( tabBar()->count() > 1 )
			closeTab( 0 );
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
		QString fieldName = r.fieldName( i.column() );

		if( fieldName == "RendererType" )
		{
			const QMetaObject& mo = Graph::staticMetaObject;
			int enumIndex = mo.indexOfEnumerator( "RendererType" );
			QMetaEnum e = mo.enumerator( enumIndex );
			QString valStr = r.value( "RendererType" ).toString();
			g->setRenderer( static_cast<Graph::RendererType>( e.keyToValue( C_STR( valStr ) ) ) );
		}
		else if( fieldName == "highQualityAA" )
			g->setHighQualityAntialiasing( r.value( "highQualityAA" ).toBool() );

		else
			g->setAttributes( r );
	}
}
