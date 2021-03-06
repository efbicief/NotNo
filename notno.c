#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* NotNo - Not Nano, a text editor
Felix Bowyer 2020-2021
*/

// ncurses functions

int createPrompt(WINDOW* inputborder, char* prompt) {
    // Creates a prompt message within a window border
    box(inputborder, 0, 0);
    //move cursor to (0,2) to begin writing
    wmove(inputborder, 0, 2);
    if (prompt != "") wprintw(inputborder, "%s", prompt);
    wrefresh(inputborder);
}

char* getInput(WINDOW* inputwindow, WINDOW* inputborder, char* prompt) {
    // Gets input from an ncurses window
    createPrompt(inputborder, prompt);

    //enable all keys
    keypad(inputwindow, true);
    char inputchar;
    int charcount = 0;
    int cursory, cursorx;

    // issue here - can't filter out arrow keys and other escape codes
    while((inputchar = wgetch(inputwindow)) != '\n') { //until enter pressed effectively
        if (inputchar == 127) { //127 = backspace
            if (charcount != 0) { //backspace shouldn't do anything if there are no characters in the window yet
                getyx(inputwindow, cursory, cursorx);
                mvwdelch(inputwindow, cursory, cursorx-1); //move back 1 char and delete
                wrefresh(inputwindow);
                charcount--;
            }
        } else {
            //put character in box
            waddch(inputwindow, inputchar);
            wrefresh(inputwindow);
            charcount++;
        }
    }

    char* inputstr = (char*)malloc(charcount+1);

    // get characters in box
    mvwinnstr(inputwindow, 0, 0, inputstr, charcount);
    werase(inputwindow);
    createPrompt(inputborder, "");

    charcount = 0;
    return inputstr;
}

int outputToWin(WINDOW* outputwindow, char* output) {
    // Output to ncurses window
    werase(outputwindow);
    wprintw(outputwindow, output);
    wrefresh(outputwindow);
}

int appendOutputToWin(WINDOW* outputwindow, char* output) {
    // output to ncurses window without erasing
    wprintw(outputwindow, output);
    wrefresh(outputwindow);
}

int outputCharToWin(WINDOW* outputwindow, char output) {
    // output single character to ncurses window without erasing
    wprintw(outputwindow, "%c", output);
    wrefresh(outputwindow);
}

// File operation functions

int fileExists(char* fname) {
    // Checks to see if a file already exists - returns 1 if so, 0 if not
    // access returns 0 if the file exists
    if (access(fname, F_OK) == 0) return 1;
    else return 0;
}

int createFile(char* fname) {
    // Creates a new file
    FILE* fp;
    fp = fopen(fname, "w");
    fclose(fp);
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
    // deletes a file
    if (remove(fname) == 0) return 0; //0 returned on success
    else return 1;
}

int showFile(WINDOW* outputwindow, char* fname) {
    //outputs file contents to an ncurses window
    //first clear outputwindow
    outputToWin(outputwindow, "");

    FILE* fp;
    char c;
    fp = fopen(fname, "r");
    while(1){
        // go thorugh every character and output
        c = fgetc(fp);
        if (c == EOF) break;
        else outputCharToWin(outputwindow, c);
    }
    fclose(fp);
}

int appendLine(char* fname, char* line) {
    //appends a line to the end of a file
    FILE *fp;
    fp = fopen(fname, "a"); //append mode
    int i;
    fputc('\n', fp);
    for (i=0; i<strlen(line); i++) { //append each character from line to the end
        fputc(line[i], fp);
    }
    fclose(fp);
}

int showLine(char* fname, int lineNo, WINDOW* outputwin, WINDOW* outputborder, char* prompt) {
    //shows the lineNo th line of a file
    FILE *fp;
    int bufsize = 255;
    char* lineToShow = (char*)malloc(bufsize);
    fp = fopen(fname, "r"); //read mode
    int i;
    for (i=0; i<lineNo; i++) {
        fgets(lineToShow, bufsize, fp); //get every line, up to line wanted to be output
    }
    fclose(fp);
    createPrompt(outputborder, prompt);
    outputToWin(outputwin, lineToShow); //output the line
}

