#include <cglm/cglm.h>

#include "common.h"

typedef struct {
  he_vertex* Vertex; // vertex at end of half-edge
  he_edge* Twin; // opposite adjacent half-edge
  he_face* Face; // face that the half-edge borders
  he_edge* Next; // next half-edge around the face
  he_edge* Previous; 
} he_edge;

typedef struct {
  vec3 Position;
  he_edge* Edge;
} he_vertex;

typedef struct {
  he_edge* Edge;
} he_face;

he_edge CreateHalfEdges(float* vertices, u64 verticesSize, u32* indices, u64 indicesSize) {

  
}
