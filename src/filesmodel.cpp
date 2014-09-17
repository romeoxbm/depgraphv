/**
 * filesmodel.cpp
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
#include "filesmodel.h"
#include <QActionGroup>

namespace depgraphV
{
	FilesModel::FilesModel( QObject* parent )
		: CheckableFileSystemModel( parent ),
		  _listView( 0 ),
		  _showAllFiles( 0 ),
		  _selectAll( 0 ),
		  _selectNone( 0 ),
		  _invertSelection( 0 ),
		  _hdr_selectAll( 0 ),
		  _hdr_selectNone( 0 ),
		  _hdr_invertSelection( 0 ),
		  _src_selectAll( 0 ),
		  _src_selectNone( 0 ),
		  _src_invertSelection( 0 )
	{
		this->setFilter( QDir::NoDotAndDotDot | QDir::Files );
		this->setNameFilterDisables( false );
	}
	//-------------------------------------------------------------------------
	FilesModel::~FilesModel()
	{
	}
	//-------------------------------------------------------------------------
	void FilesModel::initialize( QListView* view, const QString& rootPath )
	{
		if( _initialized )
			return;

		Q_ASSERT( view && !rootPath.isNull() );
		_listView = view;
		_listView->setModel( this );
		_listView->setRootIndex( this->setRootPath( rootPath ) );

		_createContextMenu();
		_initialized = true;
	}
	//-------------------------------------------------------------------------
	bool FilesModel::isCheckable( const QModelIndex& i, int role ) const
	{
		bool baseRes = CheckableFileSystemModel::isCheckable( i, role );
		QString filter = "*." + fileInfo( i ).completeSuffix();
		return baseRes && nameFilters().contains( filter );
	}
	//-------------------------------------------------------------------------
	void FilesModel::onActionTriggered( QAction* a )
	{
		Q_ASSERT( a );
		PackedParameters* p = PackedParameters::getPtr( a->data() );
		Q_ASSERT( p );

		if( p->checkState() == -1 )
			this->invertSelection( _listView->rootIndex(), p->filesGroup() );

		else
		{
			this->changeAllCheckStates(
						_listView->rootIndex(),
						p->checkState(),
						p->filesGroup()
			);
		}
	}
	//-------------------------------------------------------------------------
	void FilesModel::onShowAllTriggered()
	{
		this->setNameFilterDisables( _showAllFiles->isChecked() );
	}
	//-------------------------------------------------------------------------
	void FilesModel::_createContextMenu()
	{
		_listView->setContextMenuPolicy( Qt::ActionsContextMenu );

		QActionGroup* group = new QActionGroup( _listView );
		connect( group, SIGNAL( triggered( QAction* ) ),
								this, SLOT( onActionTriggered( QAction* ) ) );

		//Actions
		_showAllFiles = new QAction( tr( "Show All Files" ), _listView );
		_showAllFiles->setCheckable( true );
		connect( _showAllFiles, SIGNAL( triggered() ),
				 this, SLOT( onShowAllTriggered() ) );

		QAction* sep = new QAction( group );
		sep->setSeparator( true );
		group->addAction( sep );

		_selectAll = new QAction( tr( "Select All" ), group );
		_selectAll->setData( PackedParameters::toQVariant( Qt::Checked ) );
		group->addAction( _selectAll );

		_selectNone = new QAction( tr( "Select None" ), group );
		_selectNone->setData( PackedParameters::toQVariant( Qt::Unchecked ) );
		group->addAction( _selectNone );

		_invertSelection = new QAction( tr( "Invert Selection" ), group );
		_invertSelection->setData(
					PackedParameters::toQVariant(
						static_cast<Qt::CheckState>( -1 )
					)
		);
		group->addAction( _invertSelection );

		sep = new QAction( group );
		sep->setSeparator( true );
		sep->setText( tr( "Headers" ) );
		group->addAction( sep );

		_hdr_selectAll = new QAction( tr( "Select All Headers" ), group );
		_hdr_selectAll->setData( PackedParameters::toQVariant( Qt::Checked, Hdr ) );
		group->addAction( _hdr_selectAll );

		_hdr_selectNone = new QAction( tr( "Deselect All Headers" ), group );
		_hdr_selectNone->setData( PackedParameters::toQVariant( Qt::Unchecked, Hdr ) );
		group->addAction( _hdr_selectNone );

		_hdr_invertSelection = new QAction( tr( "Invert Headers Selection" ), group );
		_hdr_invertSelection->setData(
					PackedParameters::toQVariant(
						static_cast<Qt::CheckState>( -1 ),
						Hdr
					)
		);
		group->addAction( _hdr_invertSelection );

		sep = new QAction( group );
		sep->setSeparator( true );
		sep->setText( tr( "Sources" ) );
		group->addAction( sep );

		_src_selectAll = new QAction( tr( "Select All Sources" ), group );
		_src_selectAll->setData( PackedParameters::toQVariant( Qt::Checked, Src ) );
		group->addAction( _src_selectAll );

		_src_selectNone = new QAction( tr( "Deselect All Sources" ), group );
		_src_selectNone->setData( PackedParameters::toQVariant( Qt::Unchecked, Src ) );
		group->addAction( _src_selectNone );

		_src_invertSelection = new QAction( tr( "Invert Sources Selection" ), group );
		_src_invertSelection->setData(
					PackedParameters::toQVariant(
						static_cast<Qt::CheckState>( -1 ),
						Src
					)
		);
		group->addAction( _src_invertSelection );

		_listView->addAction( _showAllFiles );
		_listView->addActions( group->actions() );

		//Tooltips
#ifndef QT_NO_TOOLTIP
		_showAllFiles->setToolTip( tr( "Show all files, including filtered ones" ) );

		_selectAll->setToolTip( tr( "Select all valid files in this folder" ) );
		_selectNone->setToolTip( tr( "Clear selection" ) );
		_invertSelection->setToolTip( tr( "Invert current files selection" ) );

		_hdr_selectAll->setToolTip( tr( "Select all headers in this folder" ) );
		_hdr_selectNone->setToolTip( tr( "Clear headers selection" ) );
		_hdr_invertSelection->setToolTip( tr( "Invert current headers selection" ) );

		_src_selectAll->setToolTip( tr( "Select all sources in this folder" ) );
		_src_selectNone->setToolTip( tr( "Clear sources selection" ) );
		_src_invertSelection->setToolTip( tr( "Invert current sources selection" ) );
#endif
	}
}
