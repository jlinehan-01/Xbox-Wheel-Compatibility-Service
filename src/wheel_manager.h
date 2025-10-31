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
#include <thread>
#include <windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>

#include "wheel.h"

using namespace winrt;
using namespace Windows::Gaming::Input;

class WheelManager
{
  private:
    static const DWORD REFRESH_DELAY_MS;
    static const int MAX_WHEELS;
    static const int WHEEL_NOT_FOUND;

    std::atomic<bool> active;
    std::vector<Wheel> wheels;

  public:
    WheelManager();
    ~WheelManager();
    // starts thread scanning for wheels
    void start();
    // sets flag to stop thread
    void stop();
};

#endif
