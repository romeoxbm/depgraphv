/**
 * settingsdialog.cpp
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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "helpers.h"

namespace depgraphV
{
	SettingsDialog::SettingsDialog( QWidget* parent )
		: QDialog( parent ),
		_ui( new Ui::SettingsDialog )
	{
		_ui->setupUi( this );
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
		button->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

		_ui->stackedWidget->addWidget( page );
		_pages.insert( key, page );

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
	void SettingsDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
		{
			_ui->retranslateUi( this );
			_ui->pageLabel->setText( _ui->stackedWidget->currentWidget()->windowTitle() );
			for( int i = 0; i < _ui->listWidget->count(); i++ )
			{
				QListWidgetItem* it = _ui->listWidget->item( i );
				it->setText( _ui->stackedWidget->widget( i )->windowTitle() );
			}
		}

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::changePage( QListWidgetItem* current,
									 QListWidgetItem* previous )
	{
		if( !current )
			current = previous;

		SettingsPage* nextPage = static_cast<SettingsPage*>(
					_ui->stackedWidget->widget( _ui->listWidget->row( current ) )
		);
		SettingsPage* currentPage = static_cast<SettingsPage*>(
					_ui->stackedWidget->widget( _ui->listWidget->row( previous ) )
		);

		bool accept = true;
		pageChanging( currentPage, nextPage, accept );
		if( accept )
		{
			_ui->stackedWidget->setCurrentWidget( nextPage );
			_ui->pageLabel->setText( nextPage->windowTitle() );
			emit pageChanged( nextPage );
		}
		else
			_ui->listWidget->setCurrentItem( previous );
	}
}
