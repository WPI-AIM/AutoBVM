# [1.0.0] - 2021-09-14

### Added

- Replace HOLD_IN_DURATION with plateau time by user
- Calculate and add Respiration Rate, I:E, PIP, PEEP Plateau to the measurement panel
- Add Flow as a graph
- Add readouts in the graph panel(top left)
- Set div on graph to 2 sec, for a 20 sec display.
- Homing instruction document.

### Changed

- Tidal volume changed to set tidal volume during inspiration.

### Fixed

- Fix waveform calculations
- UI lockup when about button is open and a user clicks outside the box
- Fix actuator pause for ~7 seconds when a user changes resp. parameters during a profile
- If there an actuator fault, there is no activity for one cycle of start/standby. Block Start, until homing completes.
- `Settings and Config` toggel to get into and out of settings mode.
  
# [0.0.3] - 2021-07-29

### Added

- Added code to allow readouts to be created anywhere.
- Added a bit of padding.
- Add version string to parser init.

### Changed
  
- Changed Tidal Volume step size from 50 to 20.
  
# [0.0.2] - 2021-07-28

### Added

- J-Link install steps to README.
 
# [0.0.1] - 2021-07-27

Initial release.

### Added
   
### Changed
 
### Fixed
