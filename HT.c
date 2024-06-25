#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reservation {
	unsigned int id;
	char* hotelName;
	unsigned char hotelCategory;
	char* clientName;
	unsigned char noOfDays;
	float sumToPay;

}Reservation;

typedef struct Node {
	Reservation info;
	struct Node* next;
}Node;

typedef struct HT {
	int size;
	Node** buckets;
}HT;

Reservation initReservation(unsigned int id, char* hotelName, unsigned char hotelCategory, char* clientName, unsigned char noOfDays, float sumToPay) {

	Reservation r = {
		id,
		_strdup(hotelName),
		hotelCategory,
		_strdup(clientName),
		noOfDays,
		sumToPay
	};

	return r;
}

void printReservation(Reservation r) {
	printf("\n\nReservation id: %u\nHotel: %s\nCategory: %hhu\nClient name: %s\nDays of stay: %hhu\nTotal to pay:%.2f\n",
		r.id, r.hotelName, r.hotelCategory, r.clientName, r.noOfDays, r.sumToPay);
}

int hashFunction(unsigned char hotelCategory, int size) {
	return hotelCategory % size;
}

HT initHT(int size) {
	
	HT ht = {
		size,
		(Node**)calloc(size, sizeof(Node*))
	};

	return ht;
}


void insertHT(HT* ht, Reservation r) {
	int pos = hashFunction(r.hotelCategory, ht->size);

	Node* newNode = (Node*)calloc(1, sizeof(Node)); 
		
	newNode->next = NULL;
	newNode->info = r;

	if (ht->buckets[pos] == NULL) {
		ht->buckets[pos] = newNode;
	}
	else {
		Node* current = ht->buckets[pos];
		while(current->next) {
			current = current->next;
		}
		current->next = newNode;
	}

}

void readFromFile(HT* ht, const char* fileName) {
	FILE* f = fopen(fileName, "r");

	
	unsigned int id;
	char hotelName[256];
	unsigned char hotelCategory;
	char clientName[256];
	unsigned char noOfDays;
	float sumToPay;
	
	while (fscanf(f, "%u %s %hhu %s %hhu %f",	&id, hotelName, &hotelCategory, clientName, &noOfDays, &sumToPay) == 6) {
		Reservation r = initReservation(id, hotelName, hotelCategory, clientName, noOfDays, sumToPay);
		insertHT(ht, r);
		
	}

	fclose(f);
}

void printHT(HT ht) {
	for (int i = 0; i < ht.size; i++) {
		printf("\nPosition: %d", i);

		Node* copy = ht.buckets[i];
		while (copy)
		{
			printReservation(copy->info);
			copy = copy->next;
		}
	}
}

void freeList(Node** head)
{
	if (*head == NULL) return;

	while (*head != NULL)
	{
		Node* it = *head;
		(*head) = (*head)->next;
		free(it->info.clientName);
		free(it->info.hotelName);
		free(it);
	}
}

void freeHashTable(HT* ht)
{
	for (int i = 0; i < ht->size; i++)
	{
		freeList(&ht->buckets[i]);
	}
}

int noReservationCategory(HT ht, unsigned char cat) {
	int no = 0;
	for (int i = 0; i < ht.size; i++) {
		Node* current = ht.buckets[i];
		while (current) {
			if (current->info.hotelCategory == cat) {
				no++;
			}
			current = current->next;
		}
	}
	return no;
}


void deleteNodeFromHT(HT* ht, Node* prev, Node* current, int pos) {
	if (prev == NULL) { // head node
		ht->buckets[pos] = current->next;
	}
	else {
		prev->next = current->next;
	}
	free(current->info.clientName);
	free(current->info.hotelName);
	free(current);
}

void modifyHotelCategory(HT* ht, const char* hotelName, unsigned char newCategory) {
	for (int i = 0; i < ht->size; i++) {
		Node* current = ht->buckets[i];
		Node* prev = NULL;
		while (current) {
			if (strcmp(current->info.hotelName, hotelName) == 0) {
				Reservation r = current->info;
				int oldPos = hashFunction(r.hotelCategory, ht->size);
				r.hotelCategory = newCategory;
				r.clientName = _strdup(current->info.clientName);
				r.hotelName = _strdup(current->info.hotelName);
				deleteNodeFromHT(ht, prev, current, oldPos);
				insertHT(ht, r);
				return;
			}
			prev = current;
			current = current->next;
		}
	}
}


void ModifyHotelName(HT* ht, const char* buyer, const char* bought) {
	for (int i = 0; i < ht->size; i++) {
		Node* current = ht->buckets[i];
		while (current) {
			if (strcmp(current->info.hotelName, bought) == 0) {
				current->info.hotelName = _strdup(buyer);
				return;
			}
			current = current->next;
		}
	}
}



int main() {

	HT ht = initHT(10);
	printHT(ht);
	printf("\n");


	readFromFile(&ht, "data.txt");
	printHT(ht);



	printf("\n\n number is: ---->   %d", noReservationCategory(ht, 5));


	modifyHotelCategory(&ht, "HotelE", 3);
	printf("\n\nAfter modifying category:\n");
	printHT(ht);

	printf("\n\nAfter modifying hotel:\n");
	ModifyHotelName(&ht, "HotelA", "HotelB");
	printf("\n\n\n");
	printHT(ht);

	freeHashTable(&ht);
}




