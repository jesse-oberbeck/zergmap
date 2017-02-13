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
    ///////////////////////////

    //Check for file name provided as arg.
    if (argc < 2)
    {
        fprintf(stderr, "Please provide a file name.\n");
        return (1);
    }
    else if (access(argv[1], F_OK) == -1)
    {
        fprintf(stderr, "Invalid file name.\n");
        return (1);
    }

    char *file = argv[1];
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
            zerg1Decode(words, zh);
        }

        //Handle Command packets.
        if (zergType == 2)
        {
            zerg2Decode(words);
        }

        //Handle GPS packets.
        if (zergType == 3)
        {
            zerg3Decode(words, nodes);
        }

        if (result > 0)
        {
            fseek(words, result, SEEK_CUR);
        }
        current_pos = ftell(words);
        puts("~");
    }
    puts("PRINTEM!!");
    //printf("ROOT: %p\n", nodes);
    scrollNodes(nodes, nodes, findAdjacencies);
    printf("CONNECTED TO ROOT: %p\n", nodes->lat);
    //findPath(nodes);
    printem(nodes);
    free(zh);
    free(c);
    fclose(words);
    return (0);
}
