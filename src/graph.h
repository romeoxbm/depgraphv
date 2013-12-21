/**
 * graph.h
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
#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsSvgItem>
#include "buildsettings.h"

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

namespace depgraphV
{
	/**
	 * @brief The Graph class
	 */
	class Graph : public QGraphicsView
	{
		Q_OBJECT
		Q_PROPERTY( RendererType RendererType READ renderer WRITE setRenderer )
		Q_PROPERTY( bool highQualityAA READ highQualityAA WRITE setHighQualityAntialiasing )

	public:
		/**
		 * @brief The RendererType enum which is used to set the render method used by Graph
		 */
		enum RendererType
		{
			Native = 0x1,
			OpenGL = 0x2
		};

		/**
		 * @brief Graph contructor
		 * @param parent The parent widget (default NULL)
		 */
		Graph( QWidget* parent = 0 );

		/**
		 * @brief Graph Destructor.
		 */
		~Graph();

		/**
		 * @brief Get the RendererType in use.
		 * @return The renderer method used by this class.
		 */
		RendererType renderer() const { return _renderer; }

		/**
		 * @return True if high quality antialiasing is enabled, false otherwise.
		 */
		bool highQualityAA() const;

		/**
		 * @brief Create a new graph vertex.
		 * @param label The label of the brand new vertex.
		 * @return A vertex pointer if everything went fine, NULL otherwise.
		 */
		Agnode_t* createVertex( const QString& label );

		/**
		 * @brief Create a new vertex with specified label only if that
		 *        vertex wasn't previously created.
		 * @param label The vertex label.
		 */
		Agnode_t* createOrRetrieveVertex( const QString& label );

		/**
		 * @brief Get a previously created graph vertex by label.
		 * @param label The label of the vertex.
		 * @return The graph vertex with specified label, NULL otherwise.
		 */
		Agnode_t* vertex( const QString& label );

		//TODO Should I need getter methods for edges?

		/**
		 * @brief Create all the outgoing edges of the graph vertex with label vertexLabel.
		 * @param absPath The absolute path to the file( required to parse includes ).
		 * @param vertexLabel The graph vertex label.
		 */
		void createEdges( const QString& absPath, const QString& vertexLabel );

		/**
		 * @brief Calculate the graph layout.
		 * @param algorithm The layout algorithm used. Valid values are: "dot", "neato", "fdp", "sfdp", "twopi", "circo", "patchwork", "osage".
		 *	Default is dot.
		 * @return true if everything went fine, false otherwise.
		 */
		bool applyLayout( const QString& algorithm = "dot" );

		/**
		 * @brief Save the graph as image.
		 * @param filename The filename where to save.
		 * @param format The image format name ("png" or "svg" for instance).
		 * @return True if the file has been saved successfully, false otherwise
		 */
		bool saveImage( const QString& filename, const QString& format ) const;

		/**
		 * @brief Save the graph as GraphViz dot file.
		 * @param filename The filename where to save.
		 * @return True if the file has been saved successfully, false otherwise.
		 */
		bool saveDot( const QString& filename ) const;

		/**
		 * @brief Prepare every logic structure to be popoluated before creating the graph.
		 */
		void prepare();

		/**
		 * @brief Get plugins list by kind
		 * @param kind The kind of plugin. Valid values are "render", "layout", "textlayout", "device" and "loadimage".
		 * @return A QStringList* containing all the available plugins of type "kind", NULL otherwise.
		 */
		QStringList* pluginsListByKind( const QString& kind ) const;

	signals:

	public slots:
		/**
		 * @brief Change the render method used by this class.
		 * @param type The chosen render type.
		 * @see RendererType
		 */
		void setRenderer( RendererType type );

		/**
		 * @brief Enable (or disable) high quality antialiasing.
		 * @param highQualityAntialiasing the new value of antialiasing property.
		 */
		void setHighQualityAntialiasing( bool highQualityAA );

		/**
		 * @brief Change value of a graph attribute by name.
		 * @param name Attribute name.
		 * @param value Attribute (new) value.
		 */
		void setGraphAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief Change value of vertices attributes by name.
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setVerticesAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief Change value of edges attributes by name.
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setEdgesAttribute( const QString& name, const QString& value ) const;

		/**
		 * @brief Clear this graph.
		 */
		void clear();

	protected:
		virtual void wheelEvent( QWheelEvent* event );

	private:
		RendererType _renderer;
		QGraphicsSvgItem* _svgItem;

		GVC_t* _context;
		Agraph_t* _graph;

		QMap<QString, Agnode_t*> _vertices;
		QList<Agedge_t*> _edges;

		QMap<QString, QStringList*> _availablePlugins;

		/**
		 * @brief Create a new edge between two vertices.
		 * @param src The source vertex.
		 * @param dest The destination vertex.
		 * @param label Optional label of the new edge.
		 * @return An edge pointer if everything went fine, or NULL if an error occurred.
		 */
		Agedge_t* _createEdge( Agnode_t* src, Agnode_t* dest, const QString& label = "" );

		/**
		 * @brief Helper method used to "translate" the graph into different formats.
		 * @param format The selected format.
		 * @param outString A string containing the graph representation in the selected format.
		 * @return True if everything went fine, false otherwise.
		 */
		bool _renderDataAs( const QString& format, QString* outString ) const;

		void _lookForAvailablePlugins();
		bool _isPluginAvailable( const QString& format, const QString& kind = "" ) const;
	};
}

#endif // GRAPH_H
