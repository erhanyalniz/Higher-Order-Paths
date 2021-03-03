#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#define MAX_WORD_SIZE 250
// This structure will hold each unique words.
typedef struct keyword{
    char *text;
    int count;
    int documentCount;
    int *documentNumbers;
    struct keyword *next;
}keyword;

// Head of our linked list.
keyword *db;

// We will store our total unique word number and document number here.
int totalWordNumber, documentNumber;

// This structure will hold cooccurences that we will find after we read documents and make relations.
typedef struct cooccurrence{
    char *text_1;
    char *text_2;
    struct cooccurrence *next;
}cooccurrence;

// First, second and third cooccurence relations will be stored in this linked lists.
cooccurrence *first, *second, *third;

// I created these variables so that the program can hold each cooccurence matches.
int firstCooccurrenceNumber, secondCooccurrenceNumber, thirdCooccurrenceNumber;

// Add words to database db. (linked list)
void addWord(char word[]);

// Parse each words from documents.
void readDocument(char filename[]);

// Check if two words were in same file.
int wasInSameFile(keyword *word_1, keyword *word_2);

// Check if an element is in first order cooccurence relations.
int isInFirst(cooccurrence *element);

// Check if an element is in second order cooccurrence relations.
int isInSecond(cooccurrence *element);

// Check if two cooccurrence have one intersection (one same word in common) then add it to second order coccurrences.
int addIfOneIntersection(cooccurrence *elementOne, cooccurrence *elementTwo);

// Find and print first order cooccurrences.
void printFirstOccurrences();

// Find and print second order cooccurrences.
void printSecondOccurrences();

// Find and print third order cooccurrences.
void printThirdOccurrences();

// Combine all print functions above.
void printDatabase();

// Sort database db (linked list) by count of each word.
void sortByCount();

// Print most 10 frequent words.
void printMostFrequent();

// Sort database db (linked list) by count of each word.
void sortByIDF();

// Print 10 words that have the most IDF.
void printMostIDF();

int main(){
	// Set encoding to ANSI
	setlocale(LC_ALL, "tr_TR.ANSI");
	
    // Reading documents.
    
    readDocument("econ/1.txt");
    readDocument("econ/2.txt");
    readDocument("econ/3.txt");
    
    /*
    readDocument("health/1.txt");
    readDocument("health/2.txt");
    readDocument("health/3.txt");
    
    readDocument("magazin/1.txt");
    readDocument("magazin/2.txt");
    */
    // Finding relations and print.
    printDatabase();
    // Sorting linked list by count.
    
    sortByCount();
    // Print most 10 frequent words.
    printMostFrequent();
    // Sorting linked list by IDF.
    sortByIDF();
    // Print most 10 words by IDF.
    printMostIDF();
    
    return 0;
}

void addWord(char word[]){
    int i;
    // Get database head.
    keyword *head = db;
    // Iterate over linked list to check if word already exists increase count.
    for(i = 0;i < totalWordNumber;i++){
        db = db->next;
        if(!strcmp(word, db->text)){
            // Check if it was read in a new document if so increase count of word document number.
            if(db->documentCount < documentNumber){
                db->documentCount++;
                db->documentNumbers = (int *)realloc(db->documentNumbers, sizeof(int)*(db->documentCount));
                db->documentNumbers[db->documentCount-1] = documentNumber;
            }
            db->count++;
            db = head;
            // Exit the function after word is found in linked list and count increased.
            return;
        }
    }
    // Increase total unique word count.
    totalWordNumber++;
    // Allocate space for new word in linked list.
    db->next = (keyword *)malloc(sizeof(keyword));
    // Iterate to new word.
    db = db->next;
    db->text = (char *)malloc(sizeof(char)*MAX_WORD_SIZE);
    // set the word text.
    strcpy(db->text, word);
    // Set the word frequency to one.
    db->count = 1;
    // Set the word document number.
    db->documentCount = 1;
    // Get which document the word was read from and transform document count to index.
    db->documentNumbers = (int *)malloc(sizeof(int)*(db->documentCount));
    db->documentNumbers[db->documentCount - 1] = documentNumber;
    db = head;
}

void readDocument(char filename[]){
    // Increase the overall document index.
    documentNumber++;
    // Allocate space for database head if we are reading the first document.
    if(documentNumber == 1) db = (keyword *)malloc(sizeof(keyword));
    // Allocate space for buffer.
    char *word = (char *)malloc(sizeof(char)*MAX_WORD_SIZE);
    FILE *fPtr = fopen(filename,"r");
    int i;
    /*
    Parse words from file.
    fscanf will return -1 if couldn't read so we will add 1 to make it 0 if couldn't read then our loop will stop.
    */
    for(i = 0;fscanf(fPtr,"%s",word)+1;i++){
        // Pass word to our function to further analysis.
        addWord(word);
    }
}

