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
 * foldersmodel.h
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
#ifndef FOLDERSMODEL_H
#define FOLDERSMODEL_H

#ifndef FILESMODEL_H
#	include "filesmodel.h"
#endif

namespace depgraphV
{
	class FoldersModel : public CheckableFileSystemModel
	{
		Q_OBJECT

	public:
		explicit FoldersModel( QObject* parent = 0 );
		void initialize();

		virtual QAbstractItemView* view() const { return _treeView; }
		virtual void setView( QAbstractItemView* view );

		void setFilesView( QAbstractItemView* view );

		FilesModel* filesModel() const { return _filesModel; }

		inline QStringList checkedFiles() const { return _filesModel->checkedFiles(); }

		/**
		 * @brief Returns true if parent has any children and haven't
		 *        the Qt::ItemNeverHasChildren flag set;
		 *        otherwise returns false.
		 *
		 * @remarks Reimplemented to avoid empty directories to be collapsables
		 *          and to implement the \c Qt::ItemNeverHasChildren flag.
		 * @see     rowCount()
		 * @see     child()
		 */
		bool hasChildren( const QModelIndex& parent ) const;

		QVariant data( const QModelIndex& i, int role ) const;

		Qt::ItemFlags flags( const QModelIndex& index ) const;

		//void setFileNameFilters( const QStringList& filters );

		friend QDataStream& operator << ( QDataStream& out, const FoldersModel* object );
		friend QDataStream& operator >> ( QDataStream& in, FoldersModel* object );

	protected:
		virtual bool eventFilter( QObject* obj, QEvent* evt );
		virtual bool isCheckable( const QModelIndex& i, int role ) const;
		virtual bool setDataImpl( const QString& path, Qt::CheckState v );

	public slots:
		//virtual void clearSelection();
		void changeRoot();
		void showHiddenFolders( bool show );
		void expandAll();
		void collapseAll();

	private slots:
		void _updateSelection( const QModelIndex& current, const QModelIndex& );
		void _itemExpandedCollapsed( const QModelIndex& );
		void _onFilesModelDataChanged( QModelIndex, QModelIndex );

	private:
		QTreeView* _treeView;
		FilesModel* _filesModel;

		QString _rootPath;
		QString _lastSelectedPath;
		QMap<QString, Qt::CheckState> _checkedFolders;

		void _connectView();
		void _disconnectView();
		bool _viewConnected;
	};
}

#endif // FOLDERSMODEL_H
