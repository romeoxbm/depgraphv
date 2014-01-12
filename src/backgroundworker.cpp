/**
 * backgroundworker.cpp
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
#include "backgroundworker.h"
#include "mainwindow.h"
#include <QDir>

namespace depgraphV
{
	BackgroundWorker::BackgroundWorker( MainWindow* win, QWidget* parent )
		: QThread( parent ),
		_win( win ),
		_abortQueued( false )
	{
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void BackgroundWorker::run()
	{
		_scanFolder( _win->rootPath() );
		emit scanFinished();

		if( _checkForAbortOnRun() )
			return;

		if( _filesFound.count() > 0 )
		{
			if( _checkForAbortOnRun() )
				return;

			_createGraph();

			if( _checkForAbortOnRun() )
				return;

			if( !_win->graph()->applyLayout() )
				emit fail( tr( "Unable to render; Plugin not found." ) );
		}
		else
			emit fail( tr( "No file has been found in selected folder; nothing to draw." ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void BackgroundWorker::on_abort()
	{
		_abortQueued = true;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void BackgroundWorker::_scanFolder( const QString& root )
	{
		if( _abortQueued )
			return;
		
		QDir dir( root );

		//Iterating subfolders, if required
		if( _win->isRecursiveScanEnabled() )
		{
			foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs ) )
			{
				if( _abortQueued )
					return;

				emit directoryFound();
				_scanFolder( entry.filePath() );
			}
		}

		//Iterating files
		dir.setNameFilters( _win->nameFilters() );
		foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files ) )
		{
			if( _abortQueued )
				return;

			_filesFound.append( qMakePair( entry.absolutePath(), entry.fileName() ) );
			emit fileFound( entry.fileName() );
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void BackgroundWorker::_createGraph()
	{
		for( int i = 0; i < _filesFound.count(); ++i )
		{
			if( _abortQueued )
				return;

			QString path = _filesFound[ i ].first;
			QString file = _filesFound[ i ].second;
			_win->graph()->createOrRetrieveVertex( file );
			_win->graph()->createEdges( path, file );

			emit fileAnalized( file );
		}

		emit graphCreated();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool BackgroundWorker::_checkForAbortOnRun()
	{
		if( !_abortQueued )
			return false;

		emit fail( tr( "Aborted." ) );
		return true;
	}
}
