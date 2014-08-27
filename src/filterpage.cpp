/**
 * filterpage.cpp
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
#include "filterpage.h"
#include "ui_filterpage.h"
#include "appconfig.h"
#include "buildsettings.h"

namespace depgraphV
{
	FilterPage::FilterPage( QWidget* parent, bool worksWithHeaders )
		: SettingsPage( parent ),
		_ui( new Ui::FilterPage ),
		_worksWithHeaders( worksWithHeaders )
	{
		_ui->setupUi( this );

		QRegExp rx( "(\\s*\\*\\.\\w+\\s*;?)+" );
		QValidator* validator = new QRegExpValidator( rx );
		_ui->customFilters->setValidator( validator );

		AppConfig* c = Singleton<AppConfig>::instancePtr();
		connect( c, SIGNAL( configRestored() ), this, SLOT( onConfigRestored() ) );

		QStringList exts;
		if( _worksWithHeaders )
		{
			exts << "*.h" << "*.hh" << "*.hxx" << "*.hpp" << "*.hp";
			_ui->parseEnabled->setText( tr( "Parse headers" ) );
			_ui->groupBox->setTitle( tr( "Headers filter" ) );

			connect( _ui->parseEnabled, SIGNAL( toggled( bool ) ),
					 c, SLOT( hdr_setParseEnabled( bool ) ) );

			connect( _ui->standardFiltersRadio, SIGNAL( toggled( bool ) ),
					 c, SLOT( hdr_setStandardFiltersEnabled( bool ) ) );

			connect( _ui->standardFilters, SIGNAL( currentIndexChanged( QString ) ),
					 c, SLOT( hdr_setCurrentStandardFilter( QString ) ) );

			connect( _ui->customFilters, SIGNAL( textChanged( QString ) ),
					 c, SLOT( hdr_setCustomFilters( QString ) ) );
		}
		else
		{
			exts << "*.cpp" << "*.cc" << "*.cp" << "*.cxx" << "*.c++" << "*.C";
			_ui->parseEnabled->setText( tr( "Parse sources" ) );
			_ui->groupBox->setTitle( tr( "Sources filter" ) );

			connect( _ui->parseEnabled, SIGNAL( toggled( bool ) ),
					 c, SLOT( src_setParseEnabled( bool ) ) );

			connect( _ui->standardFiltersRadio, SIGNAL( toggled( bool ) ),
					 c, SLOT( src_setStandardFiltersEnabled( bool ) ) );

			connect( _ui->standardFilters, SIGNAL( currentIndexChanged( QString ) ),
					 c, SLOT( src_setCurrentStandardFilter( QString ) ) );

			connect( _ui->customFilters, SIGNAL( textChanged( QString ) ),
					 c, SLOT( src_setCustomFilters( QString ) ) );
		}

		_ui->customFilters->setText( exts.join( "; " ) );
		_ui->standardFilters->addItems( exts );
	}
	//-------------------------------------------------------------------------
	FilterPage::~FilterPage()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	QString FilterPage::iconPath() const
	{
		return _worksWithHeaders
				? ":/settingsDlgIcons/hdr-filter_96x96.png"
				: ":/settingsDlgIcons/src-filter_96x96.png";
	}
	//-------------------------------------------------------------------------
	void FilterPage::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
		{
			_ui->retranslateUi( this );
			if( _worksWithHeaders )
			{
				_ui->parseEnabled->setText( tr( "Parse headers" ) );
				_ui->groupBox->setTitle( tr( "Headers filter" ) );
			}
			else
			{
				_ui->parseEnabled->setText( tr( "Parse sources" ) );
				_ui->groupBox->setTitle( tr( "Sources filter" ) );
			}
		}

		QWidget::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void FilterPage::onConfigRestored()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		if( _worksWithHeaders )
		{
			_ui->parseEnabled->setChecked( c->hdr_parseEnabled() );
			QRadioButton* radio = c->hdr_standardFiltersEnabled() ?
									  _ui->standardFiltersRadio :
									  _ui->customFilterRadio;
			radio->setChecked( true );

#ifdef QT_USE_QT5
			_ui->standardFilters->setCurrentText( c->hdr_currentStandardFilter() );
#else
			int idx = _ui->standardFilters->findText( c->hdr_currentStandardFilter() );
			if( idx != -1 )
				_ui->standardFilters->setCurrentIndex( idx );
#endif
			_ui->customFilters->setText( c->hdr_customFilters() );
		}
		else
		{
			_ui->parseEnabled->setChecked( c->src_parseEnabled() );
			QRadioButton* radio = c->src_standardFiltersEnabled() ?
									  _ui->standardFiltersRadio :
									  _ui->customFilterRadio;
			radio->setChecked( true );

#ifdef QT_USE_QT5
			_ui->standardFilters->setCurrentText( c->src_currentStandardFilter() );
#else
			int idx = _ui->standardFilters->findText( c->src_currentStandardFilter() );
			if( idx != -1 )
				_ui->standardFilters->setCurrentIndex( idx );
#endif
			_ui->customFilters->setText( c->src_customFilters() );
		}
	}
} // end of depgraphV namespace
