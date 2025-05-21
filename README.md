# Multisensory Measurement System (STM32 + Qt)

## Project Overview

This project focuses on the **integration, real-time data acquisition, and visualization** of various low-resolution sensors using a custom Qt-based application and an STM32-based data collection module. It extends an existing engineering setup by integrating multiple types of sensors and combining their measurements into a single synchronized system.

The application was developed as part of an **engineering thesis at Wrocław University of Science and Technology** and allows for flexible testing, data analysis, and real-time monitoring of sensor behavior and interactions.

---

## Key Features

- **Support for multiple sensor types**:
  - Time-of-Flight (ToF) distance sensors: **VL53L5CX**
  - Thermal infrared sensors: **AMG8833** (8×8), **MLX90640** (32×24)
  - Visual image acquisition: **Microsoft LifeCam Studio**

- **Real-time visualization**:
  - 2D raster views, value overlays, Gaussian-blurred views
  - Comparison of data from two identical sensor types (e.g., two ToF sensors)
  - Dynamic UI with live error metrics (max deviation, RMSE)

- **Flexible measurement modes**:
  - Single-shot measurements
  - Time-scheduled and periodic measurements
  - Saving all acquired data to structured `.dat` files

- **Data logging and analysis**:
  - Visualization of error values and average plots
  - Gaussian filter for 8×8 sensor smoothing
  - CSV-style export for post-processing

- **User-friendly interface**:
  - Polish and English language support
  - Camera view with zoom and pan
  - Integrated GUI controls for scaling, saving, filtering

---

## System Architecture

- **Microcontroller**: STM32L476RG Nucleo board
- **Sensors**:
  - VL53L5CX (ToF x2)
  - AMG8833 (thermal)
  - MLX90640 (thermal)
  - Microsoft LifeCam (USB)
- **Software**: Qt 6/C++, custom widgets, QSerialPort
- **Communication**: USB-UART for sensor data, USB for video stream

---

## Application Modules (Qt)

| Class Name         | Purpose |
|--------------------|---------|
| `MainWindow`       | Main GUI, mode selection, serial communication |
| `Table`            | VL53L5CX data visualization, errors, saving |
| `TableTermo`       | AMG8833 + MLX90640 data handling |
| `DataDisplay`      | Pixel-based 2D view with color scale |
| `DataDisplayText`  | Display of numerical values in matrix grid |
| `Gauss`            | Gaussian-blurred interpolation for 8×8 matrix |
| `CameraWidget`     | Camera integration, zoom/pan view |
| `TableChart`, `TableChartMLX` | Chart views of averaged values (for 8×8 and 32×24 sensors) |

---

## Measurement Capabilities

- **Simultaneous sensor reads** via custom protocol (mode symbols: `H`, `I`)
- **Measurement series** up to 50 readings with computed errors
- **Comparative studies**:
  - Sensor cross-talk (VL53L5CX)
  - Resolution effects on edge detection
  - Object distinguishability
  - Effects of surface tilt and heating on measurement errors
  - Distance vs accuracy for ToF and thermal sensors

---
## Documentation

For detailed technical background, testing methodology, and diagrams, see the full thesis [here](docs/praca_dypl.pdf).
