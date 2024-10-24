#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // For sleep()

#define TABLE_SIZE 100
#define MAX_TRANSACTIONS 100
#define MAX_WINDOW_TRANSACTIONS 5
#define TIME_WINDOW 300  // 5 minutes in seconds

// Structure for transaction data
typedef struct Transaction {
    char transaction_id[20];
    int amount;
    char date[11];
    char location[20];
    struct Transaction *next;
} Transaction;

// Hash Table
Transaction *hash_table[TABLE_SIZE];

// Structure for time-based transactions
typedef struct {
    time_t timestamp;
    char transaction_id[20];
} TransactionEvent;

TransactionEvent transaction_window[MAX_WINDOW_TRANSACTIONS];
int window_size = 0;

// Global transaction amount list for anomaly detection
int transaction_amounts[MAX_TRANSACTIONS];
int transaction_count = 0;

// Hash Function
unsigned int hash_function(char *transaction_id) {
    unsigned long hash = 0;
    int c;
    while ((c = *transaction_id++))
        hash = (hash * 33) ^ c;
    return hash % TABLE_SIZE;
}

// Add Transaction to Hash Table
void add_transaction(char *transaction_id, int amount, char *date, char *location) {
    unsigned int hash_index = hash_function(transaction_id);

    Transaction *new_transaction = (Transaction *)malloc(sizeof(Transaction));
    strcpy(new_transaction->transaction_id, transaction_id);
    new_transaction->amount = amount;
    strcpy(new_transaction->date, date);
    strcpy(new_transaction->location, location);
    new_transaction->next = NULL;

    if (hash_table[hash_index] == NULL) {
        hash_table[hash_index] = new_transaction;
        printf("Transaction %s added successfully.\n", transaction_id);
    } else {
        Transaction *current = hash_table[hash_index];
        while (current != NULL) {
            if (strcmp(current->transaction_id, transaction_id) == 0) {
                printf("Transaction %s is flagged as fraudulent (duplicate transaction).\n", transaction_id);
                return;
            }
            current = current->next;
        }
        new_transaction->next = hash_table[hash_index];
        hash_table[hash_index] = new_transaction;
        printf("Transaction %s added successfully.\n", transaction_id);
    }
}

// Detect a specific transaction (for fraud detection)
int detect_fraud(char *transaction_id) {
    unsigned int hash_index = hash_function(transaction_id);
    Transaction *current = hash_table[hash_index];
    
    while (current != NULL) {
        if (strcmp(current->transaction_id, transaction_id) == 0) {
            printf("Transaction %s is already recorded. Potential fraud detected!\n", transaction_id);
            return 1;
        }
        current = current->next;
    }
    printf("Transaction %s is unique.\n", transaction_id);
    return 0;
}

// Function to insert amount in a sorted array
void add_transaction_amount(int amount) {
    int i = transaction_count - 1;
    
    while (i >= 0 && transaction_amounts[i] > amount) {
        transaction_amounts[i + 1] = transaction_amounts[i];
        i--;
    }
    transaction_amounts[i + 1] = amount;
    transaction_count++;
}

// Function to detect if the amount is anomalous
void detect_anomaly(int amount) {
    if (transaction_count == 0) {
        printf("No previous transactions to compare with.\n");
        return;
    }

    int min_amount = transaction_amounts[0];
    int max_amount = transaction_amounts[transaction_count - 1];

    if (amount < min_amount * 0.5 || amount > max_amount * 1.5) {
        printf("Transaction of %d is flagged as anomalous.\n", amount);
    } else {
        printf("Transaction of %d is within normal range.\n", amount);
    }
}

// Add transaction to the sliding window for time-based detection
void add_transaction_event(char *transaction_id) {
    time_t current_time = time(NULL);
    
    if (window_size < MAX_WINDOW_TRANSACTIONS) {
        transaction_window[window_size].timestamp = current_time;
        strcpy(transaction_window[window_size].transaction_id, transaction_id);
        window_size++;
    } else {
        // Slide the window
        for (int i = 1; i < MAX_WINDOW_TRANSACTIONS; i++) {
            transaction_window[i - 1] = transaction_window[i];
        }
        transaction_window[MAX_WINDOW_TRANSACTIONS - 1].timestamp = current_time;
        strcpy(transaction_window[MAX_WINDOW_TRANSACTIONS - 1].transaction_id, transaction_id);
    }
}

// Detect fraud based on the number of transactions within the time window
void detect_time_based_fraud() {
    if (window_size < MAX_WINDOW_TRANSACTIONS) {
        printf("Transaction activity is normal.\n");
        return;
    }
    
    time_t current_time = time(NULL);
    
    // Check if the first and last transaction in the window are within 5 minutes
    if (difftime(current_time, transaction_window[0].timestamp) <= TIME_WINDOW) {
        printf("Multiple transactions in a short time. Potential fraud detected!\n");
    } else {
        printf("Transaction activity is normal.\n");
    }
}

// User menu for options
void user_menu() {
    int choice;
    char transaction_id[20], date[11], location[20];
    int amount;

    do {
        printf("\n--- Fraud Detection System ---\n");
        printf("1. Add a new transaction\n");
        printf("2. Detect fraud by transaction ID\n");
        printf("3. Detect anomaly by transaction amount\n");
        printf("4. Detect time-based fraud\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter transaction ID: ");
                scanf("%s", transaction_id);
                printf("Enter transaction amount: ");
                scanf("%d", &amount);
                printf("Enter transaction date (YYYY-MM-DD): ");
                scanf("%s", date);
                printf("Enter transaction location: ");
                scanf("%s", location);
                add_transaction(transaction_id, amount, date, location);
                add_transaction_amount(amount);  // Add amount for anomaly detection
                add_transaction_event(transaction_id);  // Add to time window
                break;
            case 2:
                printf("Enter transaction ID to check for fraud: ");
                scanf("%s", transaction_id);
                detect_fraud(transaction_id);
                break;
            case 3:
                printf("Enter transaction amount to check for anomaly: ");
                scanf("%d", &amount);
                detect_anomaly(amount);
                break;
            case 4:
                detect_time_based_fraud();
                break;
            case 5:
                printf("Exiting the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 5);
}

int main() {
    // Initialize the hash table to NULL
    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }

    // Start the user menu
    user_menu();

    return 0;
}
