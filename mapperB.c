#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void
printTree(
    node * root)
{
    if (root == NULL)
    {
        return;
    }
    printTree(root->left);
    printf("ID:%d HP: %d/%d DEL: %d\n", root->ID, root->HP, root->MAXHP,
           root->deleted);
    printTree(root->right);
}

node *
packetTree(
    node * tree,
    node * node)
{
    //Insert a node into a tree structure, adjusting HP values if same ID
    //is found.

    if (tree == NULL)
    {
        return (node);
    }
    printf("NODE ID: %d, TREE ID: %d\n", node->ID, tree->ID);
    if (node->ID < tree->ID)
    {
        puts("left");
        tree->left = packetTree(tree->left, node);
    }
    else if (node->ID > tree->ID)
    {
        puts("right");
        tree->right = packetTree(tree->right, node);
    }

    else if (node->ID - tree->ID == 0)
    {
        puts("ID MATCH");
        if (tree->MAXHP > node->MAXHP)
        {
            node->MAXHP = tree->MAXHP;
            node->HP = tree->HP;
            tree = node;
            printf("NODEHP: %d\n", node->HP);
        }
        else
        {
            tree->MAXHP = node->MAXHP;
            tree->HP = node->HP;
            printf("TREEHP: %d\n", tree->HP);
        }
    }

    return (tree);
}

double
haversine(
    double th1,
    double ph1,
    double th2,
    double ph2)
{
    //Distance between two points on a globe.
    double dx, dy, dz;

    ph1 -= ph2;
    ph1 *= (3.1415926536 / 180), th1 *= (3.1415926536 / 180), th2 *=
        (3.1415926536 / 180);

    dz = sin(th1) - sin(th2);
    dx = cos(ph1) * cos(th1) - cos(th2);
    dy = sin(ph1) * cos(th1);
    return (asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371) * 1000;
}

double
checkDistance(
    node * a,
    node * b)
{
    //Wrapper for haversine, so that two nodes can be passed.
    //For ease of use/typing, as it reduces the number of parameters.
    double dist = haversine(a->lat, a->lon, b->lat, b->lon);
    int altDif = a->alt - b->alt;

    dist = sqrt(pow(altDif, 2) + pow(dist, 2));
    return (dist);
}

double
checkAdjacency(
    node * a,
    node * b)
{
    //Wrapper for haversine, so that two nodes can be passed.
    //For ease of use/typing, as it reduces the number of parameters.
    //Also serves as a simplified yes/no check for "is in range."
    double dist = haversine(a->lat, a->lon, b->lat, b->lon);
    int altDif = a->alt - b->alt;

    dist = sqrt(pow(altDif, 2) + pow(dist, 2));
    if (dist < 15)
    {
        return (dist);
    }
    else
    {
        return (0);
    }
}


void
insert(
    node ** nodes,
    node * n)
{
    //Adds a node to the set of nodes.
    if (*nodes == NULL)         //If it's the first node.
    {
        *nodes = n;
        return;
    }
    while ((*nodes)->next != NULL)  //Move to the last existing node.
    {
        //puts("----moving over");
        *nodes = (*nodes)->next;
    }
    //printf("----inserting %d\n", n->ID);
    (*nodes)->next = n;
}

void
scrollNodes(
    node * base,
    node * cur,
    void (*func) (node * base,
                  node * cur))
{
    int count = 0;

    printf("Base ID: %d\n", base->ID);
    while (cur != NULL)
    {
        if (cur == base)
        {
            cur = cur->next;    //Don't compare a node to itself.
        }
        printf("SCROLL #%d\n", count);
        func(base, cur);
        cur = cur->next;
        ++count;
    }
}



void
findAdjacencies(
    node * cur,
    node * new)
{

    while (cur != NULL)         //While not at the end of the set of existing nodes.
    {

        double weight = checkAdjacency(cur, new);   //Find weight.

        //printf("DISTANCE: %f\n", weight);

        if (weight > 1.1 && weight < 16)    //If the weight comes back in the correct range...
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
        cur = cur->next;        //Move to the next node in the root set.
    }
}



node *
addStatus(
    int hp,
    int maxhp,
    int id)
{
    //Builds a node with just status information.
    node *new = calloc(sizeof(node), 1);

    new->HP = hp;
    new->MAXHP = maxhp;
    new->ID = id;
    return (new);
}

