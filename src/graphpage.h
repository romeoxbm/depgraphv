/**
 * graphpage.h
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
#ifndef GRAPHPAGE_H
#define GRAPHPAGE_H

#ifndef SETTINGSPAGE_H
#	include "settingspage.h"
#endif

#ifndef PROJECT_H
#	include "project.h"
#endif

namespace depgraphV
{
	namespace Ui
	{
		class GraphPage;
	}

	class GraphPage : public SettingsPage
	{
		Q_OBJECT

	public:
		/**
		 * @brief GraphPage constructor.
		 * @param parent The parent widget (default NULL)
		 */
		explicit GraphPage( SettingsDialog* parent );

		/**
		 * @brief GraphPage destructor.
		 */
		~GraphPage();

		virtual QString iconPath() const;
		bool mapData();

	protected:
		virtual void changeEvent( QEvent* evt );

	private slots:
		void onDataChanged( QModelIndex, QModelIndex );

	private:
		Ui::GraphPage* _ui;
	};

	//TODO Maybe it should be better to move this class in a separate file
	class ComboBoxItemDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		explicit ComboBoxItemDelegate( QObject* parent = 0 );

		void setEditorData( QWidget* editor, const QModelIndex& index ) const;
		void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
	};
}

#endif // GRAPHPAGE_H
