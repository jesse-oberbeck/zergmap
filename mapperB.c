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
    //Find distance between two points based on latitude and longitude.
    //DOES NOT CURRENTLY ACCOUNT FOR ALTITUDE!
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
    //Wrapper for haversine, so that two nodes can be passed.
    //For ease of use/typing, as it reduces the number of parameters.
    //Also serves as a simplified yes/no check for "is in range."
    int dist = haversine(a->lat, a->lon, b->lat, b->lon);
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
    //Nodes are placed into an "array" of nodes. Each node contains a list of integers.
    //These integers represent the position/index of every node.
    //This is done so that a path can be followed by checking a node's "connected" field,
    //Choosing an int from that list, and using it as an index for the "nodes" set.
    int i = 0;
    //Insert new node into node array.
    while(nodes[i] != NULL)
    {
        int j = 0;
        if(checkAdjacency(nodes[i], node))//if adjacent, add index to current node adjacencies.
        {
            while(node->connected[j] != NULL)
            //Add the existing node's indice to the current node's "connected" list.
            {
                ++j;
            }
            //Make an int pointer, add it to the set of int pointers (connected).
            int *num = malloc(sizeof(int));
            *num = i;
            node->connected[j] = num;
            
            //Add the current node's indice to the existing node's connected list.
            int k = 0;
            while(nodes[i]->connected[k] != NULL)
            {
                ++k;
            }
            //Make an int pointer, add it to the set of int pointers (connected).
            int *newNodeNum = malloc(sizeof(int));
            *newNodeNum = j;
            nodes[i]->connected[k] = newNodeNum;
        }
        ++i;
    }
    //Place the new node at the end of the array of existing nodes.
    nodes[i] = node;
}


int main(void)
{
    node **nodes;
    
}

