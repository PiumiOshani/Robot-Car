# Robot Car Project

This repository contains the development files for two robot platforms designed and built by Team HOPE. These robots demonstrate advanced mobility, object manipulation, and vision-based control using embedded systems and computer vision.

## 🤖 Robots Overview

### 🛞 Robot 1 – Mecanum Drive Robot
- Four-wheel Mecanum drive for omnidirectional movement.
- Vacuum suction mechanism for object pickup and manipulation.
- Gear-driven lifting mechanism for vertical object placement.
- Controlled by ESP32, integrated with Raspberry Pi for computer vision and task coordination.

### ⚫ Robot 2 – Line Following Robot
- Two-wheel differential drive system.
- Line-following capability using IR sensors and basic lane detection.
- Vacuum-assisted pickup with a robotic arm.
- **Robotic arm mechanism controlled by Raspberry Pi via servo motors.**
- Onboard ESP32 for navigation and low-level actuation.

---

## 🧠 Key Features

- **Omnidirectional movement** using Mecanum wheels (Robot 1).
- **Autonomous line following** and precise turning (Robot 2).
- **Vacuum-based object manipulation** integrated into both robots.
- **Gear lifting mechanism** for elevation-based tasks (Robot 1).
- **Serial communication** between Raspberry Pi and ESP32.
- **Raspberry Pi-controlled robotic arm** using PWM for servo control.
- **Real-time object and color detection** using OpenCV and Python.

---

## 🖥️ Vision System

- Initial object detection using a custom-trained YOLOv5 model.
- Transitioned to OpenCV for real-time color and blob detection.
- Vision logic optimized for Raspberry Pi performance.
- Debugging included detection accuracy, color calibration, and frame sync issues.
- Reliable serial communication between Pi and ESP32 enabled command synchronization.

---

## ⚙️ Hardware & Control

- ESP32 used for low-level motor and vacuum control.
- Raspberry Pi used for:
  - High-level vision and decision-making
  - **Robotic arm control via servo motor signals**
- Servo motors controlled via GPIO PWM for precise movements.
- Vacuum suction activated based on detection and arm position.
- Modular design with clean separation of software and hardware responsibilities.

---
## 📸 Robot Photos

### 🤖 Robot 1 – Mecanum Drive Robot
![Robot 1 – Mecanum Drive](images/robot1.jpg)

### ⚫ Robot 2 – Line Following Robot
![Robot 2 – Line Follower](images/robot2.jpg)

---

## 📍 Path Localization

Visual representation of the robot's estimated vs. actual path using onboard sensor data and vision cues.

![Path Localization](images/path_localization.png)

---

## 📈 Step Response

A sample response of the robot’s control system to step input for position/movement control.

![Step Response](images/step_response.png)

---
## 👥 Team Contributions

| Member | Roles & Responsibilities |
|--------|---------------------------|
| **Yehen Asuramuni** | Vision system, object tracking, Pi-ESP32 serial communication |
| **Chamath Diunugala** | lifting mechanism, integration, Mechanical support, chassis and assembly|
| **Oshani Dewasumithra** | Hardware design, Vacuum control , robotic arm setup and motion control |
| **Kavishka Jayakody** | Line-following logic, ESP32 programming, debugging |
| **Nilakna Warushavithana** | Line-following logic, ESP32 programming, debugging |

---
