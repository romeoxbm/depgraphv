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

#include <QComboBox>
#include <QMetaEnum>

#define C_STR( s ) s.toStdString().c_str()

namespace depgraphV
{
	class Helpers
	{
	public:
		//TODO This method should be removed
		static void setCurrentText( QComboBox* combo, const QString& text );

		static QString LoadTextFromResources( const QString& filename );

		static QString EnumToQString( const QMetaObject& o, const char* enumName, int value );

		template<typename T>
		static T QStringToEnum( const QMetaObject& o, const char* enumName, const QString& value )
		{
			QMetaEnum e = o.enumerator( o.indexOfEnumerator( enumName ) );
			return static_cast<T>( e.keyToValue( value.toStdString().c_str() ) );
		}

	private:
		Helpers(){}
		~Helpers(){}

		static QMap<QString, QString> _readTexts;
	};
}

#endif // HELPERS_H
