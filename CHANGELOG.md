# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.4.0] - 2026-04-12
### Changed
- Now you can open save files that are bigger than 512 bytes. Despite that Super Mario 64 save data is only 512 bytes, some emulators generate bigger files with padding zeroes at the end.
- The program now uses SDL3 instead of GLFW for handling the window, and Vulkan / Direct3D 12 instead of OpenGL for rendering. This has the following advantages:
  - Rendering performance has improved.
  - It uses less memory.
  - Now, when opening a file, the program uses the operating system's native open file dialog, which has more features, like accessing favorite folders, copying and pasting paths, etc.

  But the following disadvantages:
  - The size of the executable has increased.
  - It might require newer hardware than the previous versions.
  - Removed the window transparency setting for now, as it's currently not compatible with SDL / Vulkan / Direct3D 12.

- The program now supports higher DPI, and it will scale all its UI accordingly. The program will also resize in real-time when moving the window between monitors with different DPI settings.
- It uses a new font that will look better when it's scaled up.
- Now the program will show a progress bar in the taskbar that represents the percentage of Stars obtained in the current save slot (This might depend on the operating system and/or desktop environment).
- The program now saves a list of the 5 more recent opened files, for easier access.
- Other small improvements to improve code maintainability.

### Fixed
- Fixed not being able to open files that were located in paths with special characters.

## [1.3.1] - 2025-08-01
### Fixed
- Corrections to some stars descriptions.
- Don't treat missing config file as error.

## [1.3.0] - 2025-01-15
### Added
- Show tooltips with the name of each star
### Fixed
- Optimized OpenGL renderer.

## [1.2.2] - 2025-01-06
### Changed
- Disable Save menu item when there's no save file loaded.
### Fixed
- Now the initial window position is properly set when booting the program, avoiding the window appearing at a different position for an instance before finally moving to the center of the screen.
- Rendering performance improvements.

## [1.2.1] - 2024-12-30
### Changed
- Changed config format from JSON to INI.
- Added used libraries in the About window and updated hyperlinks code, since now ImGui supports it natively.
### Fixed
- Fixed some potential crash issues.

## [1.2.0] - 2023-07-23
### Added
- Be able to see the total star count of each save slot.
- Added the star symbol of the game replacing asterisks.

## [1.1.0] - 2023-07-23
### Added
- Implemented configurable window opacity.

### Fixed
- Prevent exceptions if any config values are missing.
- Save settings when closing the program.
- Settings tab is now read only like the others when "Show backup data" is enabled.

## [1.0.0] - 2023-07-23
- Initial release.