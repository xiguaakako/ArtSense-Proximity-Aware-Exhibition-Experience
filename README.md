# ArtSense: Proximity-Aware Exhibition Experience

[![GitHub Stars](https://img.shields.io/github/stars/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience?style=social)](https://github.com/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience/stargazers)
[![GitHub Issues](https://img.shields.io/github/issues/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience)](https://github.com/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience/issues)

ArtSense is an innovative project designed to enhance the museum and gallery experience through proximity-aware technology. By leveraging Bluetooth Low Energy (BLE), ArtSense provides visitors with contextually relevant information about artworks as they explore exhibitions, and offers curators powerful tools for managing and securing their collections.

This repository contains the complete source code and documentation for the ArtSense project, encompassing both the Android application for visitors and curators, and the firmware for the STM32 microcontroller-based beacons.

## Project Overview

ArtSense aims to bridge the gap between visitors and artworks by providing an engaging and informative experience. It also empowers curators with tools for efficient management and enhanced security.

**Target Users:**

* **Visitors:** Art enthusiasts and gallery visitors seeking a more interactive and informative experience.
* **Curators:** Museum and gallery staff responsible for managing artwork information and ensuring the security of their collections.

**Key Features:**

**For Visitors (Android Application):**

* **Proximity-Based Information:** Automatically detects nearby artworks using BLE beacons and displays relevant information.
* **Detailed Artwork Information:** Provides artist name, title, year of creation, and a comprehensive description for each artwork.
* **Self-Guided Tours:** Enables visitors to embark on personalized tours, receiving information seamlessly as they move through the exhibition.

**For Curators (Android Application & STM32 Firmware):**

* **Real-time Content Management:** Allows curators to easily update artwork information, which is instantly reflected in the visitor application.
* **Remote Beacon Management:** Enables curators to associate physical BLE beacons with specific artworks through the curator application.
* **Centralized Beacon Control (STM32):** Utilizes an STM32 microcontroller board to manage and configure BLE beacons within the exhibition space.
* **Environmental Monitoring and Alerts (STM32 & Android):** Receives accelerometer data from STM32-powered beacons and alerts curators (and potentially informs visitors) of potential risks (e.g., movement or tampering) to specific artworks.

**Technology Stack:**

* **Android Application:**
    * Kotlin for application logic
    * Android SDK for UI development and BLE interaction
* **STM32 Firmware:**
    * C for microcontroller programming
    * STM32 HAL library for hardware abstraction
    * BLE stack for Bluetooth Low Energy communication
* **Communication:** Bluetooth Low Energy (BLE)

## Architecture

The ArtSense system comprises the following key components:

1. **ArtSense Visitor Application (Android):** Scans for BLE beacons, retrieves artwork information based on detected beacons, and displays it to the visitor.
2. **ArtSense Curator Application (Android):** Allows curators to manage artwork information and associate artworks with specific BLE beacons.
3. **STM32 Microcontroller Firmware:** Runs on the STM32 boards acting as BLE beacons. It broadcasts beacon signals, monitors for movement using an accelerometer, and can receive commands from the curator application.
4. **BLE Beacons (STM32 based):** Strategically placed near artworks, these devices broadcast unique identifiers and monitor for potential tampering.

## Getting Started

To set up and run the ArtSense project, follow these steps:

**1. Clone the Repository:**

```bash
git clone https://github.com/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience.git
cd ArtSense-Proximity-Aware-Exhibition-Experience
```

**2. Set up the Android Applications:**

   * **Visitor Application:**
      * Navigate to the `android-app` directory.
      * Open the project in Android Studio.
      * Build the project (`Build` > `Make Project`).
      * Run the application on an Android device or emulator (`Run` > `Run 'app'`). Refer to the [Android Application README](android-app/README.md) for more detailed instructions and troubleshooting.

   * **Curator Application:** (If applicable - may be integrated into the same Android project or a separate one. Refer to specific branch or documentation)
      * Follow similar steps as the Visitor Application setup, navigating to the relevant curator app directory (if separate).

**3. Set up the STM32 Firmware:**

   * Navigate to the `ARTSENSE-STM32` directory (or the directory containing the STM32 project).
   * Open the project in STM32CubeIDE.
   * Configure the project for your specific STM32 board.
   * Build the project.
   * Flash the firmware onto your STM32 development board. Refer to the [STM32 Firmware README](ARTSENSE-STM32/README.md) for detailed instructions on customization, debugging, and flashing.

**4. Configure the System:**

   * **Deploy STM32 Beacons:** Place the flashed STM32 boards (acting as BLE beacons) near the artworks in your exhibition space.
   * **Associate Beacons with Artworks:** Use the Curator Android Application to connect to the STM32 beacons and associate their unique identifiers with the corresponding artwork information in your database (or within the application's data).

## Customization

**Android Applications:**

Refer to the individual README files within the Android application directories (`android-app/README.md`) for details on:

* UI customization
* Data management and integration
* BLE scanning parameters
* Adding new features

**STM32 Firmware:**

Refer to the [STM32 Firmware README](ARTSENSE-STM32/README.md) for details on customizing:

* **Device Name (Artwork Association):** Configure the default name broadcast by the beacon.
* **Rock Sensitivity:** Adjust the accelerometer sensitivity for detecting movement.
* **BLE Advertising Parameters:** Modify advertising intervals and other BLE settings.
* **Debugging Options:** Enable debugging output for development.

## Project Structure

```
ArtSense-Proximity-Aware-Exhibition-Experience/
├── android-app/          # Source code for the Android Visitor Application
│   ├── app/
│   ├── ...
│   └── README.md
├── ARTSENSE-STM32/     # Source code for the STM32 Microcontroller Firmware
│   ├── BlueNRG_MS/
│   ├── ...
│   └── README.md
├── README.md            # This file (Project-level README)
└── ...
```

## Contact

For questions, bug reports, or feature requests, please open an issue on our [GitHub repository](https://github.com/xiguaakako/ArtSense-Proximity-Aware-Exhibition-Experience/issues).

