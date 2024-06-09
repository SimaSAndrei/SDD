#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

typedef struct Prescription {
	unsigned int prescriptionNo;
	char* patientName;
	char* doctorName;
	char* specialStatus;
	unsigned char numberOfdrugs;
	float percentage;
}Prescription;

struct Node {
	Prescription info;
	Node* next;
	Node* prev;

};

typedef struct LDI {
	Node* head;
	Node* tail;
}LDI;

void printPrescriptionData(Prescription prescription) {
	printf("\nPrescription no. %u, patient %s, doctor %s, special status %s, number of drugs prescribed %i, compensation %.2f",
		prescription.prescriptionNo, prescription.patientName, prescription.doctorName,
		prescription.specialStatus, prescription.numberOfdrugs, prescription.percentage);
}

Prescription initPrescription(unsigned int prescriptionNo, const char* patientName,
	const char* doctorName, const char* specialStatus,
	unsigned char numberOfdrugs, float percentage) {

	Prescription p;
	p.prescriptionNo = prescriptionNo;
	p.patientName = (char*)malloc(sizeof(char) * (strlen(patientName) + 1));
	strcpy(p.patientName, patientName);

	p.doctorName = (char*)malloc(sizeof(char) * (strlen(doctorName) + 1));
	strcpy(p.doctorName, doctorName);

	p.specialStatus = (char*)malloc(sizeof(char) * (strlen(specialStatus) + 1));
	strcpy(p.specialStatus, specialStatus);

	p.numberOfdrugs = numberOfdrugs;

	p.percentage = percentage;

	return p;
}

void insertRight(Prescription p, LDI* list) {
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

void insertLeft(Prescription p, LDI* list) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = p;
	newNode->next = list->head;
	newNode->prev = NULL;


	if (list->head) {
		list->head->prev = newNode;

	}
	list->head = newNode;

	if (list->tail == NULL) {
		list->tail = newNode;
	}
}

void readFromFileRight(LDI* list, const char* fileName) {
	FILE* f = fopen(fileName, "r");
	if (!f) return;

	unsigned int prescriptionNo;
	char patientName[256];
	char doctorName[256];
	char specialStatus[256];
	unsigned char numberOfdrugs;
	float percentage;

	while (fscanf(f, "%u %99s %99s %99s %hhu %f", &prescriptionNo, patientName, doctorName, specialStatus, &numberOfdrugs, &percentage) == 6) {
		insertRight(initPrescription(prescriptionNo, patientName, doctorName, specialStatus, numberOfdrugs, percentage), list);
	}
}

void readFromFileLeft(LDI* list, const char* fileName) {
	FILE* f = fopen(fileName, "r");
	if (!f) return;

	unsigned int prescriptionNo;
	char patientName[256];
	char doctorName[256];
	char specialStatus[256];
	unsigned char numberOfdrugs;
	float percentage;

	while (fscanf(f, "%u %99s %99s %99s %hhu %f", &prescriptionNo, patientName, doctorName, specialStatus, &numberOfdrugs, &percentage) == 6) {
		insertLeft(initPrescription(prescriptionNo, patientName, doctorName, specialStatus, numberOfdrugs, percentage), list);
	}
}

void printList(LDI* list) {
	if (!list) {
		printf("/nList is empty!!!!");
	}
	Node* current = list->head;
	while (current) {
		printPrescriptionData(current->info);
		current = current->next;
	}
}



void insertAtPosition(Prescription p, LDI* list, int position) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = p;
	newNode->prev = newNode->next = NULL;

	if (!(list->head)) {
		list->head = list->tail = newNode;
	}
	else {
		Node* current = list->head;
		int i = 0;

		while (current && i < position) {
			current = current->next;
			i++;
		}
		if (!current) {
			list->tail->next = newNode;
			newNode->prev = list->tail;
			list->tail = newNode;

		}
		else if (current == list->head) {
			newNode->next = list->head;
			list->head->prev = newNode;
			list->head = newNode;
		}
		else {
			newNode->next = current;
			current->prev->next = newNode;
			current->prev = newNode;
			newNode->prev = current->prev;
		}
	}

}

void deleteNode(LDI* list, unsigned char value) {
	Node* current = (Node*)malloc(sizeof(Node));
	current = list->head;
	while (current) {
		Node* next = current->next;
		if (current->info.numberOfdrugs < value) {
			if (current->next) {
				current->next->prev = current->prev;
			}
			else {
				list->tail = current->prev;
			}
			if (current->prev) {
				current->prev->next = current->next;
			}
			else {
				list->head = current->next;
			}

			free(current->info.patientName);
			free(current->info.doctorName);
			free(current->info.specialStatus);
			free(current);
		}
		current = next;

	}
}

void freeList(LDI* list) {
	Node* current = list->head;
	while (current) {
		free(current->info.patientName);
		free(current->info.doctorName);
		free(current->info.specialStatus);
		current = current->next;
		if (current) free(current->prev);
	}
	free(list->tail);
	list->head = list->tail = NULL;
}



int main() {
	LDI list = { NULL, NULL };
	readFromFileRight(&list, "data.txt");
	printList(&list);
	deleteNode(&list, 3);
	printf("\n");
	printList(&list);
	freeList(&list);

	printf("\n");

	readFromFileLeft(&list, "data.txt");
	printList(&list);


	deleteNode(&list, 3);
	printf("\n");
	printList(&list);
	// Assuming a function to free the list's resources properly

	freeList(&list);


	return 0;

}