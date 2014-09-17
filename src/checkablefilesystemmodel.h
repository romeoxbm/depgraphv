/**
 * checkablefilesystemmodel.h
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
#ifndef CHECKABLEFILESYSTEMMODEL_H
#define CHECKABLEFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QAbstractItemView>
#include <functional>

#include "iserializableobject.h"
#include "memento.h"

namespace depgraphV
{
	class CheckableFileSystemModel : public QFileSystemModel, public ISerializableObject
	{
		Q_OBJECT
		Q_PROPERTY( QStringList checkedItems READ checkedItems WRITE setCheckedItems )

	public:
		explicit CheckableFileSystemModel( QObject* parent = 0 );
		virtual ~CheckableFileSystemModel() = 0;

		virtual QAbstractItemView* view() const = 0;

		Qt::ItemFlags flags( const QModelIndex& index ) const;

		QVariant data( const QModelIndex& i, int role ) const;
		bool setData( const QModelIndex& i, const QVariant& value, int role );

		inline const QStringList& checkedItems() const { return _checkedItemsList.state(); }

		virtual QList<const char*> propList() const;

		virtual void commitChanges();
		virtual void revertChanges();

		bool initialized() const { return _initialized; }

		enum FilesGroup
		{
			Hdr,
			Src,
			All
		};

	protected:
		bool _initialized;

		virtual bool isCheckable( const QModelIndex& i, int role ) const;

	public slots:
		void setCheckedItems( const QStringList& value ) { _checkedItemsList.setState( value ); commitChanges(); }
		virtual void clearSelection();
		void changeAllCheckStates( const QModelIndex& parent, Qt::CheckState state, FilesGroup v = All );
		void invertSelection( const QModelIndex& parent, FilesGroup v = All );

	private slots:
		void _folderLoaded( const QString& folder );

	private:
		bool _skipOperation( const QModelIndex& i, const std::function<void()>& f );
		bool _skipChild( const QModelIndex& child, FilesGroup v );
		QStringList _loadedFolders;
		QMap<QString, std::function<void()> > _queuedOperations;

		Memento<QStringList> _checkedItemsList;
	};
}

#endif // CHECKABLEFILESYSTEMMODEL_H
