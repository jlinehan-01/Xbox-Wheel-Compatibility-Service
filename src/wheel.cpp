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
const double Wheel::NO_INPUT = 0.0;
const DWORD Wheel::INJECTOR_INIT_DELAY_MS = 500;

Wheel::Wheel(RacingWheel racingWheel)
    : racingWheel(racingWheel), active{false}, injector{nullptr},
      packetNumber{0}
{
}

Wheel::~Wheel()
{
    stop();
}

// reads and injects input from wheel
void Wheel::run()
{
    std::cout << "Wheel active" << std::endl;
    while (active.load())
    {
        if (injector && racingWheel)
        {
            try
            {
                RacingWheelReading reading = racingWheel.GetCurrentReading();

                // map buttons
                GamepadButtons buttons = GamepadButtons::None;
                if ((reading.Buttons & RacingWheelButtons::DPadDown) ==
                    RacingWheelButtons::DPadDown)
                {
                    buttons |= GamepadButtons::DPadDown;
                }
                if ((reading.Buttons & RacingWheelButtons::DPadUp) ==
                    RacingWheelButtons::DPadUp)
                {
                    buttons |= GamepadButtons::DPadUp;
                }
                if ((reading.Buttons & RacingWheelButtons::DPadLeft) ==
                    RacingWheelButtons::DPadLeft)
                {
                    buttons |= GamepadButtons::DPadLeft;
                }
                if ((reading.Buttons & RacingWheelButtons::DPadRight) ==
                    RacingWheelButtons::DPadRight)
                {
                    buttons |= GamepadButtons::DPadRight;
                }
                if ((reading.Buttons & RacingWheelButtons::NextGear) ==
                    RacingWheelButtons::NextGear)
                {
                    buttons |= GamepadButtons::RightShoulder;
                }
                if ((reading.Buttons & RacingWheelButtons::PreviousGear) ==
                    RacingWheelButtons::PreviousGear)
                {
                    buttons |= GamepadButtons::LeftShoulder;
                }
                if ((reading.Buttons & RacingWheelButtons::Button1) ==
                    RacingWheelButtons::Button1)
                {
                    buttons |= GamepadButtons::Menu;
                }
                if ((reading.Buttons & RacingWheelButtons::Button2) ==
                    RacingWheelButtons::Button2)
                {
                    buttons |= GamepadButtons::View;
                }
                if ((reading.Buttons & RacingWheelButtons::Button3) ==
                    RacingWheelButtons::Button3)
                {
                    buttons |= GamepadButtons::A;
                }
                if ((reading.Buttons & RacingWheelButtons::Button4) ==
                    RacingWheelButtons::Button4)
                {
                    buttons |= GamepadButtons::B;
                }
                if ((reading.Buttons & RacingWheelButtons::Button5) ==
                    RacingWheelButtons::Button5)
                {
                    buttons |= GamepadButtons::X;
                }
                if ((reading.Buttons & RacingWheelButtons::Button6) ==
                    RacingWheelButtons::Button6)
                {
                    buttons |= GamepadButtons::Y;
                }

                // compile output
                GamepadReading newOutput;
                newOutput.Timestamp = packetNumber++;
                newOutput.Buttons = buttons;
                newOutput.LeftTrigger = reading.Brake;
                newOutput.RightTrigger = reading.Throttle;
                newOutput.LeftThumbstickX = reading.Wheel;
                newOutput.LeftThumbstickY = NO_INPUT;
                newOutput.RightThumbstickX = NO_INPUT;
                newOutput.RightThumbstickY = NO_INPUT;

                // inject output
                this->output = newOutput;
                InjectedInputGamepadInfo gamepadInfo(newOutput);
                injector.InjectGamepadInput(gamepadInfo);
            }
            catch (const hresult_error &ex)
            {
                std::cerr << "Injection error: " << to_string(ex.message())
                          << '\n';
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(INJECTOR_INIT_DELAY_MS));
                continue;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                break;
            }
            catch (...)
            {
                std::cerr << "Unknown error while injecting input" << '\n';
                break;
            }
        }
        else
        {
            break;
        }

        // sleep until next scan
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_DELAY_MS));
    }
}

// returns the racingWheel associated with a wheel object
RacingWheel Wheel::getRacingWheel()
{
    return this->racingWheel;
}

// returns the most recent output of a wheel object
GamepadReading Wheel::getOutput()
{
    return this->output;
}

// starts thread scanning for wheels
void Wheel::start()
{
    // prevent re-running thread if already started
    if (active.load())
    {
        return;
    }
    std::cout << "Wheel connected" << std::endl;

    // initialise wheel
    std::cout << "Initialising wheel..." << std::endl;
    try
    {
        injector = InputInjector::TryCreate();
        if (!injector)
        {
            std::cerr << "Failed to create injector" << std::endl;
            stop();
            return;
        }
        // give injector time to stabilise
        std::this_thread::sleep_for(
            std::chrono::milliseconds(INJECTOR_INIT_DELAY_MS));
        injector.InitializeGamepadInjection();
    }
    catch (const hresult_error &ex)
    {
        std::cerr << "Failed to initialise injector: "
                  << to_string(ex.message()) << '\n';
        std::this_thread::sleep_for(
            std::chrono::milliseconds(INJECTOR_INIT_DELAY_MS));
        return;
    }
    // run wheel
    active.store(true);
    thread = std::thread(&Wheel::run, this);
}

// sets flag to stop thread
void Wheel::stop()
{
    bool expected = true;
    if (active.compare_exchange_strong(expected, false))
    {
        std::cout << "Wheel disconnected" << std::endl;
        if (thread.joinable())
        {
            thread.join();
        }
        if (injector)
        {
            injector.UninitializeGamepadInjection();
        }
        injector = nullptr;
    }
}

// returns if the wheel manager is running
bool Wheel::running()
{
    return active.load();
}
