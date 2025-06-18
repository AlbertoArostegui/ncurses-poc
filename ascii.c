#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define FONT_WXH_RATIO 0.4

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1; // Exit with an error code
    }

    char chars[] = "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
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
                unsigned char R = *pixels++;
                unsigned char G = *pixels++;
                unsigned char B = *pixels++;
                if (pixelSize >= 4) {
                    //unsigned char A = *pixels++;
                    pixels++;
                }

                // Calculate the average of RGB values
                float Avg = (R+G+B) / 3.0f;
                // Calculate the value to index the character array
                int charIndex = (int)(charsLen * (Avg / 255.0f));
                //Print out the indexed char
                addch(chars[charIndex]);
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
