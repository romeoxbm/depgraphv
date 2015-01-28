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
 * graph.h
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
#ifndef GRAPH_H
#define GRAPH_H

#ifndef FOLDERSMODEL_H
#	include "foldersmodel.h"
#endif

namespace depgraphV
{
	/**
	 * @brief The Graph class
	 */
	class Graph : public QGraphicsView
	{
		Q_OBJECT
		Q_PROPERTY( RendererType rendererType READ renderer WRITE setRenderer )
		Q_PROPERTY( bool highQualityAA READ highQualityAA WRITE setHighQualityAA )
		Q_PROPERTY( QString layoutAlgorithm READ layoutAlgorithm WRITE setLayoutAlgorithm )
		Q_ENUMS( RendererType )

	public:
		/**
		 * @brief The RendererType enum which is used to set the render method
		 * used by Graph
		 */
		enum RendererType
		{
			Native = 0x1,
			OpenGL = 0x2
		};

		/**
		 * @brief Graph contructor.
		 * @param parent The parent widget (default NULL).
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
		 * @brief Return the default value for renderer property (Native).
		 */
		static RendererType defaultRenderer() { return Graph::Native; }

		/**
		 * @brief Return the default value for renderer property (Native)
		 * as QString.
		 */
		static QString defaultRendererString() { return "Native"; }

		/**
		 * @return True if high quality antialiasing is enabled, false otherwise.
		 */
		bool highQualityAA() const;

		/**
		 * @brief Return the default value for highQualityAA property (false).
		 */
		static bool defaultHighQualityAA() { return false; }

		/**
		 * @brief Return the currently in use layout algorithm.
		 */
		const QString& layoutAlgorithm() const;

		/**
		 * @brief Return the default value for layoutAlgorithm property (dot).
		 */
		static QString defaultLayoutAlgorithm() { return "dot"; }

		/**
		 * @brief Return the graph attribute value.
		 * @param name The name of the graph attribute.
		 * @return An empty string if the attribute wasn't previously set,
		 * its value otherwise.
		 */
		QString graphAttribute( const QString& name ) const;

		/**
		 * @brief Return the vertices attribute value.
		 * @param name The name of the vertices attribute.
		 * @return An empty string if the attribute wasn't previously set,
		 * its value otherwise.
		 */
		QString verticesAttribute( const QString& name ) const;

		/**
		 * @brief Return the edges attribute value.
		 * @param name The name of the edges attribute.
		 * @return An empty string if the attribute wasn't previously set,
		 * its value otherwise.
		 */
		QString edgesAttribute( const QString& name ) const;

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
		 * @brief Create all the outgoing edges of the graph vertex with
		 * label vertexLabel.
		 * @param absPath The absolute path to the file
		 * ( required to parse includes ).
		 * @param vertexLabel The graph vertex label.
		 */
		void createEdges( const QString& absPath, const QString& vertexLabel );

		/**
		 * @brief Calculate the graph layout.
		 * @return true if everything went fine, false otherwise.
		 */
		bool applyLayout();

		/**
		 * @return True if this graph has been drawn
		 * (structure created and layout applied), false otherwise.
		 */
		bool drawn() const { return _drawn; }

		FoldersModel* model() const { return _foldersModel; }

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
		 * @brief Get plugins list by kind
		 * @param kind The kind of plugin. Valid values are "render", "layout",
		 *	"textlayout", "device" and "loadimage".
		 * @return A QStringList* containing all the available plugins of type
		 *	"kind", NULL otherwise.
		 */
		static QStringList* pluginsListByKind( const QString& kind );

		/**
		 * @brief Clear available plugins list.
		 */
		static void clearPluginsList();

		/**
		 * @brief Return how many items has been selected to draw this graph.
		 * @param scanByFolders
		 */
		unsigned int selectionCount( bool scanByFolders ) const;

		/**
		 * @brief This method remove from parsed files list a file that
		 * has changed, has been renamed or removed from disk.
		 * @param absoluteFilePath The absolute path to the modified file.
		 */
		static void notifyFileHasChanged( const QString& absoluteFilePath );

	signals:
		void vertexCreated( Agnode_t* );
		void edgeCreated( Agedge_t* );

		void layoutApplied();

	public slots:
		/**
		 * @brief Change the render method used by this class.
		 * @param type The chosen render type.
		 * @see RendererType
		 */
		void setRenderer( RendererType type );

		/**
		 * @brief Enable (or disable) high quality antialiasing.
		 * @param highQualityAA The new value for high quality antialiasing
		 * property.
		 */
		void setHighQualityAA( bool highQualityAA );

		void setLayoutAlgorithm( const QString& value );

		/**
		 * @brief Change value of a graph attribute by name.
		 * @param name Attribute name.
		 * @param value Attribute (new) value.
		 */
		void setGraphAttribute( const QString& name, const QString& value );

		/**
		 * @brief Change value of vertices attributes by name.
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setVerticesAttribute( const QString& name, const QString& value );

		/**
		 * @brief Change value of edges attributes by name.
		 * @param name Attribute name.
		 * @param value Attribute value.
		 */
		void setEdgesAttribute( const QString& name, const QString& value );

		void clearLayout();

		/**
		 * @brief Clear this graph.
		 */
		void clearGraph();

	protected:
		virtual void wheelEvent( QWheelEvent* event );

	private:
		static unsigned short _instances;
		RendererType _renderer;
		QString _layoutAlgorithm;

		QGraphicsSvgItem* _svgItem;

		static GVC_t* _context;
		Agraph_t* _graph;

		QMap<QString, Agnode_t*> _vertices;

		static QMap<QString, QStringList*> _availablePlugins;
		static QMap<QString, QStringList*> _parsedFiles;

		typedef QMap<QString, QString> NameValuePair;
		NameValuePair _graphAttributes;
		NameValuePair _verticesAttributes;
		NameValuePair _edgesAttributes;

		bool _drawn;
		FoldersModel* _foldersModel;

		/**
		 * @brief Create a new edge between two vertices.
		 * @param src The source vertex.
		 * @param dest The destination vertex.
		 * @param label Optional label of the new edge.
		 * @return An edge pointer if everything went fine, or NULL if an error
		 * occurred.
		 */
		Agedge_t* _createEdge( Agnode_t* src, Agnode_t* dest,
							   const QString& label = ""
		);

		/**
		 * @brief Helper method used to "translate" the graph into different
		 * formats.
		 * @param graph
		 * @param format The selected format.
		 * @param outString A string containing the graph representation in the
		 * selected format.
		 * @return True if everything went fine, false otherwise.
		 */
		static bool _renderDataAs( Agraph_t* graph, const QString& format,
								   QString* outString
		);

		/**
		 * @brief This method restore all attributes after clearing this graph.
		 */
		void _restoreAttributes();

		void _setDefaultAttributes();

		static void _lookForAvailablePlugins();
		static bool _isPluginAvailable( const QString& format,
										const QString& kind = ""
		);
	};
}

#endif // GRAPH_H
