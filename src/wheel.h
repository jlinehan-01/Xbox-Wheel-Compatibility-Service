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
 * wheel.h                                                                    *
 *                                                                            *
 * Wrapper for wheel functionality                                            *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#ifndef WHEEL_H
#define WHEEL_H

#include <atomic>
#include <iostream>
#include <thread>
#include <windows.h>
#include <winrt/Windows.Gaming.Input.h>

using namespace winrt;
using namespace Windows::Gaming::Input;

class Wheel
{
  private:
    static const DWORD REFRESH_DELAY_MS;

    RacingWheel racingWheel;
    std::atomic<bool> active;
    std::thread thread;

    // reads and injects input from wheel
    void run();

  public:
    Wheel(RacingWheel racingWheel);
    ~Wheel();
    // returns the racingWheel associated with a wheel object
    RacingWheel getRacingWheel();
    // parses button input from a RacingWheel into a usable format for gamepads
    static std::string parseButtons(RacingWheelButtons wheelButtons);
    // starts thread scanning for wheels
    void start();
    // sets flag to stop thread
    void stop();
    // returns if the wheel manager is running
    bool running();
};

#endif
