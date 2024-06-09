#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Project {
	unsigned int id;
	char* title;
	char* contractor;
	unsigned char daysToDeploy;
	float buget;
	float percentageDone;
}Project;

typedef struct Node {
	struct Node* left;
	struct Node* right;
	Project info;
}Node;


Project initProject(unsigned int id, char* title, char* contractor, unsigned char daysToDeploy, float buget, float percentageDone) {
	Project p = {
		id,
		_strdup(title),
		_strdup(contractor),
		daysToDeploy,
		buget,
		percentageDone
	};
	return p;
}

void freeProject(Project p) {
	free(p.title);
	free(p.contractor);
}

void printProject(Project p) {
	printf("Project: id: %u, title %s, contractor:%s, duration: %hhu, buget:%.2f, progress:%.1f percent\n",
		p.id, p.title, p.contractor, p.daysToDeploy, p.buget, p.percentageDone * 100);
}

void insertInTree(Node** root, Project p) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->info = p;
	newNode->left = newNode->right = NULL;
	if (*root == NULL) {
		
		*root = newNode;
	}
	else if (p.id < (*root)->info.id) {
		insertInTree(&(*root)->left, p);
	}
	else {
		insertInTree(&(*root)->right, p);
	}
}

void readFromFile(Node** root, const char* fileName) {
	FILE* f = fopen(fileName, "r");

	if (!f) return;

	unsigned int id;
	char title[256];
	char contractor[256];
	unsigned char daysToDeploy;
	float buget;
	float percentageDone;
	while (fscanf(f, "%u \"%[^\"]\" \"%[^\"]\" %hhu %f %f",
		&id, title, contractor, &daysToDeploy, &buget, &percentageDone) == 6) {
		insertInTree(root, initProject(id, title, contractor, daysToDeploy, buget, percentageDone));
	}
	fclose(f);
}

void preorderPrint(const Node* root) {
	if (!root) return;

	printProject((*root).info);
	preorderPrint((*root).left);
	preorderPrint((*root).right);
}

void inorderPrint(const Node* root) {
	if (!root) return;

	inorderPrint((*root).left);
	printProject((*root).info);
	inorderPrint((*root).right);
}

void freeTree(Node** root) {
	if (!(*root)) return;

	freeTree(&(*root)->left);
	freeTree(&(*root)->right);
	freeProject((*root)->info);
	free(*root);
	*root = NULL;
}

int countContractor(const Node* root, char* contractorName) {
	if(!root) return 0;

	if (strcmp(root->info.contractor, contractorName) == 0) {
		return 1 + countContractor(root->left, contractorName) + countContractor(root->right, contractorName);
	}
	else 	return countContractor(root->left, contractorName) + countContractor(root->right, contractorName);

}

void modifyProgress(Node* root, unsigned int id, float increase) {
	if (!root) return;

	if (root->info.id == id) {

		root->info.percentageDone = increase;
	}
	else if (id < root->info.id) {
		modifyProgress(root->left, id, increase);

	}else{
		modifyProgress(root->right, id, increase);
	}
}

void removeLeaves(Node** root) {
	if (!(*root)) return;

	if ((*root)->left == NULL && (*root)->right == NULL) {
		freeProject((*root)->info);
		free(*root);
		*root = NULL;
	}
	else {
		removeLeaves(&(*root)->left);
		removeLeaves(&(*root)->right);

	}
}

int main() {
	Node* root = NULL;
	readFromFile(&root, "data.txt");
	preorderPrint(root);
	printf("\nFound: %d contractors", countContractor(root,"OpenAI"));
	unsigned int id= 3;
	modifyProgress(root, id, 0.99);
	printf("\nAfter increase to id %u: \n", id);
	preorderPrint(root);

	
	printf("\nAfter removing leaves\n");
	removeLeaves(&root);
	preorderPrint(root);

	freeTree(&root);
	
}