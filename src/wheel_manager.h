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
 * wheel_manager.h                                                            *
 *                                                                            *
 * Handles searching for and managing racing wheel devices                    *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#ifndef WHEEL_MANAGER_H
#define WHEEL_MANAGER_H

#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>

#include "output_manager.h"
#include "wheel.h"

using namespace winrt;
using namespace Windows::Gaming::Input;

class WheelManager
{
  private:
    static const DWORD REFRESH_DELAY_MS;
    static const DWORD TELEMETRY_DELAY_MS;
    static const int MAX_WHEELS;
    static const int WHEEL_NOT_FOUND;

    std::atomic<bool> active;
    std::vector<std::unique_ptr<Wheel>> wheels;
    std::thread thread;
    std::thread telemetryThread;
    std::atomic<bool> telemetryActive;

    // scans for racing wheels
    void run();
    // prints wheel input to console
    void telemetry();

  public:
    WheelManager();
    ~WheelManager();
    // starts thread scanning for wheels
    void start();
    // sets flag to stop thread
    void stop();
    // returns if the wheel manager is running
    bool running();
    // starts telemetry thread
    void startTelemetry();
    // stops telemetry thread
    void stopTelemetry();
    // toggles telemetry thread on/off
    void toggleTelemetry();
};

#endif
