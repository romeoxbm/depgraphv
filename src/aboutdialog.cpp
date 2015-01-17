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
 * aboutdialog.cpp
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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "appconfig.h"
#include "helpers.h"

namespace depgraphV
{
	AboutDialog::AboutDialog( QWidget* parent )
		: QDialog( parent, Qt::WindowTitleHint ),
		_ui( new Ui::AboutDialog ),
		_donateBtnIcoDirty( true )
	{
		_ui->setupUi( this );

#ifndef DEPGRAPHV_USE_OPENGL
		_ui->glSupportValue->setText( "NO" );
#endif
		_ui->projectNameLabel->setText( QApplication::applicationName() );
		_ui->projectVersionLabel->setText( "v" + QApplication::applicationVersion() );

		_setTabStopWidth( _ui->authorsText );
		_ui->authorsText->setPlainText( Helpers::LoadTextFromResources( "AUTHORS" ) );

		_setTabStopWidth( _ui->todoText );
		_ui->todoText->setPlainText( Helpers::LoadTextFromResources( "TODO" ) );

		_setTabStopWidth( _ui->changelogText );
		_ui->changelogText->setPlainText( Helpers::LoadTextFromResources( "Changelog" ) );

		_setTabStopWidth( _ui->licenseText );
		_ui->licenseText->setPlainText( Helpers::LoadTextFromResources( "COPYING" ) );

		_ui->qtVersion->setText( QT_VERSION_STR );

		GVC_t* context = gvContext();
		_ui->graphvizVersion->setText( gvcVersion( context ) );
		gvFreeContext( context );

		connect( Singleton<AppConfig>::instancePtr(), SIGNAL( configRestored() ),
				 this, SLOT( _onConfigRestored() )
		);
	}
	//-------------------------------------------------------------------------
	AboutDialog::~AboutDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	int AboutDialog::exec( bool showDonationsTab )
	{
		if( showDonationsTab )
		{
			int idx = _ui->tabWidget->indexOf( _ui->donationsTab );
			_ui->tabWidget->setCurrentIndex( idx );
		}

		//Update donate button icon (if necessary)
		if( _ui->donateButton->icon().isNull() || _donateBtnIcoDirty )
		{
			AppConfig* c = Singleton<AppConfig>::instancePtr();
			QPixmap p( QString( ":/donateBtns/donate_%1.png" ).arg( c->language() ) );
			_ui->donateButton->setIcon( QIcon( p ) );
			_donateBtnIcoDirty = false;
		}

		return QDialog::exec();
	}
	//-------------------------------------------------------------------------
	bool AboutDialog::event( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
		{
			_ui->retranslateUi( this );
			_donateBtnIcoDirty = true;
		}

		return QDialog::event( evt );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::_onDonateButtonClicked()
	{
		QString itemName = tr( "Donation+to+dep-graphV" );
		QUrl url( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&"
				  "business=guastella.francesco@gmail.com&item_name=" + itemName );
		QDesktopServices::openUrl( url );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::_onDonateCheckBoxClicked()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		c->setShowDonateOnExit( !_ui->donateCheckBox->isChecked() );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::_onConfigRestored()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		_ui->donateCheckBox->setChecked( !c->showDonateOnExit() );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::_setTabStopWidth( QPlainTextEdit* e, int width )
	{
		QFontMetrics metrics( e->font() );
		e->setTabStopWidth( width * metrics.width( ' ' ) );
	}
}
