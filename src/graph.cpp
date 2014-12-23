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
 * graph.cpp
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
#include "depgraphv_pch.h"
#include "graph.h"
#include "helpers.h"

#define G_STR( str ) str.toUtf8().data()

#ifdef GraphViz_USE_CGRAPH
#	define NEW_GRAPH()	_graph = agopen( G_STR( QString( "" ) ), Agdirected, 0 )
#else
#	define NEW_GRAPH()	_graph = agopen( G_STR( QString( "" ) ), AGDIGRAPH )
#endif

namespace depgraphV
{
	GVC_t* Graph::_context = 0;
	QMap<QString, QStringList*> Graph::_availablePlugins;
	QMap<QString, QStringList*> Graph::_parsedFiles;

	unsigned short Graph::_instances = 0;

	Graph::Graph( QWidget* parent )
		: QGraphicsView( parent ),
		  _svgItem( 0 ),
		  _drawn( false ),
		  _foldersModel( new FoldersModel( this ) )
	{
		_instances++;

		if( !_context )
			_context = gvContext();

		//TODO After last changes, Do I still need these following two lines?
		/*qRegisterMetaType<NameValuePair>( "NameValuePair" );
		qRegisterMetaTypeStreamOperators<NameValuePair>( "NameValuePair" );*/

		NEW_GRAPH();

		setScene( new QGraphicsScene( this ) );
		_setDefaultAttributes();

		//TODO Following code line needs to be tested
		setResizeAnchor( QGraphicsView::AnchorUnderMouse );
	}
	//-------------------------------------------------------------------------
	Graph::~Graph()
	{
		_instances--;
		clearGraph();

		_graphAttributes.clear();
		_verticesAttributes.clear();
		_edgesAttributes.clear();

		if( !_instances )
		{
			foreach( QStringList* l, _availablePlugins )
				delete l;

			foreach( QStringList* l, _parsedFiles )
				delete l;

			_availablePlugins.clear();

			gvFreeContext( _context );
			_context = 0;
		}

		agclose( _graph );
		_graph = 0;
	}
	//-------------------------------------------------------------------------
	void Graph::setRenderer( RendererType type )
	{
		_renderer = type;

		if( _renderer == OpenGL )
		{
#ifdef DEPGRAPHV_USE_OPENGL
			setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers ) ) );
