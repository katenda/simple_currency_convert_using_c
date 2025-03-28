#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_CURRENCIES 50
#define HISTORY_FILE "conversion_history.txt"

////////////////////////////////
////// Structure to store currency data
////////////////////////////
struct Currency {
    char code[4];  
    char name[50];
    double rate;   
    char lastUpdated[20];
};


//////////////////////////////
//// Currency list array
////////////
struct Currency currencies[MAX_CURRENCIES]; 
int numCurrencies = 0;          


////////////////////////////////
// Function to load currencies from a CSV file
//////////////////////////

void loadCurrencies(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[200];
    int isEnhancedFormat = 0;
    
    // Check if first line contains headers (enhanced format)
    if (fgets(line, sizeof(line), file)) {
        if (strstr(line, "name") != NULL && strstr(line, "rate") != NULL) {
            isEnhancedFormat = 1;
        } else {
            // Rewind to beginning for simple format
            rewind(file);
        }
    }


    while (fgets(line, sizeof(line), file)) {
        char code[4] = {0};
        char name[30] = "Unknown";
        char symbol[5] = "";
        double rate = 0.0;
        char lastUpdated[20] = "Unknown";
        
        if (isEnhancedFormat) {

            // Parse enhanced format: code,name,symbol,rate,last_updated
            if (sscanf(line, "%3[^,],%29[^,],%4[^,],%lf,%19[^\n]",code, name, symbol, &rate, lastUpdated) >= 3) {
                // Successfully parsed enhanced format
            }

        } else {


            // Parse simple format: code,rate
            if (sscanf(line, "%3[^,],%lf", code, &rate) == 2) {
                // Set default names for known currencies
                if (strcmp(code, "USD") == 0) strcpy(name, "US Dollar");
                else if (strcmp(code, "EUR") == 0) strcpy(name, "Euro");
                else if (strcmp(code, "GBP") == 0) strcpy(name, "British Pound");
                else if (strcmp(code, "JPY") == 0) strcpy(name, "Japanese Yen");
                else if (strcmp(code, "INR") == 0) strcpy(name, "Indian Rupee");
            }

        }

        if (code[0] != '\0' && rate != 0.0 && numCurrencies < MAX_CURRENCIES) {
            strcpy(currencies[numCurrencies].code, code);
            strcpy(currencies[numCurrencies].name, name);
            currencies[numCurrencies].rate = rate;
            numCurrencies++;
        }
    }

    fclose(file);
    
    // Print loading summary
    printf("Loaded %d currencies from %s (%s format)\n", 
           numCurrencies, filename, isEnhancedFormat ? "enhanced" : "simple");
}



////////////////////////////
///// display of currencies with details
///////////////////////////



void displayCurrencies() {
    printf("\n%-8s %-30s %-12s %s\n", "Code", "Currency Name", "Rate", "Last Updated");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < numCurrencies; i++) {
        printf("%-8s %-30s %-12.4f %s\n", 
               currencies[i].code, 
               currencies[i].name, 
               currencies[i].rate,
               currencies[i].lastUpdated);
    }
    printf("\n");
}




/////////////////////////////////////
// Convert currency function
/////////////////////
double convertCurrency(double amount, char source[], char target[]) {
    double sourceRate = 0.0, targetRate = 0.0;
    int sourceFound = 0, targetFound = 0;

    // Find exchange rates for source and target currencies
    for (int i = 0; i < numCurrencies; i++) {
        if (strcmp(currencies[i].code, source) == 0) {
            sourceRate = currencies[i].rate;
            sourceFound = 1;
        }
        if (strcmp(currencies[i].code, target) == 0) {
            targetRate = currencies[i].rate;
            targetFound = 1;
        }
    }

    // Check if currencies were found
    if (!sourceFound) {
        printf("Error: Source currency '%s' not found.\n", source);
        return -1;
    }
    if (!targetFound) {
        printf("Error: Target currency '%s' not found.\n", target);
        return -1;
    }

    // Perform conversion
    return (amount / sourceRate) * targetRate;
}



/////////////////////////////////////
// Log conversion to history file
/////////////////////
void logConversion(double amount, char source[], char target[], double result) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (!file) {
        printf("Warning: Could not open history file for logging.\n");
        return;
    }

    time_t now;
    time(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(file, "[%s] %.2f %s -> %.2f %s\n", timestamp, amount, source, result, target);
    fclose(file);
}



/////////////////////////////////////
// Display conversion history
/////////////////////
void displayHistory() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        printf("No conversion history found.\n");
        return;
    }

    printf("\nConversion History:\n");
    printf("-------------------\n");
    
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    printf("\n");
}



/////////////////////////////////////
// Validate currency code input
/////////////////////
int isValidCurrencyCode(char code[]) {
    if (strlen(code) != 3) return 0;
    
    for (int i = 0; i < 3; i++) {
        if (!isalpha(code[i])) return 0;
    }
    
    return 1;
}



/////////////////////////////////////
// Convert string to uppercase
/////////////////////
void toUpperString(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}



/////////////////////////////////////
// Main menu
/////////////////////
void displayMenu() {
    printf("\nCurrency Converter Menu:\n");
    printf("1. Convert Currency\n");
    printf("2. View All Currencies\n");
    printf("3. View Conversion History\n");
    printf("4. Exit\n");
    printf("Enter your choice (1-4): ");
}



///////////////////////
///////////// main method definition
/////////////
int main() {
    // Load currencies from CSV file
    loadCurrencies("currencies.csv");
    
    int choice;
    char source[4], target[4];
    double amount, result;

    printf("Welcome to Simple Currency Converter!\n");
    
    //////////////////////
    //do while loop to keep application open
    ///////////////////////

    do {
        //display menu
        displayMenu();
        scanf("%d", &choice);
        
        // Clear input buffer
        while (getchar() != '\n');

        ///////////////////
        /////use of switch to navigate 

        switch (choice) {
            case 1: // Convert currency
                displayCurrencies();
                
                printf("Enter amount to convert: ");
                while (scanf("%lf", &amount) != 1 || amount <= 0) {
                    printf("Invalid amount. Please enter a positive number: ");
                    while (getchar() != '\n'); // Clear invalid input
                }
                
                printf("Enter source currency code (e.g., USD): ");
                scanf("%3s", source);
                toUpperString(source);

                //validate  currency code 
                while (!isValidCurrencyCode(source)) {
                    printf("Invalid currency code. Please enter a 3-letter code: ");
                    scanf("%3s", source);
                    toUpperString(source);
                }

                
                printf("Enter target currency code (e.g., EUR): ");
                scanf("%3s", target);
                toUpperString(target);

                //validate  currency code 
                while (!isValidCurrencyCode(target)) {
                    printf("Invalid currency code. Please enter a 3-letter code: ");
                    scanf("%3s", target);
                    toUpperString(target);
                }

                ///////////////////////////////////////
                //check if target is the same as source
                if (strcmp(source, target) == 0) {
                    printf("Source and target currencies are the same. No conversion needed.\n");
                    break;
                }
                
                result = convertCurrency(amount, source, target);
                if (result != -1) {
                    printf("\n%.2f %s = %.2f %s\n", amount, source, result, target);
                    logConversion(amount, source, target, result);
                }
                break;
                
            case 2: // View all currencies
                displayCurrencies();
                break;
                
            case 3: // View history
                displayHistory();
                break;
                
            case 4: // Exit
                printf("Thank you for using the Currency Converter!\n");
                break;
                
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
        }
        
    } while (choice != 4);

    return 0;
}