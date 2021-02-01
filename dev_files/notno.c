// NotNo - Not Nano
// Felix Bowyer 2020-2021
//todo - improve buffers using custom string to double string size when out of chars

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int fileExists(char* fname) {
    // Checks to see if a file already exists - returns 1 if so, 0 if not
    // access returns 0 if the file exists
    if (access(fname, F_OK) == 0) return 1;
    else return 0;
}

/*char* getStringInput(char* pString, char* prompt) {
    // Gets input as a string - solely to improve readability and reduce repeated code
    
}*/

int createFile(char* fname) {
    // Creates a new file
    FILE* fp;
    if (fileExists(fname)) {
        char input;
        printf("File already exists - continue?\n[y/N]\t");
        scanf("%c", &input);
        if (input == 'y') {
            fp = fopen(fname, "w");
            printf("File created.\n");
            fclose(fp);
        }
    } else {
        fp = fopen(fname, "w");
        printf("File created.\n");
        fclose(fp);
    }
}

int copyFile(char* ffrom, char* fto) {
    // Creates a new file (fto) with the contents of another (ffrom) - could be done more efficiently with fread()?
    // open the two files, one to read, one to write
    FILE* fromp = fopen(ffrom, "r");
    FILE* top = fopen(fto, "w");
    char c;
    // append every character in ffrom to fto
    do {
        c = fgetc(fromp);
        if (c != EOF) fputc(c, top);
    } while (c != EOF);
    fclose(fromp);
    fclose(top);
}

int deleteFile(char* fname) {
    if (remove(fname) == 0) printf("The file was successfully deleted\n");
    else printf("The file does not exist or could not be deleted.\n");
}

int showFile(char* fname) {
    FILE* fp;
    char c;
    fp = fopen(fname, "r");
    while(1){
        c = fgetc(fp);
        if (c == EOF) break;
        else printf("%c", c);
    }
    fclose(fp);
    printf("\n");
}

int appendLine(char* fname, char* line) {
    FILE *fp;
    fp = fopen(fname, "a");
    fputc('\n', fp);
    int i;
    for (i=0; i<strlen(line); i++) {
        fputc(line[i], fp);
    }
    fclose(fp);
    // this can be improved with fprintf https://stackoverflow.com/questions/30213716/how-to-properly-append-lines-to-already-existing-file
}

int showLine(char* fname, int lineNo) {
    FILE *fp;
    char lineToShow[255];
    fp = fopen(fname, "r");
    int i;
    for (i=0; i<lineNo; i++) {
        fgets(lineToShow, sizeof(lineToShow), fp);
    }
    printf("%s\n", lineToShow);
    fclose(fp);
}

int insertLine(char* fname, char* lineToInsert, int lineNo) {
    // Inserts a line into a file by copying lines to a temporary file, including the one to be inserted, deleting the original, and then renaming the temp file
    FILE *ffromp;
    FILE *ftempp;
    char lineBuffer[255];
    ffromp = fopen(fname, "r");

    char orig_fname[255]; //max filename length?
    strncpy(orig_fname, fname, 255);
    strcat(fname, ".temp"); //this *might* cause a problem? not sure. adds .temp to the end of fname
    ftempp = fopen(fname, "a");

    int i;
    for (i=0; i<(lineNo-1); i++) {
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        //make sure line ends in \n
        strtok(lineBuffer, "\n"); // removes end newline if there is one
        strcat(lineBuffer, "\n"); // adds back the \n
        fprintf(ftempp, lineBuffer);
    }
    fprintf(ftempp, lineToInsert);
    // make sure there is no \n at end of file if inserted at last line
    for (i=lineNo; i<=8; i++) { //replace 8 with a function that counts the number of lines in a file pleasde
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        fprintf(ftempp, lineBuffer);
    }
    
    fclose(ffromp);
    fclose(ftempp);
    remove(orig_fname);
    rename(fname, orig_fname);
}

int main() {
    //a lot of this code can be removed or put in functions... review at some point
    // Selection menu
    printf("Select a function:\n---FILE OPERATIONS---\n[1]\tCreate File\n[2]\tCopy File\n[3]\tDelete File\n[4]\tShow File\n---LINE OPERATIONS---\n[5]\tAppend to file\n[6]\tShow a line\n[7]\tInsert a line\n");
    char choice;
    scanf("%c", &choice);
    char* fname;
    char temp;
    switch (choice) {
        case '1':;
            printf("Enter a file name to create:\n");
            // note %ms isn't standard c, it is a posix extension (as of 2008) which tells it to expand memory as the string is read
            scanf("%ms", &fname);
            createFile(fname);
            break;
        case '2':;
            char* fto;
            printf("Enter a file name to copy from:\n");
            scanf("%ms", &fname);
            printf("Enter a file name to copy to:\n");
            scanf("%ms", &fto);
            copyFile(fname, fto);
            free(fto);
            break;
        case '3':;
            printf("Enter a file name to delete:\n");
            scanf("%ms", &fname);
            deleteFile(fname);
            break;
        case '4':;
            printf("Enter a file name to display:\n");
            scanf("%ms", &fname);
            showFile(fname);
            break;
        case '5':;
            char lineToAp[255]; // max length 255, seems reasonable
            printf("Enter a file name to append to:\n");
            scanf("%ms", &fname);
            printf("Enter the line to append to the file:\n");
            scanf("%c", &temp); // clears stdin buffer-this is a complete bodge, but it took too long to find a workaround so the bodge stays
            fgets(lineToAp, sizeof(lineToAp), stdin); //include whitespace in resultant string
            strtok(lineToAp, "\n"); // removes end newline introduced by fgets
            appendLine(fname, lineToAp);
            break;
        case '6':;
            int lineShowNum;
            printf("Enter a filename:\n");
            scanf("%ms", &fname);
            printf("Enter a line number:\n");
            scanf("%c", &temp); // clears stdin buffer
            scanf("%d", &lineShowNum);
            showLine(fname, lineShowNum);
            break;
        case '7':;
            char lineToInsert[255];
            int lineInsNum;
            printf("Enter a filename:\n");
            scanf("%ms", &fname);
            printf("Enter a line to insert in the file:\n");
            scanf("%c", &temp); // clears stdin buffer
            fgets(lineToInsert, sizeof(lineToInsert), stdin);
            printf("Enter a line number to insert the line at:\n");
            scanf("%d", &lineInsNum);
            printf("%d\n", lineInsNum);
            insertLine(fname, lineToInsert, lineInsNum);
            break;
        default:
            printf("No option selected\n");
    }
    free(fname); //must free after using %ms

}