node *
buildNode(
    double lat,
    double lon,
    float alt,
    int id)
{
    //Builds a new node.
    printf("NEW NODE LAT: %f LON: %f ALT: %f\n", lat, lon, alt);
    node *new = calloc(sizeof(node), 1);

    new->lat = lat;
    new->lon = lon;
    new->alt = alt;
    new->ID = id;
    new->connected = NULL;
    return (new);
}

void
printem(
    node * cur)
{
    //Prints nodes and their adjacencies.
    int count = 0;

    while (cur != NULL)
    {
        printf("\npacket #%d ID: %d\n", count, cur->ID);
        edge *adj = cur->connected;

        //puts("NullCheck");
        printf("adj addr: %p\n", (void *) cur->connected);
        while (adj != NULL)
        {
            //puts("NotNull");
            printf("adj: %d > %f\n", adj->node->ID, adj->node->lat);
            adj = adj->next;
        }
        cur = cur->next;
        ++count;
    }
}

void
trimLeaves(
    node * root,
    int *nodeCount)
{
    //Cyclically trims nodes with only one adjacency (leaves).
    puts("TL CALLED");
    node *start = root;
    int noLeaf = 1;

    while (root != NULL)
    {
        int edgeCount = 0;
        edge *e = root->connected;

        while (e != NULL)
        {
            ++edgeCount;
            e = e->next;
        }
        if (edgeCount == 1)
        {
            noLeaf = 0;
            //DELETE EDGE FROM FAR NODE
            edge *farEdge = root->connected->node->connected;   //Edge from opposing node, pointing to the leaf.
            edge *farPrev = farEdge;

            while (farEdge->node->ID != root->ID)   //Until the far edge points to root.
            {
                farPrev = farEdge;
                farEdge = farEdge->next;
            }
            edge *temp = farEdge;

            if (root->connected->node->connected != farEdge)
            {
                farPrev->next = farEdge->next;
                free(temp);
            }
            else
            {
                root->connected->node->connected = farEdge->next;
                free(temp);
                temp = NULL;
            }
            //farPrev->next = farEdge->next;
            //free(farEdge);
            //DELETE FROM CURRENT NODE
            printf("Deleting... %d\n", root->ID);
            free(root->connected);
            root->connected = NULL;
            root->deleted = 1;
            *nodeCount -= 1;
        }
        root = root->next;
    }
    //do another check afterward, call func again if leaf found
    if (noLeaf == 0)
    {
        trimLeaves(start, nodeCount);
    }
}

node *
leastAdj(
    node * root)
{
    //Finds the node with the least number of adjacencies. Not used.
    edge *edgeCur = NULL;
    node *least = calloc(sizeof(node), 1);
    int leastAdj = 0;
    int numAdj = 0;

    while (root != NULL)
    {
        numAdj = 0;
        edgeCur = root->connected;
        while (edgeCur != NULL)
        {
            ++numAdj;
            edgeCur = edgeCur->next;
        }
        if (numAdj < leastAdj)
        {
            leastAdj = numAdj;
            least = edgeCur->node;
        }
        root = root->next;
    }
    printf("LEAST: %p\n", (void *) least);
    return (least);
}

node *
shortestRoute(
    node * root)
{
    //Find shortest path from node.
    int index = 0;
    int lowest = 15;
    node *lowNode = NULL;       //calloc(sizeof(node), 1);
    edge *edge = root->connected;

    while (edge != NULL)
    {
        //puts("EDGE");
        if ((edge->weight < lowest) && (edge->node->visited == 0))
        {
            //puts("NEW LOW");
            lowest = edge->weight;
            //saveIndex = index;
            lowNode = edge->node;
        }
        ++index;
        edge = edge->next;
    }
    printf("shortest route: %p weight: %d\n", (void *) lowNode, lowest);
    return (lowNode);
}

void
visitClear(
    node * root)
{
    //Clears visited status from all nodes.
    if (root == NULL)
    {
        return;
    }
    visitClear(root->left);
    root->visited = 0;
    //printf("ID:%d HP: %d/%d ALT: %f\n", root->ID, root->HP, root->MAXHP, root->alt);
    visitClear(root->right);
}


int
endProbe(
    node * n,
    node * end)
{
    //Tests if node n is connected to node end.
    int found = 0;
    edge *e = n->connected;

    while (e != NULL)
    {
        if (e->node == end)
        {
            found = 1;
        }
        e = e->next;
    }
    return (found);
}

