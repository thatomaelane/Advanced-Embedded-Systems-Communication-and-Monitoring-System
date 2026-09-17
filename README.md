# Advanced Embedded Systems Communication and Monitoring System

> A two-microcontroller embedded systems project integrating temperature sensing, water-level measurement, switch and LED status monitoring, UART communication, an I2C real-time clock, keypad-based time configuration, LCD interfaces, Proteus simulation, and physical hardware implementation on veroboard.

![C](https://img.shields.io/badge/C-Embedded%20C-blue)
![Microcontroller](https://img.shields.io/badge/Microcontroller-ATmega328P-orange)
![UART](https://img.shields.io/badge/Communication-UART-green)
![I2C](https://img.shields.io/badge/Communication-I2C-purple)
![Proteus](https://img.shields.io/badge/Simulation-Proteus-red)
![LCD](https://img.shields.io/badge/Display-16x2%20LCD-yellow)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

---

## Project Overview

This Advanced Embedded Systems project implements communication between two ATmega328P-based microcontroller units.

The system consists of:

- **UC0:** Master microcontroller
- **UC1:** Slave microcontroller

The Master and Slave communicate using **UART serial communication**.

UC1 measures environmental and system inputs, including:

- Temperature
- Water quantity
- Switch conditions

The measured information is displayed locally on **LCD1** and transmitted to UC0.

UC0 receives the information and displays the received data on **LCD0**.

The Master unit also interfaces with:

- An I2C Real-Time Clock (RTCC)
- A hexadecimal keypad
- An LCD display

The RTCC provides the current time, while the keypad allows the user to configure the time.

The complete system was first developed and tested in **Proteus simulation** and subsequently implemented on physical hardware using a **veroboard**.

---

## Project Objectives

The main objectives of the project were to:

- Implement communication between two microcontrollers.
- Measure temperature using an analogue temperature sensor.
- Measure water quantity using an analogue input.
- Monitor switch conditions.
- Control LEDs according to switch states.
- Implement UART communication between UC0 and UC1.
- Implement I2C communication with a Real-Time Clock.
- Implement keypad-based RTC configuration.
- Display system information on LCD displays.
- Simulate the complete design using Proteus.
- Implement the simulated circuit on physical hardware.
- Develop practical embedded C programming skills.
- Gain experience designing and testing embedded systems on veroboard.

---

## System Architecture

```
                         ┌─────────────────────┐
                         │      HEX KEYPAD      │
                         └──────────┬──────────┘
                                    │
                                    │
                         ┌──────────▼──────────┐
                         │       UC0 MASTER     │
                         │      ATmega328P      │
                         │                     │
                         │  UART Receiver      │
                         │  I2C Interface      │
                         │  Keypad Interface   │
                         └──────┬───────┬──────┘
                                │       │
                         UART   │       │ I2C
                                │       │
                  ┌─────────────┘       └─────────────┐
                  │                                   │
          ┌───────▼────────┐                  ┌───────▼────────┐
          │      LCD0      │                  │      RTCC      │
          │                │                  │ Real-Time Clock│
          │ Time           │                  │                │
          │ Temperature    │                  │ Time / Date    │
          │ Water Quantity │                  └────────────────┘
          │ Switch Status  │
          └────────────────┘


                                UART
                                  │
                                  │
                         ┌────────▼─────────┐
                         │     UC1 SLAVE    │
                         │     ATmega328P   │
                         │                  │
                         │ ADC0 Temperature │
                         │ ADC1 Water Level │
                         │ PORTD Switches   │
                         │ PORTC LEDs       │
                         │ UART Transmitter │
                         └───────┬──────────┘
                                 │
                 ┌───────────────┼────────────────┐
                 │               │                │
          ┌──────▼─────┐  ┌──────▼──────┐  ┌──────▼──────┐
          │ LM35 Temp. │  │ Water Sensor │  │   Switches  │
          │   Sensor   │  │              │  │             │
          └────────────┘  └──────────────┘  └─────────────┘
                                                │
                                         ┌──────▼──────┐
                                         │     LEDs    │
                                         │ Switch      │
                                         │ Indicators  │
                                         └─────────────┘
```

---

## Microcontroller Configuration

### UC0 — Master

The Master microcontroller is responsible for:

- Receiving information from UC1.
- Displaying received temperature.
- Displaying received water quantity.
- Displaying switch status.
- Communicating with the RTCC.
- Reading time from the RTCC.
- Displaying the time on LCD0.
- Receiving keypad input.
- Configuring the RTCC using the keypad.

### UC1 — Slave

The Slave microcontroller is responsible for:

- Measuring temperature through ADC0.
- Measuring water quantity through ADC1.
- Reading switches connected to PORTD.
- Driving LEDs connected to PORTC.
- Displaying measurements on LCD1.
- Transmitting measured data to UC0 through UART.

---

## Hardware Components

The project uses the following major components:

- ATmega328P / Arduino development board
- Two LCD displays
- LM35 temperature sensor
- Water quantity sensor
- Resistors
- Capacitors
- LEDs
- Switches
- Hexadecimal keypad
- I2C Real-Time Clock
- Veroboard
- Connecting wires
- PC
- Proteus Design Suite

---

## Communication Protocols

### UART Communication

UART is used for communication between UC1 and UC0.

UC1 transmits four data values:

```c
uart_send[0] = Switch Status
uart_send[1] = Temperature
uart_send[2] = Water Quantity - Low Byte
uart_send[3] = Water Quantity - High Byte
```

The Master receives the four-byte packet and reconstructs the water quantity from the two transmitted bytes.

```
             UC1 SLAVE
                 │
                 │ UART
                 ▼
        ┌──────────────────┐
        │ 4-Byte Data Frame│
        ├──────────────────┤
        │ Byte 0           │
        │ Switch Status    │
        ├──────────────────┤
        │ Byte 1           │
        │ Temperature      │
        ├──────────────────┤
        │ Byte 2           │
        │ Water Low Byte   │
        ├──────────────────┤
        │ Byte 3           │
        │ Water High Byte  │
        └────────┬─────────┘
                 │
                 ▼
             UC0 MASTER
```

### I2C Real-Time Clock

UC0 communicates with the Real-Time Clock using the I2C interface.

The Master performs:

- I2C initialization.
- RTC configuration.
- Time writing.
- Time reading.
- BCD conversion.
- LCD display.

The system displays:

```
Time=HH:MM:SS
```

The keypad provides the user interface for setting:

- Hour: 00 – 23
- Minute: 00 – 59
- Second: 00 – 59

### Keypad Interface

The hexadecimal keypad is connected to the Master microcontroller.

The keypad is used to enter the RTC configuration values. The system uses an external interrupt to detect keypad activity.

```
Keypad
   │
   ▼
INT0
   │
   ▼
UC0 Master
   │
   ├── Hour
   ├── Minute
   └── Second
   │
   ▼
I2C RTCC
```

---

## LCD0 — Master Display

LCD0 displays information received from UC1 as well as the current time.

The display contains:

```
Time=HH:MM:SS
T=XX  W=XXXX  S=XXXX
```

Where:

- HH = Hour
- MM = Minute
- SS = Second
- T = Temperature
- W = Water quantity
- S = Switch status

## LCD1 — Slave Display

LCD1 displays the measurements and switch status locally.

Example:

```
T=XX C
W=XXXX L  S=XXXX
```

The LED status corresponds to the switch status.

```
Switch → UC1 → LED
             ↓
          LCD1
             ↓
          UART
             ↓
          UC0 → LCD0
```

---

## Temperature Measurement

The temperature sensor is connected to ADC0 of UC1.

The ADC measurement is converted into a temperature value and displayed on LCD1. The measured temperature is also transmitted to UC0.

```
LM35
 │
 ▼
ADC0
 │
 ▼
ATmega328P
 │
 ├── LCD1
 │
 └── UART
       │
       ▼
      UC0
       │
       ▼
      LCD0
```

---

## Water Quantity Measurement

The water quantity sensor is connected to ADC1.

The ADC reading is converted into a corresponding water quantity. The result is displayed on LCD1 and transmitted to UC0.

```
Water Sensor
      │
      ▼
     ADC1
      │
      ▼
  Water Value
      │
      ├──────────────► LCD1
      │
      ▼
     UART
      │
      ▼
     UC0
      │
      ▼
     LCD0
```

---

## Switch and LED Monitoring

The switches are connected to PORTD of UC1. A closed switch is represented by a logic 0.

The switch states are read by UC1 and used to control the LEDs connected to PORTC.

```
Switches
   │
   ▼
PORTD
   │
   ▼
UC1
   │
   ├──────────► PORTC LEDs
   │
   ├──────────► LCD1
   │
   └──────────► UART
                    │
                    ▼
                   UC0
                    │
                    ▼
                   LCD0
```

---

## Proteus Simulation

The system was first developed and tested using Proteus.

The simulation was used to verify:

- Microcontroller operation.
- LCD communication.
- ADC measurements.
- UART communication.
- I2C communication.
- RTCC operation.
- Keypad input.
- LED switching.
- Switch monitoring.
- Data transmission between UC0 and UC1.

The Proteus project files are included in this repository.

---

## Hardware Implementation

After successful simulation, the circuit was implemented on a physical veroboard.

The physical implementation demonstrates the transition from:

```
Software Design
       ↓
Proteus Simulation
       ↓
Circuit Verification
       ↓
Hardware Assembly
       ↓
Hardware Testing
```

The hardware implementation includes the two microcontroller units, LCD displays, sensors, switches, LEDs, keypad and RTCC.

### Hardware Design

The project was intentionally implemented on veroboard rather than a breadboard, providing practical experience with permanent circuit construction and embedded hardware integration.

The physical implementation required:

- Component placement.
- Soldering.
- Power distribution.
- Signal wiring.
- LCD connections.
- Sensor connections.
- UART connections.
- I2C connections.
- Switch and LED connections.
- Hardware debugging.

---

## Software Architecture

```
                 ┌─────────────────────┐
                 │       UC0 MASTER     │
                 └──────────┬──────────┘
                            │
             ┌──────────────┼───────────────┐
             │              │               │
             ▼              ▼               ▼
          Keypad           UART            I2C
             │              │               │
             ▼              ▼               ▼
          RTC Setup      UC1 Data          RTCC
                            │
                            ▼
                           LCD0


                 ┌─────────────────────┐
                 │       UC1 SLAVE     │
                 └──────────┬──────────┘
                            │
             ┌──────────────┼───────────────┐
             │              │               │
             ▼              ▼               ▼
           ADC0           ADC1            PORTD
             │              │               │
             ▼              ▼               ▼
        Temperature     Water Level      Switches
             │              │               │
             └──────────────┼───────────────┘
                            │
                            ▼
                          LCD1
                            │
                            ▼
                           UART
                            │
                            ▼
                           UC0
```

---

## Master Code

The Master firmware implements:

- LCD control.
- UART reception.
- I2C communication.
- RTCC configuration.
- RTCC time reading.
- Keypad input.
- External interrupt handling.
- Received sensor data processing.
- LCD data presentation.

The complete source code is available in:

```
Master/
└── master.c
```

## Slave Code

The Slave firmware implements:

- LCD control.
- UART transmission.
- ADC configuration.
- Temperature measurement.
- Water quantity measurement.
- Switch monitoring.
- LED control.
- Sensor data transmission.

The complete source code is available in:

```
Slave/
└── slave.c
```

---

## ADC Configuration

The ATmega328P ADC is used for analogue sensor measurements.

| Measurement | ADC Channel |
|---|---|
| Temperature | ADC0 |
| Water Quantity | ADC1 |

The ADC is configured with the required reference and prescaler settings before measurements are performed.

---

## UART Configuration

The UART communication system uses:

| Parameter | Value |
|---|---|
| Baud Rate | 9600 |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |

UC1 operates as the transmitter while UC0 operates as the receiver.

---

## I2C Configuration

The I2C interface is used by UC0 to communicate with the RTCC.

The implementation includes:

```c
i2c_init();
i2c_start();
i2c_write();
i2c_read();
i2c_stop();
```

These functions provide the basic I2C communication sequence required to configure and read the RTC.

---

## Interrupts

The project uses interrupts for event-driven embedded system operation.

**External Interrupt (INT0)**

The external interrupt is used to detect keypad activity.

**UART Receive Interrupt**

UC0 uses the UART receive interrupt to capture incoming data from UC1.

```c
ISR(USART_RX_vect)
```

The received bytes are stored in a four-byte buffer before being processed.

### Data Packet

UC1 transmits the following four-byte packet:

```
Byte 0 → Switch Status
Byte 1 → Temperature
Byte 2 → Water Quantity Low Byte
Byte 3 → Water Quantity High Byte
```

The Master reconstructs the water quantity using:

```c
water = Recieved[2] | (Recieved[3] << 8);
```

---

## Project Workflow

```
1. Analyse the practical requirements
              ↓
2. Design the embedded system
              ↓
3. Design the UC0 Master
              ↓
4. Design the UC1 Slave
              ↓
5. Implement LCD interfaces
              ↓
6. Implement ADC sensing
              ↓
7. Implement UART communication
              ↓
8. Implement I2C RTCC
              ↓
9. Implement keypad interface
              ↓
10. Build Proteus simulation
              ↓
11. Debug the simulation
              ↓
12. Assemble the hardware
              ↓
13. Solder circuit on veroboard
              ↓
14. Program the microcontrollers
              ↓
15. Test the physical system
```

---

## Repository Structure

```
Advanced-Embedded-Systems/
│
├── README.md
│
├── Master/
│   └── master.c
│
├── Slave/
│   └── slave.c
│
├── Proteus/
│   ├── Advanced_Embedded_System.pdsprj
│   └── Simulation/
│
├── Hardware/
│   ├── hardware_front.jpg
│   └── hardware_back.jpg
│
├── Documentation/
│   └── project_documentation.pdf
│
└── Videos/
    └── hardware_demo.mp4
```

---

## Project Files

**Source Code**
- `Master/master.c`
- `Slave/slave.c`

**Proteus Simulation**
- `Proteus/` — Contains the Proteus schematic and simulation files.

**Hardware Images**
- `Hardware/` — Contains photographs of the completed physical implementation.

**Hardware Demonstration**
- `Videos/` — Contains a demonstration of the physical embedded system operating on veroboard.

---

## Technologies Used

**Programming**
- C
- Embedded C

**Microcontroller**
- ATmega328P
- Arduino R3 development board

**Simulation**
- Proteus Design Suite

**Communication**
- UART
- I2C

**Sensors**
- LM35 temperature sensor
- Water quantity sensor

**Displays**
- 16x2 LCD

**Input Devices**
- Hexadecimal keypad
- Switches

**Timing**
- I2C Real-Time Clock

**Hardware**
- Veroboard
- LEDs
- Resistors
- Capacitors
- Connecting wires

---

## Skills Demonstrated

- Embedded C Programming
- ATmega328P Programming
- Arduino Development
- ADC Programming
- UART Communication
- I2C Communication
- Real-Time Clock Integration
- LCD Interfacing
- Keypad Interfacing
- Sensor Integration
- Interrupt Programming
- Digital I/O
- Embedded System Debugging
- Proteus Simulation
- Hardware Prototyping
- Veroboard Assembly
- Circuit Soldering
- Hardware Testing
- System Integration

---

## Engineering Concepts Demonstrated

This project provided practical experience in integrating multiple embedded-system subsystems into one working platform.

The main engineering concepts include:

```
Analogue Measurement
        +
Digital I/O
        +
Microcontroller Programming
        +
Serial Communication
        +
I2C Communication
        +
Human-Machine Interface
        +
Real-Time Clock
        +
Hardware Construction
        =
Integrated Embedded System
```

---

## Project Demonstration

The repository contains both the simulated and physical implementations of the system.

The demonstration materials include:

- Proteus simulation.
- Master source code.
- Slave source code.
- Physical hardware photographs.
- Hardware demonstration video.
- Project documentation.

---

## Learning Outcomes

Through this project, I gained practical experience in:

- Designing a distributed embedded system.
- Developing firmware for multiple microcontrollers.
- Communicating between microcontrollers.
- Reading analogue sensor inputs.
- Controlling digital outputs.
- Implementing serial communication protocols.
- Working with real-time clock devices.
- Developing keypad interfaces.
- Designing LCD user interfaces.
- Debugging embedded firmware.
- Simulating electronic systems in Proteus.
- Translating a simulated design into physical hardware.
- Soldering and constructing circuits on veroboard.

---

## Academic Context

This project was completed as part of an Advanced Embedded Systems practical/final-year engineering project at:

**Tshwane University of Technology**
Department of Electrical Engineering

The project formed part of a practical progression from simulation and firmware development to physical embedded-system implementation.

---

## Author

**Thato Maelane**

Electrical Engineering
Tshwane University of Technology
South Africa

---

## Repository Contents

- ✓ Master C Source Code
- ✓ Slave C Source Code
- ✓ Proteus Simulation
- ✓ Hardware Photographs
- ✓ Hardware Demonstration Video
- ✓ Project Documentation

---

## Future Improvements

Potential future improvements include:

- Implementing more robust UART packet framing.
- Adding UART error detection.
- Adding checksum or CRC validation.
- Improving sensor calibration.
- Adding real-time sensor graphs.
- Adding alarm conditions for abnormal temperature.
- Adding water-level warning thresholds.
- Adding EEPROM storage for configuration.
- Improving keypad validation.
- Adding date configuration to the RTCC.
- Improving LCD user interface design.
- Adding fault detection for disconnected sensors.
- Migrating the design to a custom PCB.
- Adding wireless communication between microcontrollers.
- Adding data logging.
- Adding remote monitoring capabilities.

---

## Academic Project Disclaimer

This repository contains an academic embedded systems project developed for educational purposes.

The project demonstrates the design, simulation, programming, assembly and testing of a multi-microcontroller embedded system.

---

⭐ If you find this project useful, consider giving the repository a star.
