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
 * selectfilesdialog.h
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
#ifndef SELECTFILESDIALOG_H
#define SELECTFILESDIALOG_H

#ifndef FOLDERSMODEL_H
#	include "foldersmodel.h"
#endif

#ifndef PROJECT_H
#	include "project.h"
#endif

namespace depgraphV
{
	namespace Ui
	{
		class SelectFilesDialog;
	}

	//Forward declaration
	class MainWindow;

	class SelectFilesDialog : public QDialog, public Singleton<SelectFilesDialog>
	{
		Q_OBJECT
		Q_PROPERTY( QByteArray graphModel READ graphModel WRITE setGraphModel USER true )

	public:
		/**
		 * @brief \a SelectFilesDialog constructor
		 * @param parent The parent \a MainWindow.
		 */
		explicit SelectFilesDialog( MainWindow* parent );

		/**
		 * @brief \a SelectFilesDialog destructor.
		 */
		~SelectFilesDialog();

		int exec();

		QByteArray graphModel();
		void setGraphModel( const QByteArray& );

		/**
		 * @brief \a setGraphIndex set the graph index we're are working on
		 * when loading/saving a project file.
		 * @param index The index of the graph.
		 */
		void setGraphIndex( int index ) { _graphIndex = index; }

	signals:
		void selectionChanged();

	protected:
		virtual void changeEvent( QEvent* );

	private slots:
		void _onClose( int );
		void _onProjectOpened( Project* );
		void _onProjectClosed();

	private:
		Ui::SelectFilesDialog* _ui;
		FoldersModel* _model;
		int _graphIndex;

		void _getModelFromGraphIndex();
		void _connect();
		void _disconnect();
	};
}

#endif // SELECTFILESDIALOG_H
