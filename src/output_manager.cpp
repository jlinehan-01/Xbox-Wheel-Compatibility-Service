// Xbox Wheel Compatibility Service
// Copyright (C) 2025 Joshua Linehan
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/******************************************************************************
 * output_manager.cpp                                                         *
 *                                                                            *
 * Singleton class to handle text output                                      *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#include "output_manager.h"

const int OutputManager::OUTPUT_WIDTH = 80;

OutputManager::OutputManager()
    : outputPos{0, 0}, maxPos{0, 0}, hConsole{GetStdHandle(STD_OUTPUT_HANDLE)}
{
    try
    {
        outputPos = OutputManager::getCursorPos(hConsole);
    }
    catch (...)
    {
        // leave outputPos as is
    }
}

OutputManager::~OutputManager()
{
    // return cursor to next line
    std::lock_guard<std::mutex> lock(outputMutex);
    outputPos.X = 0;
    SetConsoleCursorPosition(hConsole, outputPos);
};

// returns the current cursor position
COORD OutputManager::getCursorPos(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return csbi.dwCursorPosition;
    }
    else
    {
        throw;
    }
}

// returns the singleton instance
OutputManager &OutputManager::getInstance()
{
    static OutputManager instance;
    return instance;
}

// prints a message to the screen
void OutputManager::print(std::ostream &stream, std::string message)
{
    std::lock_guard<std::mutex> lock(outputMutex);
    SetConsoleCursorPosition(hConsole, outputPos);
    stream << std::left << std::setw(OUTPUT_WIDTH) << message << std::endl;
    outputPos.Y += 1;
    // clear output below
    COORD currentPos = outputPos;
    while (currentPos.Y < maxPos.Y)
    {
        std::cout << std::setw(OUTPUT_WIDTH) << "" << std::endl;
        currentPos.Y++;
    }
}

// prints a message to the screen
void OutputManager::log(std::string message)
{
    print(std::cout, message);
}

// logs an error
void OutputManager::error(std::string message)
{
    print(std::cerr, message);
}

// prints telemetry data to the screen
void OutputManager::printTelemetry(std::vector<std::string> lines)
{
    std::lock_guard<std::mutex> lock(outputMutex);
    // print each line
    SetConsoleCursorPosition(hConsole, outputPos);
    for (std::string line : lines)
    {
        std::cout << std::left << std::setw(OUTPUT_WIDTH) << line << std::endl;
    }
    // record bottom of output
    COORD currentPos = getCursorPos(hConsole);
    if (currentPos.Y > maxPos.Y)
    {
        maxPos = currentPos;
    }
    std::cout << std::flush;
}

// clears telemetry output from screen
void OutputManager::clearTelemetry()
{
    std::lock_guard<std::mutex> lock(outputMutex);
    // clear telemetry output
    SetConsoleCursorPosition(hConsole, outputPos);
    COORD currentPos = outputPos;
    while (currentPos.Y < maxPos.Y)
    {
        std::cout << std::setw(OUTPUT_WIDTH) << "" << std::endl;
        currentPos.Y++;
    }
    // return cursor to correct position
    SetConsoleCursorPosition(hConsole, outputPos);
}
