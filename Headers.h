#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED


#include <iostream>
#include <ncurses.h> // Library ncurses only

// Typedefs based on your style
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef char           c8;

// Constants
#define InitRowValue    0
#define InitColumnValue 0
#define InitColor       1
#define InitDelay       1
#define ESC_KEY         27

// Struct for the editor buffer (Using pointers/heap)
struct TextBuffer {
    c8* data;       // Pointer to heap array
    u32 length;     // Current number of chars
    u32 capacity;   // Max size before realloc
    u32 cursor;     // Current cursor index
};


#endif // HEADERS_H_INCLUDED
