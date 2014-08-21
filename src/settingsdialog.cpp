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
#include "mainwindow.h"

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
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::addPage( const QString& buttonText, SettingsPage* page )
	{
		Q_ASSERT( !buttonText.isEmpty() && page );
		
		QListWidgetItem* button = new QListWidgetItem( _ui->listWidget );
		button->setIcon( QIcon( page->iconPath() ) );
		button->setText( buttonText );
		button->setTextAlignment( Qt::AlignHCenter );
		button->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

		_ui->stackedWidget->addWidget( page );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void SettingsDialog::changePage( QListWidgetItem* current,
									 QListWidgetItem* previous )
	{
		if( !current )
			current = previous;

		_ui->stackedWidget->setCurrentIndex( _ui->listWidget->row( current ) );
		emit pageChanged( current );
	}
}
