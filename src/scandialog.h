/**
 * scandialog.h
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
#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QTime>

namespace depgraphV
{
	namespace Ui
	{
		class ScanDialog;
	}

	class MainWindow;
	class BackgroundWorker;

	class ScanDialog : public QDialog
	{
		Q_OBJECT

	public:
		/**
		 * @brief ScanDialog constructor.
		 * @param parent The parent widget (default NULL)
		 */
		ScanDialog( MainWindow* win );

		/**
		 * ScanDialog destructor.
		 */
		~ScanDialog();

	signals:

	public slots:

	private slots:
		void on_directoryFound();
		void on_fileFound( QString file );
		void on_scanFinished();
		void on_graphCreated();
		void on_fileAnalized( QString file );

		void on_layoutFinished();
		void on_fail( QString message );

		void on_timeout();
		void on_toggleDetails();

		void on_abort_clicked();

	private:
		Ui::ScanDialog* _ui;
		BackgroundWorker* _worker;
		bool _detailsShown;
		bool _failed;

		uint _foldersCount;
		uint _filesCount;
		uint _analizedFilesCount;

		QTimer* _timer;
		QTime _elapsedTime;

		virtual void showEvent( QShowEvent* evt );
		void _freeWorker();
	};
}

#endif // SCANDIALOG_H
