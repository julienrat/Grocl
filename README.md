# ESP32/ESP8266 Angle Broadcast System

This project implements an angle broadcasting system using ESP32 and ESP8266 (Wemos D1 Mini) with ESP-NOW protocol.

## Components

The system consists of three parts:
1. **Sender (ESP32)**: Broadcasts an angle (0-180°) every 5 seconds using ESP-NOW
2. **Receiver (ESP32)**: Receives the broadcasted angle and controls a servo motor
3. **Receiver (Wemos D1 Mini)**: ESP8266-based receiver that also controls a servo motor

## Project Structure

- `/broadcast_sender`: ESP32 sender code
- `/broadcast_receiver`: ESP32 receiver code
- `/broadcast_receiver_wemos`: Wemos D1 Mini (ESP8266) receiver code

## Requirements

- PlatformIO
- ESP32 board
- ESP8266 board (Wemos D1 Mini)
- Servo motors
- USB cables for programming

## Setup

1. Open each project folder in PlatformIO
2. Build and upload the code to respective devices
3. Connect servo motors to the receiver boards
4. Power up the devices

## Serial Monitoring

Use the following command to monitor the devices:
```bash
pio device monitor -p /dev/ttyUSB0  # Adjust port as needed
```
