/**
 * handlerootsdialog.h
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
#ifndef HANDLEROOTSDIALOG_H
#define HANDLEROOTSDIALOG_H

#include <QDialog>
#include "appconfig.h"

namespace depgraphV
{
	namespace Ui
	{
		class HandleRootsDialog;
	}

	class HandleRootsDialog : public QDialog, public Singleton<HandleRootsDialog>
	{
		Q_OBJECT

	public:
		/**
		 * @brief HandleRootsDialog constructor.
		 * @param parent The parent widget (default NULL)
		 */
		HandleRootsDialog( QWidget* parent = 0 );

		/**
		 * @brief HandleRootsDialog destructor.
		 */
		~HandleRootsDialog();

	protected:
		virtual bool event( QEvent* evt );
		virtual bool eventFilter( QObject* obj, QEvent* evt );

	signals:

	public slots:

	private slots:
		void _invertSelection();
		void _updateEnabledFlags();
		void on_actionAdd_triggered();
		void on_actionRemove_Selection_triggered();
		void onClose( int );
		void onConfigRestored();

	private:
		Ui::HandleRootsDialog* _ui;
		QStringList* _selectedFolders;
	};
}

#endif // HANDLEROOTSDIALOG_H