int wasInSameFile(keyword *word_1, keyword *word_2){
    int i, j;
    // Iterate over document number arrays and check if words have the same document index.
    for(i = 0;i < word_1->documentCount;i++){
        for(j = 0;j < word_2->documentCount;j++){
            if(word_1->documentNumbers[i] == word_2->documentNumbers[j]){
                return 1;
            }    
        }
    }
    return 0;
}

int isInFirst(cooccurrence *element){
    int i;
    // Get the coccurrence linked list head.
    cooccurrence *head = first;
    // Iterate over whole first order cooccurence linked list and return 1 if exact same element as our argument is in first order cooccurences.
    for(i = 0;i < firstCooccurrenceNumber; i++){
        head = head->next;
        if(!strcmp(head->text_1, element->text_1) && !strcmp(head->text_2, element->text_2)){
            return 1;
        }
    }
    return 0;
}

int isInSecond(cooccurrence *element){
    int i;
    // Get the coccurrence linked list head.
    cooccurrence *head = second;
    // Iterate over whole second order cooccurence linked list and return 1 if exact same element as our argument is in second order cooccurences.
    for(i = 0;i < secondCooccurrenceNumber; i++){
        head = head->next;
        if(!strcmp(head->text_1, element->text_1) && !strcmp(head->text_2, element->text_2)){
            return 1;
        }
    }
    return 0;
}

int addIfOneIntersection(cooccurrence *elementOne, cooccurrence *elementTwo){
    // Get cooccurrence head.
    cooccurrence *head = second;
    // Check if first cooccurrence's first text and second cooccurrence's first text is same.
    if(!strcmp(elementOne->text_1, elementTwo->text_1)){
        // Check if first cooccurrence's second text and second cooccurrence's second text is not same.
        if(strcmp(elementOne->text_2, elementTwo->text_2)){
            // Create a new cooccurence.
            cooccurrence *new = (cooccurrence *)malloc(sizeof(cooccurrence)); 
            // Allocate spaces for words inside our cooccurrence structure.
            new->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            new->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            // Get the texts that are not same. (non-intersecting ones)
            strcpy(new->text_1, elementOne->text_2);
            strcpy(new->text_2, elementTwo->text_2);
            // Check if our new cooccurrence was in first order cooccurences if so discard this new cooccurence.
            // Because there can only be second order relation if there is not first order.
            if(isInFirst(new)){
                // Return 0 so that there wasn't any new cooccurrence added to second order cooccurrence linked list.
                return 0;
            }
            // If wasn't in first order cooccurrences then add the built cooccurrence to second order cooccurrence linked list.
            head->next = new;
            // Iterate to new element.
            head = head->next;
            // Return 1 because a cooccurrence is successfully addded to second order cooccurrence linked list.
            return 1;
        }
    // If first cooccurrence's first text and second cooccurrence's first text is not same.
    }else{
        // Check if first cooccurrence's second text and second cooccurrence's second text is not same.
        if(!strcmp(elementOne->text_2, elementTwo->text_2)){
            // Create a new cooccurence.
            cooccurrence *new = (cooccurrence *)malloc(sizeof(cooccurrence));
            // Allocate spaces for words inside our cooccurrence structure. 
            new->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            new->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            // Get the texts that are not same. (non-intersecting ones)
            strcpy(new->text_1, elementOne->text_1);
            strcpy(new->text_2, elementTwo->text_1);
            // Check if our new cooccurrence was in first order cooccurences if so discard this new cooccurence.
            // Because there can only be second order relation if there is not first order.
            if(isInFirst(new)){
                // Return 0 so that there wasn't any new cooccurrence added to second order cooccurrence linked list.
                return 0;
            }
            // If wasn't in first order cooccurrences then add the built cooccurrence to second order cooccurrence linked list.
            head->next = new;
            // Iterate to new element.
            head = head->next;
            // Return 1 because a cooccurrence is successfully addded to second order cooccurrence linked list.
            return 1;
        }
    }
    // Check if first cooccurrence's first text and second cooccurrence's second text is not same.
    if(strcmp(elementOne->text_1, elementTwo->text_2)){
        // Check if first cooccurrence's second text and second cooccurrence's first text is same.
        if(!strcmp(elementOne->text_2, elementTwo->text_1)){
            // Create a new cooccurence.
            cooccurrence *new = (cooccurrence *)malloc(sizeof(cooccurrence)); 
            // Allocate spaces for words inside our cooccurrence structure.
            new->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            new->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            // Get the texts that are not same. (non-intersecting ones)
            strcpy(new->text_1, elementOne->text_1);
            strcpy(new->text_2, elementTwo->text_2);
            // Check if our new cooccurrence was in first order cooccurences if so discard this new cooccurence.
            // Because there can only be second order relation if there is not first order.
            if(isInFirst(new)){
                // Return 0 so that there wasn't any new cooccurrence added to second order cooccurrence linked list.
                return 0;
            }
            // If wasn't in first order cooccurrences then add the built cooccurrence to second order cooccurrence linked list.
            head->next = new;
            // Iterate to new element.
            head = head->next;
            // Return 1 because a cooccurrence is successfully addded to second order cooccurrence linked list.
            return 1;
        }
    // Check if first cooccurrence's first text and second cooccurrence's second text is same.
    }else{
        // Check if first cooccurrence's second text and second cooccurrence's first text is not same.
        if(strcmp(elementOne->text_2, elementTwo->text_1)){
            // Create a new cooccurence.
            cooccurrence *new = (cooccurrence *)malloc(sizeof(cooccurrence));
            // Allocate spaces for words inside our cooccurrence structure. 
            new->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            new->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            // Get the texts that are not same. (non-intersecting ones)
            strcpy(new->text_1, elementOne->text_2);
            strcpy(new->text_2, elementTwo->text_1);
            // Check if our new cooccurrence was in first order cooccurences if so discard this new cooccurence.
            // Because there can only be second order relation if there is not first order.
            if(isInFirst(new)){
                // Return 0 so that there wasn't any new cooccurrence added to second order cooccurrence linked list.
                return 0;
            }
            // If wasn't in first order cooccurrences then add the built cooccurrence to second order cooccurrence linked list.
            head->next = new;
            // Iterate to new element.
            head = head->next;
            // Return 1 because a cooccurrence is successfully addded to second order cooccurrence linked list.
            return 1;
        }
    }
    // Return 0 because there was not a case where there is only one intersecting text and two different.
    return 0;
}

