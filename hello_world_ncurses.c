#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

uint32_t menu_width = 50;
uint32_t menu_height = 20;
uint32_t n_dir_entries;
size_t needed_width;

bool check_dir(char* dirpath) {
    bool is_dir = false;
    DIR *dp;
    dp = opendir(dirpath);
    if (dp != NULL)
        is_dir = true;

    return is_dir;
}

void listdir(char* dirpath, uint32_t index, char* buffer) {
    DIR *dp;
    struct dirent *ep;
    char* e_name;
    size_t curr_name;

    dp = opendir(dirpath);
    if (dp != NULL) {
        n_dir_entries = 0;
        while ((ep = readdir(dp)) != NULL) {
            e_name = ep->d_name;
            if (index == n_dir_entries) {
                size_t e_name_len = strlen(e_name);
                strncpy(buffer, e_name, e_name_len);
            }

            curr_name = strlen(e_name);
            needed_width = curr_name > needed_width ? curr_name : needed_width;

            n_dir_entries++;
        }
        needed_width += 3;

        closedir(dp);
    }
}

void print_menu(WINDOW *menu_win, int highlight, char* dirpath) {
    DIR *dp;
    struct dirent *ep;
    int x = 2;
    int y = 1;

    wclear(menu_win);
    dp = opendir(dirpath);
    if (dp != NULL) {
        box(menu_win, 0, 0); // Draw a box around the window
        int cont = 0;

        while ((ep = readdir(dp)) != NULL) {
            if (highlight == cont) {

                wattron(menu_win, A_REVERSE); // Turn on reverse video
                mvwprintw(menu_win, y, x, "%s", ep->d_name);
                wattroff(menu_win, A_REVERSE); // Turn off reverse video
                
            } else {
                mvwprintw(menu_win, y, x, "%s", ep->d_name);
            }

            ++y;
            cont++;
        }
        closedir(dp);
        wrefresh(menu_win);
    }
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
    char* entry = NULL;
    initscr();                      /* Start curses mode              */
    // start_color();                    /* Start color functionality    */
    // init_pair(1, COLOR_CYAN, COLOR_BLACK);
    mvchgat(0, 0, -1, A_BLINK, 1, NULL);
    noecho();
    refresh();
    cbreak();
    keypad(stdscr, TRUE); // Enable function keys like arrows

    char* cwd = (char *) malloc(BUFF_SIZE);
    getcwd(cwd, BUFF_SIZE);
    entry = (char *) malloc(BUFF_SIZE);
    listdir(cwd, 0, entry);
    free(entry);
    refresh();
    
    uint16_t startx = COLS / 2;
    uint16_t starty = LINES / 2;
    // char* str = "Macintosh mamahuevo del meno";
    // size_t len = strlen(str);

    /*
    // Greeting
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
    */
    curs_set(0);
    startx = (COLS - needed_width) / 2;
    starty = (LINES - n_dir_entries) / 2;

    mvprintw(starty, startx, "Please select an option:");
    refresh();

    menu_win = newwin(n_dir_entries + 2, needed_width + 1, starty, startx);
    keypad(menu_win, TRUE); // Also enable function keys for the new window

    // --- MAIN LOOP ---
    print_menu(menu_win, highlight, cwd);
    while (1) {
        bool is_dir = false;
        c = wgetch(menu_win); // Wait for user input in the menu window
        switch (c) {
        case KEY_UP:
            if (highlight == 0)
                highlight = n_dir_entries - 1;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_dir_entries - 1)
                highlight = 0;
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

        char* new_dir;
        if (choice) {
            entry = (char *) malloc(BUFF_SIZE);
            listdir(cwd, highlight, entry);
            size_t curr_dir_len = strlen(cwd);
            size_t entry_len = strlen(entry);

            new_dir = (char *) malloc(curr_dir_len + entry_len + 2);
            new_dir = strncpy(new_dir, cwd, curr_dir_len);
            strcat(new_dir, "/");
            strncat(new_dir, entry, entry_len);

            mvprintw(0,0,"%s", new_dir);
            refresh();
            is_dir = check_dir(new_dir);
            free(entry);
            if (is_dir) {
                cwd = new_dir;

                entry = (char *) malloc(BUFF_SIZE);
                listdir(cwd, highlight, entry);
                free(entry);

                print_menu(menu_win, highlight, cwd);
            } else {
                free(new_dir);
            }
        } else {
            print_menu(menu_win, highlight, cwd);
        }

        choice = 0;
    }

    // --- CLEANUP ---
    // Clear the window and screen before exiting
    wclear(menu_win);
    wrefresh(menu_win);
    clear();
    
    /*
    if (choice ==) {
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
    }*/

    endwin(); // End curses mode and restore the terminal
    return 0;

}
