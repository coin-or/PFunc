%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dot_reader/dag.h>

/**
 * Author: Prabhanjan Kambadur
 * A YACC file that creates a DAG based on the input DOT format file. As of 
 * now, we parse only a subset of the DOT language. Here are the specifics:
 *
 * 1. Before each edge is added, both the vertices should have been added 
 *    to the DAG.
 * 2. The only attribute that is currently allowed is *weight*. One can define
 *    *weight* of a vertex as followes:
 *    0 [weight = "10"]; // edge weight
 *    0 -> 1 [weight = "10"]; // case of edge weight
 * 3. In total, the DOT file should be as follows:
 * 
 * digraph <name> {
 *  0;
 *  1 [weight = "10"];
 *  0 -> 1 [weight = "20"];
 *  2;
 *  1 -> 2;
 * }
 *
 * There can be as many blank spaces as you possibly want --- they are ignored.
 * Notice that "->" is parsed as one token, so there cannot be a blank space 
 * in there. Also, the value for "weight" has to be convertible to integer.
 */

void yyerror(const char *str) {fprintf(stderr,"error: %s\n",str);}
   
int yywrap() {return 1;}
%}

%token NUMBER EDGE WEIGHT EQUAL QUOTE OSBRACE WORD
%token CSBRACE SEMICOLON OBRACE CBRACE DIGRAPH NAME

%%
root: /* empty */
     | start commands stop
     ;

commands: /* empty */
        | commands command
        ;

start: DIGRAPH WORD OBRACE
       {
#ifdef DEBUG
         printf ("Starting the processing\n");
#endif
       }
       ;

stop: CBRACE
     {
#ifdef DEBUG
       printf ("Done processing\n");
#endif
       return 0;
     }
     ;

command: NUMBER EDGE NUMBER SEMICOLON
         {
           /* Find both the vertexs */
           struct vertex_t* parent;
           struct vertex_t* child;

           if (NULL == (parent = get_vertex ($1)) || 
               NULL == (child = get_vertex ($3))) {
             printf ("One or both of (%d,%d) have not been added\n", $1, $3);
           } else {
             /* Add the relationship */
             add_edge (parent, child, 0);
#ifdef DEBUG
             printf ("Found an edge from %d to %d\n", $1, $3);
#endif
           }
         }
         |
         NUMBER EDGE NUMBER OSBRACE WEIGHT EQUAL QUOTE NUMBER QUOTE CSBRACE SEMICOLON
         {
           /* Find both the vertexs */
           struct vertex_t* parent;
           struct vertex_t* child;

           if (NULL == (parent = get_vertex ($1)) || 
               NULL == (child = get_vertex ($3))) {
             printf ("One or both of (%d,%d) have not been added\n", $1, $3);
           } else {
             /* 
              * Add the relationship --- somehow make sure that the 
              * weight is also registered 
              */
             add_edge (parent, child, $8);
#ifdef DEBUG
             printf ("Found an edge from %d to %d of weight=%d\n", $1, $3, $8);
#endif
           }
         }
         |
         NUMBER SEMICOLON
         {
           /* we have to add the vertex into the graph */
           struct vertex_list_t* iterator = &HEAD;
           while (NULL != iterator->next) iterator = iterator->next;
           iterator->next = 
            (vertex_list_t*) malloc (sizeof(struct vertex_list_t));
           iterator = iterator->next;
           iterator->next = NULL;
           iterator->data = (vertex_t*) malloc (sizeof(struct vertex_t));
           vertex_init (iterator->data);
           iterator->data->id = $1;
           ++num_vertices;

#ifdef DEBUG
           printf ("Found vertex %d with no weight\n", $1);
#endif
         }
         ;
%%
