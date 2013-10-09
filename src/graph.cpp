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

#ifdef USE_OPENGL
#	include <QGLWidget>
#endif

#define G_STR( str ) str.toUtf8().data()

namespace depgraphV
{
	Graph::Graph( QWidget* parent )
		: QGraphicsView( parent ),
		  _renderer( Native ),
		  _svgItem( 0 ),
		  _context( 0 ),
		  _graph( 0 )
	{
		setScene( new QGraphicsScene( this ) );
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
#ifdef USE_OPENGL
			setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers ) ) );
#endif
		}
		else
			setViewport( new QWidget );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	bool Graph::highQualityAntialiasing() const
	{
#ifdef USE_OPENGL
		return ( renderHints() & QPainter::HighQualityAntialiasing ) == QPainter::HighQualityAntialiasing;
#else
		return false;
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setHighQualityAntialiasing( bool highQualityAntialiasing )
	{
#ifdef USE_OPENGL
		setRenderHint( QPainter::HighQualityAntialiasing, highQualityAntialiasing );
#else
		Q_UNUSED( highQualityAntialiasing );
#endif
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agnode_t* Graph::createVertex( const QString& label )
	{
		Q_ASSERT( _graph && !label.isEmpty() );
		Agnode_t* v = agnode( _graph, G_STR( label ), TRUE );
		if( !v )
		{
			qWarning() << tr( "Invalid vertex:" ) << label;
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
	void Graph::applyLayout()
	{
		Q_ASSERT( _context && _graph && !_svgItem );
		if( gvLayout( _context, _graph, "dot" ) != 0 )
		{
			qCritical() << tr( "Layout render error" ) << agerrors() << QString::fromUtf8( aglasterr() );
			return;
		}

		QXmlStreamReader xmlReader( _renderDataAs( "svg" ) );
		QSvgRenderer* r = new QSvgRenderer( &xmlReader );

		_svgItem = new QGraphicsSvgItem();
		_svgItem->setSharedRenderer( r );
		_svgItem->setFlags( QGraphicsItem::ItemClipsToShape );
		_svgItem->setCacheMode( QGraphicsItem::NoCache );
		_svgItem->setZValue( 0 );

		QGraphicsScene* s = scene();
		s->addItem( _svgItem );
		s->setSceneRect( _svgItem->boundingRect().adjusted( -10, -10, 10, 10 ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::saveImage( const QString& filename, const QString& format ) const
	{
		gvRenderFilename( _context, _graph, G_STR( format ), G_STR( filename ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::saveDot( const QString& filename ) const
	{
		QFile f( filename );
		if( !f.open( QIODevice::WriteOnly | QIODevice::Text ) )
		{
			QMessageBox::critical( 0, tr( "Save as dot" ), tr( "Unable to save file" ) );
			return;
		}

		QTextStream stream( &f );
		stream << _renderDataAs( "dot" );
		f.close();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::prepare()
	{
		Q_ASSERT( !_context && !_graph );
		_context = gvContext();
		_graph = agopen( G_STR( QString( "" ) ), Agdirected, 0 );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setGraphAttribute( const QString& name, const QString& value ) const
	{
		agattr( _graph, AGRAPH, G_STR( name ), G_STR( value ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setVerticesAttribute( const QString& name, const QString& value ) const
	{
		agattr( _graph, AGNODE, G_STR( name ), G_STR( value ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void Graph::setEdgesAttribute( const QString& name, const QString& value ) const
	{
		agattr( _graph, AGEDGE, G_STR( name ), G_STR( value ) );
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
	void Graph::paintEvent( QPaintEvent* event )
	{
		if( _renderer != Image )
		{
			QGraphicsView::paintEvent( event );
			return;
		}

		if( _image.size() != viewport()->size() )
			_image = QImage( viewport()->size(), QImage::Format_ARGB32_Premultiplied );

		QPainter imagePainter( &_image );
		QGraphicsView::render( &imagePainter );
		imagePainter.end();

		QPainter p( viewport() );
		p.drawImage( 0, 0, _image );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	Agedge_t* Graph::_createEdge( Agnode_t* src, Agnode_t* dest, const QString& label )
	{
		Q_ASSERT( _graph && src && dest );
		Agedge_t* e = agedge( _graph, src, dest, G_STR( label ), TRUE );
		if( !e )
		{
			qWarning() << tr( "Invalid egde:" ) << label;
			return 0;
		}

		_edges << e;
		return e;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	QString Graph::_renderDataAs( const QString& format ) const
	{
		unsigned int length;
		char* rawData = 0;
		gvRenderData( _context, _graph, G_STR( format ), &rawData, &length );
		Q_ASSERT( rawData );
		return QString::fromUtf8( rawData, length );
	}
} // end of depgraph namespace
