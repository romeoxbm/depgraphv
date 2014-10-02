/**
 * helpers.h
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
#ifndef HELPERS_H
#define HELPERS_H

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

	private:
		Helpers(){}
		~Helpers(){}

		static QMap<QString, QString> _readTexts;
	};
}

#endif // HELPERS_H
