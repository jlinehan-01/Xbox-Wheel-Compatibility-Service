# Xbox Wheel Compatibility Service

This project allows Xbox One racing wheels to be used with Windows. This project was inspired by https://github.com/cmumme/XboxWheelCompatibility, however uses C++ instead of C# to avoid issues with .NET which caused the installer to fail, and to provide a more lightweight experience.

Multiple wheel inputs should be supported, however this feature has not been tested.

## Table of Contents
- [1 - Usage](#1---usage)
  - [1.1 - Options](#11---options)


## 1 - Usage

WheelCompatibilityService.exe \[OPTIONS\]

### 1.1 - Options
| Option | Name      | Description              |
|--------|-----------|--------------------------|
| -h     | Help      | Displays usage help      |
| -t     | Telemetry | Prints output from wheel |
