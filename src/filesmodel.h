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
 * filesmodel.h
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
#ifndef FILESMODEL_H
#define FILESMODEL_H

#ifndef CHECKABLEFILESYSTEMMODEL_H
#	include "checkablefilesystemmodel.h"
#endif

namespace depgraphV
{
	class FoldersModel;

	class FilesModel : public CheckableFileSystemModel
	{
		Q_OBJECT

	public:
		enum FileGroup
		{
			Hdr,
			Src,
			All
		};

		explicit FilesModel( FoldersModel* fmodel, QObject* parent = 0 );
		void initialize();

		virtual QAbstractItemView* view() const { return _listView; }
		virtual void setView( QAbstractItemView* view );

		int selectedCount( const QString& rootPath = "", FileGroup g = FilesModel::All ) const;
		QStringList checkedFiles() const;

		QVariant data( const QModelIndex& i, int role ) const;

		//virtual void commitChanges();
		//virtual void revertChanges();

		bool belongsToFileGroup( const QString& filePath, FileGroup v ) const;
		bool belongsToFileGroup( const QModelIndex& i, FileGroup v ) const;

		void setListeningFoldersModelDataChanged( bool value ) const;

		friend QDataStream& operator << ( QDataStream& out, const FilesModel& object );
		friend QDataStream& operator >> ( QDataStream& in, FilesModel& object );

	protected:
		virtual bool eventFilter( QObject* obj, QEvent* evt );
		virtual bool isCheckable( const QModelIndex& i, int role ) const;
		virtual bool setDataImpl( const QString& path, Qt::CheckState v );

	public slots:
		virtual void clearSelection();
		void contextMenuActionTriggered();
		void showAllFiles();

	private slots:
		void onFoldersModelDataChanged( QModelIndex, QModelIndex );

	private:
		QListView* _listView;
		FoldersModel* _foldersModel;

		//Memento<QMap<QString, QStringList*> > _checkedFiles;
		QMap<QString, QStringList*> _checkedFiles;

		void _updateEnabledFlags();
		int _filesCount( const QModelIndex& parent, FileGroup g = FilesModel::All ) const;
	};
}

#endif // FILESMODEL_H
