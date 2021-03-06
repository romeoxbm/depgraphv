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
 * project.h
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
#ifndef PROJECT_H
#define PROJECT_H

#ifndef SINGLETON_H
#	include "singleton.h"
#endif

#ifndef CUSTOMITEMDELEGATE_H
#	include "customitemdelegate.h"
#endif

#ifndef BINARYRADIOWIDGET_H
#	include "binaryradiowidget.h"
#endif

#ifndef GRAPH_H
#	include "graph.h"
#endif

namespace depgraphV
{
	class Project : public QObject, public Singleton<Project>
	{
		Q_OBJECT

	public:
		static Project* create( QObject* parent = 0 );
		static Project* open( QObject* parent = 0, const QString& fileName = "" );

		static const QString defaultExtension;

		~Project();

		const QString& name() const { return _name; }
		const QString& path() const { return _path; }
		const QString& fullPath() const { return _fullPath; }

		/**
		 * @brief Return the project file version.
		 */
		unsigned short version() const { return _version; }

		/**
		 * @brief Return the project file size in bytes.
		 */
		qint64 size() const { return _size; }

		bool isModified() const { return _modified; }
		bool hasUnsubmittedChanges() const { return _hasUnsubmittedChanges; }

		QStandardItemModel* tableModel() const { return _model; }

		void addMapping( QWidget* w, const QString& fieldName,
						 const QString& prefix = "",
						 const QVariant& defaultValue = QVariant()
		);

		void addMapping( QWidget* w, int fieldIndex,
						 const QVariant& defaultValue = QVariant()
		);

		void addMapping( QRadioButton* radios[ 2 ],
						 const QString& fieldName,
						 const QString& prefix = "",
						 bool defaultValue = true
		);

		void addMapping( QRadioButton* radios[ 2 ],
						 int fieldIndex,
						 bool defaultValue = true
		);

		/**
		 * @brief Return the index of the field \a "prefix + fieldName", or -1
		 * if not found.
		 * @param fieldName The field name.
		 * @param prefix The field name prefix. The default value is an empty
		 * string.
		 */
		int fieldIndex( const QString& fieldName,
						const QString& prefix = ""
		) const;

		/**
		 * @brief Return the value of the field specified by
		 * \a "prefix + fieldName" at \a recordIndex.
		 * @param recordIndex Index of the record (row).
		 * @param fieldName The field name.
		 * @param prefix The field name prefix. The default value is an empty
		 * string.
		 */
		QVariant value( int recordIndex,
						const QString& fieldName,
						const QString& prefix = ""
		)const;

		/**
		 * @brief Return the value of the field specified by \a fieldIndex at
		 * \a recordIndex.
		 * @param recordIndex Index of the record (row).
		 * @param fieldIndex Field index.
		 */
		QVariant value( int recordIndex, int fieldIndex ) const;

		void setValue( const QVariant& value,
					   int recordIndex,
					   const QString& fieldName,
					   const QString& prefix = ""
		);

		void setValue( const QVariant& value,
					   int recordIndex,
					   int fieldIndex
		);

		/**
		 * @brief Return the value of the field specified by
		 * \a "prefix + fieldName" at the index of the current shown graph.
		 * @param fieldName The field name.
		 * @param prefix The field name prefix. The default value is an empty
		 * string.
		 */
		QVariant currentValue( const QString& fieldName,
							   const QString& prefix = ""
		) const;

		/**
		 * @brief Return the value of the field specified by \a fieldIndex at
		 * the index of the current shown graph.
		 * @param fieldIndex Field index.
		 */
		QVariant currentValue( int fieldIndex ) const;

		void setCurrentValue( const QVariant& value,
							  const QString& fieldName,
							  const QString& prefix = ""
		);

		void setCurrentValue( const QVariant& value, int fieldIndex );

		int recordCount() const;

		//Mapper methods
		QDataWidgetMapper* mapper() const { return _mapper; }

		int indexOf( Graph* g ) const;
		Graph* graph( int index ) const;
		Graph* currentGraph() const;

		bool load();
		bool save();
		bool saveAs( const QString& filePath = "" );

		QStringList nameFilters( FilesModel::FileGroup g = FilesModel::All );

		bool renameGraph( int index, const QString& newName );

		/**
		 * @brief Start keeping track of any change on file in \a filePath.
		 * @param The path to the file being watched.
		 */
		void watchFile( const QString& filePath );

	signals:
		void modified( bool );
		void unsubmittedChanges( bool );

		void graphCreated( const QString& name, Graph* );
		void graphRemoving( Graph* g );
		void graphRemoved( int index );
		void graphRenamed( int index, const QString& newName );
		void graphCountChanged( int count );

		void fileHasChanged( const QString& );

	public slots:
		void createGraph();
		void removeGraph( int index );

		void submitChanges();
		void revertChanges();

	private slots:
		void _onDataChanged();
		void _onFileChanged( const QString& );

	private:
		explicit Project( const QString& filePath, QObject* parent = 0 );
		void _updateProjectProperties();
		void _triggerModified( bool );
		void _triggerUnsubmittedChanges( bool );
		void _newGraph( QStandardItem* = 0 );

		QString _name;
		QString _path;
		QString _fullPath;
		qint64 _size;
		unsigned short _version;

		QHash<QString, int> _fields;
		QHash<int, QVariant> _defaultValues;

		QStandardItemModel* _model;
		CustomItemDelegate* _delegate;

		QDataWidgetMapper* _mapper;

		QVector<Graph*> _graphs;

		bool _hasUnsubmittedChanges;
		bool _modified;

		QFileSystemWatcher* _watcher;
	};
}

#endif // PROJECT_H
