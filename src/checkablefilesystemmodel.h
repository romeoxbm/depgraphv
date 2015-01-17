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
 * checkablefilesystemmodel.h
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
#ifndef CHECKABLEFILESYSTEMMODEL_H
#define CHECKABLEFILESYSTEMMODEL_H

namespace depgraphV
{
	class CheckableFileSystemModel : public QFileSystemModel
	{
		Q_OBJECT

	public:
		explicit CheckableFileSystemModel( QObject* parent = 0 );
		virtual ~CheckableFileSystemModel() = 0;

		virtual QAbstractItemView* view() const = 0;
		virtual void setView( QAbstractItemView* ) = 0;

		Qt::ItemFlags flags( const QModelIndex& index ) const;

		bool setData( const QModelIndex& i, const QVariant& value, int role );

		bool initialized() const { return _initialized; }

	protected:
		bool _initialized;

		virtual bool isCheckable( const QModelIndex& i, int role ) const;
		virtual bool setDataImpl( const QString& path, Qt::CheckState v ) = 0;

	public slots:
		//virtual void clearSelection() {}
	};
}

#endif // CHECKABLEFILESYSTEMMODEL_H
