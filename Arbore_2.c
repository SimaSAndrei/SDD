#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct Reservation {
	unsigned short int id;
	char* hotelName;
	unsigned char numberOfOccupiedRooms;
	char* clientName;
	float totalToPay;
}Reservation;

typedef struct Node {
	struct Node* left;
	struct Node* right;
	Reservation info;
}Node;

Reservation initReservation(unsigned short int id, char* hotelName,
	unsigned char numberOfOccupiedRooms, char* clientName, float totalToPay) {

	Reservation r = {
		id,
		_strdup(hotelName),
		numberOfOccupiedRooms,
		_strdup(clientName),
		totalToPay
	};

	return r;
}

void printReservation(Reservation r) {
	printf("\nID: %hu\nHotel Name: %s\nNo. of occupied rooms: %hhu\nClient: %s\nTotal to pay: %.2f\n", 
		r.id, r.hotelName, r.numberOfOccupiedRooms, r.clientName, r.totalToPay);
}

void freeReservation(Reservation r) {
	free(r.hotelName);
	free(r.clientName);
}

void insertInTree(Node** root, Reservation r) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = r;
	newNode->left = newNode->right = NULL;

	if (!(*root)) {
		*root = newNode;
	}
	else {
		if (r.id < newNode->info.id) {
			insertInTree(&(*root)->left, r);
		}
		else {
			insertInTree(&(*root)->right, r);
		}
	}
}

void readFromFile(const char* filename, Node** root) {
	FILE* f = fopen(filename, "r");

	unsigned short int id;
	char hotelName[256];
	unsigned char numberOfOccupiedRooms;
	char clientName[256];
	float totalToPay;

	while (fscanf(f, "%hu %s %hhu %s %f", &id, hotelName, &numberOfOccupiedRooms, clientName, &totalToPay) == 5) {
		insertInTree(&(*root), initReservation(id, hotelName, numberOfOccupiedRooms, clientName, totalToPay));
	}

	fclose(f);
}

void preorderPrint(const Node* root) {
	if (!root) return;

	printReservation((*root).info);
	preorderPrint((*root).left);
	preorderPrint((*root).right);
}

void freeTree(Node** root) {

	if (!(*root)) {
		return;
	}

	freeTree(&(*root)->left);
	freeTree(&(*root)->right);
	freeReservation((*root)->info);
	free(*root);
	*root = NULL;
}

int isLeaf(Node* node) {
	return (node->left == NULL && node->right == NULL);
}

int totalRoomsFromLeafNodes(Node* root) {
	if (root == NULL) {
		return 0;
	}
	if (isLeaf(root)) {
		return root->info.numberOfOccupiedRooms;
	}
	return totalRoomsFromLeafNodes(root->left) + totalRoomsFromLeafNodes(root->right);
}

int treeHeight(Node* root) {
	if (root == NULL) {
		return 0;
	}
	int leftHeight = treeHeight(root->left);
	int rightHeight = treeHeight(root->right);
	return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

int main() {
	Node* root = NULL;
	readFromFile("data.txt", &root);

	preorderPrint(root);

	int totalRooms = totalRoomsFromLeafNodes(root);
	printf("\nTotal number of rooms reserved from leaf nodes: %d\n", totalRooms);

	int height = treeHeight(root);
	printf("\nHeight of the tree: %d\n", height);

	freeTree(&root);
}
