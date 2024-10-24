#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100  
#define MAX_INPUT 50    
#define MAX_TRANSACTIONS 100  

typedef struct Node {
    char transactionID[MAX_INPUT];  
    struct Node* next;              
} Node;

Node* hashTable[TABLE_SIZE];

unsigned int hashFunction(char* transactionID) {
    unsigned int hash = 0;
    for (int i = 0; transactionID[i] != '\0'; i++) {
        hash += transactionID[i];
    }
    return hash % TABLE_SIZE;
}

void insertTransaction(char* transactionID) {
    unsigned int hashIndex = hashFunction(transactionID);

    Node* temp = hashTable[hashIndex];
    while (temp != NULL) {
        if (strcmp(temp->transactionID, transactionID) == 0) {
            printf("Fraud detected: Duplicate transaction ID '%s'\n", transactionID);
            return;
        }
        temp = temp->next;
    }
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->transactionID, transactionID);
    newNode->next = hashTable[hashIndex];
    hashTable[hashIndex] = newNode;

    printf("Transaction ID '%s' added successfully.\n", transactionID);
}

void freeHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* temp = hashTable[i];
        while (temp != NULL) {
            Node* toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
}

int main() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    int numTransactions;
    char transactionID[MAX_INPUT];

    printf("Enter the number of transactions: ");
    scanf("%d", &numTransactions);

    for (int i = 0; i < numTransactions; i++) {
        printf("Enter transaction ID %d: ", i + 1);
        scanf("%s", transactionID);
        insertTransaction(transactionID);
    }

    freeHashTable();

    return 0;
}
