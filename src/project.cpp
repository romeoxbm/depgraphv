/**
 * project.cpp
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
#include "project.h"
#include "helpers.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

namespace depgraphV
{
	Project::Project( const QString& filePath, QObject* parent )
		: QObject( parent ),
		  Singleton<Project>(),
		  _db( QSqlDatabase::addDatabase( "QSQLITE" ) )
	{
		_db.setDatabaseName( filePath );
		if( !_db.open() )
			_error( tr( "Project creation" ) );

		QFileInfo f( filePath );
		_name = f.baseName();
		_path = f.absolutePath();
	}
	//-------------------------------------------------------------------------
	Project::~Project()
	{
		if( _db.isOpen() )
			_db.close();
	}
	//-------------------------------------------------------------------------
	Project* Project::createNew( const QString& filePath, QObject* parent )
	{
		Project* p = new Project( filePath, parent );

		//TODO Create here tables, views, etc.
		QString sql = Helpers::LoadTextFromResources( "db-structure.sql" );

		return p;
	}
	//-------------------------------------------------------------------------
	Project* Project::open( const QString& filePath, QObject* parent )
	{
		Project* p = new Project( filePath, parent );

		//TODO Check for a valid file by checking if tables exist using query
		//SELECT name FROM sqlite_master WHERE type='table' AND name='table_name';

		return p;
	}
	//-------------------------------------------------------------------------
	QSqlTableModel* Project::tableModel( const QString& table, const QString& filter )
	{
		if( !_models.contains( table ) )
		{
			QSqlTableModel* m = new QSqlTableModel( this, _db );
			m->setEditStrategy( QSqlTableModel::OnManualSubmit );
			m->setTable( table );
			if( !filter.isEmpty() )
				m->setFilter( filter );

			if( !m->select() )
			{
				_error( tr( "Select table" ), m );
				return 0;
			}

			_models.insert( table, m );

			connect( m, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
					 this, SLOT( onDataChanged( QModelIndex, QModelIndex ) )
			);
		}

		return _models[ table ];
	}
	//-------------------------------------------------------------------------
	bool Project::applyChanges( QSqlTableModel* model )
	{
		if( !_modifiedModels.contains( model ) )
		{
			qDebug() << tr( "Requested an useless applyChanges" );
			return false;
		}

		if( !_db.transaction() )
		{
			_error( tr( "Cannot apply changes" ) );
			return false;
		}

		if( model->submitAll() && _db.commit() )
		{
			_modifiedModels.remove( _modifiedModels.indexOf( model ) );
			if( _modifiedModels.isEmpty() )
				emit pendingChanges( false );

			return true;
		}
		else
			_error( tr( "Cannot apply changes" ), _db.rollback() ? model : 0 );

		return false;
	}
	//-------------------------------------------------------------------------
	bool Project::applyChanges( const QString& table )
	{
		return applyChanges( tableModel( table ) );
	}
	//-------------------------------------------------------------------------
	void Project::revertAll( const QString& table )
	{
		_modifiedModels.clear();
		tableModel( table )->revertAll();
		emit pendingChanges( false );
	}
	//-------------------------------------------------------------------------
	bool Project::hasPendingChanges( const QString& table ) const
	{
		if( table.isEmpty() )
			return !_modifiedModels.isEmpty();

		return _modifiedModels.contains( _models[ table ] );
	}
	//-------------------------------------------------------------------------
	bool Project::hasPendingChanges( QSqlTableModel* model ) const
	{
		return _modifiedModels.contains( model );
	}
	//-------------------------------------------------------------------------
	bool Project::applyAllChanges()
	{
		if( !_modifiedModels.isEmpty() )
		{
			foreach( QSqlTableModel* m, _modifiedModels )
			{
				if( !applyChanges( m ) )
					return false;
			}
		}
		else
		{
			//TODO Should I return false when there's no changes to apply?
		}

		return true;
	}
	//-------------------------------------------------------------------------
	void Project::onDataChanged( QModelIndex, QModelIndex )
	{
		_modifiedModels << static_cast<QSqlTableModel*>( sender() );
		emit pendingChanges( true );
	}
	//-------------------------------------------------------------------------
	void Project::_error( const QString& dlgTitle, QSqlTableModel* model )
	{
		QMessageBox::warning(
					0,
					dlgTitle,
					tr( "An error occurred: %1" ).arg(
						model ? model->lastError().text() :
								_db.lastError().text()
					)
		);
	}
}
