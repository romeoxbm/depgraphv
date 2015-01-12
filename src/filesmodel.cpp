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
 * filesmodel.cpp
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
#include "depgraphv_pch.h"
#include "foldersmodel.h"
#include "project.h"
#include "helpers.h"

namespace depgraphV
{
	FilesModel::FilesModel( FoldersModel* fModel, QObject* parent )
		: CheckableFileSystemModel( parent ),
		  _listView( 0 ),
		  _foldersModel( fModel )
	{
		setFilter( QDir::NoDotAndDotDot | QDir::Files );
		setNameFilterDisables( false );

		setListeningFoldersModelDataChanged( true );
	}
	//-------------------------------------------------------------------------
	void FilesModel::initialize()
	{
		_listView->setModel( this );

		Project* p = Singleton<Project>::instancePtr();
		//TODO
		/*_checkedFiles.setState(
					p->currentBinaryValue<QMap<QString, QStringList*> >( "checkedFiles" )
		);*/
		setNameFilters( p->nameFilters() );

		_initialized = true;
	}
	//-------------------------------------------------------------------------
	void FilesModel::setView( QAbstractItemView* view )
	{
		//Sanity checks first of all: view cannot be null
		Q_ASSERT( view );

		//setting the same view?
		if( view == _listView )
			return;

		_listView = qobject_cast<QListView*>( view );

		//Second sanity check: view MUST be a QListView*
		Q_ASSERT( _listView );
		_listView->viewport()->removeEventFilter( view->model() );
		_listView->viewport()->installEventFilter( this );
	}
	//-------------------------------------------------------------------------
	int FilesModel::selectedCount( const QString& rootPath, FileGroup g ) const
	{
		int count = -1;

		if( !rootPath.isEmpty() )
		{
			if( _checkedFiles.contains( rootPath ) )
			{
				if( g == FilesModel::All )
					count = _checkedFiles[ rootPath ]->count();
				else
				{
					count = 0;
					foreach( QString f, *_checkedFiles[ rootPath ] )
					{
						if( belongsToFileGroup( f, g ) )
							count++;
					}
				}
			}
			else
				count = 0;
		}
		else
		{
			//Count ALL selected files, not just in a single directory
			count = 0;
			QMapIterator<QString, QStringList*> it( _checkedFiles );
			while( it.hasNext() )
			{
				it.next();

				if( g == FilesModel::All )
					count += it.value()->count();
				else
				{
					foreach( QString f, *it.value() )
					{
						if( belongsToFileGroup( f, g ) )
							count++;
					}
				}
			}
		}

		return count;
	}
	//-------------------------------------------------------------------------
	QStringList FilesModel::checkedFiles() const
	{
		QStringList res;
		foreach( QStringList* l, _checkedFiles )
			res.append( *l );

		return res;
	}
	//-------------------------------------------------------------------------
	QVariant FilesModel::data( const QModelIndex& i, int role ) const
	{
		if( isCheckable( i, role ) )
		{
			QString fp = filePath( i );
			QString dp = filePath( i.parent() );
			//QMap<QString, QStringList*>& items = _checkedFiles.state();

			if( _checkedFiles.contains( dp ) && _checkedFiles[ dp ]->contains( fp ) )
				return Qt::Checked;

			return Qt::Unchecked;
		}

		return QFileSystemModel::data( i, role );
	}
	//-------------------------------------------------------------------------
	/*void FilesModel::commitChanges()
	{
		if( !_checkedFiles.commit() )
			return;

		Project* p = Singleton<Project>::instancePtr();
		//TODO
		//p->setCurrentBinaryValue( _checkedFiles.state(), "checkedFiles" );
	}*/
	//-------------------------------------------------------------------------
	/*void FilesModel::revertChanges()
	{
		_checkedFiles.revert();
	}*/
	//-------------------------------------------------------------------------
	bool FilesModel::belongsToFileGroup( const QString& filePath, FileGroup v ) const
	{
		return belongsToFileGroup( index( filePath ), v );
	}
	//-------------------------------------------------------------------------
	bool FilesModel::belongsToFileGroup( const QModelIndex& i, FileGroup v ) const
	{
		if( !i.flags().testFlag( Qt::ItemIsEnabled ) )
			return false;

		if( v != All )
		{
			QFileInfo fi = fileInfo( i );
			QString filter = "*." + fi.completeSuffix();

			//TODO Where should I take name filters? (probably from Project)
			/*AppConfig* c = Singleton<AppConfig>::instancePtr();
			QStringList filters;
			if( v == Hdr )
				filters = c->headerNameFilters();

			else if( v == Src )
				filters = c->sourceNameFilters();

			if( !filters.contains( filter ) )
				return false;*/
		}

		return true;
	}
	//-------------------------------------------------------------------------
	void FilesModel::setListeningFoldersModelDataChanged( bool value ) const
	{
		if( value )
		{
			connect( _foldersModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
				 this, SLOT( onFoldersModelDataChanged( QModelIndex, QModelIndex ) )
			);
		}
		else
		{
			_foldersModel->disconnect( this,
				SLOT( onFoldersModelDataChanged( QModelIndex, QModelIndex ) )
			);
		}
	}
	//-------------------------------------------------------------------------
	bool FilesModel::eventFilter( QObject* obj, QEvent* evt )
	{
		if( obj == _listView->viewport() && evt->type() == QEvent::ContextMenu )
			_updateEnabledFlags();

		return CheckableFileSystemModel::eventFilter( obj, evt );
	}
	//-------------------------------------------------------------------------
	bool FilesModel::isCheckable( const QModelIndex& i, int role ) const
	{
		bool baseRes = CheckableFileSystemModel::isCheckable( i, role );
		QString filter = "*." + fileInfo( i ).completeSuffix();
		return baseRes && nameFilters().contains( filter );
	}
	//-------------------------------------------------------------------------
	bool FilesModel::setDataImpl( const QString& path, Qt::CheckState v )
	{
		QMap<QString, QStringList*>& l = _checkedFiles;
		QModelIndex pIdx = index( path );
		QString fp = filePath( pIdx );
		QString dp = filePath( pIdx.parent() );
		if( l.contains( dp ) )
		{
			if( v == Qt::Checked && !l[ dp ]->contains( fp ) )
				l[ dp ]->append( fp );

			else if( v == Qt::Unchecked && l[ dp ]->contains( fp ) )
				l[ dp ]->removeAll( fp );

			if( l[ dp ]->isEmpty() )
			{
				delete l[ dp ];
				l.remove( dp );
			}
		}
		else if( v == Qt::Checked )
		{
			l.insert( dp, new QStringList );
			l[ dp ]->append( fp );
		}

		return true;
	}
	//-------------------------------------------------------------------------
	void FilesModel::clearSelection()
	{
		foreach( QStringList* l, _checkedFiles )
			delete l;

		_checkedFiles.clear();
	}
	//-------------------------------------------------------------------------
	void FilesModel::contextMenuActionTriggered()
	{
		QAction* a = static_cast<QAction*>( sender() );
		typedef QPair<Qt::CheckState, FileGroup> Pair;
		Pair* p;
		p = Helpers::fromQVariant<Pair>( a->data() );
		Q_ASSERT( p );

		QModelIndex root = _listView->rootIndex();
		int rows = rowCount( root );

		for( int i = 0; i < rows; i ++ )
		{
			QModelIndex child = root.child( i, 0 );

			if( !belongsToFileGroup( child, p->second ) )
				continue;

			//Invert selection
			if( p->first == -1 )
			{
				Qt::CheckState c = (Qt::CheckState)data( child, Qt::CheckStateRole ).toInt();
				setDataImpl( filePath( child ), c == Qt::Unchecked ? Qt::Checked : Qt::Unchecked );
			}
			else
				setDataImpl( filePath( child ), p->first );
		}
		emit dataChanged( index( 0, 0, root ), index( rows - 1, 0, root ) );
	}
	//-------------------------------------------------------------------------
	void FilesModel::showAllFiles()
	{
		QAction* a = static_cast<QAction*>( sender() );
		setNameFilterDisables( a->isChecked() );
	}
	//-------------------------------------------------------------------------
	void FilesModel::onFoldersModelDataChanged( QModelIndex i, QModelIndex )
	{
		QModelIndex parent = index( _foldersModel->filePath( i ) );
		int rows = rowCount( parent );
		blockSignals( true );
		for( int j = 0; j < rows; j++ )
		{
			QModelIndex childIdx = parent.child( j, 0 );
			setData( childIdx, i.data( Qt::CheckStateRole ), Qt::CheckStateRole );
		}
		blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void FilesModel::_updateEnabledFlags()
	{
		QString path = filePath( _listView->rootIndex() );
		int allSelectedCount = selectedCount( path );
		int headersSelectedCount = selectedCount( path, FilesModel::Hdr );
		int sourcesSelectedCount = selectedCount( path, FilesModel::Src );

		int count = _filesCount( _listView->rootIndex() );
		int headersCount = _filesCount( _listView->rootIndex(), FilesModel::Hdr );
		int sourcesCount = _filesCount( _listView->rootIndex(), FilesModel::Src );

		QList<QAction*> aList = _listView->actions();
		//All
		aList[ 2 ]->setEnabled( count && allSelectedCount < count );
		aList[ 3 ]->setEnabled( count && allSelectedCount );
		aList[ 4 ]->setEnabled( count );

		//Headers
		aList[ 6 ]->setEnabled( headersCount&& headersSelectedCount < headersCount );
		aList[ 7 ]->setEnabled( headersCount && headersSelectedCount );
		aList[ 8 ]->setEnabled( headersCount );

		//Sources
		aList[ 10 ]->setEnabled( sourcesCount && sourcesSelectedCount < sourcesCount );
		aList[ 11 ]->setEnabled( sourcesCount && sourcesSelectedCount );
		aList[ 12 ]->setEnabled( sourcesCount );
	}
	//-------------------------------------------------------------------------
	int FilesModel::_filesCount( const QModelIndex& parent, FileGroup g ) const
	{
		Q_ASSERT( parent.isValid() );
		int count = rowCount( parent );
		if( nameFilterDisables() )
		{
			for( int i = 0; i < rowCount( parent ); i++ )
			{
				QModelIndex c = parent.child( i, 0 );
				if( !c.flags().testFlag( Qt::ItemIsEnabled ) ||
					( g != FilesModel::All && !belongsToFileGroup( c, g ) ) )
				{
					count--;
				}
			}
		}
		else if( g != FilesModel::All )
		{
			for( int i = 0; i < rowCount( parent ); i++ )
			{
				QModelIndex c = parent.child( i, 0 );
				if( !belongsToFileGroup( c, g ) )
					count--;
			}
		}

		return count;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator << ( QDataStream& out, const FilesModel& object )
	{
		QMapIterator<QString, QStringList*> it( object._checkedFiles );
		out << object._checkedFiles.count();

		while( it.hasNext() )
		{
			it.next();
			out << it.key();
			out << *it.value();
		}

		return out;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator >> ( QDataStream& in, FilesModel& object )
	{
		int count = 0;
		in >> count;
		//QMap<QString, QStringList*> data;
		QMap<QString, QStringList*>& l = object._checkedFiles;

		if( !l.isEmpty() )
			l.clear();

		for( int i = 0; i < count; i++ )
		{
			QString key;
			in >> key;
			l.insert( key, new QStringList );
			in >> *l[ key ];
		}

		//object._checkedFiles.setState( data );
		return in;
	}
}
