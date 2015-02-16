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
 * settingsdialog.cpp
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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "helpers.h"
#include "mainwindow.h"
#include "project.h"

namespace depgraphV
{
	SettingsDialog::SettingsDialog( MainWindow* w )
		: QDialog( w ),
		_ui( new Ui::SettingsDialog ),
		_currentPage( 0 ),
		_arePagesEnabled( false )
	{
		_ui->setupUi( this );
		enableApplyChanges( false );

		connect( w, SIGNAL( projectOpened( Project* ) ),
				 this, SLOT( _onProjectOpened( Project* ) )
		);
		connect( w, SIGNAL( projectClosed() ),
				 this, SLOT( _onProjectClosed() )
		);
	}
	//-------------------------------------------------------------------------
	SettingsDialog::~SettingsDialog()
	{
		delete _ui;
		_pages.clear();
	}
	//-------------------------------------------------------------------------
	bool SettingsDialog::addPage( const QString& key, SettingsPage* page )
	{
		Q_ASSERT( !key.isEmpty() && page );
		if( _pages.contains( key ) )
			return false;

		QListWidgetItem* button = new QListWidgetItem( _ui->listWidget );
		button->setIcon( QIcon( page->iconPath() ) );
		button->setText( page->windowTitle() );
		button->setTextAlignment( Qt::AlignHCenter );
		_enablePage( button, !page->dependsOnGraphs() );

		_ui->stackedWidget->addWidget( page );
		_pages.insert( key, page );

		if( !_currentPage )
		{
			_currentPage = page;
			lockToCurrentGraph( false );
			_ui->pageLabel->setText( _currentPage->windowTitle() );
		}

		return true;
	}
	//-------------------------------------------------------------------------
	SettingsPage* SettingsDialog::page( const QString& key ) const
	{
		if( _pages.contains( key ) )
			return _pages[ key ];

		return 0;
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::lockToCurrentGraph( bool lockEnabled ) const
	{
		Q_ASSERT( _currentPage );

		bool value = !lockEnabled && _currentPage->dependsOnGraphs();

		_ui->graphName->setEnabled( value );
		_ui->graphNameLabel->setEnabled( value );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::enableApplyChanges( bool enabled ) const
	{
		_ui->buttonBox->button( QDialogButtonBox::Apply )->setEnabled( enabled );
		_ui->buttonBox->button( QDialogButtonBox::Close )->setEnabled( enabled );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
		{
			_ui->retranslateUi( this );
			_ui->pageLabel->setText( _ui->stackedWidget->currentWidget()->windowTitle() );

			//Update listwidget
			for( int i = 0; i < _ui->listWidget->count(); i++ )
			{
				QListWidgetItem* it = _ui->listWidget->item( i );
				it->setText( _ui->stackedWidget->widget( i )->windowTitle() );
			}
		}

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_changePage( QListWidgetItem* current,
									 QListWidgetItem* previous )
	{
		if( !current )
			current = previous;

		SettingsPage* nextPage = static_cast<SettingsPage*>(
					_ui->stackedWidget->widget( _ui->listWidget->row( current ) )
		);

		bool accept = true;
		emit pageChanging( _currentPage, nextPage, accept );
		if( accept )
		{
			_ui->stackedWidget->setCurrentWidget( nextPage );
			_ui->pageLabel->setText( nextPage->windowTitle() );
			_currentPage = nextPage;
			lockToCurrentGraph( false );
			emit pageChanged( nextPage );
		}
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_onProjectOpened( Project* p )
	{
		_ui->graphName->setModelColumn( p->fieldIndex( "name" ) );
		_ui->graphName->setModel( p->tableModel() );

		//TODO What about connections in CustomItemDelegate?
		_blockActivatedSignal( false );
		_blockMapperCurrentIndexChangedSignal( p->mapper(), false );

		connect( p, SIGNAL( graphCountChanged( int ) ),
				 this, SLOT( _onGraphCountChanged( int ) )
		);
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_onProjectClosed()
	{
		_blockActivatedSignal( true );
		_enablePages( false );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_changeCurrentGraph( int index )
	{
		if( sender() == _ui->graphName )
		{
			Project* p = Singleton<Project>::instancePtr();
			QDataWidgetMapper* m = p->mapper();
			_blockMapperCurrentIndexChangedSignal( m, true );

			AppConfig* c = Singleton<AppConfig>::instancePtr();
			if( c->autoApplySettingChanges() && p->hasUnsubmittedChanges() )
			{
				QAbstractButton* b = _ui->buttonBox->button(
							QDialogButtonBox::Apply
				);
				_onButtonClicked( b );
			}

			m->setCurrentIndex( index );

			//Notify current graph has changed
			foreach( SettingsPage* page, _pages )
				page->onGraphChanged();

			_blockMapperCurrentIndexChangedSignal( m, false );
		}
		else
		{
			_blockActivatedSignal( true );
			_ui->graphName->setCurrentIndex( index );
			_blockActivatedSignal( false );
		}
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_onGraphCountChanged( int count )
	{
		_enablePages( count != 0 );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_onButtonClicked( QAbstractButton* button )
	{
		Project* p = Singleton<Project>::instancePtr();

		QDialogButtonBox::ButtonRole r = _ui->buttonBox->buttonRole( button );
		if( r == QDialogButtonBox::ApplyRole || r == QDialogButtonBox::RejectRole )
		{
			if( p )
				p->submitChanges();

			foreach( SettingsPage* page, _pages )
				page->commitChanges();

			if( r == QDialogButtonBox::RejectRole )
				accept();
		}
		else if( r == QDialogButtonBox::DestructiveRole )
		{
			if( p )
				p->revertChanges();

			foreach( SettingsPage* page, _pages )
				page->revertChanges();

			reject();
		}
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_blockActivatedSignal( bool block )
	{
		if( block )
			_ui->graphName->disconnect( this, SLOT( _changeCurrentGraph( int ) ) );
		else
		{
			connect( _ui->graphName, SIGNAL( activated( int ) ),
					 this, SLOT( _changeCurrentGraph( int ) )
			);
		}
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_blockMapperCurrentIndexChangedSignal(
			QDataWidgetMapper* m, bool block )
	{
		if( block )
			m->disconnect( this, SLOT( _changeCurrentGraph( int ) ) );
		else
		{
			connect( m, SIGNAL( currentIndexChanged( int ) ),
					 this, SLOT( _changeCurrentGraph( int ) )
			);
		}
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_enablePages( bool enabled )
	{
		if( !( _arePagesEnabled ^ enabled ) )
			return;

		for( int i = 0; i < _ui->listWidget->count(); i++ )
		{
			SettingsPage* page = static_cast<SettingsPage*>(
						_ui->stackedWidget->widget( i )
			);

			if( page->dependsOnGraphs() )
				_enablePage( _ui->listWidget->item( i ), enabled );
		}

		_arePagesEnabled = enabled;
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::_enablePage( QListWidgetItem* pageButton, bool enabled ) const
	{
		Qt::ItemFlags flags = Qt::ItemIsSelectable;

		if( enabled )
			flags |= Qt::ItemIsEnabled;

		pageButton->setFlags( flags );
	}
}
