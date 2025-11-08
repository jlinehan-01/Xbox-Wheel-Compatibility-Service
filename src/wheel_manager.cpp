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
const DWORD WheelManager::TELEMETRY_DELAY_MS = 100;
const int WheelManager::MAX_WHEELS = 8;
const int WheelManager::WHEEL_NOT_FOUND = -1;

WheelManager::WheelManager() : active{false}, wheels{}, telemetryActive{false}
{
}

WheelManager::~WheelManager()
{
    // stop thread
    stop();
}

// scans for racing wheels
void WheelManager::run()
{
    while (active.load())
    {
        // get wheels
        auto racingWheels = RacingWheel::RacingWheels();
        // compare to each recorded wheels
        int wheelMap[MAX_WHEELS] = {
            WHEEL_NOT_FOUND, WHEEL_NOT_FOUND, WHEEL_NOT_FOUND, WHEEL_NOT_FOUND,
            WHEEL_NOT_FOUND, WHEEL_NOT_FOUND, WHEEL_NOT_FOUND, WHEEL_NOT_FOUND};
        int numWheels = wheels.size();
        for (int i = 0; i < racingWheels.Size(); i++)
        {
            bool wheelFound = false;
            for (int j = 0; j < numWheels; j++)
            {
                if (wheels[j]->getRacingWheel() == racingWheels.GetAt(i))
                {
                    wheelFound = true;
                    wheelMap[j] = i;
                    break;
                }
            }
            // handle new wheels
            if (!wheelFound)
            {
                wheels.push_back(
                    std::make_unique<Wheel>(racingWheels.GetAt(i)));
                wheels.back()->start();
            }
        }
        // handle disconnected wheels
        for (int i = numWheels - 1; i >= 0; i--)
        {
            if (wheelMap[i] == WHEEL_NOT_FOUND || !wheels[i]->running())
            {
                wheels[i]->stop();
                wheels.erase(wheels.begin() + i);
            }
        }
        // sleep until next scan
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_DELAY_MS));
    }
}

// prints wheel input to console
void WheelManager::telemetry()
{
    OutputManager &outputManager = OutputManager::getInstance();
    std::stringstream ss;
    while (telemetryActive.load())
    {
        std::vector<std::string> output;
        // add newline before telemetry
        output.push_back("");
        // print telemetry for each wheel
        for (int i = 0; i < wheels.size(); i++)
        {
            if (!wheels[i])
            {
                continue;
            }
            if (wheels[i]->running())
            {
                ss << "Wheel " << i + 1;
                output.push_back(ss.str());
                ss.str("");
                GamepadReading reading = wheels[i]->getOutput();
                ss << "Steering: " << std::fixed << std::setw(7)
                   << std::showpoint << std::setprecision(2)
                   << reading.LeftThumbstickX * 100 << "%";
                output.push_back(ss.str());
                ss.str("");
                ss << "Throttle: " << std::fixed << std::setw(7)
                   << std::showpoint << std::setprecision(2)
                   << reading.RightTrigger * 100 << "%";
                output.push_back(ss.str());
                ss.str("");
                ss << "Brake: " << std::fixed << std::setw(10) << std::showpoint
                   << std::setprecision(2) << reading.LeftTrigger * 100 << "%";
                output.push_back(ss.str());
                ss.str("");
                ss << "Buttons: " <<
                    // lambda to parse buttons
                    (
                        [](GamepadButtons buttons)
                        {
                            std::string buttonsStr;
                            // add each button to string
                            if ((buttons & GamepadButtons::DPadUp) ==
                                GamepadButtons::DPadUp)
                            {
                                buttonsStr += "DPadUp, ";
                            }
                            if ((buttons & GamepadButtons::DPadDown) ==
                                GamepadButtons::DPadDown)
                            {
                                buttonsStr += "DPadDown, ";
                            }
                            if ((buttons & GamepadButtons::DPadLeft) ==
                                GamepadButtons::DPadLeft)
                            {
                                buttonsStr += "DPadLeft, ";
                            }
                            if ((buttons & GamepadButtons::DPadRight) ==
                                GamepadButtons::DPadRight)
                            {
                                buttonsStr += "DPadRight, ";
                            }
                            if ((buttons & GamepadButtons::LeftShoulder) ==
                                GamepadButtons::LeftShoulder)
                            {
                                buttonsStr += "LB, ";
                            }
                            if ((buttons & GamepadButtons::RightShoulder) ==
                                GamepadButtons::RightShoulder)
                            {
                                buttonsStr += "RB, ";
                            }
                            if ((buttons & GamepadButtons::Menu) ==
                                GamepadButtons::Menu)
                            {
                                buttonsStr += "Menu, ";
                            }
                            if ((buttons & GamepadButtons::View) ==
                                GamepadButtons::View)
                            {
                                buttonsStr += "View, ";
                            }
                            if ((buttons & GamepadButtons::A) ==
                                GamepadButtons::A)
                            {
                                buttonsStr += "A, ";
                            }
                            if ((buttons & GamepadButtons::B) ==
                                GamepadButtons::B)
                            {
                                buttonsStr += "B, ";
                            }
                            if ((buttons & GamepadButtons::X) ==
                                GamepadButtons::X)
                            {
                                buttonsStr += "X, ";
                            }
                            if ((buttons & GamepadButtons::Y) ==
                                GamepadButtons::Y)
                            {
                                buttonsStr += "Y, ";
                            }

                            // remove last comma
                            if (!buttonsStr.empty())
                            {
                                buttonsStr.pop_back();
                                buttonsStr.pop_back();
                            }
                            return buttonsStr;
                        })(reading.Buttons);
                output.push_back(ss.str());
                ss.str("");
                // add new line after each wheel
                output.push_back("");
            }
        }
        outputManager.printTelemetry(output);

        // sleep until next reading
        std::this_thread::sleep_for(
            std::chrono::milliseconds(TELEMETRY_DELAY_MS));
    }
    outputManager.clearTelemetry();
}

// starts thread scanning for wheels
void WheelManager::start()
{
    // prevent running if already active
    if (active.load())
    {
        return;
    }

    active.store(true);
    OutputManager::getInstance().log("Scanning for wheels...");
    thread = std::thread(&WheelManager::run, this);
}

// sets flag to stop thread
void WheelManager::stop()
{
    stopTelemetry();
    if (active.load())
    {
        active.store(false);
        for (int i = 0; i < wheels.size(); i++)
        {
            if (wheels[i]->running())
            {
                wheels[i]->stop();
            }
        }
        if (thread.joinable())
        {
            thread.join();
        }
        wheels.clear();
    }
}

// returns if the wheel manager is running
bool WheelManager::running()
{
    return active.load();
}

// starts telemetry thread
void WheelManager::startTelemetry()
{
    if (telemetryActive.load())
    {
        return;
    }
    telemetryActive.store(true);
    telemetryThread = std::thread(&WheelManager::telemetry, this);
}

// stops telemetry thread
void WheelManager::stopTelemetry()
{
    telemetryActive.store(false);
    if (telemetryThread.joinable())
    {
        telemetryThread.join();
    }
}

// toggles telemetry thread on/off
void WheelManager::toggleTelemetry()
{
    telemetryActive.load() ? stopTelemetry() : startTelemetry();
}
