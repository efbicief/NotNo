#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/*int countFileLines(char* fname) {
    FILE* fp;
    char c;
    int newlines = 1;
    fp = fopen(fname, "r");
    while (1) {
        c = fgetc(fp);
        if (c == '\n') newlines++;
        else if (c == EOF) break;
    }
    fclose(fp);
    return newlines;
}*/

//int insertLine(char* fname, char* lineToInsert, int lineNo) {

//}

int replace(char* searchString, char* replString, char* fname) {
    //clear outputwindow
    //outputToWin(outputwindow, "");
    printf("i am here\n");
    //first to ops similar to find, but instead just save the indexes of matching strings
    int* matchIndices = (int*) malloc(0); //start with no items matched

    FILE* fp = fopen(fname, "r");
    size_t bufLength = strlen(searchString);
    char* searchBuffer = (char*) malloc(bufLength * sizeof(char));
    //go through file, write next bufLength chars to buffer, compare to see if it is the same
    //ToDo put some condition in so that it doesnt search when the file's length is smaller than the search string
    int numFound = 0;
    int i;
    int currChar = 0;

    //initial buffer fill
    for (i=0; i<bufLength; i++) {
        searchBuffer[i] = fgetc(fp);
        currChar++;
    }
    printf("buffer filled\n");
    //check if buffer matches searchString
    if (strncmp(searchString, searchBuffer, bufLength) == 0) {
        numFound++;
        // now add index to array
        matchIndices = (int*) realloc(matchIndices, sizeof(int) * numFound);
        matchIndices[numFound-1] = currChar;
    }
    printf("checked first time\n");
    char* toPrint;
    while (searchBuffer[bufLength-1] != EOF) {
        //shift buffer
        for (i=0; i<bufLength-1; i++) {
            searchBuffer[i] = searchBuffer[i+1];
            printf("%i\n", i);
        }
        //add new char to end
        searchBuffer[bufLength-1] = fgetc(fp);
        printf("unbifshiftn\n");

        //check if buffer matches searchString
        if (strncmp(searchString, searchBuffer, bufLength) == 0) {
            //appendOutputToWin(outputwindow, "Found\n");
            numFound++;
            matchIndices = (int*) realloc(matchIndices, sizeof(int) * numFound);
            matchIndices[numFound-1] = currChar;
        }
        currChar++;
    }
    printf("repl stsge");
    //now, use the indices to replace
    fclose(fp);
    fp = fopen(fname, "r"); //reopen to get back to start
    FILE* tempp = fopen(".temp", "w"); //write to temp file first
    currChar = 0;
    numFound = 0;
    char c = 'p'; //placeholder
    while (c != EOF) {
        c = fgetc(fp);
        if (matchIndices[numFound] == currChar) { //if we are at a position where we need to replace
            fputs(replString, tempp);
            numFound++;
            for (i=0; i<bufLength; i++) {
                fgetc(fp);
                currChar++;
            }
        } else {
            fputc(c, tempp);
            currChar++;
        }
    }
    fclose(fp);
    fclose(tempp);
    remove(fname);
    rename(".temp", fname);

    //showFile(outputwindow, fname);
}

int main() {
    printf("heyo\n");
    replace("opop", "zzzz", "test3.txt");
}