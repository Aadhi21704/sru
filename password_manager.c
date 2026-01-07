#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SITE_SIZE 50
#define USER_SIZE 50
#define PASS_SIZE 50

struct Credential {
    char site[SITE_SIZE];
    char username[USER_SIZE];
    char password[PASS_SIZE];
};

/* Function declarations */
void clearBuffer();
void inputString(char *str, int size);
void encrypt(char *str);
void decrypt(char *str);

void setupMasterPassword();
int verifyMasterPassword();

void addCredential();
void viewAll();
void searchCredential();
void generatePassword(char *pass, int length);

/* ===================== MAIN ===================== */

int main() {
    int choice;

    if (!verifyMasterPassword()) {
        printf("Access denied!\n");
        return 0;
    }

    while (1) {
        printf("\n===== PASSWORD MANAGER =====\n");
        printf("1. Add Credential\n");
        printf("2. View All Credentials\n");
        printf("3. Search Credential\n");
        printf("4. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: addCredential(); break;
            case 2: viewAll(); break;
            case 3: searchCredential(); break;
            case 4: return 0;
            default: printf("Invalid choice!\n");
        }
    }
}

/* ===================== UTILITIES ===================== */

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void inputString(char *str, int size) {
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}

/* Caesar cipher */
void encrypt(char *str) {
    while (*str) {
        *str += 3;
        str++;
    }
}

void decrypt(char *str) {
    while (*str) {
        *str -= 3;
        str++;
    }
}

/* ===================== MASTER PASSWORD ===================== */

void setupMasterPassword() {
    FILE *fp = fopen("master.txt", "w");
    char master[PASS_SIZE];

    printf("Set Master Password: ");
    inputString(master, PASS_SIZE);

    encrypt(master);
    fprintf(fp, "%s\n", master);
    fclose(fp);

    printf("Master password set successfully!\n");
}

int verifyMasterPassword() {
    FILE *fp = fopen("master.txt", "r");
    char stored[PASS_SIZE], input[PASS_SIZE];

    if (!fp) {
        printf("No master password found.\n");
        setupMasterPassword();
        return 1;
    }

    fscanf(fp, "%s", stored);
    fclose(fp);

    printf("Enter Master Password: ");
    inputString(input, PASS_SIZE);

    encrypt(input);

    return strcmp(stored, input) == 0;
}

/* ===================== PASSWORD GENERATOR ===================== */

void generatePassword(char *pass, int length) {
    const char chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#$%";

    for (int i = 0; i < length - 1; i++) {
        pass[i] = chars[rand() % (sizeof(chars) - 1)];
    }
    pass[length - 1] = '\0';
}

/* ===================== CORE FEATURES ===================== */

void addCredential() {
    FILE *fp = fopen("passwords.txt", "a");
    if (!fp) {
        printf("Error opening file!\n");
        return;
    }

    struct Credential c;
    int option;

    printf("Enter Website/App Name: ");
    inputString(c.site, SITE_SIZE);

    printf("Enter Username: ");
    inputString(c.username, USER_SIZE);

    printf("1. Enter password manually\n");
    printf("2. Generate random password\n");
    printf("Choose option: ");
    scanf("%d", &option);
    clearBuffer();

    if (option == 2) {
        generatePassword(c.password, 12);
        printf("Generated Password: %s\n", c.password);
    } else {
        printf("Enter Password: ");
        inputString(c.password, PASS_SIZE);
    }

    encrypt(c.site);
    encrypt(c.username);
    encrypt(c.password);

    /* FIXED: write in exact same order always */
    fprintf(fp, "%s %s %s\n", c.site, c.username, c.password);
    fclose(fp);

    printf("Credential saved securely!\n");
}

void viewAll() {
    FILE *fp = fopen("passwords.txt", "r");
    if (!fp) {
        printf("No stored credentials!\n");
        return;
    }

    struct Credential c;

    printf("\n===== SAVED CREDENTIALS =====\n");

    /* FIXED: read in same order they were written */
    while (fscanf(fp, "%s %s %s", c.site, c.username, c.password) == 3) {
        decrypt(c.site);
        decrypt(c.username);
        decrypt(c.password);

        printf("Site     : %s\n", c.site);
        printf("Username : %s\n", c.username);
        printf("Password : %s\n", c.password);
        printf("---------------------------\n");
    }

    fclose(fp);
}

void searchCredential() {
    FILE *fp = fopen("passwords.txt", "r");
    if (!fp) {
        printf("No stored credentials!\n");
        return;
    }

    char query[SITE_SIZE];
    struct Credential c;
    int found = 0;

    printf("Enter site/app to search: ");
    inputString(query, SITE_SIZE);

    while (fscanf(fp, "%s %s %s", c.site, c.username, c.password) == 3) {
        decrypt(c.site);
        decrypt(c.username);
        decrypt(c.password);

        if (strcmp(c.site, query) == 0) {
            printf("\nMATCH FOUND!\n");
            printf("Site     : %s\n", c.site);
            printf("Username : %s\n", c.username);
            printf("Password : %s\n", c.password);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("No credentials found for '%s'\n", query);
    }

    fclose(fp);
}
