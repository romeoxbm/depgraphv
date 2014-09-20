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
			qDebug() << _db.lastError();

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
	Project* Project::newProject( const QString& filePath, QObject* parent )
	{
		Project* p = new Project( filePath, parent );

		//TODO Create here tables, views, etc.

		return p;
	}
	//-------------------------------------------------------------------------
	Project* Project::openProject( const QString& filePath, QObject* parent )
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
			m->select();
			_models.insert( table, m );
		}

		return _models[ table ];
	}
	//-------------------------------------------------------------------------
	bool Project::applyChanges( QSqlTableModel* model )
	{
		_db.transaction();
		if( model->submitAll() )
			return _db.commit();
		else
		{
			_db.rollback();
			QMessageBox::warning(
						0,
						tr( "Cannot apply changes" ),
						tr( "An error occurred: %1" ).arg( model->lastError().text() )
			);
		}

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
		tableModel( table )->revertAll();
	}
	//-------------------------------------------------------------------------
	bool Project::applyAllChanges()
	{
		foreach( QSqlTableModel* m, _models )
		{
			if( !applyChanges( m ) )
				return false;
		}

		return true;
	}
}