int addIfNoIntersection(keyword *elementOne, keyword *elementTwo){
    // Get cooccurrence head.
    cooccurrence *head = third;
    // Build a new cooccurrence and allocate space for new cooccurrence.
    cooccurrence *new = (cooccurrence *)malloc(sizeof(cooccurrence));
    // Allocate spaces for words inside our cooccurrence structure.
    new->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    new->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    // Check if two texts are different.
    if(strcmp(elementOne->text, elementTwo->text)){
        // Copy text into our newly built cooccurrence.
        strcpy(new->text_1, elementOne->text);
        strcpy(new->text_2, elementTwo->text);
        // Check if our newly built element is not in second or first order cooccurrence linked list.
        if(!(isInSecond(new) || isInFirst(new))){
            // Add to third order cooccurrence linked list if all conditions above passed.
            head->next = new;
            // Return 1 so that new element is added.
            return 1;
        }
    }
    // Return 0 so that new element is discarded.
    return 0;
}

void printFirstOccurrences(){
    // Allocate space to the first order cooccurrence linked list head.
    first = (cooccurrence *)malloc(sizeof(cooccurrence));
    // Get the cooccurrence head to iterate after.
    cooccurrence *head = first;
    // Set trigger to 1 so that if we are printing the first element we print it in different format.
    int i, j, trigger = 1;
    // Use two heads to iterate over each two's combinations of database db. (unique word linked list)
    keyword *head_1 = db;
    keyword *head_2 = db;
    printf("First order term co-occurrence ");
    // Iterate head 1 in whole database.
    for(i = 0;i < totalWordNumber;i++){
        head_1 = head_1->next;
        // Decrease the iteration number in each element as our first head already tried combinations of head 2.
        for(j = 0;j < i+1; j++){
            head_2 = head_2->next;
        }
        // Make combinations of head 1 and head 2
        for(j = i+1;j < totalWordNumber; j++){
            // Iterate head 2
            head_2 = head_2->next;
            // Check if head 1 and head 2 in same file.
            if(wasInSameFile(head_1, head_2)){
                // If condition passed then allocate space for new cooccurrence.
                head->next = (cooccurrence *)malloc(sizeof(cooccurrence));
                // Iterate the cooccurence head.
                head = head->next;
                // Allocate space for words.
                head->text_1 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
                head->text_2 = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
                // Copy head 1 and head 2's texts to cooccurrence structure.
                strcpy(head->text_1, head_1->text);
                strcpy(head->text_2, head_2->text);
                // Increase first order cooccurence number.
                firstCooccurrenceNumber++;
                // Check if the element to print is the first element that is found.
                if(trigger){
                    printf("{%s, %s}", head->text_1, head->text_2);
                    trigger = 0;
                }else{
                    printf(", {%s, %s}", head->text_1, head->text_2);
                }
            }
        }
        // Return head 2 to start of database.
        head_2 = db;
    }
}

