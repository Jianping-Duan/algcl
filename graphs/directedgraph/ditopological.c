#include "topological/ditopological.h"
#include "digraphcycle.h"
#include "digraphdfso.h"
#include "linearlist.h"

static int vequal(const void *, const void *);

/* 
 * Determines whether the digraph g has a topological order and,
 * if so, finds such a topological order.
 */

void 
ditplg_use_dfso(const struct digraph *g, struct single_list *order)
{
	struct digraph_cycle dc;
	struct digraph_dfso dfso;

	digraph_cycle_init(&dc, g);
	if (DIGRAPH_HASCYCLE(&dc)) {
		slist_init(order, 0, NULL);
		return;
	}

	digraph_dfso_init(&dfso, g);
	DIGRAPH_DFSO_REVERSEPOST(&dfso, order);

	DIGRAPH_CYCLE_CLEAR(&dc);
	DIGRAPH_DFSO_CLEAR(&dfso);
}

void 
ditplg_use_queue(const struct digraph *g, struct single_list *order)
{
	unsigned int v, *w, *x, cnt;
	int *indegree;
	struct queue qu;
	struct single_list *adj;
	struct slist_node *nptr;

	/* indegrees of remaining vertices */
	indegree = (int *)algcalloc(DIGRAPH_VERTICES(g), sizeof(int));
	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		indegree[v] = DIGRAPH_INDEGREE(g, v);
	
	/* Initialize queue to contain all vertices with indegree = 0. */
	QUEUE_INIT(&qu, sizeof(int));
	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		if (indegree[v] == 0)
			enqueue(&qu, &v);

	cnt = 0;
	slist_init(order, sizeof(int), vequal);
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&w);
		slist_append(order, w);
		cnt++;
		adj = DIGRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(adj, nptr, unsigned int, x) {
			indegree[*x]--;
			if (indegree[*x] == 0)
				enqueue(&qu, x);
		}
	}
	ALGFREE(w);
	ALGFREE(indegree);
	queue_clear(&qu);

	/* 
	 * There is a directed cycle in subgraph of vertices with
	 * indegree >= 1.
	 */
	assert(cnt == DIGRAPH_VERTICES(g));
}

/******************** static function boundary ********************/

static int 
vequal(const void *k1, const void *k2)
{
	int *v = (int *)k1, *w = (int *)k2;
	return *v == *w;
}
