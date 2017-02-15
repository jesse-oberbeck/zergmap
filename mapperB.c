#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

//TODO: Adjust build nodes to be usable for status packets, incorporate into status type packet handler.

void
printTree(
    node * root)
{
    if (root == NULL)
    {
        return;
    }
    printTree(root->left);
    printf("ID:%d HP: %d/%d ALT: %f\n", root->ID, root->HP, root->MAXHP, root->alt);
    printTree(root->right);
}

node * packetTree(node *tree, node *node)
{
    if(tree == NULL)
    {
        return(node);
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
        tree->left = packetTree(tree->left, node);
    }
    else
    {
        tree->right = packetTree(tree->right, node);
    }
    return(tree);
}

double haversine(double th1, double ph1, double th2, double ph2)
{
	double dx, dy, dz;
	ph1 -= ph2;
	ph1 *= (3.1415926536 / 180), th1 *= (3.1415926536 / 180), th2 *= (3.1415926536 / 180);
 
	dz = sin(th1) - sin(th2);
	dx = cos(ph1) * cos(th1) - cos(th2);
	dy = sin(ph1) * cos(th1);
	return (asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371)*1000;
}

double checkAdjacency(node *a, node*b)
{
    //Wrapper for haversine, so that two nodes can be passed.
    //For ease of use/typing, as it reduces the number of parameters.
    //Also serves as a simplified yes/no check for "is in range."
    double dist = haversine(a->lat, a->lon, b->lat, b->lon);
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


void insert(node **nodes, node *n)
{
    if(*nodes == NULL)//If it's the first node.
    {
        printf("!!!!!!!!!!!!!!!!!FIRST NODE %d\n", n->ID);
        *nodes = n;
        return;
    }
    while((*nodes)->next != NULL)//Move to the last existing node.
    {
        puts("----moving over");
        *nodes = (*nodes)->next;
    }
    printf("----inserting %d\n", n->ID);
    (*nodes)->next = n;
}

void scrollNodes(node *base, node *cur, void(*func)(node *base, node *cur))
{
    int count = 0;
    printf("Base ID: %d\n", base->ID);
    while(cur != NULL)
    {
        if(cur == base)
        {
            cur = cur->next;//Don't compare a node to itself.
        }
        printf("SCROLL #%d\n", count);
        func(base, cur);
        cur = cur->next;
        ++count;
    }
}
/*
void findAdjacencies(node *cur, node *new)
{

    while(cur != NULL)//While not at the end of the set of existing nodes.
    {

        edge *edgeCursor = cur->connected;//is now the first edge for the node, or NULL
        edge *edgeCursorNew = new->connected;//Same, but for the new node.
        double weight = checkAdjacency(cur, new);//Find weight.
        printf("DISTANCE: %f\n", weight);

        if(weight > 1.1 && weight < 16)//If the weight comes back in the correct range...
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
                printf("adj ID: %d\n", e->node->ID);
            }

            //Move to the last good edge of the current node in the root set.
            while(edgeCursor->next != NULL)
            {
                puts("Moving1");
                if(edgeCursor->next->node == new)
                {
                    break;
                }
                edgeCursor = edgeCursor->next;
            }
            //Make the new edge.
            edge *e = calloc(sizeof(edge), 1);
            e->node = new;//Point the new edge to the matching node.
            e->weight = weight;//Add the weight.
            e->next = NULL;//Make sure it points to NULL.
            edgeCursor->next = e;//Assign the new edge to the presently NULL next.
            printf("adj ID: %d\n", e->node->ID);
            puts("added to list");



            if(edgeCursorNew == NULL)
            {
                puts("First Edge NewNode");
                edge *en = calloc(sizeof(edge), 1);
                en->node = cur;
                en->weight = weight;
                en->next = NULL;
                new->connected = en;
                edgeCursorNew = new->connected;
                printf("adj ID: %d\n", e->node->ID);
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
            //puts("added to new");


        }
        cur = cur->next;//Move to the next node in the root set.
    }
}
*/
















void findAdjacencies(node *cur, node *new)
{

    while(cur != NULL)//While not at the end of the set of existing nodes.
    {

        //edge *edgeCursor = cur->connected;//is now the first edge for the node, or NULL
        //edge *edgeCursorNew = new->connected;//Same, but for the new node.
        double weight = checkAdjacency(cur, new);//Find weight.
        printf("DISTANCE: %f\n", weight);

        if(weight > 1.1 && weight < 16)//If the weight comes back in the correct range...
        {

            //make new edge for new node
            edge *newEdge = calloc(sizeof(edge), 1);
            newEdge->weight = weight;
            newEdge->node = cur;
            newEdge->next = new->connected;
            new->connected = newEdge;

            //make new edge for current node
            edge *curEdge = calloc(sizeof(edge), 1);
            curEdge->weight = weight;
            curEdge->node = new;
            curEdge->next = cur->connected;
            cur->connected = curEdge;

        }
        cur = cur->next;//Move to the next node in the root set.
    }
}
















node *addStatus(int hp, int maxhp, int id)
{
    node *new = calloc(sizeof(node), 1);
    new->HP = hp;
    new->MAXHP = maxhp;
    new->ID = id;
    return(new);
}

node *buildNode(double lat, double lon, float alt, int id)
{
    printf("NEW NODE LAT: %f LON: %f ALT: %f\n", lat, lon, alt);
    node *new = calloc(sizeof(node), 1);
    new->lat = lat;
    new->lon = lon;
    new->alt = alt;
    new->ID = id;
    new->connected = NULL;
    return(new);
}

void printem(node *cur)
{
    int count = 0;
    while(cur != NULL)
    {
        printf("\npacket #%d ID: %d\n", count, cur->ID);
        edge *adj = cur->connected;
        //puts("NullCheck");
        printf("adj addr: %p\n", cur->connected);
        while(adj != NULL)
        {
            //puts("NotNull");
            printf("adj: %d > %f\n", adj->node->ID, adj->node->lat);
            adj = adj->next;
        }
        cur = cur->next;
        ++count;
    }
}

node *leastAdj(node *root)
{
    edge *edgeCur = NULL;
    node *least = calloc(sizeof(node), 1);
    int leastAdj = 0;
    int numAdj = 0;
    while(root != NULL)
    {
        numAdj = 0;
        edgeCur = root->connected;
        while(edgeCur != NULL)
        {
            ++numAdj;
            edgeCur = edgeCur->next;
        }
        if(numAdj < leastAdj)
        {
            leastAdj = numAdj;
            least = edgeCur->node;
        }
        root = root->next;
    }
    printf("LEAST: %p\n", least);
    return(least);
}

node * shortestRoute(node *root)
{
    //Find shortest path from node.
    int index = 0;
    int lowest = 15;
    node *lowNode = calloc(sizeof(node), 1);
    edge *edge = root->connected;
    while(edge != NULL)
    {
        puts("EDGE");
        if((edge->weight < lowest) && (edge->node->visited == 0))
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
    node *start = leastAdj(root);//Node with least adjacencies is start point.
    node *end = root;
    while(end != NULL)
    {
        if(end == start)
        {
            end = end->next;//Keep from trying to find paths from start to itself.
        }
        //DO STUFF
        end = end->next;
    }


    node *shortest = NULL;
    shortest = shortestRoute(root);//Find shortest route from starting node.
    printf("SCHRUTE: %p\n", shortest);
    findPath(shortest);
    free(start);
    free(shortest);
}





