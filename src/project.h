/**
 * project.h
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
#ifndef PROJECT_H
#define PROJECT_H

#ifndef SINGLETON_H
#	include "singleton.h"
#endif

namespace depgraphV
{
	class Project : public QObject, public Singleton<Project>
	{
		Q_OBJECT

	public:
		static Project* createNew( const QString& filePath, QObject* parent = 0 );
		static Project* open( const QString& filePath, QObject* parent = 0 );

		~Project();

		const QString& name() const { return _name; }
		const QString& path() const { return _path; }
		const QString& fullPath() const { return _fullPath; }

		QSqlTableModel* tableModel( const QString& table, const QString& filter = "" );

		bool applyChanges( QSqlTableModel* model );
		bool applyChanges( const QString& table );

		void revertAll( const QString& table );

		bool hasPendingChanges( const QString& table = "" ) const;
		bool hasPendingChanges( QSqlTableModel* model ) const;

	signals:
		void pendingChanges( bool );

	public slots:
		bool applyAllChanges();

	private slots:
		void onDataChanged( QModelIndex, QModelIndex );

	private:
		explicit Project( const QString& filePath, QObject* parent = 0 );
		QSqlDatabase _db;
		QString _name;
		QString _path;
		QString _fullPath;
		QMap<QString, QSqlTableModel*> _models;
		QVector<QSqlTableModel*> _modifiedModels;

		void _error( const QString& dlgTitle, QSqlTableModel* model = 0 );
	};
}

#endif // PROJECT_H
