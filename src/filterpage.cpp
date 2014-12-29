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
#include "depgraphv_pch.h"
#include "filterpage.h"
#include "ui_filterpage.h"
#include "project.h"

namespace depgraphV
{
	FilterPage::FilterPage( MainWindow* w, SettingsDialog* parent, bool worksWithHeaders )
		: SettingsPage( w, parent ),
		_ui( new Ui::FilterPage ),
		_worksWithHeaders( worksWithHeaders )
	{
		_ui->setupUi( this );

		QRegExp rx( "\\*\\.\\*|(\\s*\\*\\.\\w+\\s*;?)+" );
		_ui->customFilters->setValidator( new QRegExpValidator( rx, this ) );

		if( _worksWithHeaders )
		{
			_defaultCustomExts << "*.h" << "*.hh" << "*.hxx" << "*.hpp" << "*.hp";
			_ui->parseEnabled->setText( tr( "Parse headers" ) );
			setWindowTitle( tr( "Headers filter" ) );

			/*connect( c, SIGNAL( headerNameFiltersChanging( bool& ) ),
					 this, SLOT( onFiltersChanging( bool& ) )
			);*/
		}
		else
		{
			_defaultCustomExts << "*.cpp" << "*.cc" << "*.cp" << "*.cxx" << "*.c++" << "*.C";
			_ui->parseEnabled->setText( tr( "Parse sources" ) );
			setWindowTitle( tr( "Sources filter" ) );

			/*connect( c, SIGNAL( sourceNameFiltersChanging( bool& ) ),
					 this, SLOT( onFiltersChanging( bool& ) )
			);*/
		}

		connect( parent, SIGNAL( pageChanging( SettingsPage*, SettingsPage*, bool& ) ),
				 this, SLOT( _onPageChanging( SettingsPage*, SettingsPage*, bool& ) ) );

		_ui->standardFilters->addItems( _defaultCustomExts );
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
				setWindowTitle( tr( "Headers filter" ) );
			}
			else
			{
				_ui->parseEnabled->setText( tr( "Parse sources" ) );
				setWindowTitle( tr( "Sources filter" ) );
			}
		}

		QWidget::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void FilterPage::_onCustomFilterTextChanged( QString )
	{
		Qt::GlobalColor c = _ui->customFilters->hasAcceptableInput() ? Qt::black : Qt::red;
		QPalette p = _ui->customFilters->palette();
		if( p.color( QPalette::Text ) != c )
		{
			p.setColor( QPalette::Text, c );
			_ui->customFilters->setPalette( p );
		}
	}
	//-------------------------------------------------------------------------
	void FilterPage::_onPageChanging( SettingsPage* currentPage,
									 SettingsPage*, bool& accept )
	{
		if( currentPage == this && !_ui->customFilters->hasAcceptableInput() )
		{
			QMessageBox::StandardButton answer = QMessageBox::warning(
				this,
				tr( "Invalid name filters specified!" ),
				tr( "Would you like to discard last changes?" ),
				QMessageBox::Yes | QMessageBox::No,
				QMessageBox::No
			);

			if( answer == QMessageBox::No )
				accept = false;
		}
	}
	//-------------------------------------------------------------------------
	void FilterPage::_onFiltersChanging( bool& accept )
	{
		Project* p = Singleton<Project>::instancePtr();
		Q_ASSERT( p );

		//TODO Check also selected files count > 0
		if( p->currentValue( "scanByFolders" ).toBool() )
			return;

		QMessageBox::StandardButton answer = QMessageBox::warning(
			this,
			tr( "Filters" ),
			tr( "By changing filters, you invalidate your current files selection"
				".\nWould you like to continue?" ),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No
		);

		accept = answer != QMessageBox::No;
	}
	//-------------------------------------------------------------------------
	void FilterPage::onProjectOpened( Project* p )
	{
		SettingsPage::onProjectOpened( p );
		QString prefix = _worksWithHeaders ? "hdr_" : "src_";

		p->addMapping( _ui->parseEnabled, "parseEnabled", prefix, true );

		QRadioButton* radios[] = { _ui->standardFiltersRadio, _ui-> customFilterRadio };
		p->addMapping( radios, "standardFiltersEnabled", prefix );

		p->addMapping( _ui->standardFilters, "currentStandardFilter", prefix, _defaultCustomExts[ 0 ] );
		p->addMapping( _ui->customFilters, "customFilters", prefix, _defaultCustomExts.join( "; " ) );
	}
	//-------------------------------------------------------------------------
	void FilterPage::onProjectClosed()
	{
		//TODO
		SettingsPage::onProjectClosed();
	}
} // end of depgraphV namespace
