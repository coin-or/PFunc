#ifndef DAG_H
#define DAG_H

/**
 * A representation for the DAG parsed by dot.l and dot.y. The DAG 
 * representation is explained in this section.
 *
 * The DAG is represented as a linked list of DAG vertexs. This linked list is
 * represented by vertex_list_t.
 *
 * Each dag_vertex has one such list for its sources, and one list for its 
 * destinationren. The only internal data for a DAG are ID and weight.
 *
 * Any application specific data has to be stored externally using ID as an 
 * index for the vertices.
 *
 */ 

#include <assert.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif
/** Forward declare vertex_t */
struct vertex_t;

/**
 * A structure that represents a list of DAG vertexs.
 */
struct vertex_list_t {
  struct vertex_t* data;
  struct vertex_list_t* next;
};

/**
 * A structure that represents a list of edges.
 */
struct edge_t {
  struct vertex_t* destination;
  int weight;
  struct edge_t* next;
};

/**
 * A structure that represents a vertex in the DAG.
 */
struct vertex_t {
  int id; /**< for identification */
  int in_degree; /**< for the in degree */
  int out_degree; /**< for the out degree */

  struct edge_t edges; /**< list of edges */
};

/**< A global list of all the vertexs */
extern struct vertex_list_t HEAD;

/**< A count of the number of vertices added */
extern int num_vertices;

/**< A count of the number of edges added */
extern int num_edges;

/**< Declarations for the parser */
extern int yylex();
extern void yyerror(const char*);
extern int yywrap();
extern int yyparse();

/**
 * Initialize a DAG vertex.
 * @param[inout] vertex The vertex that needs to be initialized.
 */
static void vertex_init (struct vertex_t* vertex) {
  vertex->id = -1;
  vertex->in_degree = vertex->out_degree = 0;
  vertex->edges.destination = NULL;
  vertex->edges.next = NULL;
  vertex->edges.weight = -1;
}

/**
 * Add an edge between the source and the destination. 
 * @param [inout] source The source of the edge.
 * @param [inout] destination The destination of the edge.
 * @param [in] weight The weight of the edge.
 *
 * Precondition:
 * Both source and destination should have been added as vertexs before!
 */
static void add_edge (struct vertex_t* source, 
                      struct vertex_t* destination,
                      const int weight) {
  struct edge_t* iterator;

  // Make sure that both are valid vertexs
  assert (source != NULL);
  assert (destination != NULL);

  // Add the destination to the source's edge list.
  iterator = &(source->edges);
  while (NULL != iterator->next) iterator = iterator->next;

  iterator->next = (struct edge_t*) malloc (sizeof(struct edge_t));
  iterator = iterator->next;
  iterator->next = NULL;
  iterator->destination = destination;
  iterator->weight = weight;

  // Update the relevant counts
  ++(source->out_degree);
  ++(destination->in_degree);
  ++(num_edges);
}

/**
 * Retrieve the vertex with the given ID.
 * @param[in] ID of the vertex to be retrieved.
 * @return The vertex that has the given ID.
 */
static struct vertex_t* get_vertex (const int id) {
  struct vertex_list_t* iterator = &HEAD;
  while (NULL != iterator) 
    if (NULL != iterator->data && id == iterator->data->id)
      return iterator->data;
    else iterator = iterator->next;
  return NULL;
}

/**
 * Print the vertices in the DAG out.
 */
static void pretty_print () {
  struct vertex_list_t* vertex_iterator = &HEAD;
  while (NULL != vertex_iterator) {
    if (NULL != vertex_iterator->data) {
      struct vertex_t* this_vertex = vertex_iterator->data;
      // First print the vertex
      printf ("%d(IN:%d, OUT:%d) : ", this_vertex->id, 
                                this_vertex->in_degree, 
                                this_vertex->out_degree);
      // Now print its edges
      struct edge_t* edge_iterator = &(this_vertex->edges);
      while (NULL != edge_iterator) {
        if (NULL != edge_iterator->destination) {
          struct vertex_t* destination = edge_iterator->destination;
          printf ("%d(weight=%d), ", destination->id, edge_iterator->weight);
        }
        edge_iterator = edge_iterator->next;
      }
    }
    vertex_iterator = vertex_iterator->next;
    printf ("\n");
  }
}

/**
 * Given a vertex, return the first edge. Remember that "edges" is just the 
 * header --- it does not contain any valid edges. So, return the one after
 * that. That is, return edges.next.
 */
static struct edge_t* get_first_edge (struct vertex_t* vertex) {
  return vertex->edges.next;
}

/**
 * Return a symbolic place holder for the last edge --- bound to be NULL.
 * Notice that we are not returning the last edge, but symbolically, we are 
 * returning the edge one past the last. This means that the edges for a 
 * particular vertex are the semi-open interval 
 * [ get_first_edge(), get_last_edge() ).
 */
static struct edge_t* get_last_edge (struct vertex_t* vertex) {
  return NULL;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DAG_H
