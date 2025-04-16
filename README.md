# **COMPENG 4DS4 – Embedded Systems**  
**McMaster University**  
**Term:** Winter 2025  
**Group:** 07  

---

## 📘 **Course Overview**  
COMPENG 4DS4 focuses on real-time embedded systems using microcontrollers.  
Topics include interrupt handling, multitasking with RTOS, peripheral interfacing, and low-level firmware development.  
The course emphasizes hands-on learning through labs and projects involving sensors, actuators, and real-time constraints.

---

## 🧪 **Labs Overview**

### 🔹 **Lab 0 – Bare-Metal Applications**  
Wrote a basic C application that runs without an operating system.  
Practiced GPIO configuration and delay loops for LED control.

### 🔹 **Lab 1 – Bare-Metal I/O: Sensors and Actuators**  
Implemented polling and interrupt-driven sensor input and actuator output.  
Controlled LEDs via button presses and introduced concepts like debouncing and ISR handling.

### 🔹 **Lab 2 – FreeRTOS**  
Created multitasking embedded applications using FreeRTOS.  
Utilized tasks, semaphores, and queues to demonstrate concurrent behavior, synchronization, and task scheduling.

---

## 🧩 **Projects**

### 🎮 **Project 1 – Remote-Controlled Car**  
Developed a system to control a car using an external controller via UART communication.  
The car responded to directional input (forward, backward, left, right) with motor actuation,  
and LED indicators were used to confirm actions.

**Key Features:**  
- Real-time command processing via UART  
- Motor control using GPIO and PWM signals  
- Visual feedback with onboard LEDs  
- Basic interrupt-driven design for responsiveness

---

### 📷 **Project 2 – Vision-Based Obstacle Avoidance**  
Built an embedded system that uses a camera to detect obstacles and autonomously steer a car left or right.  
The project combined image processing logic with real-time hardware control  
to demonstrate decision-making based on visual input.

**Key Features:**  
- Live image capture and object detection  
- Obstacle-based steering logic  
- Integrated motor control for movement adjustment  
- Coordination between camera data and embedded control flow

---

## 🛠️ **Tools & Technologies**  
- **Languages:** C, C++  
- **IDE:** MCUXpresso  
- **Microcontroller:** NXP LPC1769  
- **RTOS:** FreeRTOS  
- **Interfaces:** GPIO, ADC, SPI, PWM, UART  
- **Tools:** Oscilloscope, Logic Analyzer, Camera Module

---

## 📚 **Learning Outcomes**  
- Developed real-time embedded systems for interactive and autonomous tasks  
- Understood interrupt-driven programming and task coordination  
- Integrated cameras, sensors, and motor drivers in hardware-software co-design  
- Built multitasking applications using FreeRTOS  
- Practiced embedded debugging, signal analysis, and communication protocols

