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
#include <winrt/Windows.UI.Input.Preview.Injection.h>

using namespace winrt;
using namespace Windows::Gaming::Input;
using namespace Windows::UI::Input::Preview::Injection;

class Wheel
{
  private:
    static const DWORD REFRESH_DELAY_MS;
    static const double NO_INPUT;
    static const DWORD INJECTOR_INIT_DELAY_MS;

    RacingWheel racingWheel;
    std::atomic<bool> active;
    std::thread thread;
    InputInjector injector;
    uint64_t packetNumber;
    GamepadReading output;

    // reads and injects input from wheel
    void run();

  public:
    Wheel(RacingWheel racingWheel);
    ~Wheel();
    // returns the racingWheel associated with a wheel object
    RacingWheel getRacingWheel();
    // returns the most recent output of a wheel object
    GamepadReading getOutput();
    // starts thread scanning for wheels
    void start();
    // sets flag to stop thread
    void stop();
    // returns if the wheel manager is running
    bool running();
};

#endif
