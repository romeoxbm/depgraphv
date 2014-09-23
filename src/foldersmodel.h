/**
 * foldersmodel.h
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
#ifndef FOLDERSMODEL_H
#define FOLDERSMODEL_H

#include <QTreeView>
#include "filesmodel.h"

namespace depgraphV
{
	class FoldersModel : public CheckableFileSystemModel
	{
		Q_OBJECT
		Q_PROPERTY( QString selectedFolderPath READ selectedFolderPath WRITE setSelectedFolderPath )

	public:
		explicit FoldersModel( QObject* parent = 0 );

		virtual QAbstractItemView* view() const { return _treeView; }

		inline const QStringList& checkedFiles() const { return _filesModel->checkedItems(); }

		void initialize( QTreeView* view, QListView* filesView, const QString& rootPath );

		const QString& selectedFolderPath() const { return _selectedFolderPath; }

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
		bool setData( const QModelIndex& i, const QVariant& value, int role );

		void setFileNameFilters( const QStringList& filters );

		virtual QList<const char*> propList() const;

		virtual void commitChanges();
		virtual void revertChanges();

	protected:
		virtual bool isCheckable( const QModelIndex& i, int role ) const;

	public slots:
		virtual void clearSelection();
		void changeRoot( const QString& newRoot );
		void showHiddenFolders( bool show );

		void setSelectedFolderPath( const QString& value );

	signals:
		void folderCheckStateChanged( QModelIndex, Qt::CheckState );

	private slots:
		void _updateSelection( const QModelIndex& current, const QModelIndex& );
		void _itemExpandedCollapsed( const QModelIndex& );
		void _on_changeRoot_triggered();
		void _on_expandAll();
		void _on_collapseAll();

	private:
		QTreeView* _treeView;
		FilesModel* _filesModel;
		QAction* _changeRoot;
		QAction* _showHiddenFolders;
		QAction* _expandAll;
		QAction* _collapseAll;

		QString _selectedFolderPath;

		void _createContextMenu();
		void _connectSignalsToSlots();
	};
}

#endif // FOLDERSMODEL_H
