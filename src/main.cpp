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
static HHOOK g_keyboardHook = nullptr;

BOOL WINAPI controlHandler(DWORD signal);
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

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
    if (telemetry)
    {
        wheelManager.startTelemetry();
    }

    // set control handler
    if (!SetConsoleCtrlHandler(controlHandler, TRUE))
    {
        outputManager.error("unable to set control handler");
    }

    // Install keyboard hook
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc,
                                      GetModuleHandle(nullptr), 0);
    if (!g_keyboardHook)
    {
        outputManager.error("Failed to install keyboard hook");
    }

    wheelManager.start();
    outputManager.log("Done");

    MSG msg;
    while (wheelManager.running())
    {
        // Process all pending messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_DURATION_MS));
    }

    // Cleanup hook
    if (g_keyboardHook)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

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

// handles keyboard input
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        if (kbd->vkCode == 'T')
        {
            if (g_wheelManager)
            {
                g_wheelManager->toggleTelemetry();
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}
