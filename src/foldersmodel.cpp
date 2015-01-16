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
 * foldersmodel.cpp
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
#include "selectfilesdialog.h"
#include "project.h"

namespace depgraphV
{
	FoldersModel::FoldersModel( QObject* parent )
		: CheckableFileSystemModel( parent ),
		  _treeView( 0 ),
		  _filesModel( new FilesModel( this, parent ) ),
		  _lastPath( QDir::homePath() ),
		  _viewConnected( false )
	{
		setFilter( QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks );
		connect( _filesModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
				 this, SLOT( _onFilesModelDataChanged( QModelIndex, QModelIndex ) )
		);
	}
	//-------------------------------------------------------------------------
	void FoldersModel::initialize()
	{
		_filesModel->initialize();
		_treeView->setModel( this );
		_treeView->setRootIndex( setRootPath( _lastPath ) );
		QModelIndex i = index( _lastPath );
		_treeView->setCurrentIndex( i );
		_updateSelection( i, QModelIndex() );

		if( !_initialized )
		{
			_treeView->hideColumn( 1 );
			_treeView->hideColumn( 2 );
			_treeView->hideColumn( 3 );
			_initialized = true;
		}
	}
	//-------------------------------------------------------------------------
	void FoldersModel::setView( QAbstractItemView* view )
	{
		//Sanity checks first of all: view cannot be null
		Q_ASSERT( view );
		SelectFilesDialog* d = Singleton<SelectFilesDialog>::instancePtr();

		if( _treeView )
		{
			//setting the same view?
			if( view == _treeView )
				return;

			_disconnectView();
			d->removeEventFilter( view->model() );
		}

		d->installEventFilter( this );
		_treeView = qobject_cast<QTreeView*>( view );

		//Second sanity check: view MUST be a QTreeView*
		Q_ASSERT( _treeView );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::setFilesView( QAbstractItemView* view )
	{
		_filesModel->setView( view );
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::hasChildren( const QModelIndex& parent ) const
	{
#if( QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 ) )
		// return false if item can't have children
		if( parent.flags().testFlag( Qt::ItemNeverHasChildren ) )
			return false;
#endif

		// return if at least one child exists
		return QDirIterator(
					filePath( parent ),
					filter()
		).hasNext();
	}
	//-------------------------------------------------------------------------
	QVariant FoldersModel::data( const QModelIndex& i, int role ) const
	{
		if( role == Qt::ToolTipRole )
			return filePath( i );

		if( isCheckable( i, role ) )
		{
			QString p = filePath( i );
			return _checkedFolders.contains( p ) ? _checkedFolders[ p ] : Qt::Unchecked;
		}

		return QFileSystemModel::data( i, role );
	}
	//-------------------------------------------------------------------------
	Qt::ItemFlags FoldersModel::flags( const QModelIndex& index ) const
	{
		Qt::ItemFlags f = QFileSystemModel::flags( index );
		if( isCheckable( index, Qt::CheckStateRole ) )
		{
			f |= Qt::ItemIsUserCheckable;
			f |= Qt::ItemIsTristate;
		}

		return f;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::setFileNameFilters( const QStringList& filters )
	{
		clearSelection();
		_filesModel->setNameFilters( filters );
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::isCheckable( const QModelIndex& i, int role ) const
	{
		//TODO QDirIterator slow down the execution of the application..maybe a queued
		//operation could be faster, but I need to figure out how make filters and
		//name filters works..
		return QDirIterator(
					filePath( i ),
					_filesModel->nameFilters(),
					_filesModel->filter()
		).hasNext() && CheckableFileSystemModel::isCheckable( i, role );
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::eventFilter( QObject* obj, QEvent* evt )
	{
		SelectFilesDialog* d = Singleton<SelectFilesDialog>::instancePtr();
		if( obj == d && evt->type() == QEvent::Show )
			_connectView();

		return QFileSystemModel::eventFilter( obj, evt );
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::setDataImpl( const QString& path, Qt::CheckState v )
	{
		if( sender() != _filesModel && v == Qt::PartiallyChecked )
			v = Qt::Checked;

		QMap<QString, Qt::CheckState>& l = _checkedFolders;
		bool res = true;

		if( v == Qt::Unchecked && l.contains( path ) )
			l.remove( path );

		else if( v != Qt::Unchecked )
		{
			if( l.contains( path ) )
				l[ path ] = v;
			else
				l.insert( path, v );
		}

		else
			res = false;

		if( res )
			_updateSelection( index( path ), QModelIndex() );

		return res;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::clearSelection()
	{
		_filesModel->clearSelection();
		_checkedFolders.clear();
	}
	//-------------------------------------------------------------------------
	void FoldersModel::changeRoot()
	{
		//Project* p = Singleton<Project>::instancePtr();
		QString root = QFileDialog::getExistingDirectory(
					_treeView,
					tr( "Change Root..." ),
					_lastPath
					//p->currentValue( "rootFolder" ).toString()
		);

		if( root.isNull() )
			return;

		_treeView->setRootIndex( setRootPath( root ) );
		_filesModel->view()->setRootIndex( _filesModel->setRootPath( root ) );
		//p->setCurrentValue( root, "rootFolder" );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::showHiddenFolders( bool show )
	{
		//TODO
		//https://bugreports.qt-project.org/browse/QTBUG-12413
		QDir::Filters f = filter();

		if( show )
			f |= QDir::Hidden;
		else
			f &= ~QDir::Hidden;

		setFilter( f );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::expandAll()
	{
		_treeView->blockSignals( true );
		_treeView->expandAll();
		_treeView->resizeColumnToContents( 0 );
		_treeView->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::collapseAll()
	{
		_treeView->blockSignals( true );
		_treeView->collapseAll();
		_treeView->resizeColumnToContents( 0 );
		_treeView->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_onFilesModelDataChanged( QModelIndex i, QModelIndex )
	{
		QModelIndex root = i.parent();
		QString fPath = _filesModel->filePath( root );
		int count = rowCount( root );
		if( _filesModel->nameFilterDisables() )
		{
			for( int i = 0; i < count; i++ )
			{
				if( !_filesModel->belongsToFileGroup( root.child( i, 0 ), FilesModel::All ) )
				{
					//Decrementing just one time is enough to achieve
					//the goal; doing so, the best case is O(1).
					count--;
					break;
				}
			}
		}

		Qt::CheckState fState = Qt::Checked;
		int selectedCount = _filesModel->selectedCount( fPath );
		if( selectedCount == 0 )
			fState = Qt::Unchecked;

		else if( selectedCount < count )
			fState = Qt::PartiallyChecked;

		_filesModel->setListeningFoldersModelDataChanged( false );
		setData( index( fPath ), fState, Qt::CheckStateRole );
		_filesModel->setListeningFoldersModelDataChanged( true );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_updateSelection( const QModelIndex& current,
										 const QModelIndex& )
	{
		//TODO DO I need to reflect _lastPath changes in Project???
		//Project* p = Singleton<Project>::instancePtr();
		_lastPath = fileInfo( current ).absoluteFilePath();
		_filesModel->view()->setRootIndex( _filesModel->setRootPath( _lastPath ) );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_itemExpandedCollapsed( const QModelIndex& )
	{
		_treeView->resizeColumnToContents( 0 );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_connectView()
	{
		if( _viewConnected )
			return;

		connect( _treeView->selectionModel(),
				 SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
				 this,
				 SLOT( _updateSelection( QModelIndex, QModelIndex ) )
		);
		connect( _treeView,
				 SIGNAL( collapsed( QModelIndex ) ),
				 this,
				 SLOT( _itemExpandedCollapsed( QModelIndex ) )
		);
		connect( _treeView,
				 SIGNAL( expanded( QModelIndex ) ),
				 this,
				 SLOT( _itemExpandedCollapsed( QModelIndex ) )
		);

		_viewConnected = true;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_disconnectView()
	{
		if( !_viewConnected )
			return;

		_treeView->selectionModel()->disconnect( _treeView->model() );
		_treeView->disconnect( _treeView->model() );
		_viewConnected = false;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator << ( QDataStream& out, const FoldersModel* object )
	{
		out<< object->_lastPath;

		const QMap<QString, Qt::CheckState>& l = object->_checkedFolders;
		QMapIterator<QString, Qt::CheckState> it( l );
		out << l.count();

		while( it.hasNext() )
		{
			it.next();
			out << it.key();
			out << static_cast<short>( it.value() );
		}

		out << *object->_filesModel;
		return out;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator >> ( QDataStream& in, FoldersModel* object )
	{
		in >> object->_lastPath;

		int count = 0;
		in >> count;

		QMap<QString, Qt::CheckState>& l = object->_checkedFolders;

		if( !l.isEmpty() )
			l.clear();

		for( int i = 0; i < count; i++ )
		{
			QString key;
			short value;
			in >> key >> value;
			l.insert( key, static_cast<Qt::CheckState>( value ) );
		}

		in >> *object->_filesModel;
		return in;
	}
}
