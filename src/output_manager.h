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
 * output_manager.h                                                           *
 *                                                                            *
 * Singleton class to handle text output                                      *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H

#include <iomanip>
#include <iostream>
#include <mutex>
#include <vector>
#include <windows.h>

class OutputManager
{
  private:
    static const int OUTPUT_WIDTH;
    static OutputManager instance;
    std::mutex outputMutex;
    COORD outputPos;
    COORD maxPos;
    HANDLE hConsole;
    // returns the current cursor position
    static COORD getCursorPos(HANDLE hConsole);
    OutputManager();
    ~OutputManager();
    // prints a message to the screen
    void print(std::ostream &stream, std::string message);

  public:
    // returns the singleton instance
    static OutputManager &getInstance();
    OutputManager &operator=(const OutputManager &) = delete;
    OutputManager(const OutputManager &) = delete;
    // prints a message to the screen
    void log(std::string message);
    // logs an error
    void error(std::string message);
    // prints telemetry data to the screen
    void printTelemetry(std::vector<std::string> lines);
    // clears telemetry output from screen
    void clearTelemetry();
};

#endif
