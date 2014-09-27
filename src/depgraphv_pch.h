/**
 * depgraphv_pch.h
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
#ifndef DEPGRAPHV_PCH_H
#define DEPGRAPHV_PCH_H

#include "buildsettings.h"

//Qt Includes
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <qglobal.h>
#include <QGraphicsSvgItem>
#include <QGraphicsView>
#include <QImageReader>
#include <QInputDialog>
#include <QItemDelegate>
#include <QLibraryInfo>
#include <QList>
#include <QListView>
#include <QListWidgetItem>
#include <QMainWindow>
#include <qmath.h>
#include <QMessageBox>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QModelIndex>
#include <QNetworkReply>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>
#include <QStringList>
#include <QSvgRenderer>
#include <QTabBar>
#include <QTabWidget>
#include <QTextStream>
#include <QToolBar>
#include <QTranslator>
#include <QTreeView>
#include <QUrl>
#include <QVector>
#include <QWheelEvent>
#include <QWidget>
#include <QXmlStreamReader>

#ifdef DEPGRAPHV_USE_OPENGL
#	include <QGLWidget>
#endif

#ifndef QT_NO_CONCURRENT
#	ifdef DEPGRAPHV_USE_QT5
#		include <QtConcurrent>
#	else
#		include <QtCore>
#	endif
#else
//TODO Warn on missing QtConcurrent support
#endif // QT_NO_CONCURRENT

//Graphviz includes
#ifndef Q_MOC_RUN
#	include <gvc.h>
#	ifdef GraphViz_USE_CGRAPH
#		include <cgraph.h>
#		ifndef WITH_CGRAPH
#			define WITH_CGRAPH 1
#		endif
#	else
#		include <graph.h>
#	endif // Using_CGRAPH
#endif // Q_MOC_RUN

//other includes
#include <stdio.h>

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#include <functional>

#endif // DEPGRAPHV_PCH_H