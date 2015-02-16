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
 * checkablefilesystemmodel.cpp
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
#include "checkablefilesystemmodel.h"

namespace depgraphV
{
	CheckableFileSystemModel::CheckableFileSystemModel( QObject* parent )
		: QFileSystemModel( parent ),
		  _initialized( false )
	{
		setReadOnly( true );
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
	bool CheckableFileSystemModel::setData( const QModelIndex& i,
											const QVariant& value, int role )
	{
		if( !isCheckable( i, role ) )
			return QFileSystemModel::setData( i, value, role );

		QString path = filePath( i );
		Qt::CheckState v = static_cast<Qt::CheckState>( value.toInt() );

		if( !setDataImpl( path, v ) )
			return QFileSystemModel::setData( i, value, role );
		else
		{
			emit dataChanged( i, i );
			return true;
		}
	}
	//-------------------------------------------------------------------------
	bool CheckableFileSystemModel::isCheckable( const QModelIndex& i, int role ) const
	{
		return i.isValid() &&
				i.column() == 0 &&
				role == Qt::CheckStateRole;
	}
}
