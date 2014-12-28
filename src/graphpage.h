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
		 * @brief \a GraphPage constructor.
		 * @param w \a MainWindow pointer
		 * @param parent The parent widget (default \a NULL)
		 */
		explicit GraphPage( MainWindow* w, SettingsDialog* parent = 0 );

		/**
		 * @brief \a GraphPage destructor.
		 */
		~GraphPage();

		virtual QString iconPath() const;
		virtual void commitChanges();
		virtual void revertChanges();

		virtual bool dependsOnGraphs() const { return true; }

	protected:
		virtual void changeEvent( QEvent* evt );

	private slots:
		virtual void onProjectOpened( Project* );
		virtual void onProjectClosed();

		void _onHelpRequested() const;
		void _onValueChanged();

	private:
		Ui::GraphPage* _ui;
		void _retranslate();

		QVector<QWidget*> _runtimeWidgets;
		QMap<Graph*, QList<QWidget*> > _uncommittedChanges;

		void _parseAttributeTag( const QXmlStreamReader& xml, QString* name, QString* group, QString* link ) const;
		QWidget* _parseWidget( QXmlStreamReader& xml ) const;

		template<typename T>
		QMap<QString, T> _toNameValuePairs( const QStringList& data ) const;
	};
}

#endif // GRAPHPAGE_H
