/*
 ============================================================================================
 Name        : database.c
 Author      : Pulasthi Peiris
 Description : A simple program to manage a small database of student
             : records using B-Trees for storage.
 ============================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// To make the "C" implementation completely analogous to Java, one has to create
// an object for each student record and attach it to a corresponding bNode
// object in a B-Tree data structure.  These objects are represented by the
// corresponding structure templates below.

//CONSTANTS
#define MAXLEN 20 //Max length for a string
#define false 0  //Define the value for false to be 0
#define true !false //Define true to be not equal to false

// Prototypes and templates should go in a file called sortFile.h which
// is subsequently included in sortFile.c.  For a small program like this one,
// a single file will do.


// Structure Templates

typedef struct SR {				// The student record object
    char Last[MAXLEN];			//Last name
	char First[MAXLEN];			//First name
	int ID;						//Student ID
	int marks;					//Student marks
} SRecord;

typedef struct bN {				// The bNode object
	struct SR *Srec;
	struct bN *left;
	struct bN *right;
} bNode;

// Function Prototypes
//
// Notice that there are two versions of add_node, inorder, and search.
// This is to accommodate sorting the database by last name OR student ID
// You will have to write these for a6.

bNode *addNode_Name(bNode *root, SRecord *Record);
bNode *addNode_ID(bNode *root, SRecord *Record);
bNode *makeNode(SRecord *data);
void inorder(bNode *root);
void search_Name(bNode *root, char *data);
void search_ID(bNode *root, int ID);
void str2upper(char *string);
void help();

//
// Since we haven't done pointer-pointers in this course, we'll use a
// global variable to return the matching student record.  This is
// equivalent to what we did in Java using an instance variable.

bNode *match; //global variable

int main(int argc, char *argv[]) {

// Internal declarations

    FILE * NAMESIDS;        // File descriptor (an object)!
	FILE * MARKS;           // Will have two files open

    bNode *root_N;   		// Pointer to names B-Tree
    bNode *root_I;   		// Pointer to IDs B-Tree
    SRecord *Record;   		// Pointer to current record read in


	int NumRecords;
	char cmd[MAXLEN], sName[MAXLEN];
	int sID;

	// Argument check

        if (argc != 3) { //if argument is not equal to 3
                printf("Usage: sdb [Names+IDs] [marks] \n");
                return -1;
        }

// Attempt to open the user-specified file.  If no file with
// the supplied name is found, exit the program with an error
// message.

// If NULL, then provide the following messages or returns

        if ((NAMESIDS=fopen(argv[1],"r"))==NULL)
            {
                printf("Can't read from file %s\n",argv[1]);
                return -2;
            }

        if ((MARKS=fopen(argv[2],"r"))==NULL)
            {
                printf("Can't read from file %s\n",argv[2]);
                fclose(NAMESIDS);
                return -2;
            }

// Initialize B-Trees by creating the root pointers;
// Initializing B-Tree

    root_N=NULL;
	root_I=NULL;
    NumRecords=0;

	printf("Building database...\n");

	while(true)

        // 	Allocate an object to hold the current data

        {

		Record = (SRecord *)malloc(sizeof(SRecord));
		if (Record == NULL) {
			printf("Failed to allocate object for data in main\n");
			return -1;
		}

//  Read in the data.  If the files are not the same length, the shortest one
//  terminates reading.
		int status = fscanf(NAMESIDS,"%s%s%d",Record->First,Record->Last,&Record->ID);
		if (status == EOF) break;
		status = fscanf(MARKS,"%d",&Record->marks);
		if (status == EOF) break;
		NumRecords++;

//	Add the record just read in to 2 B-Trees - one ordered
//  by name and the other ordered by student ID.

	    root_N=addNode_Name(root_N,Record);
	    root_I=addNode_ID(root_I,Record);
	}

// Close files once we're done

	fclose(NAMESIDS);
	fclose(MARKS);

	printf("Finished, %d records found...\n",NumRecords);

//
//  Simple Command Interpreter
//


	while (1) {
	    printf("sdb> ");
	    scanf("%s",cmd);					  // read command
	    str2upper(cmd);

	    if (0) {							  // This is a temporary stub
	    	continue;
	    }

// List by Name

		if (strncmp(cmd,"LN",2)==0) {         // List all records sorted by name
			printf("Student Record Database sorted by Last Name\n\n");
			inorder(root_N);
			printf("\n");
		}

// List by ID

		else if (strncmp(cmd,"LI",2)==0) {    // List all records sorted by ID
			printf("Student Record Database sorted by Student ID\n\n");
			inorder(root_I);
			printf("\n");
		}

// Find record that matches Name

		else if (strncmp(cmd,"FN",2)==0) {    // List record that matches name
			printf("Enter name to search: ");
			scanf("%s",sName);
			match=NULL;
			search_Name(root_N,sName);
			if (match==NULL)
			  printf("There is no student with that name.\n");
	        else {
			  if (strlen(match->Srec->First)+strlen(match->Srec->Last)>15) {
				printf("\nStudent Name:\t%s %s\n",match->Srec->First,match->Srec->Last);
			  } else {
				printf("\nStudent Name:\t\t%s %s\n",match->Srec->First,match->Srec->Last);
			  }
			  printf("Student ID:\t\t%d\n",match->Srec->ID);
			  printf("Total Grade:\t\t%d\n\n",match->Srec->marks);
	        }
		}

// Find record that matches ID


		else if (strncmp(cmd,"FI",2)==0) {    // List record that matches ID
			printf("Enter ID to search: ");
			scanf("%d",&sID);
			match=NULL;
			search_ID(root_I,sID);
			if (match==NULL)
			  printf("There is no student with that ID.\n");
	        else {
			  if (strlen(match->Srec->First)+strlen(match->Srec->Last)>15) {
				printf("\nStudent Name:\t%s %s\n",match->Srec->First,match->Srec->Last);
			  } else {
				printf("\nStudent Name:\t\t%s %s\n",match->Srec->First,match->Srec->Last);
			  }
			printf("Student ID:\t\t%d\n",match->Srec->ID);
			printf("Total Grade:\t\t%d\n\n",match->Srec->marks);
	      }
		}

// Help

		else if (strncmp(cmd,"H",1)==0) {  // Help
			help();
		}

		else if (strncmp(cmd,"?",2)==0) {     // Help
			help();
		}

// Quit

		else if (strncmp(cmd,"Q",1)==0) {  // Help
			printf("Program terminated...\n");
			return 0;
		}

// Command not understood

		else {
			printf("Command not understood.\n");
		}
	}
}

//
//	Write and insert the functions listed in the prototypes here.
//

bNode *addNode_Name(bNode *root, SRecord *Record){ //adds Name nodes 
	bNode *current;

	if(root==NULL){
		root=makeNode(Record);
	}
	else{
		current = root;
		while(true){
			
		//	if(Record->Last<current->Srec->Last){
			if(strcmp(Record->Last, current->Srec->Last) == -1){

				if(current->left==NULL){
					current->left=makeNode(Record);
					break;
				}
				else {
					current=current->left;
				}
			}
			else{
				if(current->right==NULL){
					current->right=makeNode(Record);
					break;
				}
				else{
					current=current->right;
				}
			}
		}
	}
	return root;
}

bNode *addNode_ID(bNode *root, SRecord *Record){ //adds ID nodes 
	bNode *current;

	if(root==NULL){
		root=makeNode(Record);
	}
	else{
		current = root;
		while(true){
			if(Record->ID<current->Srec->ID){

				if(current->left==NULL){
					current->left=makeNode(Record);
					break;
				}
				else {
					current=current->left;
				}
			}
			else{
				if(current->right==NULL){
					current->right=makeNode(Record);
					break;
				}
				else{
					current=current->right;
				}
			}
		}
	}
	return root;
}

bNode *makeNode(SRecord *Record){
	bNode *node = malloc(sizeof(bNode));
	node->Srec = Record;
	node->left = NULL;
	node->right= NULL;
	return node;
}

void inorder(bNode *root){
	if (root->left !=NULL) inorder(root->left);
	printf("%-20s %-20s %-5d %d \n", root->Srec->First, root->Srec->Last, root->Srec->ID, root->Srec->marks);
	if (root->right !=NULL) inorder(root->right);
}

void search_Name(bNode *root, char *data){
	if(root == NULL || strcasecmp(root->Srec->Last, data)==0){
		match = root;
	}
	else if((strcasecmp(root->Srec->Last, data) >0)) {
		search_Name(root->left, data);

	}
	else{
		search_Name(root->right, data);
	}
}

void search_ID(bNode *root, int ID){
	if(root == NULL || (root->Srec->ID == ID)){
		match = root;
	}
	else if(root->Srec->ID > ID) {
		search_ID(root->left, ID);
	}
	else{
		search_ID(root->right, ID);
	}
}

//
//  Convert a string to upper case
//

void str2upper (char *string) {
    int i;
    for(i=0;i<strlen(string);i++)
       string[i]=toupper(string[i]);
     return;
}

// Help
// prints command list

void help() {
	printf("LN List all the records in the database ordered by last name.\n");
	printf("LI List all the records in the database ordered by student ID.\n");
	printf("FN Prompts for a name and lists the record of the student with the corresponding name.\n");
	printf("FI Prompts for a name and lists the record of the student with the Corresponding ID.\n");
	printf("HELP Prints this list.\n");
	printf("? Prints this list.\n");
	printf("Q Exits the program.\n\n");

	return;
}
