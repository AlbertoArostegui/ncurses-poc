#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define FONT_WXH_RATIO 0.4

#define BLACK          0
#define BLUE           1
#define GREEN          2
#define CYAN           3
#define RED            4
#define MAGENTA        5
#define BROWN          6
#define WHITE          7
#define BRIGHT_BLACK   8
#define BRIGHT_BLUE    9
#define BRIGHT_GREEN   10
#define BRIGHT_CYAN    11
#define BRIGHT_RED     12
#define BRIGHT_MAGENTA 13
#define BRIGHT_BROWN   14
#define BRIGHT_WHITE   15

typedef struct {
  unsigned char r, g, b;
} color_t;

color_t colors[16];
 
void init_colors() {
  colors[BLACK]          = (color_t){0x00, 0x00, 0x00};
  colors[BLUE]           = (color_t){0x00, 0x00, 0xa0};
  colors[GREEN]          = (color_t){0x00, 0xa0, 0x00};
  colors[CYAN]           = (color_t){0x00, 0xa0, 0xa0};
  colors[RED]            = (color_t){0xa0, 0x00, 0x00};
  colors[MAGENTA]        = (color_t){0xa0, 0x00, 0xa0};
  colors[BROWN]          = (color_t){0xa0, 0x50, 0x00};
  colors[WHITE]          = (color_t){0xa0, 0xa0, 0xa0};
  colors[BRIGHT_BLACK]   = (color_t){0x50, 0x50, 0x50};
  colors[BRIGHT_BLUE]    = (color_t){0x50, 0x50, 0xf0};
  colors[BRIGHT_GREEN]   = (color_t){0x50, 0xf0, 0x50};
  colors[BRIGHT_CYAN]    = (color_t){0x50, 0xf0, 0xf0};
  colors[BRIGHT_RED]     = (color_t){0xf0, 0x50, 0x50};
  colors[BRIGHT_MAGENTA] = (color_t){0xf0, 0x50, 0xf0};
  colors[BRIGHT_BROWN]   = (color_t){0xf0, 0xf0, 0x50};
  colors[BRIGHT_WHITE]   = (color_t){0xf0, 0xf0, 0xf0};
}

int color_diff(color_t c1, color_t c2) {
  int diff = abs(c1.r - c2.r) + abs(c1.g - c2.g) + abs(c1.b - c2.b);
  return diff;
}

short curs_color(char fg)
{
    switch (fg & 7) {           /* RGB */
    case 0:                     /* 000 */
        return (COLOR_BLACK);
    case 1:                     /* 001 */
        return (COLOR_BLUE);
    case 2:                     /* 010 */
        return (COLOR_GREEN);
    case 3:                     /* 011 */
        return (COLOR_CYAN);
    case 4:                     /* 100 */
        return (COLOR_RED);
    case 5:                     /* 101 */
        return (COLOR_MAGENTA);
    case 6:                     /* 110 */
        return (COLOR_YELLOW);
    case 7:                     /* 111 */
        return (COLOR_WHITE);
    default:
      return (COLOR_BLACK);
    }
}

void gen_pairs() {
  for (char fg = 0; fg < 8; fg++) {
    init_pair(fg, curs_color(fg), COLOR_BLACK);
  }
}

void set_color(char fg) {
  if (fg >= 8) {
    attron(A_BOLD);
    attron(COLOR_PAIR(fg-8));
  } else {
    attron(COLOR_PAIR(fg));
  }
}

void unset_color(char fg) {
  if (fg >= 8) {
    attroff(A_BOLD);
    attroff(COLOR_PAIR(fg-8));
  } else {
    attroff(COLOR_PAIR(fg));
  }
}

char get_color(color_t color) {
  int min_diff, col, diff;
  min_diff = 9999;
  for (int i = 0; i < 16; i++) {
    diff = color_diff(color, colors[i]);
    if (diff < min_diff) {
      min_diff = diff;
      col = i;
    }
  }
  //printf("or: %d og: %d ob: %d  tr: %d tg: %d tb: %d\n", color.r, color.g, color.b, colors[col].r, colors[col].g, colors[col].b);
  return col;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1; // Exit with an error code
    }

    char chars[] = " `^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
    int charsLen = strlen(chars);

    int width, height, pixelSize;

    unsigned char *ImageData = stbi_load(argv[1], &width, &height, &pixelSize, 0);
    unsigned char *resizedData;

    int s_row,s_col,row,col;

    double aspect_ratio_img, aspect_ratio_scr;

    initscr();
    getmaxyx(stdscr, s_row, s_col);
    endwin();

    aspect_ratio_img = (float)width/(float)height;
    aspect_ratio_scr = FONT_WXH_RATIO * (float)s_col/(float)s_row;

    start_color();
    gen_pairs();
    init_colors();

    printf("r: %d c: %d ai: %f as: %f \n", s_row, s_col, aspect_ratio_img, aspect_ratio_scr);

    if (aspect_ratio_img < aspect_ratio_scr) {
      col = (int)((float)s_col * aspect_ratio_img/aspect_ratio_scr);
      row = s_row;
    } else {
      col = s_col;
      row = (int)((float)s_row * (aspect_ratio_scr/aspect_ratio_img));
    }

    aspect_ratio_scr = FONT_WXH_RATIO * (float)col/(float)row;
    printf("r: %d c: %d ai: %f as: %f \n", row, col, aspect_ratio_img, aspect_ratio_scr);
    //resizedData = malloc(width * height * pixelSize);
    resizedData = malloc(col * row * pixelSize);
    stbir_resize_uint8_linear(ImageData, width, height, width*pixelSize,
                                            resizedData, col, row, col*pixelSize, 
                                            pixelSize);

    if(resizedData) {

        //printf("Width: %d\nHeight: %d\nPixelSize: %d\n", width, height, pixelSize);
        /*
        int row,col;
        initscr();
        mvprintw(1, 0,"This screen has %d rows and %d columns",row,col);
        mvprintw(2, 0, "So %d x %d", col, row);
        refresh();
        getch();
        endwin();
        */

        initscr();
        unsigned char *pixels = resizedData;
        for (int rowIndex = 0; rowIndex < row; rowIndex++)
        {
            for (int columIndex = 0; columIndex < col; columIndex++)
            {
                // Take out the RGBA data
                color_t color;
                color.r = *pixels++;
                color.g = *pixels++;
                color.b = *pixels++;
                if (pixelSize >= 4) {
                    //unsigned char A = *pixels++;
                    pixels++;
                }

                // Calculate the average of RGB values
                float Avg = (color.r+color.g+color.b) / 3.0f;
                // Calculate the value to index the character array
                int charIndex = (int)((charsLen-1) * (Avg / 255.0f));
                //Print out the indexed char
                char c = get_color(color);
                set_color(c);
                addch(chars[charIndex]);
                unset_color(c);
            }
            if (col < s_col) addch('\n');
        }
        getch();

        endwin();
    } else {
        printf("Failed to load image %s\n", argv[1]);
    }

    return 0;
}
