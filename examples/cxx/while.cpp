/**
 * Author: Prabhanjan Kambadur
 *
 * Parallel For operates when the collection of elements over which we iterate
 * allows random access. That is, if A is the collection of elements, then, we
 * can access A[i] in constant time. This property does not hold true for many
 * data structures such as linked lists and trees. If the computation involved
 * when processing every element in such data structures is sufficiently large,
 * then it is benefical to parallelize the execution of such loops.
 *
 * Due to the nature of the data structures involved, we do not think of 
 * parallelizing over the iteration space, but rather in terms of processing 
 * each element in parallel. Since we do not know the number of elements that
 * need to be processed, we call such parallelization as Parallel While.
 *
 * A simple way of thinking about the parallelization of collections that 
 * provide only sequential access to its elements is to imagine iterating 
 * through the collection and spawing a task to process each element as we 
 * come accross it. This assumes that processing each task is independent 
 * of one another.
 */

#include <iostream>
#include <vector>
#include "dot_reader/dag.h"
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>
#include <pfunc/pfunc_atomics.h>
#include <pfunc/experimental/parallel_while.hpp>

/**< A global list of all the nodes */
vertex_list_t HEAD = {NULL, NULL};

/**< A count of the number of vertices added */
int num_vertices = 0;

/**< A count of the number of edges added */
int num_edges = 0;

/**< Color of the vertices */
static const char WHITE = 0;
static const char GREY = 1;
static const char BLACK = 2;

/**
 * A structure that acts as an input iterator. We want to, given a node,
 * iterate over all its edges. To create the final value, all we need to 
 * do is have NULL value as the edge.
 */
struct dag_iterator {
  public:
  typedef vertex_t* value_type;
  typedef int distance_type;

  private:
  edge_t* current_edge;

  public:
  dag_iterator (edge_t* current_edge) : current_edge (current_edge) {}

  dag_iterator& operator++ () { 
    current_edge = current_edge->next; 
    return *this;
  } 

  void operator++ (int) { current_edge = current_edge->next; }

  vertex_t* operator* () { return current_edge->destination; } 

  bool operator== (const dag_iterator& other) { 
    return (current_edge == other.current_edge) ? true: false; 
  } 

  bool operator!= (const dag_iterator& other) { 
    return (current_edge == other.current_edge) ? false: true; 
  } 
};

/**
 * A structure that does BFS traversal for the nodes. This is a model of 
 * the WhileExecutable concept.
 */
template <typename PFuncInstanceType>
struct inorder {
  private:
  std::vector<char>& color_map;
  typename PFuncInstanceType::taskmgr& current_taskmgr;

  public:
  inorder (std::vector<char>& color_map, 
           typename PFuncInstanceType::taskmgr& current_taskmgr): 
    color_map (color_map), current_taskmgr (current_taskmgr) {}
  
  /**
   * This is invoked by parallel_while. The operator has to be a const.
   * @param[in] node The current vertex that needs to be explored.
   *
   * Some of the vertices that we have an edge to might already have been
   * discovered. Check this carefully and ensure that we do not visit the
   * the same vertex twice. This is done using the color_map. If the 
   * color_map is NOT WHITE, we omit expanding. If it is WHITE, then this 
   * vertex might be eligible for expansion. Try to atomically swap its 
   * color and if successful, start a new parallel_while for the edges 
   * of that vertex. If not successful, we know that someone else changed
   * the color and they will perform the expansion.
   */
  void operator()(vertex_t* vertex) const {

    // Try to compare and swap the value
    if (WHITE == color_map[vertex->id] && 
        WHITE == pfunc_fetch_and_add_8 (&(color_map[vertex->id]), 1)) {
      std::cout << vertex->id << ":" 
                << ((color_map[vertex->id]==WHITE) ? "WHITE" : "BLACK")
                << std::endl;
      // Spawn one for all my edges.
      pfunc::parallel_while<PFuncInstanceType, 
                            dag_iterator, 
                            inorder<PFuncInstanceType> > 
        edge_while (get_first_edge (vertex), 
                    get_last_edge (vertex), 
                    *this,
                    current_taskmgr);
      typename PFuncInstanceType::task edge_task;
      pfunc::spawn (current_taskmgr, edge_task, edge_while);
      pfunc::wait (current_taskmgr, edge_task);
    }
  }
};

