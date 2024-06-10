#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Order {
    unsigned int orderId;
    char* releaseDate;
    char* deliveryDate;
    char* clientName;
    float paymentAmount;
} Order;

typedef struct Node {
    Order data;
    struct Node* next;
} Node;

typedef struct HashTable {
    int size;
    Node** buckets;
} HashTable;

Order createOrder(unsigned int id, const char* release, const char* delivery, const char* client, float amount) {
    Order newOrder = {
        id,
        _strdup(release),
        _strdup(delivery),
        _strdup(client),
        amount

    };
    
    return newOrder;
}

unsigned int hash(const char* str, int tableSize) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % tableSize;
}

void insertOrder(HashTable* ht, Order order) {
    int index = hash(order.clientName, ht->size);
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = order;
    newNode->next = ht->buckets[index];
    ht->buckets[index] = newNode;
}

HashTable* createHashTable(int size) {
    HashTable* ht = malloc(sizeof(HashTable));
    ht->size = size;
    ht->buckets = calloc(size, sizeof(Node*));
    return ht;
}

void readOrdersFromFile(const char* filename, HashTable* ht) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
    unsigned int id;
    char releaseDate[50], deliveryDate[50], clientName[50];
    float paymentAmount;
    while (fscanf(file, "%u %s %s %s %f", &id, releaseDate, deliveryDate, clientName, &paymentAmount) == 5) {
        Order order = createOrder(id, releaseDate, deliveryDate, clientName, paymentAmount);
        insertOrder(ht, order);
    }
    fclose(file);
}

void printOrder(Order order) {
    printf("Order ID: %u, Release Date: %s, Delivery Date: %s, Client Name: %s, Payment Amount: %.2f\n",
        order.orderId, order.releaseDate, order.deliveryDate, order.clientName, order.paymentAmount);
}

void printHashTable(const HashTable* ht) {
    if (!ht) {
        printf("Hash table is empty or not initialized.\n");
        return;
    }

    printf("Hash Table Contents:\n");
    for (int i = 0; i < ht->size; i++) {
        Node* node = ht->buckets[i];
        if (node == NULL) {
            printf("Bucket %d: empty\n", i);
        }
        else {
            printf("Bucket %d:\n", i);
            while (node != NULL) {
                printOrder(node->data);
                node = node->next;
            }
        }
    }
}

void freeOrder(Order* order) {
    // Free all dynamically allocated strings within an order.
    free(order->releaseDate);
    free(order->deliveryDate);
    free(order->clientName);
}

void freeList(Node* head) {
    // Free all nodes in the linked list
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        freeOrder(&temp->data);  // Free the order data inside the node
        free(temp);             // Free the node itself
    }
}

void freeHashTable(HashTable* ht) {
    // Check if the hash table is already null
    if (ht == NULL) return;

    // Iterate over each bucket
    for (int i = 0; i < ht->size; i++) {
        if (ht->buckets[i] != NULL) {
            freeList(ht->buckets[i]);  // Free each list in the bucket
        }
    }

    // Free the array of bucket pointers
    free(ht->buckets);

    // Free the hash table structure itself
    free(ht);
}

unsigned int countOrdersByClient(const HashTable* ht, const char* clientName) {
    unsigned int count = 0;
    if (ht == NULL) return count;

    // Iterate over each bucket
    for (int i = 0; i < ht->size; i++) {
        Node* current = ht->buckets[i];
        // Iterate over each node in the linked list
        while (current != NULL) {
            if (strcmp(current->data.clientName, clientName) == 0) {
                count++;  // Increment count if the client name matches
            }
            current = current->next;
        }
    }
    return count;
}

int updateDeliveryDate(HashTable* ht, unsigned int orderId, const char* newDate) {
    if (ht == NULL) return -1;  // Hash table not initialized

    // Calculate which bucket to search based on a hash function
    // Assuming orderId could be directly used or another hash function could be applied
    int index = orderId % ht->size;  // Simple modulo for the example; could be another hash method

    Node* current = ht->buckets[index];
    while (current != NULL) {
        if (current->data.orderId == orderId) {
            // Found the order, now update the delivery date
            free(current->data.deliveryDate);  // Free the old date
            current->data.deliveryDate = strdup(newDate);  // Assign the new date
            return 1;  // Success
        }
        current = current->next;
    }

    return 0;  // Order ID not found
}

int isEarlier(const char* date1, const char* date2) {
    // Returns 1 if date1 is earlier than date2; otherwise, returns 0
    return strcmp(date1, date2) < 0;
}

void deleteOldOrders(HashTable* ht, const char* currentDate) {
    if (ht == NULL) return;

    for (int i = 0; i < ht->size; i++) {
        Node* current = ht->buckets[i];
        Node* prev = NULL;

        while (current != NULL) {
            // Check if the order's delivery date is earlier than the current date
            if (isEarlier(current->data.deliveryDate, currentDate)) {
                Node* toDelete = current;

                // Remove the node from the list
                if (prev == NULL) {
                    // The node to delete is the first node in the list
                    ht->buckets[i] = current->next;
                }
                else {
                    prev->next = current->next;
                }

                current = current->next;  // Continue traversal

                // Free the memory allocated for the order's fields and the node itself
                free(toDelete->data.releaseDate);
                free(toDelete->data.deliveryDate);
                free(toDelete->data.clientName);
                free(toDelete);

            }
            else {
                prev = current;  // Move prev to current node
                current = current->next;  // Move to the next node
            }
        }
    }
}



int main() {
    HashTable* ht = createHashTable(10);  // Create a hash table with 10 buckets.
    readOrdersFromFile("data.txt", ht); // Assume orders.txt has the appropriate data format.

    printHashTable(ht);

    
    unsigned int result = countOrdersByClient(ht, "Alex");
    printf("\n\n Number of orders: %u ", result);

    freeHashTable(ht);
    // Implement other functionalities such as displaying the hash table, searching for an order, etc.
    return 0;
}
