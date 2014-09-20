CREATE TABLE "graphSettings"
(
	"graphID" INTEGER PRIMARY KEY NOT NULL,
	"layoutAlgorithm" TEXT NOT NULL DEFAULT 'dot',
	"splines" TEXT NOT NULL DEFAULT 'spline',
	"nodesep" DOUBLE NOT NULL DEFAULT 0.4,
	"shape" TEXT NOT NULL DEFAULT 'box',
	"vert_style" TEXT NOT NULL DEFAULT 'rounded',
	"minlen" INTEGER NOT NULL DEFAULT 3,
	"edge_style" TEXT NOT NULL DEFAULT 'solid'
);