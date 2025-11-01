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
 * wheel.cpp                                                                  *
 *                                                                            *
 * Wrapper for wheel functionality                                            *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#include "wheel.h"

const DWORD Wheel::REFRESH_DELAY_MS = 1;

Wheel::Wheel(RacingWheel racingWheel) : racingWheel(racingWheel), active{false}
{
}

Wheel::~Wheel()
{
    stop();
}

// reads and injects input from wheel
void Wheel::run()
{
    while (active.load())
    {
        // sleep until next scan
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_DELAY_MS));
    }
}

// returns the racingWheel associated with a wheel object
RacingWheel Wheel::getRacingWheel()
{
    return this->racingWheel;
};

// starts thread scanning for wheels
void Wheel::start()
{
    // prevent re-running thread if already started
    if (active.load())
    {
        return;
    }
    active.store(true);
    std::cout << "Wheel connected" << std::endl;
    thread = std::thread(&Wheel::run, this);
}
// sets flag to stop thread
void Wheel::stop()
{
    if (active.load())
    {
        std::cout << "Wheel disconnected" << std::endl;
        active.store(false);
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

// returns if the wheel manager is running
bool Wheel::running()
{
    return active.load();
}
