#pragma once

#include "geometrycentral/surface/surface_mesh.h"

namespace geometrycentral {
namespace surface {

class ManifoldSurfaceMesh : public SurfaceMesh {

public:
  ManifoldSurfaceMesh();

  // Build a halfedge mesh from polygons, with a list of 0-indexed vertices incident on each face, in CCW order.
  // Assumes that the vertex listing in polygons is dense; all indices from [0,MAX_IND) must appear in some face.
  // (some functions, like in meshio.h preprocess inputs to strip out unused indices).
  // The output will preserve the ordering of vertices and faces.
  ManifoldSurfaceMesh(const std::vector<std::vector<size_t>>& polygons);

  // TODO
  ManifoldSurfaceMesh(const std::vector<std::vector<size_t>>& polygons,
                      const std::vector<std::vector<std::tuple<size_t, size_t>>>& twins);

  virtual ~ManifoldSurfaceMesh();

  int eulerCharacteristic() const; // compute the Euler characteristic [O(1)]
  int genus() const;               // compute the genus [O(1)]
  virtual bool isManifold() override;


  // === Methods that mutate the mesh.
  // Note that all of these methods retain the validity of any MeshData<> containers, as well as any element handles
  // (like Vertex). The only thing that can happen is that deletions may make the mesh no longer be _compressed_ (i.e.
  // the index space may have gaps in it). This can be remedied by calling compress(), which _does_ invalidate
  // non-dynamic element handles (but still keeps MeshData<> containers valid).

  // Flip an edge. Edge is rotated clockwise. Return true if the edge was actually flipped (one can only flip manifold,
  // interior, triangular edges which are not incident on degree-1 vertices). Does _not_ create any new elements, or
  // cause the mesh to become decompressed.
  bool flip(Edge e);

  // Adds a vertex along an edge, increasing degree of faces. Returns ptr along the edge, with he.vertex() as new
  // vertex and he.edge().halfedge() == he. Preserves canonical direction of edge.halfedge() for both halves of new
  // edge. The original edge is repurposed as one of the two new edges (same for original halfedges).
  Halfedge insertVertexAlongEdge(Edge e);

  // Split an edge, also splitting adjacent faces. Returns new halfedge which points away from the new vertex (so
  // he.vertex() is new vertex), and is the same direction as e.halfedge() on the original edge. The halfedge
  // direction of the other part of the new split edge is also preserved.
  Halfedge splitEdgeTriangular(Edge e);

  // Add vertex inside face and triangulate. Returns new vertex.
  Vertex insertVertex(Face f);

  // The workhorse version of connectVertices(). heA.vertex() will be connected to heB.vertex().
  // Returns new halfedge with vA at tail. he.twin().face() is the new face.
  Halfedge connectVertices(Halfedge heA, Halfedge heB);

  // Given a non-boundary edge e, cuts the mesh such that there are two copies of e with a boundary between them.
  // As necessary, either creates a new boundary loop or merges adjacent boundary loops.
  // Returns returns the halfedges along the cut edge which exist where {e.halfedge(), e.halfedge().twin()} were (which
  // may or may not be new)
  std::tuple<Halfedge, Halfedge> separateEdge(Edge e);

  // Collapse an edge. Returns the vertex adjacent to that edge which still exists. Returns Vertex() if not
  // collapsible. Assumes triangular simplicial complex as input (at least in neighborhood of collapse).
  Vertex collapseEdge(Edge e);

  // Removes a vertex, leaving a high-degree face. If the input is a boundary vertex, preserves an edge along the
  // boundary. Return Face() if impossible.
  Face removeVertex(Vertex v);

  // Make the edge a mirror image of itself, switching the side the two halfedges are on.
  Halfedge switchHalfedgeSides(Edge e);

  // Removes an edge, unioning two faces. Input must not be a boundary edge. Returns Face() if impossible.
  Face removeEdge(Edge e);

  // Remove a face along the boundary. Currently does not know how to remove ears or whole components.
  bool removeFaceAlongBoundary(Face f);

  // Triangulate in a face, returns all subfaces
  std::vector<Face> triangulate(Face face);


  // == Utility functions
  std::unique_ptr<ManifoldSurfaceMesh> copy() const;

protected:
  // Helpers
  bool ensureEdgeHasInteriorHalfedge(Edge e);     // impose invariant that e.halfedge is interior
  void ensureVertexHasBoundaryHalfedge(Vertex v); // impose invariant that v.halfedge is start of half-disk
  Vertex collapseEdgeAlongBoundary(Edge e);

  virtual std::unique_ptr<SurfaceMesh> copyToSurfaceMesh() const override;
};

} // namespace surface
} // namespace geometrycentral
