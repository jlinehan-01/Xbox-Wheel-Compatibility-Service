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
 * main.cpp                                                                   *
 *                                                                            *
 * The entry point of the program                                             *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#include <atomic>
#include <iostream>
#include <windows.h>

#include "output_manager.h"
#include "wheel_manager.h"

static const DWORD SLEEP_DURATION_MS = 100;

static WheelManager *g_wheelManager = nullptr;
static std::atomic<bool> g_shutdownComplete{false};

BOOL WINAPI controlHandler(DWORD signal);

int main(int argc, char **argv)
{
    bool telemetry = false;
    // parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-t")
        {
            telemetry = true;
        }
        else
        {
            // print help message
            std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl
                      << std::endl
                      << "Options:" << std::endl
                      << "-h Show this help message and exit" << std::endl
                      << "-t Print wheel input data to console" << std::endl;
            if (arg == "-h")
            {
                return EXIT_SUCCESS;
            }
            else
            {
                return EXIT_FAILURE;
            }
        };
    }

    OutputManager &outputManager = OutputManager::getInstance();
    outputManager.log("Initialising...");

    try
    {
        init_apartment();
    }
    catch (const hresult_error &ex)
    {
        outputManager.error(to_string(ex.message()));
        uninit_apartment();
        return EXIT_FAILURE;
    }

    WheelManager wheelManager;
    g_wheelManager = &wheelManager;

    // set control handler
    if (!SetConsoleCtrlHandler(controlHandler, TRUE))
    {
        outputManager.error("unable to set control handler");
    }

    outputManager.log("Done");
    wheelManager.start();
    if (telemetry)
    {
        wheelManager.startTelemetry();
    }

    MSG msg;
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    while (wheelManager.running())
    {
        DWORD numEvents;
        if (GetNumberOfConsoleInputEvents(hConsole, &numEvents) &&
            numEvents > 0)
        {
            INPUT_RECORD inputRecord;
            DWORD numRead;
            if (ReadConsoleInput(hConsole, &inputRecord, 1, &numRead))
            {
                if (inputRecord.EventType == KEY_EVENT &&
                    inputRecord.Event.KeyEvent.bKeyDown &&
                    inputRecord.Event.KeyEvent.wVirtualKeyCode == 'T')
                {
                    wheelManager.toggleTelemetry();
                }
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_DURATION_MS));
    }

    SetConsoleMode(hConsole, mode);

    // wait for shutdown to complete
    while (!g_shutdownComplete.load())
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_DURATION_MS));
    }

    g_wheelManager = nullptr;
    uninit_apartment();

    return EXIT_SUCCESS;
}

// stops wheel manager on program exit
BOOL WINAPI controlHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        OutputManager::getInstance().log("Shutting down...");
        if (g_wheelManager)
        {
            g_wheelManager->stop();
            g_shutdownComplete.store(true);
        }
        return TRUE;
    }
    return FALSE;
}
