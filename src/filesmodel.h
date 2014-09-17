/**
 * filesmodel.h
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
#ifndef FILESMODEL_H
#define FILESMODEL_H

#include <QListView>
#include <QAction>

#include "checkablefilesystemmodel.h"

namespace depgraphV
{
	class FilesModel : public CheckableFileSystemModel
	{
		Q_OBJECT

	public:
		explicit FilesModel( QObject* parent = 0 );
		~FilesModel();

		virtual QAbstractItemView* view() const { return _listView; }

		void initialize( QListView* view );

	protected:
		virtual bool isCheckable( const QModelIndex& i, int role ) const;

	public slots:

	private slots:
		void onActionTriggered( QAction* );
		void onShowAllTriggered();

	private:
		class PackedParameters
		{
		public:
			Qt::CheckState checkState() const { return _checkstate; }
			CheckableFileSystemModel::FilesGroup filesGroup() const { return _group; }

			static PackedParameters* getPtr( const QVariant& v )
			{
				return static_cast<PackedParameters*>( v.value<void*>() );
			}

			static QVariant toQVariant( Qt::CheckState c,
										CheckableFileSystemModel::FilesGroup g = CheckableFileSystemModel::All )
			{
				PackedParameters* p = new PackedParameters( c, g );
				return qVariantFromValue( static_cast<void*>( p ) );
			}

		private:
			PackedParameters( Qt::CheckState c, CheckableFileSystemModel::FilesGroup g )
				: _checkstate( c ),
				  _group( g )
			{
			}

			Qt::CheckState _checkstate;
			CheckableFileSystemModel::FilesGroup _group;
		};

		QListView* _listView;
		QAction* _showAllFiles;
		QAction* _selectAll;
		QAction* _selectNone;
		QAction* _invertSelection;

		QAction* _hdr_selectAll;
		QAction* _hdr_selectNone;
		QAction* _hdr_invertSelection;

		QAction* _src_selectAll;
		QAction* _src_selectNone;
		QAction* _src_invertSelection;

		void _createContextMenu();
	};
}

#endif // FILESMODEL_H
