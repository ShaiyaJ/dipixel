/* 
 * DIPIXEL - DIsplay PIXELs to the terminal
 * A library for displaying pixels to a modern terminal by using ANSI escape sequences and the 
 * unicode half block character. Allows the use of the terminal to create bitmap graphics.
 *
 * DIPIXEL isn't dependent on DISEQ 
 *
 * Licensed under MIT - Shaiya J.
 */

#ifndef H_DIPIXEL
#define H_DIPIXEL

#include <stdbool.h>


// =====----- DIPIXEL constants and types -----===== //
#define DP_COLOR_HEADER_SIZE sizeof("\033[x8;2;")
#define DP_COLOR_ATTR_SIZE   sizeof("000;000;000m")

#ifndef DP_BLOCK_CHR
//#   define DP_BLOCK_CHR "▄"
#   define DP_BLOCK_CHR "▀"
#endif

#define DP_BLOCK_CHR_SIZE sizeof(DP_BLOCK_CHR)

typedef struct {
    char top_header[DP_COLOR_HEADER_SIZE]; 
    char top[DP_COLOR_ATTR_SIZE];

    char bottom_header[DP_COLOR_HEADER_SIZE];
    char bottom[DP_COLOR_ATTR_SIZE];

    char block_chr[DP_BLOCK_CHR_SIZE];
} Cell;

typedef struct {
    int pixel_width;    /* Number of pixels in the x-axis */
    int pixel_height;   /* Number of pixels in the y-axis */

    int cell_width;     /* Cell width will always be the same as pixel width */
    int cell_height;    /* Cell height will be half of pixel_height as 1 cell displays two pixels - for odd numbers of height it will be +1 */

    Cell* data;
} Buffer; 

// =====----- DISEQ function defs -----===== //
#ifdef DIPIXEL_STATIC
#   define DIPIXEL_DEF static
#else
#   define DIPIXEL_DEF extern
#endif

// =====----- DISEQ functions -----===== //

// Buffer management commands
DIPIXEL_DEF Buffer* dp_create_buffer(   // Creates a new buffer - returns NULL as an error value
    int width,                          /* Width (in pixels) of the buffer */
    int height                          /* Height (in pixels) of the buffer */
);

DIPIXEL_DEF void dp_free_buffer(        // Frees a created buffer
    Buffer* buffer                      /* Pointer to the buffer created with dp_create_buffer */
);

// Drawing commands
DIPIXEL_DEF void dp_draw_buffer(        // Draws a buffer to a certain position 
    Buffer* buffer,                     /* Input buffer to draw to the terminal */
    int row,                            /* Row in the terminal to draw to */
    int col                             /* Column in the terminal to draw to */
);

DIPIXEL_DEF void dp_draw_buffer_q(      // Draws a buffer at current cursor position - includes optimisations for this common case
    Buffer* buffer                      /* Input buffer to draw to the terminal */
); // NOTE: This function is primarily for people who want to draw at 0,0

DIPIXEL_DEF void dp_set_pixel(
    Buffer* buffer,                     /* Input buffer to draw to */
    int x,                              /* Pixel x position in the buffer to draw to - 0-indexed */
    int y,                              /* Pixel y position in the buffer to draw to - 0-indexed */
    unsigned char r,                    /* Red color component */
    unsigned char g,                    /* Green color component */
    unsigned char b                     /* Blue color component */
);

// =====----- DISEQ libc impl -----===== //
#ifdef DIPIXEL_IMPLEMENTATION
// ------------------------------------- //

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

DIPIXEL_DEF Buffer* dp_create_buffer(int width, int height) {
    // Allocate buffer
    Buffer* buf = (Buffer*) malloc(sizeof(Buffer));

    if (buf == NULL)
        return NULL;

    // Set up height information
    buf->pixel_width = width;
    buf->pixel_height = height;

    buf->cell_width = width;
    buf->cell_height = (height/2) + (height%2); // Requires an extra cell if it's an odd number

    // Set up data area 
    buf->data = (Cell*) malloc(buf->cell_width * buf->cell_height * sizeof(Cell));

    if (buf->data == NULL) {
        free(buf);
        return NULL;
    }

    Cell default_cell = (Cell) {
        .top_header    = "\033[38;2;",
        .top           = "000;000;000m",
        .bottom_header = "\033[48;2;",
        .bottom        = "000;000;000m",
        .block_chr     = DP_BLOCK_CHR
    };

    for (int i = 0; i < buf->cell_width * buf->cell_height; i++)
        buf->data[i] = default_cell;

    return buf;
}

DIPIXEL_DEF void dp_free_buffer(Buffer* buffer) {
    free(buffer->data);
    free(buffer);
}

// Drawing commands
DIPIXEL_DEF void dp_draw_buffer(Buffer* buffer, int row, int col) {
    /*
    // Iterate through rows and draw each line
    for (int row = 0; row < buffer->cell_height; row++) {
        // Align for write
        for (int i = 0; i < col; i++)
            putc(' ', stdout);
        write(STDOUT_FILENO, 
              buffer->data + (buffer->cell_width * row), 
              buffer->cell_width * sizeof(Cell)
        );

        // Align and set styles for next iteration
        putc('\n', stdout);
        fputs("\033[0m", stdout);
    }


    // Force output to show to screen immediately
    fflush(stdout);
    */
}

DIPIXEL_DEF void dp_draw_buffer_q(Buffer* buffer) {
    // Iterate through rows and draw each line
    for (int row = 0; row < buffer->cell_height; row++) {
        write(STDOUT_FILENO, 
              buffer->data + (buffer->cell_width * row), 
              buffer->cell_width * sizeof(Cell)
        );

        // Align for next iteration
        putc('\n', stdout);
    }

    // Reset styles (technically optional) // TODO: look into making this a configuration
    fputs("\033[0m", stdout);

    // Force output to show to screen immediately
    fflush(stdout);
}

DIPIXEL_DEF void dp_set_pixel(Buffer* buffer, int x, int y, unsigned char r, unsigned char g, unsigned char b){
    Cell* target = buffer->data + (buffer->cell_width * (y/2)) + x;

    char* buf = y % 2 ? target->bottom : target->top; 

    snprintf(buf, DP_COLOR_ATTR_SIZE, "%03d;%03d;%03dm", r, g, b);
}


// ------------------------------------- //
#endif
// =====---------------------------===== //


#endif
