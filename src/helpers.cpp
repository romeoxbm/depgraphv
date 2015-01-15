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
 * helpers.cpp
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
#include "helpers.h"

namespace depgraphV
{
	QMap<QString, QString> Helpers::_readTexts;

	//TODO This method should/could be removed
	void Helpers::setCurrentText( QComboBox* combo, const QString& text )
	{
#if( QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 ) )
		combo->setCurrentText( text );
#else
		int idx = combo->findText( text );
		if( idx != -1 )
			combo->setCurrentIndex( idx );
#endif
	}
	//-------------------------------------------------------------------------
	QString Helpers::LoadTextFromResources( const QString& filename )
	{
		if( !_readTexts.contains( filename ) )
		{
			QFile f( ":/text/" + filename );
			if( !f.open( QFile::ReadOnly | QFile::Text ) )
			{
				qWarning() << qPrintable( QObject::tr( "Cannot read " ) + filename );
				return "";
			}

			QTextStream in( &f );
			in.setCodec( "UTF-8" );
			_readTexts.insert( filename, in.readAll() );
			f.close();
		}

		return _readTexts[ filename ];
	}
	//-------------------------------------------------------------------------
	bool Helpers::EnumToQString( const QMetaObject& o, const char* enumName, int value, QString* dest )
	{
		int index = o.indexOfEnumerator( enumName );
		if( index == -1 )
			return false;

		QMetaEnum e = o.enumerator( index );
		const char* val = e.valueToKey( value );
		if( !val )
			return false;

		*dest = val;
		return true;
	}
	//-------------------------------------------------------------------------
	void Helpers::insertSeparator( QAbstractItemView* view )
	{
		QAction* sep = new QAction( view );
		sep->setSeparator( true );
		view->addAction( sep );
	}
	//-------------------------------------------------------------------------
	bool Helpers::addExtension( QString& filename, const QString& ext )
	{
		if( filename.isEmpty() )
			return false;

		if( !filename.toLower().endsWith( ext.toLower() ) )
			filename += ext.startsWith( '.' ) ? ext : "." + ext;

		return true;
	}
	//-------------------------------------------------------------------------
	QString Helpers::ucFirst( const QString& value )
	{
		QStringList split = value.split( " " );
		for( int i = 0; i < split.count(); ++i )
		{
			QString& s = split[ i ];
			s[ 0 ] = s[ 0 ].toUpper();
		}

		return split.join( " " );
	}
}
