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

#define Cyan_Highlight "\033[46m"
#define RST_color "\033[0m"

// prints the help menu
void printHelpMenu(int time_change_state) {
    setCursorPos(0,6);
    if (time_change_state == 1) { std::cout << Cyan_Highlight; }
    std::cout << "1) Adjust Seconds\n" << RST_color;
    if (time_change_state == 2) { std::cout << Cyan_Highlight; }
    std::cout << "2) Adjust Minutes\n" << RST_color;
    if (time_change_state == 3) { std::cout << Cyan_Highlight; }
    std::cout << "3) Adjust Hours\n" << RST_color;
    std::cout << "q) Exit";
}

void printMainUI() {
    std::cout << "\033[2J"
              << "*******************************         *******************************\n"
              << "*        12-Hour Clock        *         *        24-Hour Clock        *\n"
              << "*                             *         *                             *\n"
              << "*******************************         *******************************\n";
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
    struct tm offset_datetime = *localtime(&timestamp);
    long second_offset = 0;
    long temp_second_offset = 0;
    char output[50];

    int time_change_state = 0; // the setting for the adjust time

    bool help_menu = true; // toggle for printing the help menu

    // key states
    bool question_state = false; // tracks the state of the '?' key
    bool q_state = false; // tracks the state of the 'Q' key

    bool one_state = false; // tracks the state of the 1 key
    bool two_state = false; // tracks the state of the 2 key
    bool three_state = false; // tracks the state of the 3 key
    bool four_state = false; // tracks the state of the 4 key

    bool plus_state = false; // tracks the state of the '+' key
    bool minues_state = false; // tracks the state of the '-' key

    bool esc_state = false; // tracks the state of the ESC key
    bool enter_state = false; // tracks the state of the ENTER key

    // Hide cursor
    setCursorVisibility(0);

    // build the UI
    // inside text box is 30 chars long and 2 chars wide
    setCursorPos(0,0);
    
    printMainUI();
    printHelpMenu(time_change_state);
    
    while (true) {

        // Gets the current time
        timestamp = time(NULL);
        datetime = *localtime(&timestamp);
        timestamp += second_offset;
        offset_datetime = *localtime(&timestamp);

        // prints the 12 hour clock
        setCursorPos(11,3);
        strftime(output, 50, "%I:%M:%S %p", &offset_datetime);
        printf("%s", output);

        // prints the 24 hour clock
        setCursorPos(52,3);
        strftime(output, 50, "%H:%M:%S", &offset_datetime);
        printf("%s", output);

        // prints the time offset window
        if (time_change_state) {
            bool sign = true;
            if (temp_second_offset < 0) { sign = false; }

            printHelpMenu(time_change_state);

            int hour = fabs(temp_second_offset / 3600);
            int minute  = fabs((temp_second_offset % 3600) / 60);
            int second = fabs(temp_second_offset % 60);
            strftime(output, 50, "%I:%M:%S %p", &offset_datetime);
            setCursorPos(10,5);
            printf("%s", output);
            if (sign) { printf(" + %d:%d:%d", hour, minute, second); }
            else { printf(" - %d:%d:%d", hour, minute, second); }
        }
        
        // creates a loop that lasts 1 second or until a key is pressed
        auto start_time = std::chrono::system_clock::now(); // system time when starting loop
        bool quit_loop = false; // set to true to quit the loop
        while ((std::chrono::system_clock::now() - start_time) < std::chrono::seconds(1) && !quit_loop) {

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
                if (help_menu) { printHelpMenu(time_change_state); }
                
                // removes the help menu
                if (!help_menu) {
                    setCursorPos(0,6);
                    clearArea(25,5); 
                }
            } else if (question_state) {
                question_state = false;
            }
            
            // checks for number keys
            if      (GetAsyncKeyState(0x31) & 0x0001) { time_change_state = 1; printHelpMenu(time_change_state); temp_second_offset = second_offset; quit_loop = true; } // if pressed 1 key
            else if (GetAsyncKeyState(0x32) & 0x0001) { time_change_state = 2; printHelpMenu(time_change_state); temp_second_offset = second_offset; quit_loop = true; } // if pressed 2 key
            else if (GetAsyncKeyState(0x33) & 0x0001) { time_change_state = 3; printHelpMenu(time_change_state); temp_second_offset = second_offset; quit_loop = true; } // if pressed 3 key

            if (time_change_state) {
                // if ESC key is pressed exit key change mode
                if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) {
                    time_change_state = 0;
                    temp_second_offset = 0;
                    setCursorPos(0,5);
                    clearArea(50,1); 
                    printHelpMenu(time_change_state);
                }
                // save the new offset
                else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
                    second_offset = temp_second_offset;
                    temp_second_offset = 0;
                    time_change_state = 0;
                    setCursorPos(0,5);
                    clearArea(50,1);
                    printHelpMenu(time_change_state);
                }

                // add to offset
                if (GetAsyncKeyState(VK_OEM_PLUS) & 0x0001) {
                    temp_second_offset += pow(60, time_change_state -1);
                }
                else if (GetAsyncKeyState(VK_OEM_MINUS) & 0x0001) {
                    temp_second_offset -= pow(60, time_change_state -1);
                }

                // apply min/max to second offset
                temp_second_offset = min(temp_second_offset, 86400);
                temp_second_offset = max(temp_second_offset, -86400);
            }
        }
    }

}