/**
 * Define the PFunc instance. Note that we HAVE TO USE PFUNC::USE_DEFAULT as
 * the type of the FUNCTOR because of the way in which pfunc::parallel_while
 * is defined!
 */
typedef 
pfunc::generator <pfunc::cilkS, /* Cilk-style scheduling */
                  pfunc::use_default, /* No task priorities needed */
                  pfunc::use_default /* any function type*/> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::taskmgr taskmgr;

/**
 * Main harness. Takes in the following parameters:
 * (1) 'filename': Name of the file containing the DOT file. The vertices 
 *                 have to be numbered 0 through n.
 * (2) 'start' : An integer representing the starting vertex's index.
 * (2) 'nqueues': The number of task queues to create
 * (3) 'nthreads': The number of threads PER QUEUE. Total number of threads is
 *                 'nqueues'*'nthreads'.
 * (4) 'print': Whether to print out the array before and after scaling.
 */
int main (int argc, char** argv) {
  // All inputs must be given. Else, barf.
  if (6 != argc) {
    std::cout << "Please use this program as follows" << std::endl
              << "./for <filename> <start> <nqueues> <nthreads> <print(0|1)>" 
              << std::endl;
    exit (3);
  }

  const char* filename = argv[1];
  const int start_vertex_index = atoi (argv[2]);
  const unsigned int nqueues = static_cast<unsigned int>(atoi(argv[3]));
  const unsigned int threads_per_queue = 
                               static_cast<unsigned int>(atoi(argv[4]));
  unsigned int* threads_per_queue_array = new unsigned int [nqueues];
  for (int i=0; i<nqueues; ++i) threads_per_queue_array[i] = threads_per_queue;
  const bool please_print = static_cast<bool>(atoi(argv[5]));

  // Create the dag from the file
  fclose (stdin);
  if (NULL == (stdin = fopen (argv[1], "r"))) {
    std::cout << "Could not open " << filename << " for reading" << std::endl;
    exit (3);
  } 
  yyparse ();

  // Create a vector to represent the coloring of the vertices.
  std::vector<char> color_map (num_vertices, WHITE);

  // Print out the dot file if necessary
  if (please_print) pretty_print ();

  // Get the starting vertex
  vertex_t* start_vertex = get_vertex (start_vertex_index);
  if (NULL == start_vertex) {
    std::cout << "Invalid index (" << start_vertex_index << ") entered!" 
              << std::endl;
    exit (3);
  }

  // Create the initial iteration range.
  dag_iterator root_first = get_first_edge (start_vertex);
  dag_iterator root_last = get_last_edge (start_vertex);

  // Initialize PFunc
  taskmgr global_taskmgr (nqueues, threads_per_queue_array);

  // Spawn the root task
  task root_task;
  attribute root_attribute (false /*nested*/, false /*grouped*/);
  pfunc::parallel_while<generator_type, dag_iterator, inorder<generator_type> > 
  root_while (get_first_edge (start_vertex), 
              get_last_edge (start_vertex), 
              inorder<generator_type> (color_map, global_taskmgr), 
              global_taskmgr);

  double time = micro_time();
  pfunc::spawn (global_taskmgr, root_task, root_attribute, root_while);
  pfunc::wait (global_taskmgr, root_task);
  time = micro_time() - time;

  std::cout << "Finished BFS traversal starting with " << start_vertex_index
            << " in " << time << " seconds" << std::endl;

  return 0;
}