node *
commonCheck(
    node * a,
    node * b)
{
    //Check two nodes for common edges, returns the common edge or NULL.
    edge *ae = a->connected;
    edge *be = b->connected;

    while (ae != NULL)
    {
        while (be != NULL)
        {
            if (ae->node == be->node)
            {
                return (ae->node);
            }
            be = be->next;
        }
        ae = ae->next;
    }
    return (NULL);
}

node *
findShortest(
    node * n,
    node * end)
{
    //Find the edge which brings the node closest to the end point.
    node *path = NULL;
    int dist = 999999;
    edge *e = n->connected;

    while (e != NULL)
    {
        int c = checkDistance(e->node, end);

        if (c < dist && e->node->visited == 0 && e->node->deleted == 0)
        {
            dist = c;
            path = e->node;
        }
        e = e->next;
    }
    return (path);
}

void
destroy(
    node * root)
{
    //Destroy all nodes based on their tree formation.
    if (root == NULL)
        return;

    destroy(root->left);
    destroy(root->right);
    edge *e = root->connected;

    while (e != NULL)
    {
        edge *temp = e->next;

        free(e);
        e = temp;
    }
    free(root);
}

//If path is end, or next to end, don't run the following function.
node *
checkPath(
    node * path,
    node * end,
    node * common)
{

    if (path == NULL)
    {
        return (NULL);
    }

    path->visited = 1;
    node *shortPath = findShortest(path, end);
    node *alternate = NULL;
    edge *edgeCur = path->connected;

    //Find alternate.
    while (edgeCur != NULL)
    {
        //Find a node that is not a common node, and not the shortest node.
        if (edgeCur->node != common && edgeCur->node != shortPath &&
            edgeCur->node->visited == 0)
        {
            alternate = edgeCur->node;
        }
        edgeCur = edgeCur->next;
    }

    if (shortPath != common && shortPath->visited == 0 && shortPath != NULL)
    {
        puts("shortest");
        return (shortPath);
    }
    else if (alternate != NULL)
    {
        puts("alternate");
        return (alternate);
    }
    else if (common != NULL && common->visited == 0)
    {
        puts("common");
        return (common);
    }
    else
    {
        puts("collision");
        return (NULL);
    }
}


void
startPaths(
    node * start,
    node * end)
{
    //Starts two concurrent paths from the start node, looking for the end node,
    //And manages them throughout their run.
    node *collisionPoint = NULL;
    node *path1 = NULL;
    int dist1 = 999999;         //Arbitrary high placeholder.

    node *path2 = NULL;

    //Increment when a path reaches end.
    int paths = 0;
    edge *e = start->connected;

    //Find the two paths from start closest to the end point.
    while (e != NULL)
    {
        int c = checkDistance(e->node, end);

        if (c == 0)
        {
            puts("END NEXT TO START");
            ++paths;
        }
        else if (c < dist1)
        {
            path2 = path1;
            dist1 = c;
            path1 = e->node;
        }
        e = e->next;
    }
    if (path1 != NULL)
    {
        printf("start path1: %d\n", path1->ID);
        path1->visited = 1;
    }
    if (path2 != NULL)
    {
        printf("start path2: %d\n", path2->ID);
        path2->visited = 1;
    }
    if (endProbe(path1, end))
    {
        printf("path1 (%d) next to end\n", path1->ID);
        ++paths;
        path1 = NULL;
    }
    if (path2 != NULL && endProbe(path2, end))
    {
        printf("path2 (%d) next to end\n", path2->ID);
        ++paths;
        path2 = NULL;
    }
    if (paths > 1)
    {
        printf("PATHS FOUND AT BEGINNING\n");
        return;
    }

    while (collisionPoint == NULL && paths < 2)
    {
        node *common = NULL;

        if (path1 == NULL && path2 == NULL)
        {
            puts("-----No paths-----");
            return;
        }

        else if (path1 != NULL && path2 != NULL)
        {
            common = commonCheck(path1, path2);
        }

        if (path1 != NULL)
        {
            path1 = checkPath(path1, end, common);
            if (path1 != NULL)
            {
                if (path1 == end || endProbe(path1, end))
                {
                    ++paths;
                }
                path1->visited = 1;
            }
        }

        if (path2 != NULL)
        {
            path2 = checkPath(path2, end, common);
            if (path2 != NULL)
            {
                if (path2 == end || endProbe(path2, end))
                {
                    ++paths;
                }
                path2->visited = 1;
            }
        }

    }
    puts("Paths found");
}
