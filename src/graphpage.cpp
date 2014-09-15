/**
 * graphpage.cpp
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
#include "graphpage.h"
#include "ui_graphpage.h"
#include "appconfig.h"
#include "helpers.h"

namespace depgraphV
{
	GraphPage::GraphPage( SettingsDialog* parent )
		: SettingsPage( parent ),
		_ui( new Ui::GraphPage )
	{
		_ui->setupUi( this );

		AppConfig* c = Singleton<AppConfig>::instancePtr();
		connect( c, SIGNAL( configRestored() ), this, SLOT( onConfigRestored() ) );

		QStringList* lPlugins = c->graph()->pluginsListByKind( "layout" );
		if( lPlugins )
			_ui->layoutAlgorithm->addItems( *lPlugins );

		connect( _ui->layoutAlgorithm, SIGNAL( currentIndexChanged( QString ) ),
				 c->graph(), SLOT( setLayoutAlgorithm( QString ) ) );

		//Graph attributes
		connect( _ui->splines, SIGNAL( currentIndexChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );

		connect( _ui->nodesep, SIGNAL( valueChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );

		//Vertices attributes
		connect( _ui->shape, SIGNAL( currentIndexChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );

		connect( _ui->vert_style, SIGNAL( currentIndexChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );

		//Edges attributes
		connect( _ui->minlen, SIGNAL( valueChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );

		connect( _ui->edge_style, SIGNAL( currentIndexChanged( QString ) ),
				 this, SLOT( onStringAttribChanged( QString ) ) );
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
	void GraphPage::changeEvent( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QWidget::changeEvent( evt );
	}
	//-------------------------------------------------------------------------
	void GraphPage::onConfigRestored()
	{
		Graph* g = Singleton<AppConfig>::instancePtr()->graph();
		Helpers::setCurrentText( _ui->layoutAlgorithm, g->layoutAlgorithm() );

		//Graph attributes
		Graph::NameValuePair gAttribs = g->graphAttributes();
		Helpers::setCurrentText( _ui->splines, gAttribs[ "splines" ] );
		_ui->nodesep->setValue( gAttribs[ "nodesep" ].toDouble() );

		//Vertices attributes
		Graph::NameValuePair vAttribs = g->verticesAttributes();
		Helpers::setCurrentText( _ui->shape, vAttribs[ "shape" ] );
		Helpers::setCurrentText( _ui->vert_style, vAttribs[ "style" ] );

		//Edges attributes
		Graph::NameValuePair eAttribs = g->edgesAttributes();
		Helpers::setCurrentText( _ui->edge_style, eAttribs[ "style" ] );
		_ui->minlen->setValue( eAttribs[ "minlen" ].toInt() );
	}
	//-------------------------------------------------------------------------
	void GraphPage::onStringAttribChanged( const QString& value )
	{
		Graph* g = Singleton<AppConfig>::instancePtr()->graph();
		QObject* o = sender();
		QString attrName = o->objectName();
		if( attrName.contains( '_' ) )
			attrName = attrName.split( '_' )[ 1 ];

		if( o->parent() == _ui->graphAttribsGroupBox )
			g->setGraphAttribute( attrName, value );

		else if( o->parent() == _ui->vertAttribsGroupBox )
			g->setVerticesAttribute( attrName, value );

		else if( o->parent() == _ui->edgeAttribsGroupBox )
			g->setEdgesAttribute( attrName, value );
	}
} // end of depgraphV namespace
