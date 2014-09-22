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
#include "graph.h"

#include <QComboBox>

namespace depgraphV
{
	GraphPage::GraphPage( SettingsDialog* parent )
		: SettingsPage( parent ),
		_ui( new Ui::GraphPage )
	{
		_ui->setupUi( this );

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
	bool GraphPage::mapData()
	{
		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* model = p->tableModel( "graphSettings" );
		if( !model )
			return false;

		QDataWidgetMapper* dataMapper = new QDataWidgetMapper( this );
		dataMapper->setModel( model );
		dataMapper->setItemDelegate( new ComboBoxItemDelegate( this ) );

		int nameIndex = model->fieldIndex( "name" );
		_ui->graphName->setModel( model );
		_ui->graphName->setModelColumn( nameIndex );

		dataMapper->addMapping( _ui->layoutAlgorithm, model->fieldIndex( "layoutAlgorithm" ) );

		//Rendering options
		dataMapper->addMapping( _ui->highQualityAA, model->fieldIndex( "highQualityAA" ) );
		dataMapper->addMapping( _ui->rendererType, model->fieldIndex( "RendererType" ) );

		//Graph attributes
		dataMapper->addMapping( _ui->splines, model->fieldIndex( "splines" ) );
		dataMapper->addMapping( _ui->nodesep, model->fieldIndex( "nodesep" ) );

		//Vertices attributes
		dataMapper->addMapping( _ui->shape, model->fieldIndex( "shape" ) );
		dataMapper->addMapping( _ui->vert_style, model->fieldIndex( "vert_style" ) );

		//Edges attributes
		dataMapper->addMapping( _ui->minlen, model->fieldIndex( "minlen" ) );
		dataMapper->addMapping( _ui->edge_style, model->fieldIndex( "edge_style" ) );
		dataMapper->toFirst();

		connect( _ui->graphName, SIGNAL( currentIndexChanged( int ) ),
				 dataMapper, SLOT( setCurrentIndex( int ) )
		);

		return true;
	}
	//-------------------------------------------------------------------------
	void GraphPage::changeEvent( QEvent* evt )
	{
		if( evt && evt->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QWidget::changeEvent( evt );
	}
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	ComboBoxItemDelegate::ComboBoxItemDelegate( QObject* parent )
		: QItemDelegate( parent )
	{
	}
	//-------------------------------------------------------------------------
	void ComboBoxItemDelegate::setEditorData( QWidget* editor,
										  const QModelIndex& index ) const
	{
		QComboBox* combo = qobject_cast<QComboBox*>( editor );
		if( combo )
			combo->setCurrentIndex( combo->findText( index.data().toString() ) );
		else
			QItemDelegate::setEditorData( editor, index );
	}
	//-------------------------------------------------------------------------
	void ComboBoxItemDelegate::setModelData( QWidget* editor,QAbstractItemModel* model,
										 const QModelIndex& index ) const
	{
		QComboBox* combo = qobject_cast<QComboBox*>( editor );
		if( combo )
			model->setData( index, combo->currentText() );
		else
			QItemDelegate::setModelData( editor, model, index );
	}
} // end of depgraphV namespace
