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
 * customtabwidget.h
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
#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#ifndef MAINWINDOW_H
#	include "mainwindow.h"
#endif

#ifndef PROJECT_H
#	include "project.h"
#endif

namespace depgraphV
{
	class CustomTabWidget : public QTabWidget
	{
		Q_OBJECT

	public:
		explicit CustomTabWidget( QWidget* parent = 0 );

		void setMainWindow( MainWindow* w );
		void setCurrentTabUnclosable();
		void resetUnclosableTab();

	protected:
		bool event( QEvent* evt );
		bool eventFilter( QObject* o, QEvent* evt );

	private slots:
		void _newGraph( const QString& newName, Graph* g );
		void _renameTab( int index );

		void _closeTab( int index );
		void _closeAllButCurrentTab();
		void _closeAllTabs();

		void _onProjectOpened( Project* );
		void _onProjectClosed();

	private:
		MainWindow* _mainW;
		bool _disableCloseTabQuestion;
		int _unclosableTabIndex;

		void _retranslate();
	};
}
#endif // CUSTOMTABWIDGET_H