void printSecondOccurrences(){
    // Allocate space to the second order cooccurrence linked list head.
    second = (cooccurrence *)malloc(sizeof(cooccurrence));
    // Get the cooccurrence head to iterate after.
    cooccurrence *head = second;
    // Set trigger to 1 so that if we are printing the first element we print it in different format.
    int i, j, trigger = 1;
    // Use two heads to iterate over each two's combinations of first order cooccurences. (unique word linked list)
    cooccurrence *first_head_1 = first;
    cooccurrence *first_head_2 = first;
    printf("\nSecond order term co-occurrence ");
    // Iterate head 1 in whole database.
    for(i = 0;i < firstCooccurrenceNumber;i++){
        first_head_1 = first_head_1->next;
        // Decrease the iteration number in each element as our first head already tried combinations of head 2.
        for(j = 0;j < i + 1;j++){
            first_head_2 = first_head_2->next;
        }
        // Make combinations of head 1 and head 2
        for(j = i + 1;j < firstCooccurrenceNumber;j++){
            // Iterate head 2
            first_head_2 = first_head_2->next;
            // Check if selected cooccurrences have only one intersection then choose different terms and combine.
            // Check if combined new cooccurrence isn't in first or second cooccurrence linked list.
            // If not add and print.
            if(addIfOneIntersection(first_head_1, first_head_2)){
                    // New cooccurrence has been added to the second cooccurrence linked list so increase the count.
                    secondCooccurrenceNumber++;
                    // Iterate the cooccurence head.
                    second = second->next;
                    // Check if the element to print is the first element that is found.
                    if(trigger){
                        printf("{%s, %s}", second->text_1, second->text_2);
                        trigger = 0;
                    }else{
                        printf(", {%s, %s}", second->text_1, second->text_2);
                    }
            }
        }
        // Return head 2 to start of first order cooccurrences linked list.
        first_head_2 = first;
    }
    // Return second order cooccurrences linked list head back to start of itself.
    second = head;
}

void printThirdCooccurrences(){
    // Allocate space to the third order cooccurrence linked list head.
    third = (cooccurrence *)malloc(sizeof(cooccurrence));
    // Get the cooccurrence head to iterate after.
    cooccurrence *head = third;
    // Set trigger to 1 so that if we are printing the first element we print it in different format.
    int i, j, trigger = 1;
    // Use two heads to iterate over each two's combinations of database db. (unique word linked list)
    keyword *head_1 = db;
    keyword *head_2 = db;
    printf("\nThird order term co-occurrence ");
    // Iterate head 1 in whole database.
    for(i = 0;i < totalWordNumber;i++){
        head_1 = head_1->next;
        // Decrease the iteration number in each element as our first head already tried combinations of head 2.
        for(j = 0;j < i + 1;j++){
            head_2 = head_2->next;
        }
        // Make combinations of head 1 and head 2
        for(j = i + 1;j < totalWordNumber;j++){
            // Iterate head 2
            head_2 = head_2->next;
            // Check if there is no intersection between head 1 and head 2 and build a new cooccurrence with them.
            // Check if newly built cooccurrence is not in second order cooccurrences or first order cooccurrences.
            // If not then it must be a third order cooccurrence, return 1, Else it can't be a third order, return 0.
            if(addIfNoIntersection(head_1, head_2)){
                // Iterate the cooccurence head.
                third = third->next;
                // Check if the element to print is the first element that is found.
                if(trigger){
                    printf("{%s, %s}", third->text_1, third->text_2);
                    trigger = 0;
                }else{
                    printf(", {%s, %s}", third->text_1, third->text_2);
                }
            }
        }
        head_2 = db;
    }
    second = head;
}

