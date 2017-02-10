#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
//#include "structures.h"
 //lat1 lat 1st
 //lon1 lon firs
 /*
typedef struct Node
{
    int lat;
    int lon;
    int alt;
    int visited;//For pathing.
    int **connected;//Array of pointers to nodes connected to latis node.
}node;
*/
double haversine(double th1, double ph1, double th2, double ph2)
{
	double dx, dy, dz;
	ph1 -= ph2;
	ph1 *= (3.1415926536 / 180), th1 *= (3.1415926536 / 180), th2 *= (3.1415926536 / 180);
 
	dz = sin(th1) - sin(th2);
	dx = cos(ph1) * cos(th1) - cos(th2);
	dy = sin(ph1) * cos(th1);
	return (asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371)/1000;
}

int checkAdjacency(node *a, node*b)
{
    //Wrapper for haversine, so that two nodes can be passed.
    //For ease of use/typing, as it reduces the number of parameters.
    //Also serves as a simplified yes/no check for "is in range."
    int dist = haversine(a->lat, a->lon, b->lat, b->lon);
    //printf("dist: %d\n", dist);
    if(dist < 15)
    {
        return(dist);
    }
    else
    {
        return(0);
    }
}
/*
void insert(node *nodes, node *node)
{
    //Nodes are placed into an "array" of nodes. Each node contains a list of integers.
    //These integers represent the position/index of every node.
    //This is done so that a path can be followed by checking a node's "connected" field,
    //Choosing an int from that list, and using it as an index for the "nodes" set.
    int i = 0;
    //Insert new node into node array.
    puts("INSERT");
    while(nodes->next != NULL)
    {
        int j = 0;
        if(checkAdjacency(nodes, node))//if adjacent, add index to current node adjacencies.
        {
            while(node->connected[j] != NULL)
            //Add the existing node's indice to the current node's "connected" list.
            {
                ++j;
            }
            //Make an int pointer, add it to the set of int pointers (connected).

            node->connected[j] = i;
            
            //Add the current node's indice to the existing node's connected list.
            int k = 0;
            while(nodes->connected[k] != NULL)
            {
                ++k;
            }
            //Make an int pointer, add it to the set of int pointers (connected).

            nodes->connected[k] = j;
            //Add other values from new node.
        }
        ++i;
        puts("MOVING OVER A NODE~~~~~~~~~~~~~~~~~~~");
        nodes = nodes->next;
    }
    //Place the new node at the end of the array of existing nodes.
    if(nodes == NULL)
    {
        puts("GOT A NULL");
    }
    //nodes->next = calloc(sizeof(node), 1);
    nodes->next = node;
    printf("CURRENT: %d\n", nodes->next->alt);
}
*/



void insert(node **nodes, node *node)
{
    puts("----inserting");
    if(!*nodes)
    {
        *nodes = node;
        return;
    }
    while((*nodes)->next != NULL)
    {
        puts("----moving over");
        *nodes = (*nodes)->next;
    }
    (*nodes)->next = node;
}
//TODO: FIX THIS CRAP
void findAdjacencies(node *cur, node *new)
{
    while(cur != NULL)//While not at the end of the set of existing nodes.
    {
        edge *edgeCursor = cur->connected;
        edge *edgeCursorNew = new->connected;
        int weight = checkAdjacency(cur, new);
        printf("adj check: %d\n", weight);
        if(weight > 1 && weight < 16)
        {
            //Move to the end of the edges of the current node in the root set.
            while(edgeCursor->next != NULL)
            {
                puts("Moving1");
                edgeCursor = edgeCursor->next;
            }
            //Make the edge.
            edge *e = calloc(sizeof(edge), 1);
            e->node = new;
            e->weight = weight;
            e->next = NULL;
            edgeCursor->next = e;



            //Move to the end of the edges of the new node.
            while(edgeCursorNew->next != NULL)
            {
                puts("Moving2");
                edgeCursorNew = edgeCursorNew->next;
            }
            //Make the edge for the new node.
            edge *en = calloc(sizeof(edge), 1);
            en->node = new;
            en->weight = weight;
            en->next = NULL;
            edgeCursorNew->next = en;


        }
        cur = cur->next;//Move to the next node in the root set.
    }
}





node *buildNode(double lat, double lon, float alt)
{
    printf("NEW NODE\nLAT: %f\nLON: %f\nALT: %f\n", lat, lon, alt);
    node *new = calloc(sizeof(node), 1);
    new->lat = lat;
    new->lon = lon;
    new->alt = alt;
    return(new);
}

void printem(node *cur)
{
    int count = 0;
    while(cur != NULL)
    {
        printf("\npacket #%d\n", count);
        node *adj = cur->connected;
        while(adj != NULL)
        {
            printf("adj: %p > %d\n", (void *) adj, adj->lat);
            adj = adj->connected;
        }
        cur = cur->next;
        ++count;
    }
}

/*
int main(void)
{
    node **nodes;
    
}
*/
