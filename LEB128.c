/*
 ============================================================================
 Name        : LEB128.c
 Author      : 
 Version     : Non ottimizzata
 Copyright   :
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define BUF_SIZE 33 //lavoro a 32 bit (max 2^32 -1)

void printMatrix(char varint[][8], int righe, int colonne) {
	for (int i = righe - 1; i >= 0; i--) {
		for (int j = colonne - 1; j >= 0; j--) {
			printf("%c", varint[i][j]);
		}
		printf("\n");
	}
}

//aver un riscontro visivo con i bit
char *int2bin(unsigned int a, char *buffer, int buf_size) {
	buffer += (buf_size - 1);

	for (int i = 31; i >= 0; i--) {
		*buffer-- = (a & 1) + '0';

		a >>= 1;
	}

	return buffer;
}

unsigned varint_length(unsigned int v) {
	unsigned len = 1;
	while (v >= 128) {
		v >>= 7;
		len++;
	}
	return (len);
}

void varint_encode(char *ptr, unsigned int data) {
	//dividere il numero binario in 5 array da 8 bit
	//fare un & 0111 1111 per prendere 7 bit ed uno 0 davanti ai 7 bit
	//eseguire un | 1000 0000 per assegnare un 1 davanti
	//poi devo shiftare di 7 dentro l'array e rieseguire l'operazione

	char varint[5][8];
	int righe = sizeof(varint) / sizeof(varint[0]);
	int colonne = sizeof(varint[0]) / sizeof(varint[0][0]);
	unsigned int len = varint_length(data);

	int cont = BUF_SIZE - 1;
	for (int i = 0; i < righe; i++) {
		for (int j = 0; j < colonne; j++) {
			if ((j == 7 || cont <= 0) && len == 0) {
				varint[i][j] = '0';
			} else if (j == 7 && len > 0) {
				varint[i][j] = '1';
				len--;
			} else {
				varint[i][j] = ptr[cont - 1];
				cont--;
			}
		}
	}
	printMatrix(varint, righe, colonne);
}

/*int main() {
	char buffer[BUF_SIZE];
	buffer[BUF_SIZE - 1] = '\0';
	char *ptr = buffer;

	int2bin(127, ptr, BUF_SIZE - 1);
	printf("Binario: %s\n", ptr);

	//unsigned int len = varint_length(624485);
	//printf("Lunghezza: %d\n",len);

	varint_encode(ptr,127);

}*
