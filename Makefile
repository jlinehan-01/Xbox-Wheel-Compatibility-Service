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

.PHONY: all build clean format run release
all: build

build:
	cmake -B build -S . -G "Visual Studio 17 2022" -A x64
	cmake --build build --config Debug

clean:
	del /Q /S build > nul
	rmdir /Q /S build

format:
	clang-format -style=file -i src/*.cpp src/*.h

run:
	.\build\bin\Debug\XboxWheelCompatibilityService.exe

release:
	cmake --build build --config Release