int countFileLines(char* fname) {
    // returns an int containing the number of lines in a file, by counting newline characters
    FILE* fp;
    char c;
    int newlines = 1;
    fp = fopen(fname, "r");
    c = fgetc(fp);
    if (c == EOF) return 1; //Empty files have 1 line
    else if (c == '\n') newlines++;
    while (1) {
        c = fgetc(fp);
        if (c == '\n') newlines++; //add 1 when newline found
        else if (c == EOF) break;
    }
    fclose(fp);
    return newlines;
}

int insertLine(char* fname, char* lineToInsert, int lineNo) {
    // Inserts a line into a file by copying lines to a temporary file, including the one to be inserted, deleting the original, and then renaming the temp file
    int numOfLines = countFileLines(fname);
    FILE* ffromp;
    FILE* ftempp;
    char lineBuffer[255];
    ffromp = fopen(fname, "r");

    //before i was trying to add .temp to the end of the filename, but it is easier just to call the file ".temp".
    ftempp = fopen(".temp", "a");

    int i;
    for (i=0; i<(lineNo-1); i++) {
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        //make sure line ends in \n
        strtok(lineBuffer, "\n"); // removes end newline if there is one
        strcat(lineBuffer, "\n"); // adds back the \n
        fprintf(ftempp, lineBuffer);
    }
    if (lineNo <= numOfLines) strcat(lineToInsert, "\n"); //i think this is okay, if there are issues there is a problem with the <=, maybe should be <
    fprintf(ftempp, lineToInsert);
    // make sure there is no \n at end of file if inserted at last line
    for (i=lineNo; i<=numOfLines-1; i++) {
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        fprintf(ftempp, lineBuffer);
    }
    
    fclose(ffromp);
    fclose(ftempp);
    remove(fname);
    rename(".temp", fname);
}

int deleteLine(char* fname, int lineNo) {
    // Inserts a line into a file by copying lines to a temporary file, including the one to be inserted, deleting the original, and then renaming the temp file
    int numOfLines = countFileLines(fname); 
    numOfLines--; //we want 1 less line
    FILE *ffromp;
    FILE *ftempp;
    char lineBuffer[255];
    ffromp = fopen(fname, "r");

    //before i was trying to add .temp to the end of the filename, but it is easier just to call the file ".temp".
    ftempp = fopen(".temp", "a");

    int i;
    for (i=0; i<(lineNo-1); i++) {
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        //make sure line ends in \n
        strtok(lineBuffer, "\n"); // removes end newline if there is one
        strcat(lineBuffer, "\n"); // adds back the \n
        fprintf(ftempp, lineBuffer);
    }

    fgets(lineBuffer, sizeof(lineBuffer), ffromp);

    for (i=lineNo; i<numOfLines; i++) { //append remaining lines
        fgets(lineBuffer, sizeof(lineBuffer), ffromp);
        fprintf(ftempp, lineBuffer);
    }
    
    fclose(ffromp);
    fclose(ftempp);
    remove(fname);
    rename(".temp", fname);
}

int logNumOfLines(FILE* logp, char* fname) {
    // writes the number of lines in a file to the log pointed to by logp
    int fileLines = countFileLines(fname);
    fprintf(logp, "=> File %s has %i lines\n", fname, fileLines);
}

int find(char* searchString, char* fname, WINDOW* outputwindow) {
    //clear outputwindow
    outputToWin(outputwindow, "");

    FILE* fp = fopen(fname, "r");
    size_t bufLength = strlen(searchString);
    char* searchBuffer = (char*) malloc(bufLength * sizeof(char));
    //go through file, write next bufLength chars to buffer, compare to see if it is the same
    //ToDo put some condition in so that it doesnt search when the file's length is smaller than the search string
    int numFound = 0;
    int i;

    //initial buffer fill
    for (i=0; i<bufLength; i++) {
        searchBuffer[i] = fgetc(fp);
    }

    //check if buffer matches searchString
    if (strncmp(searchString, searchBuffer, bufLength) == 0) {
        //appendOutputToWin(outputwindow, "Found on line 1\n");
        numFound++;
    }

    char* toPrint;
    int currLine = 1; //not needed
    while (searchBuffer[bufLength-1] != EOF) {
        //shift buffer
        for (i=0; i<bufLength-1; i++) {
            searchBuffer[i] = searchBuffer[i+1];
        }
        //add new char to end
        searchBuffer[bufLength-1] = fgetc(fp);
        if (searchBuffer[bufLength-1] == '\n') currLine++;

        //check if buffer matches searchString
        if (strncmp(searchString, searchBuffer, bufLength) == 0) {
            //appendOutputToWin(outputwindow, "Found\n");
            numFound++;
        }
    }
    sprintf(toPrint, "Total number of occurrences: %i", numFound);
    appendOutputToWin(outputwindow, toPrint);
}

