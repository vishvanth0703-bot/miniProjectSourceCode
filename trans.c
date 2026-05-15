// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
char currentUser[20];
// clientData structure definition
void displayRecords(FILE *fPtr);
void searchAccount(FILE *fPtr);
void transferMoney(FILE *fPtr);
void initializeFile(void);
int login(void);
void addUser(void);
void changePassword(void);

struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr = NULL;  // credit.dat file pointer
    unsigned int choice; // user's choice

    if(!login())
    {
        return 0;
    }
    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 11)
    {
        switch (choice)
        {
        // create text file from record file
        case 1:
            textFile(cfPtr);
            break;
        // update record
        case 2:
            updateRecord(cfPtr);
            break;
        // create record
        case 3:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 4:
            deleteRecord(cfPtr);
            break;
        // display if user does not select valid choice
        case 5:
            displayRecords(cfPtr);
            break;

         case 6:
            searchAccount(cfPtr);
            break;

        case 7:
            transferMoney(cfPtr);
            break;

        case 8:
            initializeFile();
            fclose(cfPtr);
            cfPtr = fopen("credit.dat","rb+");

            if(cfPtr == NULL){
                printf("Error reopening file!\n");
                exit(1);
                }
            break;

        case 9:
            if(strcmp(currentUser,"admin")==0)
            {
                addUser();
            }
            else
            {
                printf("Access denied! Admin only.\n");
            }
            break;

        case 10:
            changePassword();
            break;

        case 11:
            printf("Thank you for using the Bank Account Program.\n");
            break;


        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("%s", "Enter account to update ( 1 - 100 ): ");
    scanf("%d", &account);

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        scanf("%lf", &transaction);
        client.balance += transaction; // update record balance

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("%s", "Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;
        // move file pointer to correct record in file

        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s","\n==========================================\n"
                 "     BANK ACCOUNT MANAGEMENT SYSTEM"
                 "\n========================================="
                 "\nEnter your choice:\n"
             "1 - To store formatted text file\n"
             "2 - Update an account\n"
             "3 - Add a new account\n"
             "4 - Delete an account\n"
             "5 - Display all accounts\n"
             "6 - Search account\n"
             "7 - Transfer money\n"
             "8 - Initialize file\n"
             "9 - Add user (admin only)\n"
             "10 - Change password\n"
             "11 - Exit\n"
            "==========================================\n");

    scanf("%u", &menuChoice); // receive choice from user
    return menuChoice;
} // end function enterChoice

void displayRecords(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n","Acct","Last Name","First Name","Balance");

    while(fread(&client,sizeof(struct clientData),1,fPtr)==1)
    {
        if(client.acctNum!=0)
        {
            printf("%-6d%-16s%-11s%10.2f\n",
            client.acctNum,
            client.lastName,
            client.firstName,
            client.balance);
        }
    }
}

void searchAccount(FILE *fPtr)
{
    unsigned int account;
    struct clientData client;

    printf("Enter account number to search: ");
    scanf("%u",&account);

    fseek(fPtr,(account-1)*sizeof(struct clientData),SEEK_SET);
    fread(&client,sizeof(struct clientData),1,fPtr);

    if(client.acctNum==0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found:\n");
        printf("%-6d%-16s%-11s%10.2f\n",
        client.acctNum,
        client.lastName,
        client.firstName,
        client.balance);
    }
}

void transferMoney(FILE *fPtr)
{
    unsigned int fromAcc,toAcc;
    double amount;

    struct clientData fromClient,toClient;

    printf("Enter source account: ");
    scanf("%u",&fromAcc);

    printf("Enter destination account: ");
    scanf("%u",&toAcc);

    printf("Enter amount to transfer: ");
    scanf("%lf",&amount);

    // Read source account
    fseek(fPtr,(fromAcc-1)*sizeof(struct clientData),SEEK_SET);
    fread(&fromClient,sizeof(struct clientData),1,fPtr);

    // Read destination account
    fseek(fPtr,(toAcc-1)*sizeof(struct clientData),SEEK_SET);
    fread(&toClient,sizeof(struct clientData),1,fPtr);

    if(fromClient.acctNum==0 || toClient.acctNum==0)
    {
        printf("One of the accounts does not exist.\n");
        return;
    }

    if(fromClient.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    // Perform transfer
    fromClient.balance -= amount;
    toClient.balance += amount;

    // Write back source
    fseek(fPtr,(fromAcc-1)*sizeof(struct clientData),SEEK_SET);
    fwrite(&fromClient,sizeof(struct clientData),1,fPtr);

    // Write back destination
    fseek(fPtr,(toAcc-1)*sizeof(struct clientData),SEEK_SET);
    fwrite(&toClient,sizeof(struct clientData),1,fPtr);

    printf("Transfer successful!\n");
}

void initializeFile(void)
{
    FILE *fp = fopen("credit.dat","wb");

    struct clientData blank = {0,"","",0.0};

    for(int i=0;i<100;i++)
    {
        fwrite(&blank,sizeof(struct clientData),1,fp);
    }

    fclose(fp);

    printf("File initialized successfully.\n");
}

int login(void)
{
    char user[20], pass[20];
    char fileUser[20], filePass[20];
    char ch;
    int i, attempts = 3;

    while(attempts--)
    {
        FILE *fp = fopen("login.txt","r");

        if(fp == NULL)
        {
            printf("Login file not found!\n");
            return 0;
        }

        printf("\nEnter username: ");
        scanf("%s", user);

        printf("Enter password: ");
        i = 0;

        while(1)
        {
            ch = getch();

            if(ch == 13) break;

            if(ch == 8 && i > 0)
            {
                printf("\b \b");
                i--;
            }
            else if(i < 19)
            {
                pass[i++] = ch;
                printf("*");
            }
        }

        pass[i] = '\0';

        while(fscanf(fp,"%s %s", fileUser, filePass) == 2)
        {
            if(strcmp(user,fileUser)==0 && strcmp(pass,filePass)==0)
            {
                strcpy(currentUser, user);   // IMPORTANT
                printf("\nLogin successful!\n");
                fclose(fp);
                return 1;
            }
        }

        fclose(fp);
        printf("\nInvalid credentials! Attempts left: %d\n", attempts);
    }

    printf("Too many failed attempts. Access blocked!\n");
    return 0;
}

void addUser(void)
{
    FILE *fp = fopen("login.txt","a");

    if(fp == NULL)
    {
        printf("Error opening login file!\n");
        return;
    }

    char newUser[20], newPass[20];

    printf("Enter new username: ");
    scanf("%s", newUser);

    printf("Enter new password: ");
    scanf("%s", newPass);

    fprintf(fp,"%s %s\n", newUser, newPass);

    fclose(fp);

    printf("User added successfully!\n");
}

void changePassword(void)
{
    FILE *fp, *temp;
    char oldPass[20], newPass[20];
    char fileUser[20], filePass[20];
    int found = 0;

    fp = fopen("login.txt", "r");
    temp = fopen("temp.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("File error!\n");
        return;
    }

    printf("Enter old password: ");
    scanf("%s", oldPass);

    while(fscanf(fp, "%s %s", fileUser, filePass) == 2)
    {
        if(strcmp(currentUser, fileUser) == 0 && strcmp(oldPass, filePass) == 0)
        {
            found = 1;

            printf("Enter new password: ");
            scanf("%s", newPass);

            fprintf(temp, "%s %s\n", currentUser, newPass);
        }
        else
        {
            fprintf(temp, "%s %s\n", fileUser, filePass);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("login.txt");
    rename("temp.txt", "login.txt");

    if(found)
        printf("Password changed successfully!\n");
    else
        printf("Incorrect old password!\n");
}