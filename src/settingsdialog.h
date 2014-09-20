/**
 * settingsdialog.h
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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settingspage.h"
#include <QDialog>
#include <QListWidgetItem>
#include <QEvent>

namespace depgraphV
{
	namespace Ui
	{
		class SettingsDialog;
	}

	class SettingsDialog : public QDialog
	{
		Q_OBJECT

	public:
		/**
		 * @brief SettingsDialog constructor.
		 * @param parent The parent widget (default NULL)
		 */
		explicit SettingsDialog( QWidget* parent = 0 );

		/**
		 * @brief SettingsDialog destructor.
		 */
		~SettingsDialog();

		bool addPage( const QString& key, SettingsPage* page );
		SettingsPage* page( const QString& key ) const;

	protected:
		virtual void changeEvent( QEvent* );

	signals:
		void pageChanging( SettingsPage* currentPage, SettingsPage* nextPage, bool& accept );
		void pageChanged( SettingsPage* nextPage );

	private slots:
		void changePage( QListWidgetItem* current, QListWidgetItem* previous );

	private:
		Ui::SettingsDialog* _ui;
		QMap<QString, SettingsPage*> _pages;
	};
}

#endif // SETTINGSDIALOG_H
