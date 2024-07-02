#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Warehouse {
	unsigned char id;
	unsigned int capacity;
	unsigned char numberOfGrainTypes;
	char** grainNames;
}Warehouse;

typedef struct Node {
	Warehouse info;
	struct Node* next;
	struct Node* prev;
}Node;

typedef struct LD {
	Node* head;
	Node* tail;
}LD;

Warehouse initWarehouse(unsigned char id, unsigned int capacity, unsigned char numberOfGrainTypes, char** grainNames) {
	Warehouse w = {
		id,
		capacity,
		numberOfGrainTypes,
		(char**)malloc(numberOfGrainTypes * sizeof(char*))
	};

	if(w.numberOfGrainTypes) {
		for (int i = 0; i < numberOfGrainTypes; i++) {
			w.grainNames[i] = _strdup(grainNames[i]);
		}
	}

	return w;
}

void printWarehouse(Warehouse w) {
	printf("\nId: %hhu\nCapacity: %u\nNumber: %hhu\n", w.id, w.capacity, w.numberOfGrainTypes);
	printf("Grains: ");
	for (int i = 0; i < w.numberOfGrainTypes; i++) {
		printf("%s, ", w.grainNames[i]);
	}
	printf("\n");
}

void insertRight(Warehouse h, LD* list) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = h;
	newNode->next = NULL;
	newNode->prev = list->tail;

	if (list->tail) {
		list->tail->next = newNode;
		list->tail = newNode;
	}
	else {
		list->head = list->tail = newNode;
	}
}

void insertLeft(Warehouse h, LD* list) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = h;
	newNode->next = list->head;
	newNode->prev = NULL;

	if (list->head) {
		list->head->prev = newNode;
		list->head = newNode;
	}
	else {
		list->head = list->tail = newNode;
	}
}

void readFromFile(const char* filename, LD* list) {
	FILE* f = fopen(filename, "r");

	if (!f) return;

	unsigned char id;
	unsigned int capacity;
	unsigned char numberOfGrainTypes;
	char grainNames[256][256];

	while (fscanf(f, "%hhu %u %hhu", &id, &capacity, &numberOfGrainTypes) == 3) {

		char* grainPointers[256];

		for (int i = 0; i < numberOfGrainTypes; i++) {
			fscanf(f, "%s", grainNames[i]);
			grainPointers[i] = grainNames[i];
		}

		insertRight(initWarehouse(id, capacity, numberOfGrainTypes, grainPointers), list);
	}

	fclose(f);

}

void printLD(LD* list) {
	if (!list) return;
	Node* current = list->head;
	while (current)
	{	
		printWarehouse(current->info);
		current = current->next;
	}
}

void freeLD(LD* list) {
	Node* current = list->head;
	while (current) {
		Node* toDelete = current;
		current = current->next;
		for (int i = 0; i < toDelete->info.numberOfGrainTypes; i++) {
			free(toDelete->info.grainNames[i]);
		}
		free(toDelete->info.grainNames);
		free(toDelete);
	}
	list->head = list->tail = NULL;
}

void findTwoLargestWarehouses(LD* list, Warehouse* largest, Warehouse* secondLargest) {
	if (!list || !list->head) return;

	Node* current = list->head;
	*largest = current->info;
	*secondLargest = current->info;

	current = current->next;

	while (current) {
		if (current->info.capacity > largest->capacity) {
			*secondLargest = *largest;
			*largest = current->info;
		}
		else if (current->info.capacity > secondLargest->capacity) {
			*secondLargest = current->info;
		}
		current = current->next;
	}
}

void addGrainType(LD* list, unsigned char id, const char* newGrain) {
	Node* current = list->head;
	while (current) {
		if (current->info.id == id) {
			current->info.numberOfGrainTypes++;
			current->info.grainNames = (char**)realloc(current->info.grainNames, current->info.numberOfGrainTypes * sizeof(char*));
			current->info.grainNames[current->info.numberOfGrainTypes - 1] = _strdup(newGrain);
			return;
		}
		current = current->next;
	}
}

void deleteGrainType(LD* list, unsigned char id, const char* grainNameToDelete) {
	Node* current = list->head;
	while (current) {
		if (current->info.id == id) {
			for (int i = 0; i < current->info.numberOfGrainTypes; i++) {
				if (strcmp(current->info.grainNames[i], grainNameToDelete) == 0) {
					free(current->info.grainNames[i]);
					for (int j = i; j < current->info.numberOfGrainTypes - 1; j++) {
						current->info.grainNames[j] = current->info.grainNames[j + 1];
					}
					current->info.numberOfGrainTypes--;
					current->info.grainNames = (char**)realloc(current->info.grainNames, current->info.numberOfGrainTypes * sizeof(char*));
					return;
				}
			}
		}
		current = current->next;
	}
}

int main() {
	LD list = { NULL, NULL };

	readFromFile("data.txt", &list);
	printLD(&list);

	Warehouse first, second;

	findTwoLargestWarehouses(&list, &first, &second);
	printf("\nFirst\n");
	printWarehouse(first);
	printf("\nSecond\n");
	printWarehouse(second);
	printf("\n-----------------------------------------------------------------\n");
	printf("\n-----------------------------------------------------------------\n");
	printf("\n-----------------------------------------------------------------\n");
	addGrainType(&list, 6, "pula");

	printLD(&list);

	freeLD(&list);

	return 0;
}
