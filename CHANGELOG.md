# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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