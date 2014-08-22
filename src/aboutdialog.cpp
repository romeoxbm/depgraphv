/**
 * aboutdialog.cpp
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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "appconfig.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <gvc.h>

namespace depgraphV
{
	AboutDialog::AboutDialog( QWidget* parent )
		: QDialog( parent, Qt::WindowTitleHint ),
		_ui( new Ui::AboutDialog ),
		_donateBtnIcoDirty( true )
	{
		_ui->setupUi( this );

#ifndef QT_USE_OPENGL
		_ui->glSupportValue->setText( "NO" );
#endif
		_ui->projectNameLabel->setText( QApplication::applicationName() );
		_ui->projectVersionLabel->setText( "v" + QApplication::applicationVersion() );

		_ui->authorsText->setPlainText( _loadTextFromResources( "AUTHORS" ) );
		_ui->todoText->setPlainText( _loadTextFromResources( "TODO" ) );
		_ui->changelogText->setPlainText( _loadTextFromResources( "Changelog" ) );
		_ui->licenseText->setPlainText( _loadTextFromResources( "COPYING" ) );

		_ui->qtVersion->setText( QT_VERSION_STR );

		GVC_t* context = gvContext();
		_ui->graphvizVersion->setText( gvcVersion( context ) );
		gvFreeContext( context );

		connect( Singleton<AppConfig>::instancePtr(), SIGNAL( configRestored() ),
				 this, SLOT( onConfigRestored() )
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
	void AboutDialog::on_donateButton_clicked()
	{
		QString itemName = tr( "Donation+to+dep-graphV" );
		QUrl url( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&"
				  "business=romeoxbm@outlook.it&item_name=" + itemName );
		QDesktopServices::openUrl( url );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::on_donateCheckBox_clicked()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		c->setShowDonateOnExit( !_ui->donateCheckBox->isChecked() );
	}
	//-------------------------------------------------------------------------
	void AboutDialog::onConfigRestored()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		_ui->donateCheckBox->setChecked( !c->showDonateOnExit() );
	}
	//-------------------------------------------------------------------------
	QString AboutDialog::_loadTextFromResources( const QString& filename )
	{
		QFile f( ":/text/" + filename );
		if( !f.open( QFile::ReadOnly | QFile::Text ) )
		{
			qWarning() << qPrintable( tr( "Cannot read " ) + filename );
			return "";
		}

		QTextStream in( &f );
		in.setCodec( "UTF-8" );
		QString ret = in.readAll();
		f.close();

		return ret;
	}
}
