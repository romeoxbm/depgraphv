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
 * scanmodepage.h
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
#ifndef SCANMODEPAGE_H
#define SCANMODEPAGE_H

#ifndef SETTINGSPAGE_H
#	include "settingspage.h"
#endif

namespace depgraphV
{
	namespace Ui
	{
		class ScanModePage;
	}

	class ScanModePage : public SettingsPage
	{
		Q_OBJECT

	public:
		/**
		 * @brief \a ScanModePage constructor.
		 * @param w \a MainWindow pointer.
		 * @param parent The parent widget (default \a NULL).
		 */
		explicit ScanModePage( MainWindow* w, SettingsDialog* parent = 0 );

		/**
		 * @brief \a ScanModePage destructor.
		 */
		~ScanModePage();

		virtual QString iconPath() const;

		virtual bool dependsOnGraphs() const { return true; }

		virtual void onGraphChanged();

	protected:
		virtual bool event( QEvent* evt );

	signals:

	public slots:

	private slots:
		void _modifySelection();
		void _updateSelectionCount();
		virtual void onProjectOpened( Project* p );

	private:
		Ui::ScanModePage* _ui;
	};
}

#endif // SCANMODEPAGE_H
