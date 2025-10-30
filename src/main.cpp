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

#include <iostream>
#include <windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>

using namespace winrt;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation::Collections;

int main(int argc, char **argv)
{
    std::cout << "Initialising..." << std::endl;
    std::cout << "Done" << std::endl;

    bool wheelFound = false;
    std::cout << "Scanning for wheels..." << std::endl;
    while (!wheelFound)
    {
        auto wheels = RacingWheel::RacingWheels();
        if (wheels.Size() > 0)
        {
            wheelFound = true;
            std::cout << "Wheel Detected" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
