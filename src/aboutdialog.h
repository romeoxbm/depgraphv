/**
 * aboutdialog.h
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
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace depgraphV
{
	namespace Ui
	{
		class AboutDialog;
	}

	class AboutDialog : public QDialog
	{
		Q_OBJECT

	public:
		/**
		 * @brief AboutDialog constructor.
		 * @param appName This application name.
		 * @param appVersion This application version string.
		 * @param parent The parent widget (default NULL)
		 */
		AboutDialog( const QString& appName, const QString& appVersion, QWidget* parent = 0 );

		/**
		 * AboutDialog destructor.
		 */
		~AboutDialog();

		/**
		 * @brief Show this dialog as a modal dialog.
		 * @param showDonationsTab If true, the donation tab page will be set as the first active page.
		 */
		int exec( bool showDonationsTab );

		/**
		 * @brief Translate this dialog.
		 */
		void translateUi();

	signals:

	public slots:

	private slots:
		void on_donateButton_clicked();
		void on_donateCheckBox_clicked();

	private:
		Ui::AboutDialog* _ui;
		QString _loadTextFromResources( const QString& filename );

	};
}

#endif // ABOUTDIALOG_H
