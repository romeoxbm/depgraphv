/**
 * graph.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 Francesco Guastella aka romeoxbm
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
#include "graph.h"
#include <QSvgRenderer>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDebug>
#include <qmath.h>

#ifdef QT_USE_OPENGL
#	include <QGLWidget>
#endif

#define G_STR( str ) str.toUtf8().data()

namespace depgraphV
{
	Graph::Graph( QWidget* parent )
		: QGraphicsView( parent ),
		  _svgItem( 0 ),
		  _context( 0 ),
		  _graph( 0 )
	{
		setScene( new QGraphicsScene( this ) );
		this->setRenderer( Native );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Graph::~Graph()
	{
		this->clear();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setRenderer( RendererType type )
	{
		_renderer = type;

		if( _renderer == OpenGL )
		{
#ifdef QT_USE_OPENGL
			setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers ) ) );
#endif
		}
		else
			setViewport( new QWidget );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::highQualityAntialiasing() const
	{
#ifdef QT_USE_OPENGL
		return ( renderHints() & QPainter::HighQualityAntialiasing ) == QPainter::HighQualityAntialiasing;
#else
		return false;
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setHighQualityAntialiasing( bool highQualityAntialiasing )
	{
#ifdef QT_USE_OPENGL
		setRenderHint( QPainter::HighQualityAntialiasing, highQualityAntialiasing );
#else
		Q_UNUSED( highQualityAntialiasing );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agnode_t* Graph::createVertex( const QString& label )
	{
		Q_ASSERT( _graph && !label.isEmpty() );
#ifdef GraphViz_USE_CGRAPH
		Agnode_t* v = agnode( _graph, G_STR( label ), TRUE );
#else
		Agnode_t* v = agnode( _graph, G_STR( label ) );
#endif
		if( !v )
		{
			qWarning() << qPrintable( tr( "Invalid vertex:" ) ) << label;
			return 0;
		}

		_vertices.insert( label, v );
		return v;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agnode_t* Graph::createOrRetrieveVertex( const QString& label )
	{
		if( !_vertices.contains( label ) )
			this->createVertex( label );

		return vertex( label );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agnode_t* Graph::vertex( const QString& label )
	{
		Q_ASSERT( !label.isEmpty() );
		if( _vertices.contains( label ) )
			return _vertices[ label ];

		return 0;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::createEdges( const QString& absPath, const QString& vertexLabel )
	{
		Q_ASSERT( !absPath.isEmpty() );
		QString absFilePath = QString( "%1/%2" ).arg( absPath, vertexLabel );

		QFile f( absFilePath );
		if( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
			return;

		QTextStream stream( &f );
		QString fileContent = stream.readAll();

		QRegExp rExp( "#\\s*include\\s*((<[^>]+>)|(\"[^\"]+\"))" );
		int pos = 0;

		Agnode_t* src = vertex( vertexLabel );
		while( ( pos = rExp.indexIn( fileContent, pos ) ) != -1 )
		{
			QString match( rExp.cap( 1 ) );
			QString currentInclude( match.mid( 1, match.length() - 2 ) );
			_createEdge( src, createOrRetrieveVertex( currentInclude ) );
			pos += rExp.matchedLength();
		}

		f.close();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::applyLayout()
	{
		Q_ASSERT( _context && _graph && !_svgItem );
		if( gvLayout( _context, _graph, "dot" ) != 0 )
		{
			QMessageBox::critical(
				this->parentWidget(),
				tr( "Layout render error" ),
				QString::fromUtf8( aglasterr() )
			);

			return false;
		}

		QString data;
		if( !_renderDataAs( "svg", &data ) )
			return false;

		QXmlStreamReader xmlReader( data );
		QSvgRenderer* r = new QSvgRenderer( &xmlReader );

		_svgItem = new QGraphicsSvgItem();
		_svgItem->setSharedRenderer( r );
		_svgItem->setFlags( QGraphicsItem::ItemClipsToShape );
		_svgItem->setCacheMode( QGraphicsItem::NoCache );
		_svgItem->setZValue( 0 );

		QGraphicsScene* s = scene();
		s->addItem( _svgItem );
		s->setSceneRect( _svgItem->boundingRect().adjusted( -10, -10, 10, 10 ) );
		return true;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::saveImage( const QString& filename, const QString& format ) const
	{
		Q_ASSERT( _context && _graph && !filename.isEmpty() && !format.isEmpty() );
		return gvRenderFilename( _context, _graph, G_STR( format ), G_STR( filename ) ) == 0;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::saveDot( const QString& filename ) const
	{
		bool retValue = false;
		QFile f( filename );
		if( !f.open( QIODevice::WriteOnly | QIODevice::Text ) )
			return retValue;

		QString data;
		if( _renderDataAs( "dot", &data ) )
		{
			QTextStream stream( &f );
			stream << data;
			retValue = true;
		}

		f.close();
		return retValue;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::prepare()
	{
		Q_ASSERT( !_context && !_graph );
		_context = gvContext();
#ifdef GraphViz_USE_CGRAPH
		_graph = agopen( G_STR( QString( "" ) ), Agdirected, 0 );
#else
		_graph = agopen( G_STR( QString( "" ) ), AGDIGRAPH );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setGraphAttribute( const QString& name, const QString& value ) const
	{
#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGRAPH, G_STR( name ), G_STR( value ) );
#else
		agraphattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setVerticesAttribute( const QString& name, const QString& value ) const
	{
#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGNODE, G_STR( name ), G_STR( value ) );
#else
		agnodeattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setEdgesAttribute( const QString& name, const QString& value ) const
	{
#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGEDGE, G_STR( name ), G_STR( value ) );
#else
		agedgeattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::clear()
	{
		if( _svgItem )
		{
			this->scene()->removeItem( _svgItem );
			delete _svgItem;
			_svgItem = 0;
		}

		if( _graph )
		{
			gvFreeLayout( _context, _graph );
			_vertices.clear();
			_edges.clear();
			agclose( _graph );
			gvFreeContext( _context );

			_graph = 0;
			_context = 0;
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::wheelEvent( QWheelEvent* event )
	{
		qreal factor = qPow( 1.2, event->delta() / 240.0 );
		scale( factor, factor );
		event->accept();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agedge_t* Graph::_createEdge( Agnode_t* src, Agnode_t* dest, const QString& label )
	{
		Q_ASSERT( _graph && src && dest );
#ifdef GraphViz_USE_CGRAPH
		Agedge_t* e = agedge( _graph, src, dest, G_STR( label ), TRUE );
#else
		Agedge_t* e = agedge( _graph, src, dest );
#endif
		if( !e )
		{
			qWarning() << qPrintable( tr( "Invalid egde:" ) ) << label;
			return 0;
		}

		_edges << e;
		return e;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::_renderDataAs( const QString& format, QString* outString ) const
	{
		unsigned int length;
		char* rawData = 0;
		bool retValue = gvRenderData( _context, _graph, G_STR( format ), &rawData, &length ) == 0;
		retValue = retValue && rawData;

		if( retValue )
			*outString = QString::fromUtf8( rawData, length );

		return retValue;
	}
} // end of depgraphV namespace