int replace(char* searchString, char* replString, char* fname, WINDOW* outputwindow) {
    //clear outputwindow
    outputToWin(outputwindow, "");

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

    //check if buffer matches searchString
    if (strncmp(searchString, searchBuffer, bufLength) == 0) {
        numFound++;
        // now add index to array
        matchIndices = (int*) realloc(matchIndices, sizeof(int) * numFound);
        matchIndices[numFound-1] = currChar;
    }

    char* toPrint;
    while (searchBuffer[bufLength-1] != EOF) {
        //shift buffer
        for (i=0; i<bufLength-1; i++) {
            searchBuffer[i] = searchBuffer[i+1];
        }
        //add new char to end
        searchBuffer[bufLength-1] = fgetc(fp);

        //check if buffer matches searchString
        if (strncmp(searchString, searchBuffer, bufLength) == 0) {
            //appendOutputToWin(outputwindow, "Found\n");
            numFound++;
            matchIndices = (int*) realloc(matchIndices, sizeof(int) * numFound);
            matchIndices[numFound-1] = (currChar-bufLength);
        }
        currChar++;
    }

    //now, use the indices to replace
    fclose(fp);
    fp = fopen(fname, "r"); //reopen to get back to start
    FILE* tempp = fopen(".temp", "w"); //write to temp file first
    currChar = 0;
    numFound = 0;
    char c = 'p'; //placeholder, could be anything but EOF
    while (c != EOF) {
        c = fgetc(fp);
        if (matchIndices[numFound] == currChar-1) { //if we are at a position where we need to replace
            fputs(replString, tempp);
            numFound++;
            for (i=0; i<bufLength-1; i++) {
                fgetc(fp);
                currChar++;
            }
            currChar++;
        } else {
            if (c != EOF) {
                fputc(c, tempp);
                currChar++;
            }
        }
    }
    fclose(fp);
    fclose(tempp);
    remove(fname);
    rename(".temp", fname);

    showFile(outputwindow, fname);
}

int checkFileName(char* fname) {
    // code saver method, does some checks on filenames to validate user input
    if (strcmp(fname, "") == 0) return 1; //file name invalid
    else if (fileExists(fname)) return 2; //file already exists
    else return 0; //file does not currently exist (or is unreadable)
}

