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

// A cell represents a single character in the terminal, comprised of a top pixel, a bottom pixel, and the half block character that distinguishes between them
typedef struct {
    char top_header[DP_COLOR_HEADER_SIZE];      /* ANSI escape sequence start */
    char top[DP_COLOR_ATTR_SIZE];               /* Colour information (gets written to)  */

    char bottom_header[DP_COLOR_HEADER_SIZE];   /* ANSI escapse sequence start */
    char bottom[DP_COLOR_ATTR_SIZE];            /* Colour information (gets written to) */

    char block_chr[DP_BLOCK_CHR_SIZE];          /* Half block character copy */
} Cell;


// A buffer represents a collection of cells - a canvas that will be printed to the terminal
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

// Outputting buffers to the terminal
DIPIXEL_DEF void dp_draw_buffer(        // Draws a buffer to a certain position 
    Buffer* buffer,                     /* Input buffer to draw to the terminal */
    int x,                              /* Column in the terminal to draw to (1-indexed) */
    int y                               /* Row in the terminal to draw to (1-indexed) */
);

DIPIXEL_DEF void dp_draw_buffer_quick(  // Draws a buffer at current cursor position - includes optimisations for this common case
    Buffer* buffer                      /* Input buffer to draw to the terminal */
); // NOTE: This function is primarily for people who want to draw at 0,0

DIPIXEL_DEF void dp_draw_buffer_single( // Draws a buffer at the current cursor position as a single line - the fastest way to draw a buffer
    Buffer* buffer                      /* Input buffer to draw to the terminal */
); // NOTE: This function is primarily for people who both want to draw at 0,0 and can set the size of their terminal

// Modifying buffer's data
DIPIXEL_DEF void dp_set_pixel(          // Sets a pixel at (x,y) in a buffer to a particular rgb value
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
DIPIXEL_DEF void dp_draw_buffer(Buffer* buffer, int x, int y) {
    // Iterate through rows and draw each line
    for (int row = 0; row < buffer->cell_height; row++) {
        // Align and set styles for this iteration
        printf("\033[%d;%dH", row+y, x);
        fflush(stdout);

        // Draw line
        write(STDOUT_FILENO, 
              buffer->data + (buffer->cell_width * row), 
              buffer->cell_width * sizeof(Cell)
        );

        // Reset styles (technically optional) // TODO: look into making this a configuration
        fputs("\033[0m", stdout);
    }

    // Force output to show to screen immediately
    fflush(stdout);
}

DIPIXEL_DEF void dp_draw_buffer_quick(Buffer* buffer) {
    // Iterate through rows and draw each line
    for (int row = 0; row < buffer->cell_height; row++) {
        write(STDOUT_FILENO, 
              buffer->data + (buffer->cell_width * row), 
              buffer->cell_width * sizeof(Cell)
        );

        // Align for next iteration
        fputs("\033[1E", stdout);
        fflush(stdout);
    }

    // Reset styles (technically optional) // TODO: look into making this a configuration
    fputs("\033[0m", stdout);

    // Force output to show to screen immediately
    fflush(stdout);
}

DIPIXEL_DEF void dp_draw_buffer_single(Buffer* buffer) {
    write(STDOUT_FILENO, buffer->data, buffer->cell_width * buffer->cell_height * sizeof(Cell));

    // Reset styles (technically optional) // TODO: look into making this a configuration
    fputs("\033[0m", stdout);
}

DIPIXEL_DEF void dp_set_pixel(Buffer* buffer, int x, int y, unsigned char r, unsigned char g, unsigned char b){
    // Bounds check
    if (x < 0 || x >= buffer->pixel_width || y < 0 || y >= buffer->pixel_height)
        return;

    // Calculate the target pixel
    Cell* target = buffer->data + (buffer->cell_width * (y/2)) + x; // Target cell
    char* buf = y % 2 ? target->bottom : target->top;               // Target pixel inside that cell

    // Set value
    snprintf(buf, DP_COLOR_ATTR_SIZE, "%03d;%03d;%03dm", r, g, b);
}

// ------------------------------------- //
#endif
// =====---------------------------===== //


#endif
