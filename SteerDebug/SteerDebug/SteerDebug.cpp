#include <iostream>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

void printSteeringDirection(int xValue, int prevXValue) {
    const int center = 32767; // Center value for X axis
    const int maxDegrees = 450; // Maximum degrees to the side (total 900 degrees)

    // Map X axis value to degrees with scaling
    int degrees = (xValue - center) * maxDegrees / (center / 2);

    if (xValue != prevXValue) {
        COORD pos = { 0, 1 }; // Line position for steering direction
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

        // Clamp degrees to maximum range
        if (degrees < -maxDegrees) {
            degrees = -maxDegrees; // Clamp to maximum left
        }
        else if (degrees > maxDegrees) {
            degrees = maxDegrees; // Clamp to maximum right
        }

        // Determine direction
        if (degrees < 0) {
            std::cout << "Steering left: " << abs(degrees) << " degrees" << std::flush;
        }
        else if (degrees > 0) {
            std::cout << "Steering right: " << degrees << " degrees" << std::flush;
        }
        else {
            std::cout << "Steering centered" << std::flush;
        }
    }
}

void printButtonPresses(DWORD buttons, DWORD prevButtons) {
    if (buttons != prevButtons) {
        COORD pos = { 0, 2 }; // Line position for button presses
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << std::string(80, ' ') << std::flush; // Clear the line
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); // Move cursor back to the start of the line

        for (int i = 0; i < 32; ++i) {
            if (buttons & (1 << i)) {
                std::cout << "Button " << i + 1 << " pressed" << std::flush;
            }
        }
    }
}

int main() {
    JOYINFOEX joyInfo = { 0 };
    JOYINFOEX prevJoyInfo = { 0 };
    joyInfo.dwSize = sizeof(JOYINFOEX);
    joyInfo.dwFlags = JOY_RETURNALL;
    prevJoyInfo.dwSize = sizeof(JOYINFOEX);
    prevJoyInfo.dwFlags = JOY_RETURNALL;

    if (joyGetNumDevs() == 0) {
        std::cerr << "No joystick devices found." << std::endl;
        return -1;
    }

    if (joyGetPosEx(JOYSTICKID1, &joyInfo) != JOYERR_NOERROR) {
        std::cerr << "Failed to get joystick position." << std::endl;
        return -1;
    }

    // Print initial empty lines
    std::cout << std::string(80, ' ') << std::endl; // Line for steering direction
    std::cout << std::string(80, ' ') << std::endl; // Line for button presses

    while (true) {
        if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR) {
            // Update specific lines for joystick state
            printSteeringDirection(joyInfo.dwXpos, prevJoyInfo.dwXpos);
            printButtonPresses(joyInfo.dwButtons, prevJoyInfo.dwButtons);

            // Save current state for next iteration
            prevJoyInfo = joyInfo;
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }

        Sleep(100); // Small delay to prevent flooding the console
    }

    return 0;
}
