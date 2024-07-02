#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Candidat {
	int cod;
	char* numeCandidat;
	char* facultate;
	float medie;
	int salariu;
}Candidat;

typedef struct Node {
	struct Node* next;
	Candidat info;
}Node;

typedef struct HT {
	int size;
	Node** buckets;
	
}HT;

Candidat initCandidat(int cod, char* numeCandidat, char* facultate, float medie, int salariu) {
	Candidat c = {
		cod,
		_strdup(numeCandidat),
		_strdup(facultate),
		medie,
		salariu
	};

	return c;
}

void printCandidat(Candidat c) {

	printf("\nCod: %d\nNume Candidat: %s\nFacultate: %s\nMedie: %.2f\nSalariu: %d\n", 
		c.cod, c.numeCandidat, c.facultate, c.medie, c.salariu);
}

HT initHT(int size) {
	HT ht = {
		size,
		(Node**)calloc(size, sizeof(Node*))
	};
	return ht;
}

int hashFunction(char* facultate, int size) {
	return strlen(facultate) % size;
}

void insertInHT(HT* ht, Candidat c) {
	int pos = hashFunction(c.facultate, ht->size);

	Node* newNode = (Node*)calloc(1, sizeof(Node));

	newNode->next = NULL;
	newNode->info = c;

	if (ht->buckets[pos] == NULL) {
		ht->buckets[pos] = newNode;
	}
	else {
		Node* currnt = ht->buckets[pos];
		while (currnt->next) {
			currnt = currnt->next;
		}
		currnt->next = newNode;
	}
}

void readFromFile(const char* filename, HT* ht) {
	FILE* f = fopen(filename, "r");

	int cod;
	char numeCandidat[256];
	char facultate[256];
	float medie;
	int salariu;

	while (fscanf(f, "%d %s %s %f %d", &cod, numeCandidat, facultate, &medie, &salariu) == 5) {
		insertInHT(ht, initCandidat(cod, numeCandidat, facultate, medie, salariu));
	}

	fclose(f);
}

void printHT(HT ht) {
	for (int i = 0; i < ht.size; i++)
	{
		printf("\nPozitie: %d\n", i);

		Node* current = ht.buckets[i];
		while (current)
		{
			printCandidat(current->info); current = current->next;
		}

		
	}
	printf("\n----------------------------------------\n");
}

void freeList(Node** head) {
	if (*head == NULL) {
		return;
	}

	while (*head != NULL) {

		Node* it = *head;
		(*head) = (*head)->next;
		free(it->info.numeCandidat);
		free(it->info.facultate);
		free(it);
	}
}

void freeHT(HT* ht) {
	for (int i = 0; i < ht->size; i++) {
		freeList(&ht->buckets[i]);
	}
}



float medieMaxima(HT ht, const char* facultate) {

	int pos = hashFunction(facultate, ht.size);

	if (ht.buckets[pos] != NULL) {
		Node* current = ht.buckets[pos];
		float maxMedie = current->info.medie;
		current = current->next;
		while (current) {
			if (current->info.medie > maxMedie) {
				maxMedie = current->info.medie;
			}
			current = current->next;
		}
		return maxMedie;
	}
	else return 0;
}

void updateSalariu(HT* ht, int cod, int salariuNou) {
	
	for (int i = 0; i < ht->size; i++) {
		Node* current = ht->buckets[i];

		while (current) {
			if (current->info.cod == cod) {
				current->info.salariu = salariuNou;
			}
			current = current->next;
		}
	}
}


void deleteCandidat(HT* ht, char* nume) {

	for (int i = 0; i < ht->size; i++) {
		Node* current = ht->buckets[i];
		Node* prev = NULL;

		while (current) {
			if (strcmp(current->info.numeCandidat, nume) == 0) {
				if (prev == NULL) {
					ht->buckets[i] = current->next;
				}
				else {
					prev->next = current->next;
				}
				free(current->info.numeCandidat);
				free(current->info.facultate);
				free(current);
				return;
			}
			prev = current;
			current = current->next;
		}
	}
}


int main() {
	HT ht = initHT(10);
	printHT(ht);
	printf("\n");
	readFromFile("data.txt", &ht);
	printHT(ht);
	const char* fac = "ComputerScience";
	printf("\nMedie maxima pt facultea %s: %.2f", fac, medieMaxima(ht, fac));

	printf("\n");

	updateSalariu(&ht, 3, 66600);
	printHT(ht);

	deleteCandidat(&ht, "ClaraBrown");
	printHT(ht);

	freeHT(&ht);
}
