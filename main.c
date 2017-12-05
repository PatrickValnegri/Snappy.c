/*
 ============================================================================
 Name        : main.c
 Author      :
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define BUF_SIZE 33 //lavoro a 32 bit (max 2^32 -1)
#include "LEB128.c"

const int tempSize = 60;	//Lunghezza del blocco di lettura dell'fread
const int minCorr = 4;		//Lunghezza corrsipondenza minima


//Calcolo di quanti blocchi da 4 ci stanno nei bytes letti
int nrBlocksOfMinCorr(int bytesReaded){
    return bytesReaded-(minCorr*2-1);
}

//Calcolo di quanti sotto blocchi di confronto da 4 ci sono nei bytes letti
int nrSubBlocksOfMinCorr(int bytesReaded, int start, int nrBlock){
    return bytesReaded - start - (nrBlock + minCorr) - minCorr + 1;
}

int main() {
    FILE *file_uncompressed, *file_compressed;
    file_uncompressed   = fopen("file.txt", "rb");	//File aperto in read binario
    file_compressed     = fopen("fileo.txt", "w");	//File aperto in write

    char buffer[tempSize];	//Array di appoggio per la lettura dei byte, una cella un byte (char)
    unsigned int sizeFile;	//Variabile di appoggio per salvare la lunghezza del file
    int bytesReaded;		//Variabile di appoggio per salvare quanti bytes sono stati letti da fread

    if(file_uncompressed && file_compressed) {

        //Lunghezza del file
        fseek(file_uncompressed, 0, SEEK_END);		//Puntatore del file messo alla fine
        sizeFile = ftell(file_uncompressed);		//Lunghezza del file salvato in variabile
        rewind(file_uncompressed);					//Puntatore riportato all'inizio

        //VARINT
        char buffer[BUF_SIZE];
        buffer[BUF_SIZE - 1] = '\0';
        char *ptr = buffer;

        int2bin(127, ptr, BUF_SIZE - 1);
        printf("Binario: %s\n", ptr);

        varint_encode(ptr,127);

        int nrBlocksRedead = 0;
        int start = 0;

        //Lettura del file a blocchi di tempSize
        while((bytesReaded = fread(buffer, sizeof(char), tempSize, file_uncompressed))) {
            int nrBlocks = 0;
            nrBlocks = nrBlocksOfMinCorr(bytesReaded);  //Numero di blocchi (pattern) da 4 da confrontare

            if(nrBlocks > 0) {
                for(int i = 0; i < nrBlocks; i++) {

                    int nrSubBlocks = 0;
                    nrSubBlocks = nrSubBlocksOfMinCorr(bytesReaded, start, i);   //Numero di sotto blocchi da 4 da confrontare con il pattern

                    for(int j = 0; j < nrSubBlocks; j++) {
                        int x = 0;
                        while(buffer[start+i+x] == buffer[start+i+minCorr+j+x]) {
                            x++;
                        }
                        if(x>=4){
                            printf("----Corrispondenza----");
                            printf("I: %d", start+i);
                            printf("Blocchi: %d", nrBlocks);
                            printf("start: %d", (i+minCorr)+(j+x));
                            printf("nrBlocks: %d\n", nrBlocksOfMinCorr(bytesReaded-((i+minCorr)+(j+x))));

                            //RESET
                            start = (i+minCorr)+(j+x);                          //Nuova posizione di inizio
                            nrBlocks = nrBlocksOfMinCorr(bytesReaded-start);
                            break;//Esco dal for di j
                        }
                    }
                }
            }
            //nrBlocksRedead += bytesReaded;  //Posizione incrementata
        }

        fclose(file_uncompressed);
        fclose(file_compressed);
    }

    return 0;
}
