/**
 * graph.h
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
#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QGraphicsSvgItem>

#include <gvc.h>
#include <cgraph.h>

namespace depgraphV
{
	/**
	 * @brief The Graph class
	 */
	class Graph : public QGraphicsView
	{
		Q_OBJECT
		Q_PROPERTY( RendererType RendererType READ renderer WRITE setRenderer )
		Q_PROPERTY( bool highQualityAntialiasing READ highQualityAntialiasing WRITE setHighQualityAntialiasing )

	public:
		/**
		 * @brief The RendererType enum which is used to set the render method used by Graph
		 */
		enum RendererType
		{
			Native = 0x1,
			OpenGL = 0x2,
			Image = 0x4
		};

		/**
		 * @brief Graph Contructor
		 * @param parent The parent widget (default null)
		 */
		Graph( QWidget* parent = 0 );

		/**
		 * @brief ~Graph Destructor
		 */
		~Graph();

		/**
		 * @brief renderer Get the RendererType in use.
		 * @return The renderer method used by this class.
		 */
		RendererType renderer() const { return _renderer; }

		/**
		 * @brief highQualityAntialiasing
		 * @return True if high quality antialiasing is enabled, false otherwise.
		 */
		bool highQualityAntialiasing() const;

		/**
		 * @brief createVertex Create a new graph vertex.
		 * @param label The label of the brand new vertex.
		 * @return A vertex pointer if everything went fine, 0 otherwise.
		 */
		Agnode_t* createVertex( const QString& label );

		/**
		 * @brief createOrRetrieveVertex Create a new vertex with specified label only if that
		 *        vertex wasn't previously created.
		 * @param label The vertex label.
		 */
		Agnode_t* createOrRetrieveVertex( const QString& label );

		/**
		 * @brief vertex Get a previously created graph vertex by label.
		 * @param label The label of the vertex.
		 * @return The graph vertex with specified label, or 0 if not found.
		 */
		Agnode_t* vertex( const QString& label );

		//TODO Should I need getter methods for edges?

		/**
		 * @brief createEdges Create all the outgoing edges of the graph vertex with label vertexLabel.
		 * @param absPath The absolute path to the file( required to parse includes ).
		 * @param vertexLabel The graph vertex label.
		 */
		void createEdges( const QString& absPath, const QString& vertexLabel );

		/**
		 * @brief applyLayout Calculate the graph layout.
		 */
		void applyLayout();

		/**
		 * @brief saveImage Save the graph as image.
		 * @param filename The filename where to save.
		 * @param format The image format name ("png" or "svg" for instance).
		 */
		void saveImage( const QString& filename, const QString& format ) const;

		/**
		 * @brief saveDot Save the graph as GraphViz dot file.
		 * @param filename The filename where to save.
		 */
		void saveDot( const QString& filename ) const;

		/**
		 * @brief prepare Prepare every logic structure to be popoluated before creating the graph.
		 */
		void prepare();

	signals:

	public slots:
		/**
		 * @brief setRenderer Change the render method used by this class.
		 * @param type The chosen render type.
		 * @see RendererType
		 */
		void setRenderer( RendererType type );

		/**
		 * @brief setHighQualityAntialiasing
		 * @param highQualityAntialiasing
		 */
		void setHighQualityAntialiasing( bool highQualityAntialiasing );

		/**
		 * @brief setGraphAttribute
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setGraphAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief setVerticesAttribute
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setVerticesAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief setEdgesAttribute
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setEdgesAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief clear Clear this graph.
		 */
		void clear();

	protected:
		virtual void wheelEvent( QWheelEvent* event );
		virtual void paintEvent( QPaintEvent* event );

	private:
		RendererType _renderer;
		QGraphicsSvgItem* _svgItem;
		QImage _image;

		GVC_t* _context;
		Agraph_t* _graph;

		QMap<QString, Agnode_t*> _vertices;
		QList<Agedge_t*> _edges;

		/**
		 * @brief _createEdge Create a new edge between two vertices.
		 * @param src The source vertex.
		 * @param dest The destination vertex.
		 * @param label Optional label of the new edge.
		 * @return An edge pointer if everything went fine, or 0 if an error occurred.
		 */
		Agedge_t* _createEdge( Agnode_t* src, Agnode_t* dest, const QString& label = "" );

		/**
		 * @brief _renderDataAs Helper method used to "translate" the graph into different formats.
		 * @param format The selected format.
		 * @return A string containing the graph representation in the selected format.
		 */
		QString _renderDataAs( const QString& format ) const;
	};
}

#endif // GRAPH_H
