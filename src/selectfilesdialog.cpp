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
 * selectfilesdialog.cpp
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
#include "selectfilesdialog.h"
#include "ui_selectfilesdialog.h"
#include "helpers.h"
#include "mainwindow.h"

namespace depgraphV
{
	SelectFilesDialog::SelectFilesDialog( MainWindow* parent )
		: QDialog( parent ),
		Singleton<SelectFilesDialog>(),
		_ui( new Ui::SelectFilesDialog ),
		_previousModel( 0 )
	{
		_ui->setupUi( this );

		//ContextMenu for Folders view
		_ui->treeView->addAction( _ui->changeRoot );
		Helpers::insertSeparator( _ui->treeView );

		_ui->treeView->addAction( _ui->showHiddenFolders );

		Helpers::insertSeparator( _ui->treeView );
		_ui->treeView->addAction( _ui->expandAll );
		_ui->treeView->addAction( _ui->collapseAll );

		//ContextMenu for Files view
		_ui->listView->addAction( _ui->showAllFiles );

		Helpers::insertSeparator( _ui->listView );

		typedef QPair<Qt::CheckState, FilesModel::FileGroup> Pair;
		_ui->listView->addAction( _ui->selectAll );
		_ui->selectAll->setData( Helpers::toQVariant( new Pair( Qt::Checked, FilesModel::All ) ) );

		_ui->listView->addAction( _ui->selectNone );
		_ui->selectNone->setData( Helpers::toQVariant( new Pair( Qt::Unchecked, FilesModel::All ) ) );

		_ui->listView->addAction( _ui->invertSelection );
		_ui->invertSelection->setData(
					Helpers::toQVariant( new Pair( static_cast<Qt::CheckState>( -1 ), FilesModel::All ) )
		);

		Helpers::insertSeparator( _ui->listView );

		_ui->listView->addAction( _ui->hdr_selectAll );
		_ui->hdr_selectAll->setData( Helpers::toQVariant( new Pair( Qt::Checked, FilesModel::Hdr ) ) );

		_ui->listView->addAction( _ui->hdr_selectNone );
		_ui->hdr_selectNone->setData( Helpers::toQVariant( new Pair( Qt::Unchecked, FilesModel::Hdr ) ) );

		_ui->listView->addAction( _ui->hdr_invertSelection );
		_ui->hdr_invertSelection->setData(
					Helpers::toQVariant( new Pair( static_cast<Qt::CheckState>( -1 ), FilesModel::Hdr ) )
		);

		Helpers::insertSeparator( _ui->listView );

		_ui->listView->addAction( _ui->src_selectAll );
		_ui->src_selectAll->setData( Helpers::toQVariant( new Pair( Qt::Checked, FilesModel::Src ) ) );

		_ui->listView->addAction( _ui->src_selectNone );
		_ui->src_selectNone->setData( Helpers::toQVariant( new Pair( Qt::Unchecked, FilesModel::Src ) ) );

		_ui->listView->addAction( _ui->src_invertSelection );
		_ui->src_invertSelection->setData(
					Helpers::toQVariant( new Pair( static_cast<Qt::CheckState>( -1 ), FilesModel::Src ) )
		);

		connect( parent, SIGNAL( projectOpened( Project* ) ),
				 this, SLOT( _onProjectOpened( Project* ) )
		);
		connect( parent, SIGNAL( projectClosed() ),
				 this, SLOT( _onProjectClosed() )
		);
	}
	//-------------------------------------------------------------------------
	SelectFilesDialog::~SelectFilesDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	int SelectFilesDialog::exec()
	{
		if( _previousModel )
			_disconnect();

		Project* p = Singleton<Project>::instancePtr();

		//_previousModel now contains the current model
		_previousModel = p->currentGraph()->model();
		_connect();
		_previousModel->setView( _ui->treeView );
		_previousModel->setFilesView( _ui->listView );
		_previousModel->initialize();

		return QDialog::exec();
	}
	//-------------------------------------------------------------------------
	QByteArray SelectFilesDialog::graphModel() const
	{
		Project* p = Singleton<Project>::instancePtr();
		QByteArray data;
		QDataStream stream( &data, QIODevice::WriteOnly );
		stream << p->currentGraph()->model();

		return data;
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::setGraphModel( const QByteArray& data )
	{
		Project* p = Singleton<Project>::instancePtr();
		QDataStream stream( data );
		stream >> p->currentGraph()->model();
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::_onClose( int result )
	{
		Project* p = Singleton<Project>::instancePtr();
		if( result )
			p->submitChanges();
		else
			p->revertChanges();
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::_onProjectOpened( Project* p )
	{
		p->addMapping( this, "graphModel" );
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::_onProjectClosed()
	{
		_previousModel = 0;
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::_connect()
	{
		Project* p = Singleton<Project>::instancePtr();
		FoldersModel* m = p->currentGraph()->model();

		//ContextMenu for Folders view
		connect( _ui->changeRoot, SIGNAL( triggered() ),
			m, SLOT( changeRoot() )
		);
		connect( _ui->showHiddenFolders, SIGNAL( toggled( bool ) ),
			m, SLOT( showHiddenFolders( bool ) )
		);
		connect( _ui->expandAll, SIGNAL( triggered() ),
			m, SLOT( expandAll() )
		);
		connect( _ui->collapseAll, SIGNAL( triggered() ),
			m, SLOT( collapseAll() )
		);

		//ContextMenu for Files view
		for( int i = 0; i < _ui->listView->actions().count(); i++ )
		{
			QAction* q = _ui->listView->actions()[ i ];
			if( i == 0 )
			{
				connect( q, SIGNAL( triggered() ),
					m->filesModel(), SLOT( showAllFiles() )
				);
				continue;
			}

			if( q->isSeparator() )
				continue;

			connect( q, SIGNAL( triggered() ),
				m->filesModel(), SLOT( contextMenuActionTriggered() )
			);
		}

		//selectionChanged signal
		connect( m, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
				 this, SIGNAL( selectionChanged() )
		);
		connect( m->filesModel(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
				 this, SIGNAL( selectionChanged() )
		);
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::_disconnect()
	{
		//ContextMenu for Folders view
		_ui->changeRoot->disconnect( _previousModel );
		_ui->showHiddenFolders->disconnect( _previousModel );
		_ui->expandAll->disconnect( _previousModel );
		_ui->collapseAll->disconnect( _previousModel );

		//ContextMenu for Files view
		for( int i = 0; i < _ui->listView->actions().count(); i++ )
		{
			QAction* q = _ui->listView->actions()[ i ];
			if( q->isSeparator() )
				continue;

			q->disconnect( _previousModel->filesModel() );
		}

		_previousModel->disconnect( this, SIGNAL( selectionChanged() ) );
		_previousModel->filesModel()->disconnect( this, SIGNAL( selectionChanged() ) );
	}
}
