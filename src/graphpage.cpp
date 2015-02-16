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
 * graphpage.cpp
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
#include "graphpage.h"
#include "ui_graphpage.h"
#include "graph.h"
#include "project.h"
#include "helpers.h"

namespace depgraphV
{
	GraphPage::GraphPage( MainWindow* win, SettingsDialog* parent )
		: SettingsPage( win, parent ),
		_ui( new Ui::GraphPage )
	{
		_ui->setupUi( this );

		_ui->layoutAlgorithm->setProperty( "type", "QComboBox" );
		_ui->layoutAlgorithm->setProperty( "group", "layoutAlgorithm" );

		_ui->highQualityAA->setProperty( "type", "QCheckBox" );
		_ui->highQualityAA->setProperty( "group", "highQualityAA" );

		_ui->rendererType->setProperty( "type", "QComboBox" );
		_ui->rendererType->setProperty( "group", "rendererType" );

		//Create widgets from graphAttribs.xml resource
		QString name, group, link;
		QWidget* w = 0;
		QXmlStreamReader xml( Helpers::LoadTextFromResources( "graphAttribs.xml" ) );

		while( !xml.isEndDocument() && !xml.hasError() )
		{
			QString tagName = xml.name().toString();

			if( xml.isStartElement() )
			{
				if( tagName == "attribute" )
					_parseAttributeTag( xml, &name, &group, &link );

				else if( tagName == "content" )
				{
					w = _parseWidget( xml );
					w->setProperty( "name", name );
					w->setProperty( "group", group );

					QWidget* parent = 0;

					if( group == "graph" )
						parent = _ui->graphAttribsGroupBox;

					else if( group == "vertex" )
						parent = _ui->vertAttribsGroupBox;

					else if( group == "edge" )
						parent = _ui->edgeAttribsGroupBox;

					w->setParent( parent );
					QLabel* l = new QLabel( parent );
					l->setText( name );

					QGridLayout* layout = static_cast<QGridLayout*>( parent->layout() );
					if( !layout )
						layout = new QGridLayout( parent );

					QPushButton* b = new QPushButton( parent );
					b->setProperty( "link", link );
					b->setMaximumWidth( 30 );
					b->setMaximumHeight( 30 );
					b->setFlat( true );
					b->setIcon( QIcon( ":/toolBarIcons/help_24x24.png" ) );
					connect( b, SIGNAL( clicked() ),
							 this, SLOT( _onHelpRequested() )
					);

					layout->addWidget( l, layout->rowCount(), 0, 1, 1 );
					layout->addWidget( w, layout->rowCount() - 1, 3, 1, 1 );
					layout->addWidget( b, layout->rowCount() - 1, 4, 1, 1 );

	#ifndef QT_NO_SHORTCUT
					l->setBuddy( w );
	#endif // QT_NO_SHORTCUT
					_runtimeWidgets << w;
				}
				else if( tagName == "tooltips" )
				{
					xml.readNext();
					QString langName;
					QMap<QString, QString>* tooltips = new QMap<QString, QString>();
					while( !xml.isEndElement() || xml.name() != "tooltips" )
					{
						if( xml.isStartElement() )
							langName = xml.name().toString();

						else if( xml.isEndElement() )
							langName = "";

						else if( ( xml.isCharacters() || xml.isCDATA() ) && !langName.isEmpty() )
						{
							QString tooltip = xml.text().toString().trimmed();
							if( !tooltip.isEmpty() )
								tooltips->insert( langName, tooltip );
						}
						xml.readNext();
					}
					w->setProperty( "tooltips", Helpers::toQVariant( tooltips ) );
					continue;
				}
			}

			xml.readNext();
		}
		if( xml.hasError() )
		{
			//TODO
		}
		xml.clear();
		_retranslate();

		QStringList* lPlugins = Graph::pluginsListByKind( "layout" );
		if( lPlugins )
			_ui->layoutAlgorithm->addItems( *lPlugins );
	}
	//-------------------------------------------------------------------------
	GraphPage::~GraphPage()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	QString GraphPage::iconPath() const
	{
		return ":/settingsDlgIcons/graph-settings_96x96.png";
	}
	//-------------------------------------------------------------------------
	void GraphPage::commitChanges()
	{
		QMap<Graph*, QList<QWidget*> >::iterator it = _uncommittedChanges.begin();
		while( it != _uncommittedChanges.end() )
		{
			Graph* g = it.key();
			foreach( QWidget* w, it.value() )
			{
				QString type = w->property( "type" ).toString();
				QString group = w->property( "group" ).toString();

				QVariant newValue;

				if( type == "QComboBox" )
				{
					QComboBox* c = static_cast<QComboBox*>( w );
					newValue = c->currentText();
				}
				else if( type == "QDoubleSpinBox" )
				{
					QDoubleSpinBox* dSpin = static_cast<QDoubleSpinBox*>( w );
					newValue = QString::number( dSpin->value() );
				}
				else if( type == "QSpinBox" )
				{
					QSpinBox* spin = static_cast<QSpinBox*>( w );
					newValue = QString::number( spin->value() );
				}
				else if( type == "QCheckBox" )
				{
					QCheckBox* check = static_cast<QCheckBox*>( w );
					newValue = check->isChecked();
				}

				if( group == "graph" || group == "vertex" || group == "edge" )
				{
					QString name = w->property( "name" ).toString();

					if( group == "graph" )
						g->setGraphAttribute( name, newValue.toString() );

					else if( group == "vertex" )
						g->setVerticesAttribute( name, newValue.toString() );

					else if( group == "edge" )
						g->setEdgesAttribute( name, newValue.toString() );
				}

				else if( group == "layoutAlgorithm" )
					g->setLayoutAlgorithm( newValue.toString() );

				else if( group == "highQualityAA" )
					g->setHighQualityAA( newValue.toBool() );

				else if( group == "rendererType" )
				{
					Graph::RendererType rType;
					bool result = Helpers::QStringToEnum<Graph::RendererType>(
								Graph::staticMetaObject,
								"RendererType",
								newValue.toString(),
								&rType
					);
					if( result )
						g->setRenderer( rType );
					else
						QMessageBox::warning(
									parentWidget(),
									tr( "An error occurred"),
									tr( "Cannot apply renderer changes!" )
						);
				}
			}
			it++;
		}

		_uncommittedChanges.clear();
	}
	//-------------------------------------------------------------------------
	void GraphPage::revertChanges()
	{
		_uncommittedChanges.clear();
	}
	//-------------------------------------------------------------------------
	void GraphPage::changeEvent( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
		{
			_ui->retranslateUi( this );
			_retranslate();
		}

		QWidget::changeEvent( evt );
	}
	//-------------------------------------------------------------------------
	void GraphPage::onProjectOpened( Project* p )
	{
		SettingsPage::onProjectOpened( p );

		p->addMapping( _ui->layoutAlgorithm,
					   "layoutAlgorithm",
					   "",
					   Graph::defaultLayoutAlgorithm()
		);
		p->addMapping( _ui->highQualityAA,
					   "highQualityAA",
					   "",
					   Graph::defaultHighQualityAA()
		);
		p->addMapping( _ui->rendererType,
					   "rendererType",
					   "",
					   Graph::defaultRendererString()
		);

		foreach( QWidget* w, _runtimeWidgets )
		{
			QString prefix = w->property( "group" ).toString() + "_";
			QString name = w->property( "name" ).toString();
			p->addMapping( w, name, prefix, w->property( "default" ) );
		}
	}
	//-------------------------------------------------------------------------
	void GraphPage::onProjectClosed()
	{
		//TODO
		SettingsPage::onProjectClosed();
	}
	//-------------------------------------------------------------------------
	void GraphPage::_onHelpRequested() const
	{
		QDesktopServices::openUrl( QUrl( sender()->property( "link" ).toString() ) );
	}
	//-------------------------------------------------------------------------
	void GraphPage::_onValueChanged()
	{
		Project* p = Singleton<Project>::instancePtr();
		Graph* g = p->currentGraph();
		if( !_uncommittedChanges.contains( g ) )
			_uncommittedChanges.insert( g, QList<QWidget*>() );

		QWidget* s = static_cast<QWidget*>( sender() );
		if( !_uncommittedChanges[ g ].contains( s ) )
			_uncommittedChanges[ g ].append( s );
	}
	//-------------------------------------------------------------------------
	void GraphPage::_retranslate()
	{
#ifndef QT_NO_TOOLTIP
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		typedef QMap<QString, QString> tMap;

		foreach( QWidget* w, _runtimeWidgets )
		{
			tMap* tooltips = Helpers::fromQVariant<tMap>( w->property( "tooltips" ) );

			if( tooltips->contains( c->language() ) )
				w->setToolTip( ( *tooltips )[ c->language() ] );
		}
#endif
	}
	//-------------------------------------------------------------------------
	void GraphPage::_parseAttributeTag( const QXmlStreamReader& xml, QString* name,
										QString* group, QString* link ) const
	{
		QXmlStreamAttributes attributes = xml.attributes();

		//An attribute tag must contains all of the following attributes
		Q_ASSERT( attributes.hasAttribute( "name" ) );
		Q_ASSERT( attributes.hasAttribute( "group" ) );
		Q_ASSERT( attributes.hasAttribute( "link" ) );

		*name = attributes.value( "name" ).toString();
		*group = attributes.value( "group" ).toString();
		*link = QString( "http://www.graphviz.org/doc/info/attrs.html#%1" ).arg(
					attributes.value( "link" ).toString()
		);

		//checking for a valid group value
		Q_ASSERT( *group == "graph" || *group == "vertex" || *group =="edge" );
	}
	//-------------------------------------------------------------------------
	QWidget* GraphPage::_parseWidget( QXmlStreamReader& xml ) const
	{
		Q_ASSERT( xml.tokenType() != QXmlStreamReader::Characters );
		QWidget* w = 0;

		QXmlStreamAttributes attributes = xml.attributes();

		Q_ASSERT( attributes.hasAttribute( "type" ) );
		QString type = attributes.value( "type" ).toString();

		while( !xml.isCharacters() )
			xml.readNext();

		QStringList data = xml.text().toString().split( ',', QString::SkipEmptyParts );

		if( type == "QComboBox" )
		{
			QComboBox* c = new QComboBox();
			c->insertItems( 0, data );
			w = c;
			connect( c, SIGNAL( activated( int ) ),
					 this, SLOT( _onValueChanged() )
			);
		}
		else if( type == "QDoubleSpinBox" )
		{
			QMap<QString, double> values = _toNameValuePairs<double>( data );

			QDoubleSpinBox* dSpin = new QDoubleSpinBox();
			if( values.contains( "min" ) )
				dSpin->setMinimum( values[ "min" ] );

			if( values.contains( "max" ) )
				dSpin->setMaximum( values[ "max" ] );

			if( values.contains( "step" ) )
				dSpin->setSingleStep( values[ "step" ] );

			w = dSpin;
			connect( dSpin, SIGNAL( editingFinished() ),
					 this, SLOT( _onValueChanged() )
			);
		}
		else if( type == "QSpinBox" )
		{
			QMap<QString, int> values = _toNameValuePairs<int>( data );

			QSpinBox* spin = new QSpinBox();
			if( values.contains( "min" ) )
				spin->setMinimum( values[ "min" ] );

			if( values.contains( "max" ) )
				spin->setMaximum( values[ "max" ] );

			if( values.contains( "step" ) )
				spin->setSingleStep( values[ "step" ] );

			w = spin;
			connect( spin, SIGNAL( editingFinished() ),
					 this, SLOT( _onValueChanged() )
			);
		}

		Q_ASSERT( w );
		w->setProperty( "type", type );
		if( attributes.hasAttribute( "default" ) )
		{
			QString defValue = attributes.value( "default" ).toString();
			w->setProperty( "default", defValue );
		}
		return w;
	}
	//-------------------------------------------------------------------------
	template<typename T>
	QMap<QString, T> GraphPage::_toNameValuePairs( const QStringList& data ) const
	{
		QMap<QString, T> values;
		for( int i = 0; i < data.length(); i++ )
		{
			QStringList split = data[ i ].split( '=', QString::SkipEmptyParts );
			Q_ASSERT( split.length() == 2 );
			T d;
			bool ok = Helpers::QStringToType<T>( split[ 1 ].trimmed(), &d );
			Q_ASSERT( ok );
			values.insert( split[ 0 ].trimmed(), d );
		}

		return values;
	}
} // end of depgraphV namespace
