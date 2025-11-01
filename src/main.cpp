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

#include "wheel_manager.h"

static const DWORD SLEEP_DURATION_MS = 100;

static WheelManager *g_wheelManager = nullptr;
static std::atomic<bool> g_shutdownComplete{false};

BOOL WINAPI controlHandler(DWORD signal);

int main(int argc, char **argv)
{
    std::cout << "Initialising..." << std::endl;
    WheelManager wheelManager;
    g_wheelManager = &wheelManager;

    if (!SetConsoleCtrlHandler(controlHandler, TRUE))
    {
        std::cerr << "unable to set control handler" << std::endl;
    }
    std::cout << "Done" << std::endl;

    wheelManager.start();

    // keep program running while other threads run
    while (wheelManager.running())
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_DURATION_MS));
    }

    while (!g_shutdownComplete.load())
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_DURATION_MS));
    }

    g_wheelManager = nullptr;

    return EXIT_SUCCESS;
}

BOOL WINAPI controlHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        std::cout << "Shutting down..." << std::endl;
        if (g_wheelManager)
        {
            g_wheelManager->stop();
            g_shutdownComplete.store(true);
        }
        return TRUE;
    }
    return FALSE;
}
