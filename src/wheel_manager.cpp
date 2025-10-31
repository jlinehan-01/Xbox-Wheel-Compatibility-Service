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
 * wheel_manager.cpp                                                          *
 *                                                                            *
 * Handles searching for and managing racing wheel devices                    *
 *                                                                            *
 * Author: Joshua Linehan                                                     *
 ******************************************************************************/

#include "wheel_manager.h"

const DWORD WheelManager::REFRESH_DELAY_MS = 1000;
const int WheelManager::MAX_WHEELS = 8;
const int WheelManager::WHEEL_NOT_FOUND = -1;

WheelManager::WheelManager() : active{false}, wheels{} {}

WheelManager::~WheelManager()
{
    // stop thread
    stop();
}

// starts thread scanning for wheels
void WheelManager::start()
{
    active.store(true);
    std::cout << "Scanning for wheels..." << std::endl;
    std::thread thread(
        [this]
        {
            while (active.load())
            {
                // get wheels
                auto racingWheels = RacingWheel::RacingWheels();
                // compare to each recorded wheels
                int wheelMap[MAX_WHEELS] = {WHEEL_NOT_FOUND, WHEEL_NOT_FOUND,
                                            WHEEL_NOT_FOUND, WHEEL_NOT_FOUND,
                                            WHEEL_NOT_FOUND, WHEEL_NOT_FOUND,
                                            WHEEL_NOT_FOUND, WHEEL_NOT_FOUND};
                int numWheels = wheels.size();
                for (int i = 0; i < racingWheels.Size(); i++)
                {
                    bool wheelFound = false;
                    for (int j = 0; j < numWheels; j++)
                    {
                        if (wheels[j].getRacingWheel() == racingWheels.GetAt(i))
                        {
                            wheelFound = true;
                            wheelMap[j] = i;
                            break;
                        }
                    }
                    // handle new wheels
                    if (!wheelFound)
                    {
                        wheels.push_back(Wheel(racingWheels.GetAt(i)));
                        std::cout << "Wheel connected" << std::endl;
                    }
                }
                // handle disconnected wheels
                for (int i = numWheels - 1; i >= 0; i--)
                {
                    if (wheelMap[i] == WHEEL_NOT_FOUND)
                    {
                        wheels.erase(wheels.begin() + i);
                        std::cout << "Wheel disconnected" << std::endl;
                    }
                }
                // sleep until next scan
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(REFRESH_DELAY_MS));
            }
        });
    thread.join();
}

// sets flag to stop thread
void WheelManager::stop()
{
    active.store(false);
}