int main(int argc, char** argv) {
    //manage changelog
    if (fileExists(".notno_changelog") == 0) createFile(".notno_changelog");
    FILE* chlogp;
    chlogp = fopen(".notno_changelog", "a");
    fprintf(chlogp, "==Program start==\n");

    initscr();
    noecho();
    cbreak();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW* menuwin = newwin(yMax-13, 23, 5, 5);
    WINDOW* displaywinborder = newwin(yMax-13, xMax-34, 5, 30);
    WINDOW* displaywin = newwin(yMax-15, xMax-36, 6, 31);
    WINDOW* inputwinborder = newwin(5, xMax-8, yMax-7, 5);
    WINDOW* inputwin = newwin(3, xMax-10, yMax-6, 6);
    box(menuwin, 0, 0);
    box(displaywinborder, 0, 0);
    box(inputwinborder, 0, 0);
    refresh();
    wrefresh(menuwin);
    wrefresh(displaywinborder);
    wrefresh(displaywin);
    wrefresh(inputwinborder);
    wrefresh(inputwin);

    createPrompt(displaywinborder, "Display:");
    createPrompt(inputwinborder, "Input:");

    keypad(menuwin, true);
    int noOfChoices = 16;
    char* choices[16] = {"---FILE OPERATIONS---", "Create File", "Copy File", "Delete File", "Show File", "---LINE OPERATIONS---", "Append to File", "Show a Line", "Insert a Line", "Delete a Line","-GENERAL  OPERATIONS-", "Show Change Log", "Show no. of Lines", "Search string", "Replace string", "EXIT"};
    int choice;
    int highlight = 0;
    char* fname;
    int exit = 0;
    char confirm;

    while (exit == 0) {
        while(1) {
            int i;
            //draw menu
            for (i=0; i<noOfChoices; i++) {
                if (i == 0 || i == 5 || i == 10) wattron(menuwin, A_DIM|A_BOLD);
                if (i == highlight) wattron(menuwin, A_BOLD|A_REVERSE);
                mvwprintw(menuwin, i+1, 1, choices[i]);
                wattroff(menuwin, A_BOLD|A_REVERSE|A_DIM);
            }

            //menu navigation
            choice = wgetch(menuwin);
            switch(choice) {
                case KEY_UP:
                    highlight--;
                    break;
                case KEY_DOWN:
                    highlight++;
                    break;
                default:
                    break;
            }

            //menu looping
            if (highlight > noOfChoices-1) highlight = 0;
            else if (highlight < 0) highlight = noOfChoices-1;

            //choose menu item
            if(choice == 10 && (highlight != 0 && highlight != 5 && highlight != 10) ) break; //if enter pressed (key 10) on a valid option
        }

        // process menu choice
        switch (highlight) {
            case 1: //Create File
                fname = getInput(inputwin, inputwinborder, "Enter a file name to create:");

                //if entered name is just empty string
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;

                //Check if file already exists to prevent overwrite
                } else if (checkFileName(fname) == 2) { 
                    confirm = getInput(inputwin, inputwinborder, "File already exists - continue? [y/N] ")[0];
                    if (confirm == 'y' || confirm == 'Y') ;
                    else {
                        outputToWin(displaywin, "File not created");
                        break;
                    }
                }
                
                //File creation
                createFile(fname);
                outputToWin(displaywin, "File successfully created");
                fprintf(chlogp, "File %s created\n", fname);
                logNumOfLines(chlogp, fname);
                break;

            case 2: //copy file
                fname = getInput(inputwin, inputwinborder, "Enter a file to copy from:");

                //if name invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");

                //if file already exists
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                } else {
                    char* fnameto = getInput(inputwin, inputwinborder, "Enter a file name to copy to:");

                    //stop file copying to itself
                    if(strcmp(fname, fnameto) == 0) {
                        outputToWin(displaywin, "Cannot copy a file to itself");
                        break;
                    }

                    //if name invalid
                    if (checkFileName(fnameto) == 1) {
                        outputToWin(displaywin, "Invalid name entered");
                        break;
                    } else if (checkFileName(fnameto) == 2) {
                        confirm = getInput(inputwin, inputwinborder, "File already exists - continue? [y/N] ")[0];
                        if (confirm == 'y' || confirm == 'Y') ;
                        else {
                            outputToWin(displaywin, "File not created");
                            break;
                        }
                    }

                    //File copying
                    copyFile(fname, fnameto);
                    fprintf(chlogp, "File %s copied to %s\n", fname, fnameto);
                    outputToWin(displaywin, "File successfully copied");
                    logNumOfLines(chlogp, fname);
                    logNumOfLines(chlogp, fnameto);
                }
                break;

            case 3: //delete file
                fname = getInput(inputwin, inputwinborder, "Enter a file to delete:");
                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;

                //if file successfully deletes
                } else if (deleteFile(fname) == 0) {
                    fprintf(chlogp, "File %s deleted\n", fname);
                    outputToWin(displaywin, "File successfully deleted");
                }
                else outputToWin(displaywin, "File could not be deleted");
                break;
            
            case 4: //show file
                fname = getInput(inputwin, inputwinborder, "Enter a file to show:");
                
                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                //show the file
                createPrompt(displaywinborder, fname);
                fprintf(chlogp, "File %s shown\n", fname);
                logNumOfLines(chlogp, fname);
                showFile(displaywin, fname);
                break;

            case 6: //append line
                fname = getInput(inputwin, inputwinborder, "Enter a file to append to:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                //append the line
                char* linetoappend = getInput(inputwin, inputwinborder, "Enter a line to append:");
                appendLine(fname, linetoappend);
                fprintf(chlogp, "File %s appended to with: %s\n", fname, linetoappend);
                logNumOfLines(chlogp, fname);
                outputToWin(displaywin, "Line successfully appended to file");
                break;

            case 7: //show line
                fname = getInput(inputwin, inputwinborder, "Enter a file to show a line from:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                char* flineindex = getInput(inputwin, inputwinborder, "Enter the line number to show:");

                if (atoi(flineindex) <= 0 || atoi(flineindex) > countFileLines(fname)) outputToWin(displaywin, "Invalid line number entered");
                else {
                    showLine(fname, atoi(flineindex), displaywin, displaywinborder, flineindex);
                    fprintf(chlogp, "Line %s shown from file %s\n", flineindex, fname);
                    logNumOfLines(chlogp, fname);
                }
                break;

            case 8: //insert line
                fname = getInput(inputwin, inputwinborder, "Enter a file to insert a line to:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                char* linetoinsert = getInput(inputwin, inputwinborder, "Enter a line to insert:");
                char* finsindex = getInput(inputwin, inputwinborder, "Enter the line number to insert at:");
                if (atoi(finsindex) <= 0 || atoi(finsindex) > countFileLines(fname)) outputToWin(displaywin, "Invalid line number entered");
                else {
                    insertLine(fname, linetoinsert, atoi(finsindex));
                    fprintf(chlogp, "Line \"%s\" inserted into file %s at index %s\n", linetoinsert, fname, finsindex);
                    logNumOfLines(chlogp, fname);
                }
                break;

            case 9: //delete line
                fname = getInput(inputwin, inputwinborder, "Enter a file to delete a line from:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadablaae");
                    break;
                }

                char* fdelindex = getInput(inputwin, inputwinborder, "Enter a line number to delete:");
                if (atoi(fdelindex) <= 0 || atoi(fdelindex) > countFileLines(fname)) outputToWin(displaywin, "Invalid line number entered");
                else { 
                    deleteLine(fname, atoi(fdelindex));
                    fprintf(chlogp, "Line %s deleted from file %s\n", fdelindex, fname);
                    logNumOfLines(chlogp, fname);
                }
                break;

            case 11: //show change log
                fclose(chlogp);
                showFile(displaywin, ".notno_changelog");
                chlogp = fopen(".notno_changelog", "a");
                break;

            case 12: //show number of lines
                //printf("%i\n", countFileLines("test.txt"));
                fname = getInput(inputwin, inputwinborder, "Enter a file to count the lines of:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                char linesNum[255];
                sprintf(linesNum, "%d", countFileLines(fname));
                outputToWin(displaywin, linesNum);
                fprintf(chlogp, ("Number of lines shown in file %s: %s\n", fname, linesNum));
                logNumOfLines(chlogp, fname);
                break;

            case 13: //find
                fname = getInput(inputwin, inputwinborder, "Enter a file to search in:");
                
                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                char* searchString = getInput(inputwin, inputwinborder, "Enter a string to search:");
                if (strcmp(searchString, "") == 0) {
                    outputToWin(displaywin, "Invalid search string entered");
                    break;
                }
                find(searchString, fname, displaywin);
                fprintf(chlogp, "File %s searched for %s\n", fname, searchString);
                logNumOfLines(chlogp, fname);
                break;

            case 14: //replace
                fname = getInput(inputwin, inputwinborder, "Enter a file to replace in:");

                //if filename invalid
                if (checkFileName(fname) == 1) {
                    outputToWin(displaywin, "Invalid name entered");
                    break;
                
                //if file doesn't exist
                } else if (checkFileName(fname) == 0) {
                    outputToWin(displaywin, "File does not exist, or is unreadable");
                    break;
                }

                char* findString = getInput(inputwin, inputwinborder, "Enter a string to replace:");
                char* replString = getInput(inputwin, inputwinborder, "Enter a string to replace with:");
                replace(findString, replString, fname, displaywin);
                fprintf(chlogp, "%s replaced with %s in file %s\n",findString, replString, fname);
                logNumOfLines(chlogp, fname);
                break;

            case 15: //exit
                exit = 1;
                break;

            default:
                printf("Invalid option chosen\n");
        }

        refresh();
        wrefresh(menuwin);
        wrefresh(displaywin);
        wrefresh(inputwin);

    }

    //close the program
    endwin();

    fclose(chlogp);

    return 0;
}
