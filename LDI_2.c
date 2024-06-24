#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

typedef struct Prescription {
	unsigned int prescriptionNo;
	char* pacientName;
	char* doctorName;
	char* specialStatus;
	unsigned char numberOfDrugs;
	float percentage;

}Prescription;

struct Node {
	Prescription info;
	Node* next;
	Node* prev;
};

typedef struct LD {
	Node* head;
	Node* tail;
}LD;

Prescription initPrescription(unsigned int prescriptionNo, char* pacientName, 
	char* doctorName, char* specialStatus, unsigned char numberOfDrugs, float percentage) {


	Prescription p = {
		prescriptionNo,
		_strdup(pacientName),
		_strdup(doctorName),
		_strdup(specialStatus),
		numberOfDrugs,
		percentage
	};
	return p;
}

void printPrescription(Prescription p) {
	printf("\n \nPrescription no. %u, patient %s, doctor %s, special status %s, number of drugs prescribed %i, compensation %.2f ",
		p.prescriptionNo, p.pacientName, p.doctorName, 
		p.specialStatus, p.numberOfDrugs, p.percentage);
}

void insertRight(Prescription p, LD* list) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = p;
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

void insertLeft(Prescription p, LD* list) {
	Node* NN = (Node*)malloc(sizeof(Node));
	NN->info = p;
	NN->next = list->head;
	NN->prev = NULL;

	if (list->head) {
		list->head->prev = NN;
		list->head = NN;
	}
	else {
		list->head = list->tail = NN;
	}
}

void insertAtPosition(Prescription p, LD* list, int pos) {

	Node* nn = (Node*)malloc(sizeof(Node));
	nn->info = p;
	nn->next = nn->prev = NULL;

	if (!(list->head)) {
		list->head = list->tail = nn;
	}
	else {
		int i = 0;
		Node* current = list->head;

		while (current && i < pos) {
			current = current->next;
			i++;
		}
		if (!current) {
			list->tail->next = nn;
			nn->prev = list->tail;
			list->tail = nn;
		}
		else if (current == list->head) {
			list->head->prev = nn;
			nn->next = list->head;
			list->head = nn;
		}
		else {
			current->prev->next = nn;
			nn->next = current;
			nn->prev = current->prev;
			current->prev = nn;
		}

	}
}

void readFromFile(LD* list, const char* fileName) {
	FILE* f = fopen(fileName, "r");
	if (!f) return;

	unsigned int prescriptionNo;
	char pacientName[256];
	char doctorName[256];
	char specialStatus[256];
	unsigned char numberOfDrugs;
	float percentage;

	while (fscanf(f,"%u %99s %99s %99s %hhu %f", &prescriptionNo, pacientName, doctorName, specialStatus, &numberOfDrugs, &percentage) == 6) {
		insertAtPosition(initPrescription(prescriptionNo, pacientName, doctorName, specialStatus, numberOfDrugs, percentage),list, 10);
	}
}

void printList(LD* list) {
	if (!list) return;

	Node* current = list->head;
	while (current)
	{
		printPrescription(current->info);
		current = current->next;
	}
}

void freeList(LD* list) {
	Node* current = list->head;
	while (current)
	{
		free(current->info.pacientName);
		free(current->info.doctorName);
		free(current->info.specialStatus);
		current = current->next;
		if (current) free(current->prev);
	}
	
	list->head = list->tail = NULL;

}

int nuberOfDrugsOver(LD* list, unsigned char value) {
	Node* current = (Node*)malloc(sizeof(current));
	current = list->head;
	int noDrugs = 0;
	while (current) {
		if (current->info.numberOfDrugs >= value) {
			noDrugs++;
		}
		current = current->next;
	}
	return noDrugs;
}

void updatePercentage(LD* list, const char* name) {
	Node* current = (Node*)malloc(sizeof(Node));
	current = list->head;
	while (current) {
		if (strcmp(current->info.pacientName, name) == 0) {
			current->info.percentage += 10;
		}
		current = current->next;
	}
}

void deleteNode(LD* list, unsigned char value) {

	Node* current = (Node*)malloc(sizeof(Node));
	current = list->head;
	while (current)
	{
		Node* next = current->next;
		if (current->info.numberOfDrugs < value)
		{
			if (current->next) {
				current->next->prev = current->prev;
				//current->next = NULL;
			}else {
				list->tail = current->prev;
			}
			

			if (current->prev)
			{
				current->prev->next = current->next;
				//current->prev = NULL;

			}else {

				list->head = current->next;
			}
			

			free(current->info.pacientName);
			free(current->info.doctorName);
			free(current->info.specialStatus);
			free(current);
		}

		current = next;
		
	}
}
 
int main() {
	LD list = { NULL, NULL };
	readFromFile(&list, "data.txt");
	printList(&list);

	printf("\n");
	unsigned char value = 10;

	printf("Number of prescriptions with al least % i drugs is : % i\n", value, nuberOfDrugsOver(&list, value));

	updatePercentage(&list, "BobSmith");
	printf("\nList after update");
	printList(&list);

	printf("\nList after second update");
	deleteNode(&list, 3);
	printList(&list);

	freeList(&list);

	return 0;
}
