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

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>

using namespace winrt;
using namespace Windows::Gaming::Input;

class Wheel
{
  private:
    RacingWheel racingWheel;

  public:
    Wheel(RacingWheel racingWheel);
    // returns the racingWheel associated with a wheel object
    RacingWheel getRacingWheel();
};

#endif
