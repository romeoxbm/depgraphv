/**
 * backgroundworker.h
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
#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QThread>
#include <QPair>

namespace depgraphV
{
	class MainWindow;
	
	class BackgroundWorker : public QThread
	{
		Q_OBJECT
	public:
		BackgroundWorker( MainWindow* win, QWidget* parent = 0 );

	public slots:
		void on_abort();

	signals:
        void folderFound( QString folder );
		void fileFound( QString file );
		void scanFinished();
		void graphCreated();

		void fileAnalized( QString file );
		void fail( QString message );

	private:
		MainWindow* _win;
		QList<QPair<QString, QString> > _filesFound;
		bool _abortQueued;

		virtual void run();

		void _scanFolder( const QString& root );
		void _createGraph();
		bool _checkForAbortOnRun();
	};
}

#endif // BACKGROUNDWORKER_H
