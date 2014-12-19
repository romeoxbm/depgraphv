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
#include "depgraphv_pch.h"
#include "customtabwidget.h"
#include "helpers.h"

namespace depgraphV
{
	CustomTabWidget::CustomTabWidget( QWidget* parent )
		: QTabWidget( parent ),
		  _mainW( 0 ),
		  _disableCloseTabQuestion( false )
	{
		tabBar()->installEventFilter( this );
		tabBar()->setContextMenuPolicy( Qt::ActionsContextMenu );

		QAction* closeButCurrent = new QAction( this );
		tabBar()->addAction( closeButCurrent );

		QAction* closeAll = new QAction( this );
		tabBar()->addAction( closeAll );

		_retranslate();

		connect( closeButCurrent, SIGNAL( triggered() ),
				 this, SLOT( _closeAllButCurrentTab() )
		);
		connect( closeAll, SIGNAL( triggered() ),
				 this, SLOT( _closeAllTabs() )
		);
		connect( this, SIGNAL( tabCloseRequested( int ) ),
				 this, SLOT( _closeTab( int ) )
		);
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::setMainWindow( MainWindow* w )
	{
		Q_ASSERT( w && !_mainW );
		_mainW = w;

		connect( _mainW, SIGNAL( projectOpened() ),
				 this, SLOT( _onProjectOpened() )
		);
		connect( _mainW, SIGNAL( projectClosed() ),
				 this, SLOT( _onProjectClosed() )
		);
	}
	//-------------------------------------------------------------------------
	bool CustomTabWidget::event( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_retranslate();

		return QTabWidget::event( evt );
	}
	//-------------------------------------------------------------------------
	bool CustomTabWidget::eventFilter( QObject* o, QEvent* evt )
	{
		bool result = QTabWidget::eventFilter( o, evt );

		if( o == tabBar() && evt )
		{
			if( evt->type() == QEvent::MouseButtonDblClick )
			{
				QMouseEvent* mouseEvt = static_cast<QMouseEvent*>( evt );
				if( mouseEvt->buttons() & Qt::LeftButton )
				{
					int index = tabBar()->tabAt( mouseEvt->pos() );
					if( index == -1 )
						return result;

					_renameTab( index );
					return true;
				}
			}

			else if( evt->type() == QEvent::ContextMenu )
				tabBar()->actions()[ 0 ]->setEnabled( tabBar()->count() > 1 );
		}

		return result;
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_closeAllTabs()
	{
		/*QAction* a = qobject_cast<QAction*>( sender() );
		if( a )
		{*/
			AppConfig* c = Singleton<AppConfig>::instancePtr();
			if( !_disableCloseTabQuestion && c->warnOnGraphRemoval() )
			{
				QMessageBox::StandardButton answer = QMessageBox::question(
					this,
					tr( "Close all tabs" ),
					tr( "Are you sure?" ),
					QMessageBox::Yes | QMessageBox::No,
					QMessageBox::No
				);

				if( answer == QMessageBox::No )
					return;
			}

			_disableCloseTabQuestion = true;

			while( tabBar()->count() > 0 )
				_closeTab( 0 );

			_disableCloseTabQuestion = false;
		/*}
		else
		{
			while( tabBar()->count() > 0 )
			{
				delete widget( 0 );
				emit graphRemoved( 0 );
			}
		}

		emit graphCountChanged( 0 );*/
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_newGraph( const QString& newName, Graph* g )
	{
		//Graph* g = new Graph( count(), this );
		addTab( g, newName );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_closeTab( int index )
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		if( !_disableCloseTabQuestion && c->warnOnGraphRemoval() )
		{
			QMessageBox::StandardButton answer = QMessageBox::question(
						this,
						tr( "Remove Graph" ),
						tr( "Closing this tab, you will also remove this graph from your project\n"
							"Would you like to continue?" ),
						QMessageBox::Yes | QMessageBox::No,
						QMessageBox::No
			);

			if( answer == QMessageBox::No )
				return;
		}

		delete widget( index );
		Project* p = Singleton<Project>::instancePtr();
		if( p )
			p->removeGraph( index );

		//TODO Remove following code
		/*if( !( _model && _model->removeRow( p->indexOfRecord( _model, index ) ) ) )
		{
			p->showLastError( tr( "Remove Graph" ), _model );
			return;
		}*/
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_closeAllButCurrentTab()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		if( c->warnOnGraphRemoval() )
		{
			QMessageBox::StandardButton answer = QMessageBox::question(
				this,
				tr( "Close all tabs but current one" ),
				tr( "Are you sure?" ),
				QMessageBox::Yes | QMessageBox::No,
				QMessageBox::No
			);

			if( answer == QMessageBox::No )
				return;
		}

		_disableCloseTabQuestion = true;

		//Close all tabs *after* current selected
		for( int i = tabBar()->count() - 1; i > currentIndex(); i-- )
			_closeTab( i );

		//Now remove all tabs *before*
		while( tabBar()->count() > 1 )
			_closeTab( 0 );

		_disableCloseTabQuestion = false;
		//emit graphCountChanged( 1 );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_renameTab( int index )
	{
		bool ok;
		QString oldName = tabText( index );
		QString newName = QInputDialog::getText(
							  this,
							  tr( "Rename Graph \"%1\"" ).arg( oldName ),
							  tr( "Type a new graph name" ),
							  QLineEdit::Normal,
							  oldName,
							  &ok
		);

		if( ok && !newName.isEmpty() )
		{
			Project* p = Singleton<Project>::instancePtr();
			p->renameGraph( index, newName );
			setTabText( index, newName );
		}
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_onProjectOpened()
	{
		Project* p = Singleton<Project>::instancePtr();
		QDataWidgetMapper* m = p->createOrRetrieveMapper( "tabMapper", true );

		connect( p, SIGNAL( graphCreated( QString, Graph* ) ),
				 this, SLOT( _newGraph( QString, Graph* ) )
		);

		//TODO What about the connections in CustomItemDelegate?
		connect( this, SIGNAL( currentChanged( int ) ),
				 m, SLOT( setCurrentIndex( int ) )
		);

		//TODO
		//p->addMapping( this, "graphSettings" );
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_onProjectClosed()
	{
		_disableCloseTabQuestion = true;
		_closeAllTabs();
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::_retranslate()
	{
		QList<QAction*> l = tabBar()->actions();
		l[ 0 ]->setText( tr( "Close all tabs but current selected" ) );
		l[ 1 ]->setText( tr( "Close all tabs" ) );
	}
}