#endif
		}
		else
			setViewport( new QWidget );
	}
	//-------------------------------------------------------------------------
	bool Graph::highQualityAA() const
	{
#ifdef DEPGRAPHV_USE_OPENGL
		return ( renderHints() & QPainter::HighQualityAntialiasing ) ==
				QPainter::HighQualityAntialiasing;
#else
		return false;
#endif
	}
	//-------------------------------------------------------------------------
	void Graph::setHighQualityAA( bool highQualityAntialiasing )
	{
#ifdef DEPGRAPHV_USE_OPENGL
		setRenderHint( QPainter::HighQualityAntialiasing, highQualityAntialiasing );
#else
		Q_UNUSED( highQualityAntialiasing );
#endif
	}
	//-------------------------------------------------------------------------
	const QString& Graph::layoutAlgorithm() const
	{
		return _layoutAlgorithm;
	}
	//-------------------------------------------------------------------------
	void Graph::setLayoutAlgorithm( const QString& value )
	{
		_layoutAlgorithm = value;
	}
	//-------------------------------------------------------------------------
	QString Graph::graphAttribute( const QString& name ) const
	{
		if( _graphAttributes.contains( name ) )
			return _graphAttributes[ name ];

		return "";
	}
	//-------------------------------------------------------------------------
	QString Graph::verticesAttribute( const QString& name ) const
	{
		if( _verticesAttributes.contains( name ) )
			return _verticesAttributes[ name ];

		return "";
	}
	//-------------------------------------------------------------------------
	QString Graph::edgesAttribute( const QString& name ) const
	{
		if( _edgesAttributes.contains( name ) )
			return _edgesAttributes[ name ];

		return "";
	}
	//-------------------------------------------------------------------------
	void Graph::setGraphAttribute( const QString& name, const QString& value )
	{
		Q_ASSERT( !name.isEmpty() && "Attribute name is empty!" );
		Q_ASSERT( !value.isEmpty() && "Attribute value is empty!" );
		_graphAttributes[ name ] = value;

#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGRAPH, G_STR( name ), G_STR( value ) );
#else
		agraphattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//-------------------------------------------------------------------------
	void Graph::setVerticesAttribute( const QString& name, const QString& value )
	{
		Q_ASSERT( !name.isEmpty() && "Attribute name is empty!" );
		Q_ASSERT( !value.isEmpty() && "Attribute value is empty!" );
		_verticesAttributes[ name ] = value;

#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGNODE, G_STR( name ), G_STR( value ) );
#else
		agnodeattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//-------------------------------------------------------------------------
	void Graph::setEdgesAttribute( const QString& name, const QString& value )
	{
		Q_ASSERT( !name.isEmpty() && "Attribute name is empty!" );
		Q_ASSERT( !value.isEmpty() && "Attribute value is empty!" );
		_edgesAttributes[ name ] = value;

#ifdef GraphViz_USE_CGRAPH
		agattr( _graph, AGEDGE, G_STR( name ), G_STR( value ) );
#else
		agedgeattr( _graph, G_STR( name ), G_STR( value ) );
#endif
	}
	//-------------------------------------------------------------------------
	Agnode_t* Graph::createVertex( const QString& label )
	{
		Q_ASSERT( !label.isEmpty() && "Label cannot be empty!" );
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
		emit vertexCreated( v );
		return v;
	}
	//-------------------------------------------------------------------------
	Agnode_t* Graph::createOrRetrieveVertex( const QString& label )
	{
		if( !_vertices.contains( label ) )
			this->createVertex( label );

		return vertex( label );
	}
	//-------------------------------------------------------------------------
	Agnode_t* Graph::vertex( const QString& label )
	{
		Q_ASSERT( !label.isEmpty() && "Label cannot be empty!" );
		if( _vertices.contains( label ) )
			return _vertices[ label ];

		return 0;
	}
	//-------------------------------------------------------------------------
	void Graph::createEdges( const QString& absPath, const QString& vertexLabel )
	{
		Q_ASSERT( !absPath.isEmpty() && "Path is empty!" );
		QString absFilePath = QString( "%1/%2" ).arg( absPath, vertexLabel );

		//TODO By doing this way, any future change in already parsed files
		//will be undetectable.
		if( !_parsedFiles.contains( absFilePath ) )
		{
			QFile f( absFilePath );
			if( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
				return;

			_parsedFiles.insert( absFilePath, new QStringList );

			QTextStream stream( &f );
			QString fileContent = stream.readAll();
			f.close();

			//Before parsing includes, we remove every comment;
			//By this way, commented include statements will not match anymore.
			QString includeReg( "(//[^\\r\\n]*)|(/\\*([^*]"
						 "|[\\r\\n]|(\\*+([^*/]|[\\r\\n])))*\\*+/)"
			);
			fileContent.remove( QRegExp( includeReg ) );

			QRegExp rExp( "#\\s*include\\s*((<[^>]+>)|(\"[^\"]+\"))" );
			int pos = 0;

			while( ( pos = rExp.indexIn( fileContent, pos ) ) != -1 )
			{
				QString match( rExp.cap( 1 ) );
				QString currentInclude( match.mid( 1, match.length() - 2 ) );
				_parsedFiles[ absFilePath ]->append( currentInclude );
				pos += rExp.matchedLength();
			}
		}

		Agnode_t* src = createOrRetrieveVertex( vertexLabel );
		foreach( QString inc, *_parsedFiles[ absFilePath ] )
			_createEdge( src, createOrRetrieveVertex( inc ) );
	}
	//-------------------------------------------------------------------------
	bool Graph::applyLayout()
	{
		Q_ASSERT( !_svgItem );
		if( !_isPluginAvailable( _layoutAlgorithm, "layout" ) ||
				!_isPluginAvailable( "svg", "render" ) )
		{
			return false;
		}

		if( gvLayout( _context, _graph, G_STR( _layoutAlgorithm ) ) != 0 )
		{
			QMessageBox::critical(
				this->parentWidget(),
				tr( "Layout render error" ),
				QString::fromUtf8( aglasterr() )
			);

			return false;
		}

		emit layoutApplied();

		QString data;
		if( !_renderDataAs( _graph, "svg", &data ) )
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
		_drawn = true;
		return true;
	}
	//-------------------------------------------------------------------------
	bool Graph::saveImage( const QString& filename, const QString& format ) const
	{
		Q_ASSERT( !filename.isEmpty() && "filename cannot be empty!" );
		Q_ASSERT( !format.isEmpty() && "format cannot be empty!" );
		bool result = false;

		if( _isPluginAvailable( format, "loadimage" ) )
		{
			result = gvRenderFilename(
						 _context,
						 _graph,
						 G_STR( format ),
						 G_STR( filename )
			) == 0;

			if( !result )
			{
				QMessageBox::critical(
					this->parentWidget(),
					tr( "Save as image" ),
					tr( "Unable to save file;\n" ) + QString::fromUtf8( aglasterr() )
				);
			}
		}

		return result;
	}
	//-------------------------------------------------------------------------
	bool Graph::saveDot( const QString& filename ) const
	{
		bool retValue = false;
		QFile f( filename );
		if( !f.open( QIODevice::WriteOnly | QIODevice::Text ) )
			return retValue;

		QString data;
		if( _isPluginAvailable( "dot", "render" ) && _renderDataAs( _graph, "dot", &data ) )
		{
			QTextStream stream( &f );
			stream << data;
			retValue = true;
		}

		f.close();
		return retValue;
	}
	//-------------------------------------------------------------------------
	QStringList* Graph::pluginsListByKind( const QString& kind )
	{
		Q_ASSERT( !kind.isEmpty() && "Empty plugin kind!" );
		_lookForAvailablePlugins();

		if( _availablePlugins.contains( kind ) )
			return _availablePlugins[ kind ];

		return 0;
	}
	//-------------------------------------------------------------------------
	void Graph::clearLayout()
	{
		if( _svgItem )
		{
			this->scene()->removeItem( _svgItem );
			delete _svgItem;
			_svgItem = 0;
		}

		gvFreeLayout( _context, _graph );
		_drawn = false;
	}
	//-------------------------------------------------------------------------
	void Graph::clearGraph()
	{
		clearLayout();
		_vertices.clear();
		agclose( _graph );
		NEW_GRAPH();
		_restoreAttributes();
	}
	//-------------------------------------------------------------------------
	void Graph::wheelEvent( QWheelEvent* event )
	{
		qreal factor = qPow( 1.2, event->delta() / 240.0 );
		scale( factor, factor );
		event->accept();
	}
	//-------------------------------------------------------------------------
	Agedge_t* Graph::_createEdge( Agnode_t* src, Agnode_t* dest, const QString& label )
	{
		Q_ASSERT( src && dest && "src and dst vertices cannot be null!" );
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

		emit edgeCreated( e );
		return e;
	}
	//-------------------------------------------------------------------------
	bool Graph::_renderDataAs( Agraph_t* graph, const QString& format, QString* outString )
	{
		unsigned int length;
		char* rawData = 0;
		bool retValue = gvRenderData(
							_context,
							graph,
							G_STR( format ),
							&rawData,
							&length
		) == 0;
		retValue = retValue && rawData;

		if( retValue )
			*outString = QString::fromUtf8( rawData, length );

		return retValue;
	}
	//-------------------------------------------------------------------------
	void Graph::_restoreAttributes()
	{
		NameValuePair::iterator i = _graphAttributes.begin();
		for( ; i != _graphAttributes.end(); i++ )
			setGraphAttribute( i.key(), i.value() );

		i = _verticesAttributes.begin();
		for( ; i != _verticesAttributes.end(); i++ )
			setVerticesAttribute( i.key(), i.value() );

		i = _edgesAttributes.begin();
		for( ; i != _edgesAttributes.end(); i++ )
			setEdgesAttribute( i.key(), i.value() );
	}
	//-------------------------------------------------------------------------
	void Graph::_setDefaultAttributes()
	{
		_graphAttributes.clear();
		_verticesAttributes.clear();
		_edgesAttributes.clear();

		setHighQualityAA( Graph::defaultHighQualityAA() );
		setRenderer( Graph::defaultRenderer() );
		setLayoutAlgorithm( Graph::defaultLayoutAlgorithm() );

		//Setting default graph attributes
		setGraphAttribute( "splines", "spline" );
		setGraphAttribute( "nodesep", "0.4" );

		setVerticesAttribute( "shape", "box" );
		setVerticesAttribute( "style", "rounded" );

		setEdgesAttribute( "minlen", "3" );
		setEdgesAttribute( "style", "solid" );
	}
	//-------------------------------------------------------------------------
	void Graph::_lookForAvailablePlugins()
	{
		if( !_availablePlugins.empty() )
			return;

		qDebug() << qPrintable( tr( "Looking for available GraphViz plugins..." ) );

#ifdef GraphViz_USE_CGRAPH
		int size;
		QString kinds[ 5 ] = { "render", "layout", "textlayout", "device", "loadimage" };
		for( int k = 0; k < 5; ++k )
		{
			QString debugList;

			qDebug() << qPrintable( tr( "Plugins for kind \"%1\":" ).arg( kinds[ k ] ) );
			_availablePlugins.insert( kinds[ k ], new QStringList() );

			if( !_context )
				_context = gvContext();

			char** list = gvPluginList(
							  _context,
							  const_cast<char*>( kinds[ k ].toStdString().c_str() ),
							  &size,
							  0
			);
			for( int i = 0; i < size; ++i )
			{
				QString currentPlugin( list[ i ] );
				if( currentPlugin.contains( QRegExp( "[^a-zA-Z0-9]" ) ) )
					continue;

				_availablePlugins[ kinds[ k ] ]->append( currentPlugin );

				QString format = i < size - 1 ? "%1, " : "%1";
				debugList += QString( format ).arg( currentPlugin );
			}

			qDebug() << qPrintable( tr( "Plugins found:" ) );
			qDebug() << qPrintable( debugList + "\n" );
		}
#else
		//TODO
		//Older versions of GraphViz (2.26 for instance) doesn't have gvPluginList function.
		//Following code is just a workaround, until I get a better and cleaner solution..
		_availablePlugins.insert( "render", new QStringList );
		_availablePlugins[ "render" ]->append( "dot" );
		_availablePlugins[ "render" ]->append( "svg" );

		_availablePlugins.insert( "layout", new QStringList );
		_availablePlugins[ "layout" ]->append( "dot" );

		_availablePlugins.insert( "loadimage", new QStringList );
		_availablePlugins[ "loadimage" ]->append( "svg" );
		_availablePlugins[ "loadimage" ]->append( "png" );
		_availablePlugins[ "loadimage" ]->append( "jpeg" );
		_availablePlugins[ "loadimage" ]->append( "bmp" );

#endif
	}
	//-------------------------------------------------------------------------
	bool Graph::_isPluginAvailable( const QString& format, const QString& kind )
	{
		if( _availablePlugins.empty() )
			return false;

		if( kind.isEmpty() )
		{
			foreach( QStringList* l, _availablePlugins )
			{
				if( l->contains( format ) )
					return true;
			}

			return false;
		}
		else if( _availablePlugins.contains( kind ) )
			return _availablePlugins[ kind ]->contains( format );

		return false;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator << ( QDataStream& out, const Graph& object )
	{
		out << static_cast<short>( object.renderer() );
		out << object.layoutAlgorithm();

		out << object.highQualityAA();
		out << object._graphAttributes;
		out << object._verticesAttributes;
		out << object._edgesAttributes;
		out << *object._foldersModel;

		return out;
	}
	//-------------------------------------------------------------------------
	QDataStream& operator >> ( QDataStream& in, Graph& object )
	{
		short r;
		in >> r;
		object.setRenderer( static_cast<Graph::RendererType>( r ) );

		in >> object._layoutAlgorithm;

		bool aa;
		in >> aa;
		object.setHighQualityAA( aa );

		in >> object._graphAttributes;
		in >> object._verticesAttributes;
		in >> object._edgesAttributes;
		in >> *object._foldersModel;

		return in;
	}
} // end of depgraphV namespace
