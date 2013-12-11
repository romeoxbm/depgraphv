/**
 * aboutdialog.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 Francesco Guastella aka romeoxbm
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
#include "mainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <qglobal.h>
#include <gvc.h>

namespace depgraphV
{
	AboutDialog::AboutDialog( const QString& appName, const QString& appVersion, QWidget* parent )
		: QDialog( parent, Qt::WindowTitleHint ),
		  _ui( new Ui::AboutDialog )
	{
		_ui->setupUi( this );

		MainWindow* wnd = static_cast<MainWindow*>( this->parentWidget() );
		_ui->donateCheckBox->setChecked( !wnd->showDonateOnExit() );

		_ui->projectNameLabel->setText( appName );
		_ui->projectVersionLabel->setText( "v" + appVersion );

		_ui->authorsText->setPlainText( _loadTextFromResources( "AUTHORS" ) );
		_ui->todoText->setPlainText( _loadTextFromResources( "TODO" ) );
		_ui->changelogText->setPlainText( _loadTextFromResources( "Changelog" ) );
		_ui->licenseText->setPlainText( _loadTextFromResources( "COPYING" ) );

		_ui->qtVersion->setText( QT_VERSION_STR );

		GVC_t* context = gvContext();
		_ui->graphvizVersion->setText( gvcVersion( context ) );
		gvFreeContext( context );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	AboutDialog::~AboutDialog()
	{
		delete _ui;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	int AboutDialog::exec( bool showDonationsTab )
	{
		if( showDonationsTab )
			_ui->tabWidget->setCurrentIndex( _ui->tabWidget->indexOf( _ui->tab_7 ) );

		//Update the donate button icon
		MainWindow* wnd = static_cast<MainWindow*>( this->parentWidget() );
		QPixmap p( QString( ":/donateBtns/donate_%1.gif" ).arg( wnd->currentLocale() ) );
		_ui->donateButton->setIcon( QIcon( p ) );

		return QDialog::exec();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AboutDialog::translateUi()
	{
		_ui->retranslateUi( this );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AboutDialog::on_donateButton_clicked()
	{
		QString itemName = tr( "Donation+to+dep-graphV" );
		QUrl url( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=romeo_bm@libero.it&item_name=" + itemName );
		QDesktopServices::openUrl( url );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void AboutDialog::on_donateCheckBox_clicked()
	{
		MainWindow* wnd = static_cast<MainWindow*>( this->parentWidget() );
		wnd->setShowDonateOnExit( !_ui->donateCheckBox->isChecked() );
	}
	//--------------------------------------------------------------------------------------------------------------------------
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
