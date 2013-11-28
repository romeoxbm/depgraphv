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
#include <QDebug>
#include <qglobal.h>
#include <gvc.h>

namespace depgraphV
{
	AboutDialog::AboutDialog( const QString& appName, const QString& appVersion, QWidget* parent )
		: QDialog( parent ),
		  _ui( new Ui::AboutDialog )
	{
		_ui->setupUi( this );
		_ui->projectNameLabel->setText( appName );
		_ui->projectVersionLabel->setText( "v" + appVersion );

		_ui->authorsText->setPlainText( _loadTextFromResources( "AUTHORS" ) );
		_ui->todoText->setPlainText( _loadTextFromResources( "TODO" ) );
		_ui->changelogText->setPlainText( _loadTextFromResources( "Changelog" ) );
		_ui->licenseText->setPlainText( _loadTextFromResources( "COPYING" ) );

		_ui->qtVersion->setText( QT_VERSION_STR );

		GVC_t* context = gvContext();
		_ui->graphvizVersion->setText( gvcVersion( context ) );
		delete context;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	AboutDialog::~AboutDialog()
	{
		delete _ui;
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
