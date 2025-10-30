# Xbox Wheel Compatibility Service
# Copyright (C) 2025 Joshua Linehan
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)
project(Xbox-Wheel-Compatibility-Service VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SRC "src/*.cpp")

add_executable(XboxWheelCompatibilityService ${SRC})

target_include_directories(XboxWheelCompatibilityService PRIVATE include)

# link libraries
target_link_libraries(XboxWheelCompatibilityService PRIVATE
    WindowsApp.lib
    RuntimeObject.lib
)

# require administrator privileges
set_target_properties(XboxWheelCompatibilityService PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
