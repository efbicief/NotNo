#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

char* getInput(WINDOW* inputwindow) {
    keypad(inputwindow, true);
    char inputchar;
    int charcount = 0;
    int cursory, cursorx;

    // issue here - can't filter out arrow keys and other escape codes
    while((inputchar = wgetch(inputwindow)) != '\n') {
        if (inputchar == 127) { //127 = backspace
            if (charcount != 0) { //backspace shouldn't do anything if there are no characters in the window yet
                getyx(inputwindow, cursory, cursorx);
                mvwdelch(inputwindow, cursory, cursorx-1); //move back 1 char and delete
                wrefresh(inputwindow);
                charcount--;
            }
        } else {
            waddch(inputwindow, inputchar);
            wrefresh(inputwindow);
            charcount++;
        }
    }

    char* inputstr = (char*)malloc(charcount+1);

    mvwinnstr(inputwindow, 0, 0, inputstr, charcount);
    werase(inputwindow);
    charcount = 0;
    return inputstr;
}

int main(int argc, char** argv) {
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
    box(displaywinborder, 1, '#');
    box(inputwinborder, 0, 0);
    wprintw(displaywinborder, "Display:");
    wprintw(inputwinborder, "Input:");
    refresh();
    wrefresh(menuwin);
    wrefresh(displaywinborder);
    wrefresh(displaywin);
    wrefresh(inputwinborder);
    wrefresh(inputwin);

    keypad(menuwin, true);
    int noOfChoices = 9;
    char* choices[9] = {"---FILE OPERATIONS---", "Create File", "Copy File", "Delete File", "Show File", "---LINE OPERATIONS---", "Append to File", "Show a Line", "Insert a Line"};
    int choice;
    int highlight = 0;

    while(1) {
        int i;
        //draw menu
        for (i=0; i<noOfChoices; i++) {
            if (i == 0 || i == 5) wattron(menuwin, A_DIM|A_BOLD);
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
        if(choice == 10 && (highlight != 0 && highlight != 5) ) break; //if enter pressed (key 10) on a valid option
    }

    wprintw(displaywin, "You chose: %s\n", choices[highlight]);
    refresh();
    wrefresh(menuwin);
    wrefresh(displaywin);
    wrefresh(inputwin);

   /* wgetstr(inputwin, inputbuffer);
    wprintw(inputwin, inputbuffer);
    refresh();
    wrefresh(menuwin);
    wrefresh(displaywin);
    wrefresh(inputwin);*/

    char* inputstr = getInput(inputwin);
    wprintw(displaywin, "You entered: %s\n", inputstr);


    refresh();
    wrefresh(menuwin);
    wrefresh(displaywin);
    wrefresh(inputwin);

    getch();
    endwin();

    return 0;
}