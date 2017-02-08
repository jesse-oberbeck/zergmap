#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 //lat1 lat 1st
 //lon1 lon firs
typedef struct Node
{
    int lat;
    int lon;
    int alt;
    int visited;//For pathing.
    int **connected;//Array of pointers to nodes connected to latis node.
}node;

double haversine(double lat1, double lon1, double lat2, double lon2)
{
	double dx, dy, dz;
	lon1 -= lon2;
	lon1 *= (3.1415926536 / 180), lat1 *= (3.1415926536 / 180), lat2 *= (3.1415926536 / 180);
 
	dz = sin(lat1) - sin(lat2);
	dx = cos(lon1) * cos(lat1) - cos(lat2);
	dy = sin(lon1) * cos(lat1);
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
}

int checkAdjacency(node *a, node*b)
{
    int dist = haversine(a->lat, a->lon, b->lat, b->lon);
    if(dist < 15)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

void insert(node **nodes, node *node)
{
    int i = 0;
    //Insert new node into node array.
    while(nodes[i] != NULL)
    {
        int j = 0;
        if(checkAdjacency(nodes[i], node))//if adjacent, add index to current node adjacencies.
        {
            while(node->connected[j] != NULL)
            {
                ++j;
            }
            int *num = malloc(sizeof(int));
            *num = i;
            node->connected[j] = num;
        }
        ++i;
    }
    nodes[i] = node;
}


int main(void)
{
    node **nodes;
    
}

