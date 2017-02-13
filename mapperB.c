#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void packetTree(node *tree, node *node)
{
    if(tree == NULL)
    {
        tree = node;
    }
    if(node->ID == tree->ID)
    {
        if(tree->HP < 0)
        {
            tree->HP = node->HP;
        }
        else if(node->HP < 0)
        {
            node->HP = tree->HP;
        }
    }
    else if(node->ID < tree->ID)
    {
        packetTree(tree->left, node);
    }
    else
    {
        packetTree(tree->right, node);
    }
}

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
    int altDif = a->alt - b->alt;
    dist = sqrt( pow(altDif, 2) + pow(dist, 2) );
    if(dist < 15)
    {
        return(dist);
    }
    else
    {
        return(0);
    }
}


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

void scrollNodes(node *list, node *cur, void(*func)(node *list, node *cur))
{
    int count = 0;
    while(cur != NULL)
    {
        printf("SCROLL #%d\n", count);
        func(list, cur);
        cur = cur->next;
        ++count;
    }
}

//TODO: FIX THIS CRAP
void findAdjacencies(node *cur, node *new)
{

    while(cur != NULL)//While not at the end of the set of existing nodes.
    {
        edge *edgeCursor = cur->connected;//is now the first edge for the node, or NULL
        edge *edgeCursorNew = new->connected;//Same, but for the new node.
        int weight = checkAdjacency(cur, new);//Find weight.
        //printf("adj check: %d\n", weight);

        if(weight > 1 && weight < 16)//If the weight comes back in the correct range...
        {
            //Handle/add first edge.
            if(edgeCursor == NULL)
            {
                puts("First Edge");
                edge *e = calloc(sizeof(edge), 1);
                e->node = new;
                e->weight = weight;
                e->next = NULL;
                cur->connected = e;
                edgeCursor = cur->connected;
            }

            //Move to the last good edge of the current node in the root set.
            while(edgeCursor->next != NULL)
            {
                puts("Moving1");
                edgeCursor = edgeCursor->next;
            }
            //Make the new edge.
            edge *e = calloc(sizeof(edge), 1);
            e->node = new;//Point the new edge to the matching node.
            e->weight = weight;//Add the weight.
            e->next = NULL;//Make sure it points to NULL.
            edgeCursor->next = e;//Assign the new edge to the presently NULL next.
            puts("added to list");



            if(edgeCursorNew == NULL)
            {
                puts("First Edge NewNode");
                edge *en = calloc(sizeof(edge), 1);
                en->node = new;
                en->weight = weight;
                en->next = NULL;
                new->connected = en;
                edgeCursorNew = new->connected;
            }


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
            puts("added to new");


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
    new->connected = NULL;
    return(new);
}

void printem(node *cur)
{
    int count = 0;
    while(cur != NULL)
    {
        printf("\npacket #%d\n", count);
        edge *adj = cur->connected;
        puts("NullCheck");
        printf("adj addr: %d\n", cur->connected);
        while(adj != NULL)
        {
            puts("NotNull");
            printf("adj: %p > %d\n", (void *) adj, adj->node->lat);
            adj = adj->next;
        }
        cur = cur->next;
        ++count;
    }
}

node * shortestRoute(node *root)
{
    //Find shortest path from node.
    int index = 0;
    //int saveIndex = 0;
    int lowest = 15;
    node *lowNode = calloc(sizeof(node), 1);
    edge *edge = root->connected;
    while(edge != NULL)
    {
        puts("EDGE");
        if(edge->weight < lowest)
        {
            puts("NEW LOW");
            lowest = edge->weight;
            //saveIndex = index;
            lowNode = edge->node;
        }
        ++index;
        edge = edge->next;
    }
    printf("shortest route: %p weight: %d\n", lowNode, lowest);
    return(lowNode);
}

void findPath(node *root)
{
    node *shortest = NULL;
    shortest = shortestRoute(root);
    printf("SCHRUTE: %p\n", shortest);
    findPath(shortest);
}





