/**
 * qtimerlabel.cpp
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
#include "qtimerlabel.h"

namespace depgraphV
{
	QTimerLabel::QTimerLabel( QWidget* parent )
		: QLabel( parent ),
		_timer( new QTimer( this ) )
	{
		_updateText( true );

		//Connect signals/slots
		connect( _timer, SIGNAL( timeout() ), this, SLOT( onTimeout() ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	QTimerLabel::~QTimerLabel()
	{
		delete _timer;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void QTimerLabel::startTimer()
	{
		_updateText( true );
		_timer->start( 1000 );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void QTimerLabel::stopTimer()
	{
		_timer->stop();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void QTimerLabel::resetTimer()
	{
		this->stopTimer();
		this->startTimer();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void QTimerLabel::onTimeout()
	{
		if( !_timer->isActive() )
			return;

		_seconds++;

		if( _seconds >= 60 )
		{
			_seconds = 0;
			_minutes++;
		}

		if( _minutes >= 60 )
		{
			_minutes = 0;
			_hours++;
		}

		_updateText( false );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void QTimerLabel::_updateText( bool reset )
	{
		if( reset )
		{
			_seconds = 0;
			_minutes = 0;
			_hours = 0;
		}

		this->setText( QString( "%1:%2:%3" )
			.arg( _hours, 2, 10, QLatin1Char( '0' ) )
			.arg( _minutes, 2, 10, QLatin1Char( '0' ) )
			.arg( _seconds, 2, 10, QLatin1Char( '0' ) )
		);
	}
}
