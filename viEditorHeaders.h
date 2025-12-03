#ifndef VIEDITORHEADERS_H_INCLUDED
#define VIEDITORHEADERS_H_INCLUDED

#include "Headers.h"

// --- Utility Functions ---
void initScreen();
void closeScreen();
void delaySeconds(u16 numberOfSeconds);
void clearLine(int y); // Helper to clear a specific row

// --- Editor Logic Functions ---
void initBuffer(TextBuffer* buf);
void freeBuffer(TextBuffer* buf);
void insertChar(TextBuffer* buf, c8 c);
void deleteChar(TextBuffer* buf);
void moveCursorLeft(TextBuffer* buf);
void moveCursorRight(TextBuffer* buf);
void saveToFile(TextBuffer* buf);

// --- Menu Options ---
void newOption();
void displayOption();
void exitOption();

// --- Main Runner ---
void runMenu();
void printMenu();

#endif // VIEDITORHEADERS_H_INCLUDED
