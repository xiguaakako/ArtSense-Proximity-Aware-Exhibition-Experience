# ArtSense Peripheral Application - STM32 Firmware

## Overview

This project contains the firmware designed for the STM32 microcontroller, which acts as a crucial peripheral device within the ArtSense proximity-aware exhibition experience. The STM32 board leverages Bluetooth Low Energy (BLE) to provide key functionalities for the system.

**Key Responsibilities:**

* **BLE Broadcasting for Proximity Detection:** The STM32 constantly broadcasts a BLE signal. The strength of this signal (RSSI) is measured by the user's Android phone to estimate the distance between the user and the artwork. This enables the automatic triggering of the audio guide feature as users approach exhibits.
* **Dynamic Artwork Naming via BLE Service:** This firmware implements a BLE service that allows curators to remotely update the displayed name of the artwork associated with the specific STM32 device. This eliminates the need to re-flash the firmware whenever artwork information changes.
* **Rock Detection for Security Alerts:**  An integrated accelerometer monitors for movement. If the device is disturbed, the firmware changes the broadcasted BLE message to alert curators that the artwork may be in danger.

## Getting Started

This project is developed using the STM32CubeMX and is intended to be built and flashed using the CubeIDE.

**Prerequisites:**

* STM32CubeIDE installed on your development machine.
* Familiarity with STM32 microcontroller programming and the HAL library.

**Steps to Launch:**

1. Open the project in STM32CubeIDE.
2. Build the project.
3. Flash the generated binary onto your target STM32 development board.

## Customization

The firmware offers several customization options through preprocessor macros.

### Enabling Debugging

To enable debugging features (such as `printf` statements for logging and monitoring), uncomment or add the `BLE_DEBUG` macro definition within the `BlueNRG_MS/Target/bluenrg_conf.h` file.

```c
// BlueNRG_MS/Target/bluenrg_conf.h

// Uncomment the following line to enable debugging
//#define BLE_DEBUG
```

### Configuring Device Name and Rock Sensitivity

The default device name (representing the initial artwork) and the sensitivity of the rock detection can be configured within the `BlueNRG_MS/App/global.h` file.

**Device Name:**

Modify the `DEVICE_NAME` macro to set the desired default name of the BLE device. This name is typically associated with the artwork the STM32 is placed near.

```c
// BlueNRG_MS/App/global.h
// Example: Set the default artwork name
# define DEVICE_NAME "Guernica         " 
//                   "00000000000000000"
```

**Rock Sensitivity:**

Adjust the `SENSITIVITY` macro to control how sensitive the accelerometer is to detect rocking. A lower value will make the detection more sensitive, while a higher value will require more significant movement to trigger an alert.

```c
// BlueNRG_MS/App/global.h

#define SENSITIVITY 10 // Adjust the sensitivity of the rock detection
```

**Important Notes:**

* The `DEVICE_NAME` is the initial name broadcasted by the BLE device. This can be updated dynamically via the BLE service.
* Experiment with the `SENSITIVITY` value to find the optimal setting for your specific installation and to minimize false positives.

## Directory Structure (Relevant for Customization)

* **BlueNRG_MS/Target/bluenrg_conf.h:** Contains BlueNRG configuration settings, including the `BLE_DEBUG` macro.
* **BlueNRG_MS/App/global.h:**  Defines global macros like `DEVICE_NAME` and `SENSITIVITY`.

## Further Information

For a comprehensive understanding of the ArtSense project, including the overall system architecture, techniques leveraged, and work distribution, please refer to the project report.
