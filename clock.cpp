#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <string>
#include <windows.h>

#pragma comment(lib, "User32.lib") // tells compiler to link user32.lib for keyboard reading

/*
CLI_Clock by Androsh7
https://github.com/Androsh7/CLI_Clock

MIT License

Copyright (c) 2025 Androsh7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// sets the coordinate position for the cursor
void setCursorPos(int x, int y) {
    printf("\033[%d;%dH", y,x);
}

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // grabs the handle for the console

// sets the visibility of the cursor
void setCursorVisibility(bool visible)
{
	CONSOLE_CURSOR_INFO lpCursor;	
	lpCursor.bVisible = visible; // sets the visibility of the cursor
	lpCursor.dwSize = DWORD(20);
	SetConsoleCursorInfo(console,&lpCursor);
}

// prints the help menu
void printHelpMenu() {
    setCursorPos(0,4);
    std::cout << "----- Help Options -----\n"
              << "Press ? to toggle help menu\n"\
              << "Press H to toggle 24/12 hour mode\n"
              << "Press Q to quit\n";
}

// pads an area with space characters: ' ' 
void clearArea(int len, int lines) {
    for (int y = 0; y < lines; y++) {
        for (int x = 0; x < len; x++) {
            printf(" ");
        }
        printf("\n");
    }
}

int main () {
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp);
    char output[50];

    bool help_menu = true; // prints the help menu
    bool clock_24 = false; // toggles the 24 hour clock

    // key states
    bool question_state = false; // tracks the state of the '?' key
    bool h_state = false; // tracks the state of the 'H' key
    bool q_state = false; // tracks the state of the 'Q' key

    // Hide cursor
    setCursorVisibility(0);

    // build the UI
    setCursorPos(0,0);
    std::cout << "\033[2J"
              << "----- Clock Application -----\n"
              << "Clock:\n"
              << "Date:\n";
    printHelpMenu();
    
    while (true) {

        // Gets the current time
        timestamp = time(NULL);
        datetime = *localtime(&timestamp);
        
        // prints the 24 hour clock
        if (clock_24) {
            setCursorPos(8,2);
            strftime(output, 50, "%H:%M:%S", &datetime);
            printf("%s   ", output);
        } 
        // prints the 12 hour clock
        else {
            setCursorPos(8,2);
            strftime(output, 50, "%I:%M:%S %p", &datetime);
            printf("%s", output);
        }

        // prints the date
        setCursorPos(7,3);
        strftime(output, 50, "%B %e, %Y", &datetime);
        printf("%s", output);
        
        // creates a loop that lasts 1000 miliseconds or until a key is pressed
        auto start_time = std::chrono::system_clock::now(); // current system time
        bool quit_loop = false; // set to true to quit the loop
        while ((std::chrono::system_clock::now() - start_time) < std::chrono::seconds(1) && !quit_loop) {

            // checks the state of the 'h' key
            if (GetAsyncKeyState('H') & 0x0001 && !h_state) {
                h_state = true;
                clock_24 = !clock_24;
                quit_loop = true;
            } else if (h_state) {
                h_state = false;
            }

            // checks the state of the 'q' key
            if (GetAsyncKeyState('Q') & 0x0001 && !q_state) {
                return 0;
            }

            // checks the state of the '?' key
            if (GetAsyncKeyState(VK_OEM_2) & 0x0001 && !question_state) {
                question_state = true;
                help_menu = !help_menu;
                quit_loop = true;

                // prints the help menu
                if (help_menu) { printHelpMenu(); }
                
                // removes the help menu
                if (!help_menu) {
                    setCursorPos(0,4);
                    clearArea(36,4); 
                }
            } else if (question_state) {
                question_state = false;
            }
        }
    }

}

