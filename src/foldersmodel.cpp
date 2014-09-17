/**
 * foldersmodel.cpp
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
#include "foldersmodel.h"
#include "buildsettings.h"
#include "appconfig.h"
#include <QFileDialog>

namespace depgraphV
{
	FoldersModel::FoldersModel( QObject* parent )
		: CheckableFileSystemModel( parent ),
		  _treeView( 0 ),
		  _filesModel( new FilesModel( parent ) ),
		  _changeRoot( 0 ),
		  _showHiddenFolders( 0 ),
		  _expandAll( 0 ),
		  _collapseAll( 0 )
	{
		this->setFilter( QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::initialize( QTreeView* foldersView,
								   QListView* filesView, const QString& rootPath )
	{
		if( _initialized )
			return;

		Q_ASSERT( foldersView && filesView && !rootPath.isNull() );

		_filesModel->initialize( filesView );

		_treeView = foldersView;
		_treeView->setModel( this );
		_treeView->setRootIndex( this->setRootPath( rootPath ) );
		_treeView->hideColumn( 1 );
		_treeView->hideColumn( 2 );
		_treeView->hideColumn( 3 );

		_createContextMenu();
		_connectSignalsToSlots();

		//Force treeview selection update
		setSelectedFolderPath( _selectedFolderPath );

		_initialized = true;
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::hasChildren( const QModelIndex& parent ) const
	{
#ifdef QT_USE_QT5
		// return false if item can't have children
		if( parent.flags() & Qt::ItemNeverHasChildren )
			return false;
#endif

		// return if at least one child exists
		return QDirIterator(
					filePath( parent ),
					filter()
		).hasNext();
	}
	//-------------------------------------------------------------------------
	bool FoldersModel::setData( const QModelIndex& i, const QVariant& value, int role )
	{
		//TODO Set fullpath as hint of every tree node
		if( CheckableFileSystemModel::setData( i, value, role ) )
		{
			Qt::CheckState c = (Qt::CheckState)value.toInt();
			_updateSelection( i, QModelIndex() );
			_filesModel->changeAllCheckStates(
						_filesModel->view()->rootIndex(), c
			);

			emit folderCheckStateChanged( i, c );
			return true;
		}

		return false;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::setFileNameFilters( const QStringList& filters )
	{
		this->clearSelection();
		_filesModel->setNameFilters( filters );
	}
	//-------------------------------------------------------------------------
	QList<const char*> FoldersModel::propList() const
	{
		QList<const char*> props = CheckableFileSystemModel::propList();
		props << "selectedFolderPath";

		return props;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::commitChanges()
	{
		CheckableFileSystemModel::commitChanges();
		_filesModel->commitChanges();
	}
	//-------------------------------------------------------------------------
	void FoldersModel::revertChanges()
	{
		CheckableFileSystemModel::revertChanges();
		_filesModel->revertChanges();
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
	void FoldersModel::clearSelection()
	{
		_filesModel->clearSelection();
		CheckableFileSystemModel::clearSelection();
	}
	//-------------------------------------------------------------------------
	void FoldersModel::changeRoot( const QString& newRoot )
	{
		if( newRoot.isNull() )
			return;

		_treeView->setRootIndex( this->setRootPath( newRoot ) );
		_filesModel->view()->setRootIndex( _filesModel->setRootPath( newRoot ) );
		Singleton<AppConfig>::instancePtr()->setRootFolder( newRoot );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::showHiddenFolders( bool show )
	{
		//TODO
		//https://bugreports.qt-project.org/browse/QTBUG-12413
		QDir::Filters f = this->filter();

		if( show )
			f |= QDir::Hidden;
		else
			f &= ~QDir::Hidden;

		this->setFilter( f );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::setSelectedFolderPath( const QString& value )
	{
		if( _treeView )
			_treeView->setCurrentIndex( index( value ) );
		else
			_selectedFolderPath = value;
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_on_expandAll()
	{
		if( !_treeView )
			return;

		_treeView->blockSignals( true );
		_treeView->expandAll();
		_treeView->resizeColumnToContents( 0 );
		_treeView->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_on_collapseAll()
	{
		if( !_treeView )
			return;

		_treeView->blockSignals( true );
		_treeView->collapseAll();
		_treeView->resizeColumnToContents( 0 );
		_treeView->blockSignals( false );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_updateSelection( const QModelIndex& current,
										 const QModelIndex& )
	{
		_selectedFolderPath = this->fileInfo( current ).absoluteFilePath();
		_filesModel->view()->setRootIndex( _filesModel->setRootPath( _selectedFolderPath ) );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_itemExpandedCollapsed( const QModelIndex& )
	{
		_treeView->resizeColumnToContents( 0 );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_on_changeRoot_triggered()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		QString root = QFileDialog::getExistingDirectory(
					_treeView,
					tr( "Change Root..." ),
					c->rootFolder()
		);

		this->changeRoot( root );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_createContextMenu()
	{
		_treeView->setContextMenuPolicy( Qt::ActionsContextMenu );

		//Separators
		QAction* sep1 = new QAction( this );
		sep1->setSeparator( true );
		QAction* sep2 = new QAction( this );
		sep2->setSeparator( true );

		//Actions
		_changeRoot = new QAction( _treeView );
		_changeRoot->setText( tr( "Change Root..." ) );

		_showHiddenFolders = new QAction( _treeView );
		_showHiddenFolders->setText( tr( "Show Hidden Folders" ) );
		_showHiddenFolders->setCheckable( true );

		_expandAll = new QAction( _treeView );
		_expandAll->setText( tr( "Expand All" ) );

		_collapseAll = new QAction( _treeView );
		_collapseAll->setText( tr( "Collapse All" ) );

		//Tooltips
#ifndef QT_NO_TOOLTIP
		_changeRoot->setToolTip( tr( "Change root folder being inspected" ) );
		_showHiddenFolders->setToolTip( tr( "Show Hidden Folders" ) );
		_expandAll->setToolTip( tr( "Expand all visible folders" ) );
		_collapseAll->setToolTip( tr( "Collapse all visible folders" ) );
#endif

		//Context Menu
		_treeView->addAction( _changeRoot );
		_treeView->addAction( sep1 );
		_treeView->addAction( _showHiddenFolders );
		_treeView->addAction( sep2 );
		_treeView->addAction( _expandAll );
		_treeView->addAction( _collapseAll );
	}
	//-------------------------------------------------------------------------
	void FoldersModel::_connectSignalsToSlots()
	{
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
		connect( _treeView,
				 SIGNAL( doubleClicked( QModelIndex ) ),
				 this,
				 SLOT( invertSelection( QModelIndex ) )
		);

		//Context Menu actions
		connect( _changeRoot, SIGNAL( triggered() ),
				 this, SLOT( _on_changeRoot_triggered() )
		);
		connect( _showHiddenFolders, SIGNAL( toggled( bool ) ),
				 this, SLOT( showHiddenFolders( bool ) )
		);
		connect( _expandAll, SIGNAL( triggered() ),
				 this, SLOT( _on_expandAll() )
		);
		connect( _collapseAll, SIGNAL( triggered() ),
				 this, SLOT( _on_collapseAll() )
		);
	}
}
