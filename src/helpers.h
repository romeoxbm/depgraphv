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
 * helpers.h
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
#ifndef HELPERS_H
#define HELPERS_H

#ifndef BUILDSETTINGS_H
#	include "buildsettings.h"
#endif

#ifndef DEPGRAPHV_ENABLE_PCH_SUPPORT
#	include "depgraphv_pch.h"
#endif

namespace depgraphV
{
	class Helpers
	{
	public:
		//TODO This method should be removed
		static void setCurrentText( QComboBox* combo, const QString& text );

		static QString LoadTextFromResources( const QString& filename );

		static bool EnumToQString( const QMetaObject& o, const char* enumName, int value , QString* dest );

		template<typename T>
		static bool QStringToEnum( const QMetaObject& o, const char* enumName, const QString& value, T* dest )
		{
			int index = o.indexOfEnumerator( enumName );
			if( index == -1 )
				return false;

			QMetaEnum e = o.enumerator( index );
			int val = e.keyToValue( value.toStdString().c_str() );
			if( val == -1 )
				return false;

			*dest = static_cast<T>( val );
			return true;
		}

		static void insertSeparator( QAbstractItemView* view );

		static bool addExtension( QString& filename, const QString& ext );
		static QString ucFirst( const QString& value );
		static void showInExplorer( const QString& pathIn );

		static QString pathShortener( const QString& path, ushort maxLen );

		template<typename T>
		static bool QStringToType( const QString& s, T* value, typename std::enable_if<!std::is_same<bool, T>::value, T>::type* = 0 )
		{
			static_assert( !std::is_same<QString, T>::value, "Converting from QString to QString is pointless!" );
			Q_ASSERT( !s.isEmpty() );

			std::istringstream is( s.toStdString() );
			return !( is >> *value ).fail();
		}

		template<typename T>
		static bool QStringToType( const QString& s, T* value, typename std::enable_if<std::is_same<bool, T>::value, T>::type* = 0 )
		{
			if( !s.isEmpty() )
			{
				QString lCaseVal = s.toLower();

				if( lCaseVal == "true" || lCaseVal == "yes" || lCaseVal == "1" )
				{
					*value = true;
					return true;
				}

				else if( lCaseVal == "false" || lCaseVal == "no" || lCaseVal == "0" )
				{
					*value = false;
					return true;
				}
			}

			return false;
		}

		template<typename T>
		static T* fromQVariant( const QVariant& v )
		{
			return static_cast<T*>( v.value<void*>() );
		}

		template<typename T>
		static QVariant toQVariant( T* value )
		{
			return qVariantFromValue( static_cast<void*>( value ) );
		}

	private:
		Helpers(){}
		~Helpers(){}

		static QMap<QString, QString> _readTexts;
	};
}

#endif // HELPERS_H
