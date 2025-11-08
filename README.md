# Xbox Wheel Compatibility Service

This project allows Xbox One racing wheels to be used with Windows. This project was inspired by https://github.com/cmumme/XboxWheelCompatibility, however uses C++ instead of C# to avoid issues with .NET which caused the installer to fail, and to provide a more lightweight experience.

This project has been tested on Windows 10 and 11 with a Thrustmaster Ferrari 458 wheel.

Multiple wheel inputs should be supported, however this feature has not been tested.

## Table of Contents
- [1 - Usage](#1---usage)
  - [1.1 - Controls](#11---controls)
  - [1.2 - Options](#12---options)
- [2 - Known Issues](#2---known-issues)
  - [2.1 - Crashing](#21---crashing)


## 1 - Usage

XboxWheelCompatibilityService.exe \[OPTIONS\]

### 1.1 - Controls

Press T to toggle telemetry on/off

### 1.2 - Options

| Option | Name      | Description                          |
|--------|-----------|--------------------------------------|
| -h     | Help      | Displays usage help                  |
| -t     | Telemetry | Starts program with telemetry active |

## 2 - Known Issues

### 2.1 - Crashing

The program sometimes crashes shortly after a wheel is initialised. This is caused by the first few calls to InjectGamepadInput in the [Wheel class](src/wheel.cpp). InitializeGamepadInjection was deliberately not called in the original project, but seems to reduce the frequency of crashing in this manner. I have not been able to catch any errors from InjectGamepadInput in a try/catch block. Re-running the program seems to be an appropriate workaround; following a crash the program has worked successfully within 2-3 attempts.
