# TITAN Hardware

These folders contain the files related to hardware designed to operate TITAN. They were all designed using KiCad 6.0.2.

## Overview

The TITAN hardware is designed to primarily be a HAT (**HA**rdware on **T**op) for the Raspberry Pi 3B+s we use for video. This form factor was chosen to reduce the overall footprint of the system compared to the original 2019 system which used a ribbon cable between the RPi and boards. This also reduces the chance of incorrect connections since the HAT can only be seated one way. At the heart of the HAT, there is an STM32F103C microcontroller which communicates with the RPis and various sensors around TITAN via the various connections present.

These HATs perform 4 main functions:
1. Regulate power for their RPi and monitor from the battery. (Reverse polarity and undervoltage protection built in).
2. Facilitate communication between RPis via an STM32 microcontroller
3. Collect data from TITAN using the STM32 microcontroller
4. Handle telemetry with the chase vechicle

**Functions 2 to 4 are only performed on the "main" (front) unit, where the STM32 is.** The secondary (rear) does not have the STM32 mounted and just uses the power system and a header to connect to the main board for data.

## The Boards

There are three boards/folders/projects present in the folder, representing two revisions of the hardware. 

### TITAN Redux (2020)

Following feedback from the 2019 WHPSC and experience in developing [Blueshift](https://github.com/hpvdt/blueshift), an initial revision of TITAN was made in the fall of 2020, dubbed `titan_redux`. This was the main revision from the 2019 system, moving to the HAT form factor which worked on Blueshift as well as moving to embed the microconroller directly on the board (as opposed to seating a "Blue Pill" development board) and using SMT to reduce the overall size.

This was largely successful and allowed most of the code that went into TITAN in 2022 to be properly tested. There were only two notable issues with it:
1. The RX and TX lines for the GPS were not properly crossed over to the STM32.
2. The 5V regulator was removed for undocumented reasons, likely due to a failure.

### TITAN 2022

The more straightforwardly named `titan_2022` is a revision of the Redux board. It addresses the issues it had as well as adding support for new features while maintaining the same RPi HAT form. A brief of the changes:

- Fixed UART lines
- Rearranged board for improved button access
- Rearranged sensors to simplify routing
- Changed to a different 5V regulator (largely due to supply issues)
- Moved DHT sensor and encoder comparator off-board
- Added headers for monitoring CO2 and brake temperature

### TITAN Wheel (2022)

As part of the revision process from redux to the 2022 system a daughter board to collect wheel data was prepared called `titan_wheel`. This board houses the brake temperature sensor as well as the optical encoder and its conditioning circuitry. It was designed to fit within the wheel structure close to the brake disk.
