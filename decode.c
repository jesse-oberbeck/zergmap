#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <math.h>
#include <inttypes.h>
#include <endian.h>
#include <unistd.h>
//#include "structures.h"
#include "codec_functions.c"


int
main(
    int argc,
    char *argv[])
{
    ///////////////////////////
    node *nodes = calloc(sizeof(node), 1);
    int nodeCount = 0;
    ///////////////////////////

    //Check for file name provided as arg.
    if (argc < 2)
    {
        fprintf(stderr, "Please provide a file name.\n");
        return (1);
    }
    for(int i = 1; i < argc; ++i)
    {////////////////////////////////////////////////////////////////
    if (access(argv[i], F_OK) == -1)
    {
        fprintf(stderr, "Invalid file name.\n");
        return (1);
    }

    char *file = argv[i];
    FILE *words = fopen(file, "rb");
    int result = 0;
    int end_pos = fileSize(words);

    //Reads in file header once.
    struct FileHeader *fh = calloc(sizeof(*fh), 1);

    fread(fh, sizeof(struct FileHeader), 1, words);
    //printf("LLT: %x\n", fh->MajorVer);
    free(fh);

    int current_pos = ftell(words);

    struct ZergHeader *zh = calloc(sizeof(*zh), 1);
    struct Container *c = calloc(sizeof(*c), 1);

    //Loop to handle individual packets begins here.
    while (current_pos != end_pos)
    {
        result = processFile(words);
        if (result < 0)
        {
            free(zh);
            free(c);
            fclose(words);
            return (0);
        }

        processZergHeader(words, zh, c);
        int zergType = c->zergType;
        int totalLen = (ntohl(zh->TotalLen) >> 8) - 12;
        unsigned int check = 0;

        //Handle message packets.
        if (zergType == 0)
        {
            char *message = calloc(totalLen + 1, 1);

            check = fread(message, totalLen, 1, words);
            if (check > sizeof(*message))
            {
                fprintf(stderr, "Invalid packet detected. Stopping.\n");
                exit(1);
            }
            printf("%s\n", message);
            free(message);
        }

        //Handle Status packets.
        if (zergType == 1)
        {
            zerg1Decode(words, zh, nodes);
        }

        //Handle Command packets.
        if (zergType == 2)
        {
            zerg2Decode(words);
        }

        //Handle GPS packets.
        if (zergType == 3)
        {
            zerg3Decode(words, nodes, zh->Did, &nodeCount);//TODO: ID TYPE MIGHT HAVE TO BE CHANGED
        }

        if (result > 0)
        {
            fseek(words, result, SEEK_CUR);
        }
        current_pos = ftell(words);
        puts("~");
    free(zh);
    free(c);
    fclose(words);
    }
    }
    puts("PRINTEM!!");
    node *base = NULL;
    node *seed = nodes;
    nodes = nodes->next;
    base = nodes;
    while(base->next != NULL)
    {
        //puts("SCROLLIN SCROLLIN SCROLLIN");
        findAdjacencies(base, base);
        base = base->next;
    }
    puts("trim leaves...");
    trimLeaves(nodes, &nodeCount);
    //findPath(nodes);
    node *start = nodes;
    //printf("NODES REMAINING: %d\n", nodeCount);
    //printf("CONNECTED TO ROOT: %d\n", start->ID);
    if(nodeCount > 2)
    {
        //Move start to a non-deleted node.
        while(start != NULL && start->deleted == 1)
        {
            puts("next");
            start = start->next;
        }
        node *end = start->next;
        printf("END = %d\n",end->ID);
        printem(nodes);
        puts("\n\n");
        while(end != NULL)//TODO: break this if count < 3
        {
            if(end->deleted == 0)
            {
                printf("start: %d del: %d\n", start->ID, start->deleted);
                printf("end: %d del: %d\n", end->ID, end->deleted);
                startPaths(start, end);
                visitClear(nodes);
                
            }
            end = end->next;
        }
    }
    printem(nodes);
    free(seed);
    puts("PRINT TREE");
    printTree(nodes);
    printf("FIRST NODE: %d\n", nodes->ID);
    destroy(nodes);
    return (0);
}
