#include "Headers.h"
#include "viEditorHeaders.h"

using namespace std;

// Global Text Buffer for Heap
TextBuffer* globalEditor = nullptr;

/***************************************************** Helpers **************************************************/


void initScreen()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
}

void closeScreen()
{
    endwin();
}

void clearLine(int y)
{
    move(y, 0);
    clrtoeol();
}

/***************************************************** Editor Logic (New/Delete) ********************************/

void initBuffer(TextBuffer* buf)
{
    buf->capacity = 100; // Initial size
    buf->length = 0;
    buf->cursor = 0;

    buf->data = new c8[buf->capacity];


    for(u32 i = 0; i < buf->capacity; i++) {     // Initialize array with 0
        buf->data[i] = 0;
    }
}

void freeBuffer(TextBuffer* buf)
{
    if (buf->data != nullptr)
    {
        delete[] buf->data;
        buf->data = nullptr;
    }
    buf->length = 0;
    buf->capacity = 0;
    buf->cursor = 0;
}

// Manually expand array
void checkCapacity(TextBuffer* buf)
{
    if (buf->length >= buf->capacity - 1) {
        u32 oldCapacity = buf->capacity;
        u32 newCapacity = oldCapacity * 2;


        c8* newData = new c8[newCapacity];  // Allocate new larger block
        for (u32 i = 0; i < buf->length; i++)  // Copy old data to new block manually
        {
            newData[i] = buf->data[i];
        }

        for (u32 i = buf->length; i < newCapacity; i++)   //Initialize the rest of the new memory
        {
            newData[i] = 0;
        }
        delete[] buf->data;  // Delete old block to prevent memory leak


        buf->data = newData;
        buf->capacity = newCapacity;       // point to new block
    }
}


void insertChar(TextBuffer* buf, c8 c)
{
    checkCapacity(buf);
    for (u32 i = buf->length; i > buf->cursor; i--)      // Shift elements right
    {
        buf->data[i] = buf->data[i - 1];
    }
    buf->data[buf->cursor] = c;
    buf->length++;
    buf->cursor++;
}

void deleteChar(TextBuffer* buf)            // Shift elements left
{
    if (buf->cursor < buf->length)
    {
        for (u32 i = buf->cursor; i < buf->length - 1; i++)
        {
            buf->data[i] = buf->data[i + 1];
        }
        buf->length--;
    }
}

void moveCursorLeft(TextBuffer* buf)
{
    if (buf->cursor > 0)
        buf->cursor--;
}

void moveCursorRight(TextBuffer* buf)
{
    if (buf->cursor < buf->length)
        buf->cursor++;
}

void getVisualPos(TextBuffer* buf, int& y, int& x)
{
    y =0,x = 0;
    for (u32 i = 0; i < buf->cursor; i++)
    {
        if (buf->data[i] == '\n')
        {
            y++;
            x = 0;
        }
        else
        {
            x++;
        }
    }
}

/***************************************************** Options **************************************************/

void newOption()
{
    bool isEditing = true;
    int ch=0;
    // Allocation of the struct itself
    if (globalEditor == nullptr)
    {
        globalEditor = new TextBuffer;
    }
    else
    {
        // If it exists, clear old data before starting new session
        freeBuffer(globalEditor);
    }
    initBuffer(globalEditor);

    while (isEditing)
    {
        clear();

        // Display current buffer
        move(0, 0);
        for (u32 i = 0; i < globalEditor->length; i++) {
            addch(globalEditor->data[i]);
        }

        int vy, vx;
        getVisualPos(globalEditor, vy, vx);
        move(vy, vx);

        ch = getch();

        switch (ch) {
            case KEY_LEFT:
                moveCursorLeft(globalEditor);
            break;
            case KEY_RIGHT:
                moveCursorRight(globalEditor);
            break;
            case KEY_DC:
                deleteChar(globalEditor);
            break;
            case '\n':
                insertChar(globalEditor, '\n');
            break; // Enter
            case ESC_KEY:
                clearLine(LINES - 1);
                attron(COLOR_PAIR(2));
                mvprintw(LINES - 2, 0, "Save to memory? (y=Save / n=Cancel): ");
                attroff(COLOR_PAIR(2));
                {
                    int confirm = getch();
                    if (confirm == 'y' || confirm == 'Y')
                    {
                        // "Save" implies keeping the data in globalEditor for Display option
                        mvprintw(LINES - 1, 0, "Saved to memory! Press Backspace to return back.");
                        while ((ch = getch()) != KEY_BACKSPACE && ch != 127 && ch != 8);
                        isEditing = false;
                    }
                    else if (confirm == 'n' || confirm == 'N')
                    {
                        // "Cancel" implies discarding data
                        freeBuffer(globalEditor);
                        isEditing = false;
                    }
                }
                break;
            default:
                if (ch >= 32 && ch <= 126)
                {
                    insertChar(globalEditor, (c8)ch);
                }
                break;
        }
    }
}

void displayOption()
{
    int ch=0;
    clear();
    mvprintw(0, 0, "--- Displaying Stored Text ---");

    if (globalEditor != nullptr && globalEditor->data != nullptr && globalEditor->length > 0)
    {
        move(2, 0);
        for(u32 i=0; i<globalEditor->length; i++)
        {
            addch(globalEditor->data[i]);
        }
    }
    else
    {
        mvprintw(2, 0, "[No text saved in memory]");
    }

    mvprintw(LINES - 2, 0, "Press Backspace to return...");
    refresh();
    while ((ch = getch()) != KEY_BACKSPACE && ch != 127 && ch != 8);
}

void exitOption()                            // Clean heap before exit
{
    if (globalEditor != nullptr)
    {
        freeBuffer(globalEditor);
        delete globalEditor; // Delete the struct
    }
    closeScreen();
    exit(0);
}

/***************************************************** Menu **************************************************/

void runMenu() {
    const char* options[] = { "New", "Display", "Exit" };
    int n = 3;
    int highlight = 0;
    int choice = -1;

    while (true) {
        choice = -1;
        while (choice == -1) {
            clear();
            mvprintw(2, 30, "Simple VI Editor ");

            for (int i = 0; i < n; i++)
            {
                if (i == highlight)
                attron(COLOR_PAIR(2));
                else
                attron(COLOR_PAIR(1));

                mvprintw(i + 5, 30, " %s ", options[i]);

                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
            }

            int ch = getch();
            switch (ch)
            {
                case KEY_UP:
                    highlight = (highlight - 1 + n) % n;
                break;
                case KEY_DOWN:
                    highlight = (highlight + 1) % n;
                break;
                case 10:
                    choice = highlight;
                break;
            }
        }

        switch (choice)
        {
            case 0:
                newOption();
            break;
            case 1:
                displayOption();
            break;
            case 2:
                exitOption();
            break;
        }
    }
}

void printMenu(void)
{
    initScreen();
    runMenu();
    closeScreen();
}
