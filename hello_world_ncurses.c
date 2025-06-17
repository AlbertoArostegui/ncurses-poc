#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define MENU_WIDTH 35
#define MENU_HEIGHT 10

char *choices[] = {
    "Mamar fuerte",
    "Mamar suave",
    "Megamamar",
    "Limpieza de sable",
    "Comando",
    "Salir"
};

int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight) {
    int x = 2;
    int y = 2;

    box(menu_win, 0, 0); // Draw a box around the window
    for (int i = 0; i < n_choices; ++i) {
        // Highlight the current choice
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE); // Turn on reverse video
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE); // Turn off reverse video
        } else {
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        }
        ++y;
    }
    wrefresh(menu_win); // Refresh the window to show changes
}

int sleep_ms(uint16_t ms) {
    return usleep(1000 * ms);
}

int main()
{
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
  initscr();                      /* Start curses mode              */
  start_color();                    /* Start color functionality    */
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  mvchgat(0, 0, -1, A_BLINK, 1, NULL);
  noecho();
  refresh();
  cbreak();
    keypad(stdscr, TRUE); // Enable function keys like arrows
  uint16_t startx = COLS / 2;
  uint16_t starty = LINES / 2;
  char* str = "Macintosh mamahuevo del meno";
  size_t len = strlen(str);

  uint8_t cont = 0;
  attrset(A_BOLD | A_UNDERLINE);
  for (int i = 0; i <= len; i++) {
      move(starty, startx - cont);
      char* aux = (char *) malloc(200);
      strncpy(aux, str, i);
      printw("%s", aux);
      refresh();
      sleep_ms(35);

      if (i % 2 == 0)
          cont++;
  }
      sleep_ms(500);
    curs_set(0);
    startx = (COLS - MENU_WIDTH) / 2;
    starty = (LINES - MENU_HEIGHT) / 2;

    mvprintw(starty - 2, startx, "Please select an option:");
    refresh();

    // Create a new window for the menu
    menu_win = newwin(MENU_HEIGHT, MENU_WIDTH, starty, startx);
    keypad(menu_win, TRUE); // Also enable function keys for the new window

    // --- MAIN LOOP ---
    print_menu(menu_win, highlight);
    while (1) {
        c = wgetch(menu_win); // Wait for user input in the menu window
        switch (c) {
        case KEY_UP:
            if (highlight == 1)
                highlight = n_choices;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_choices)
                highlight = 1;
            else
                ++highlight;
            break;
        case 10: // Enter key
            choice = highlight;
            break;
        default:
            // Optional: beep on other key presses
            beep(); 
            break;
        }
        
        print_menu(menu_win, highlight);
        if (choice != 0) // User made a choice, exit loop
            break;
    }

    // --- CLEANUP ---
    // Clear the window and screen before exiting
    wclear(menu_win);
    wrefresh(menu_win);
    clear();
    
    if (choice == n_choices) {
        endwin();
        return 0;
        
    } else if (choice == n_choices - 1) {
        char* command = (char *) malloc(100);
        wclear(menu_win);
        wrefresh(menu_win);
        clear();
        move(LINES / 2, (COLS - 30) / 2);
        wprintw(menu_win, "$ ");
        echo();     
        wgetstr(menu_win, command);
        //wprintw(menu_win, "%s\n", command);
        refresh();
        FILE *fp = popen(command, "r");
        char output[1024];
        while (fgets(output, 1024, fp) != NULL) {
            wprintw(menu_win, "%s", output);
            refresh();
        }
        //wprintw(menu_win, "After command\n");
        wgetch(menu_win);

    } else {
        // Display the selected option
        mvprintw(LINES / 2, (COLS - 30) / 2, "Muy bien, %s será", choices[choice - 1]);
        refresh();
        getch(); // Wait for one more key press before exiting
    }

    endwin(); // End curses mode and restore the terminal
    return 0;

}
