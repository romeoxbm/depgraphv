/**
 * packedparameters.cpp
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
#include "depgraphv_pch.h"
#include "packedparameters.h"

namespace depgraphV
{
	PackedParameters* PackedParameters::ptr( const QVariant& v )
	{
		return static_cast<PackedParameters*>( v.value<void*>() );
	}
	//-------------------------------------------------------------------------
	QVariant PackedParameters::toQVariant( Qt::CheckState c, FilesModel::FileGroup g )
	{
		PackedParameters* p = new PackedParameters( c, g );
		return qVariantFromValue( static_cast<void*>( p ) );
	}
	//-------------------------------------------------------------------------
	PackedParameters::PackedParameters( Qt::CheckState c, FilesModel::FileGroup g )
			: _checkstate( c ),
			  _group( g )
	{
	}
}
