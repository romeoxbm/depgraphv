/**
 * appconfig.h
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
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "mainwindow.h"
#include <QObject>
#include <QSettings>

namespace depgraphV
{
	/**
	 * @brief The AppConfig class
	 */
	class AppConfig : public QObject
	{
		Q_OBJECT

	public:
		AppConfig( MainWindow* win, Graph* g );

		/**
		 * @brief save Save currently in use settings.
		 */
		void save();

		/**
		 * @brief restore Restore previously saved settings.
		 */
		void restore();

		/**
		 * @brief saveDefault Save default settings, only the first time this application run, so when there's
		 *					  still no default settings saved.
		 */
		void saveDefault();

		/**
		 * @brief restoreDefault Restore default settings.
		 */
		void restoreDefault();

	private:
		QSettings _settings;
		MainWindow* _win;
		Graph* _graph;

		/**
		 * @brief _doSave It does the save job.
		 * @param def True if saving default settings, false if saving current settings.
		 */
		void _doSave( bool def = false );

		/**
		 * @brief _doRestore It does the restore job.
		 * @param def True if saving default settings, false if saving current settings.
		 */
		void _doRestore( bool def = false );
	};
}

#endif // APPCONFIG_H
