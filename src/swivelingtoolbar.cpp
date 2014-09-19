/**
 * swivelingtoolbar.cpp
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
#include "swivelingtoolbar.h"

#include <QAction>

namespace depgraphV
{
	SwivelingToolBar::SwivelingToolBar( QWidget* parent )
		: QToolBar( parent )
	{
		QAction* a = new QAction( this );
		a->setIcon( QIcon( ":/toolBarIcons/orientation_48x48.png" ) );
		a->setText( tr( "Orientation" ) );
		a->setToolTip( tr( "Change orientation of this tool bar" ) );
		a->setVisible( false );

		connect( a, SIGNAL( triggered() ), this, SLOT( changeToolbarOrientation() ) );
		connect( this, SIGNAL( topLevelChanged( bool ) ), a, SLOT( setVisible( bool ) ) );
		addAction( a );

		QAction* sep = addSeparator();
		sep->setVisible( false );
		connect( this, SIGNAL( topLevelChanged( bool ) ), sep, SLOT( setVisible( bool ) ) );
	}
	//-------------------------------------------------------------------------
	void SwivelingToolBar::changeToolbarOrientation()
	{
		Qt::Orientation o = static_cast<Qt::Orientation>( ( orientation() % 2 ) + 1 );
		setOrientation( o );

		//Adjust toolbar size
		bool h = o == Qt::Horizontal;

		resize(
				h ? sizeHint().width() : minimumWidth(),
				h ? minimumHeight() : sizeHint().height()
		);
	}
}
