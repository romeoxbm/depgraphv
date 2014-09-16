/**
 * checkablefilesystemmodel.cpp
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
#include "checkablefilesystemmodel.h"
#include "buildsettings.h"
#include "appconfig.h"

namespace depgraphV
{
	CheckableFileSystemModel::CheckableFileSystemModel( QObject* parent )
		: QFileSystemModel( parent )
	{
		this->setReadOnly( true );

		connect( this,
				 SIGNAL( directoryLoaded( QString ) ),
				 this,
				 SLOT( _folderLoaded( QString ) )
		);

		Singleton<AppConfig>::instance().registerSerializable( this );
	}
	//-------------------------------------------------------------------------
	CheckableFileSystemModel::~CheckableFileSystemModel()
	{
	}
	//-------------------------------------------------------------------------
	Qt::ItemFlags CheckableFileSystemModel::flags( const QModelIndex& i ) const
	{
		Qt::ItemFlags f = QFileSystemModel::flags( i );
		if( isCheckable( i, Qt::CheckStateRole ) )
			f |= Qt::ItemIsUserCheckable;

		return f;
	}
	//-------------------------------------------------------------------------
	QVariant CheckableFileSystemModel::data( const QModelIndex& i, int role ) const
	{
		if( isCheckable( i, role ) )
		{
			return _checkedItemsList.state().contains( filePath( i ) ) ?
						Qt::Checked : Qt::Unchecked;
		}

		return QFileSystemModel::data( i, role );
	}
	//-------------------------------------------------------------------------
	bool CheckableFileSystemModel::setData( const QModelIndex& i,
											const QVariant& value, int role )
	{
		if( !isCheckable( i, role ) )
			return QFileSystemModel::setData( i, value, role );

		bool isChecked = value.toInt() == Qt::Checked;
		QString path = filePath( i );

		if( !( isChecked ^ (bool)_checkedItemsList.state().contains( path ) ) )
			return QFileSystemModel::setData( i, value, role );

		if( isChecked )
			_checkedItemsList.state().append( path );
		else
			_checkedItemsList.state().removeAll( path );

#ifdef QT_USE_QT5
		QVector<int> roles;
		roles << role;
		emit dataChanged( i, i, roles );
#else
		emit dataChanged( i, i );
#endif
		return true;
	}
	//-------------------------------------------------------------------------
	QList<const char*> CheckableFileSystemModel::propList() const
	{
		QList<const char*> props;
		props << "checkedItems";
		return props;
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::commitChanges()
	{
		_checkedItemsList.commit();
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::revertChanges()
	{
		_checkedItemsList.revert();
	}
	//-------------------------------------------------------------------------
	bool CheckableFileSystemModel::isCheckable( const QModelIndex& i, int role ) const
	{
		bool result = i.isValid() &&
				i.column() == 0 &&
				role == Qt::CheckStateRole;

		return result;
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::clearSelection()
	{
		/*QVector<int> roles;
		roles << Qt::CheckStateRole;*/
		_checkedItemsList.state().clear();
		//TODO emit dataChanged signal
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::changeAllCheckStates( const QModelIndex& parent,
										   Qt::CheckState state, FilesGroup v )
	{
		std::function<void()> f = std::bind(
									  &CheckableFileSystemModel::changeAllCheckStates,
									  this,
									  parent,
									  state,
									  v
		);

		if( _skipOperation( parent, f ) )
			return;

		for( int i = 0; i < rowCount( parent ); i++ )
		{
			const QModelIndex child = index( i, 0, parent );
			if( _skipChild( child, v ) )
				continue;

			setData( child, state, Qt::CheckStateRole );
		}
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::invertSelection( const QModelIndex& parent, FilesGroup v )
	{
		std::function<void()> f = std::bind(
									  &CheckableFileSystemModel::invertSelection,
									  this,
									  parent,
									  v
		);

		if( _skipOperation( parent, f ) )
			return;

		for( int i = 0; i < rowCount( parent ); i++ )
		{
			const QModelIndex child = index( i, 0, parent );
			if( _skipChild( child, v ) )
				continue;

			Qt::CheckState c = (Qt::CheckState)data( child, Qt::CheckStateRole ).toInt();
			setData( child, c == Qt::Unchecked ? Qt::Checked : Qt::Unchecked,
					 Qt::CheckStateRole );
		}
	}
	//-------------------------------------------------------------------------
	bool CheckableFileSystemModel::_skipOperation( const QModelIndex& i,
									   const std::function<void()>& f )
	{
		if( isDir( i ) )
		{
			QString path = filePath( i );
			if( !_loadedFolders.contains( path ) )
			{
				_queuedOperations.insert( path, f );
				return true;
			}
		}

		return false;
	}
	//-------------------------------------------------------------------------
	bool CheckableFileSystemModel::_skipChild( const QModelIndex& child, FilesGroup v )
	{
		if( v != All )
		{
			QFileInfo fi = fileInfo( child );
			QString filter = "*." + fi.completeSuffix();
			AppConfig* c = Singleton<AppConfig>::instancePtr();
			QStringList filters;
			if( v == Hdr )
				filters = c->headerNameFilters();

			else if( v == Src )
				filters = c->sourceNameFilters();

			if( !filters.contains( filter ) )
				return true;
		}

		return false;
	}
	//-------------------------------------------------------------------------
	void CheckableFileSystemModel::_folderLoaded( const QString& folder )
	{
		_loadedFolders.append( folder );
		if( _queuedOperations.contains( folder ) )
		{
			_queuedOperations[ folder ]();
			_queuedOperations.remove( folder );
		}
	}
}