void printDatabase(){
    // Combine all the printing functions that we defined.
    printFirstOccurrences();
    printSecondOccurrences();
    printThirdCooccurrences();
    // Add a newline to beautify.
    printf("\n");
}

void sortByCount(){
    int i, j, k;
    // Get the database db (unique word linked list) and define a temporary.
    keyword *head = db, *temp;
    // Iterate over whole linked list (bubble sort technique).
    for(i = 0;i < totalWordNumber;i++){
        for(j = 0;j < totalWordNumber - 1;j++){
            // Iterate the db.
            head = head->next;
            // If word's frequency is less then next word's frequency swap elements (by copying).
            if(head->count < head->next->count){
                // Allocate space for temporary variable.
                temp = (keyword *)malloc(sizeof(keyword));
                // Copy fields to temporary variable.
                temp->count = head->count;
                temp->documentCount = head->documentCount;
                temp->text = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
                strcpy(temp->text, head->text);
                temp->documentNumbers = (int *)malloc(sizeof(int) * temp->documentCount);
                for(k = 0;k < temp->documentCount;k++){
                    temp->documentNumbers[k] = head->documentNumbers[k];
                }
                // Copy the next element's fields to current one.
                head->count = head->next->count;
                head->documentCount = head->next->documentCount;
                strcpy(head->text, head->next->text);
                for(k = 0;k < temp->documentCount;k++){
                    head->documentNumbers[k] = head->next->documentNumbers[k];
                }
                // Copy temporary variables fields to next element.
                head->next->count = temp->count;
                head->next->documentCount = temp->documentCount;
                strcpy(head->next->text, temp->text);
                for(k = 0;k < temp->documentCount;k++){
                    head->next->documentNumbers[k] = temp->documentNumbers[k];
                }
                // Free memory allocated by temporary variable.
                free(temp);
            }
        }
        // return head to start of db.
        head = db;
    }
}

void printMostFrequent(){
    // Note that this function is thought to be run after sorting else it will just print first 10 elements added to db.
    int i;
    // Get database head.
    keyword *head = db;
    printf("\nSorted by Frequencies\'s:\n");
    // Print first 10 elements of linked list.
    for(i = 0;i < 10;i++){
        head = head->next;
        printf("%s;%d\n", head->text, head->count);
    }
}

void sortByIDF(){
    int i, j, k;
    // Get the database db (unique word linked list) and define a temporary.
    keyword *head = db, *temp;
    // Iterate over whole linked list (bubble sort technique).
    for(i = 0;i < totalWordNumber;i++){
        for(j = 0;j < totalWordNumber - 1;j++){
            // Iterate the db.
            head = head->next;
            // If word's IDF is less then next word's IDF swap elements (by copying).
            if(head->count * log(documentNumber/head->documentCount) < head->next->count * log(documentNumber/head->next->documentCount)){
                // Allocate space for temporary variable.
                temp = (keyword *)malloc(sizeof(keyword));
                // Copy fields to temporary variable.
                temp->count = head->count;
                temp->documentCount = head->documentCount;
                temp->text = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
                strcpy(temp->text, head->text);
                temp->documentNumbers = (int *)malloc(sizeof(int) * temp->documentCount);
                for(k = 0;k < temp->documentCount;k++){
                    temp->documentNumbers[k] = head->documentNumbers[k];
                }
                // Copy the next element's fields to current one.
                head->count = head->next->count;
                head->documentCount = head->next->documentCount;
                strcpy(head->text, head->next->text);
                for(k = 0;k < temp->documentCount;k++){
                    head->documentNumbers[k] = head->next->documentNumbers[k];
                }
                // Copy temporary variables fields to next element.
                head->next->count = temp->count;
                head->next->documentCount = temp->documentCount;
                strcpy(head->next->text, temp->text);
                for(k = 0;k < temp->documentCount;k++){
                    head->next->documentNumbers[k] = temp->documentNumbers[k];
                }
                // Free memory allocated by temporary variable.
                free(temp);
            }
        }
        // return head to start of db.
        head = db;
    }
}

void printMostIDF(){
    // Note that this function is thought to be run after sorting else it will just print first 10 elements added to db after applying formula of idf.
    int i;
    // Get database head.
    keyword *head = db;
    // Print first 10 elements of linked list.
    printf("\nSorted by IDF\'s:\n");
    for(i = 0;i < 10;i++){
        head = head->next;
        // Apply IDF formula to second parameter to print.
        printf("%s;%lf\n", head->text, head->count * log(documentNumber/head->documentCount));
    }
}
