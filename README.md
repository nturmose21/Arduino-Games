# OLED Minigames Console 🎮 | hack_updt

A custom-built, microcontroller-based handheld console featuring classic arcade games: Tetris and Flappy Bird. This project demonstrates hardware-to-software integration using an I2C OLED display, tactile button inputs, and active piezo buzzer audio, all running on a single microcontroller.

## 🚀 Overview

This repository contains the source code for two fully playable standalone games:
* **Tetris-hack_updt**: A full implementation of classic Tetris featuring a 14x22 grid, 7 standard tetromino shapes, and a custom non-blocking audio engine that plays the iconic "Korobeiniki" theme song during gameplay. 
* **Flappybird-hack_updt**: A physics-based clone with smooth gravity mechanics, dynamic randomized pipe generation, collision detection, and high-score tracking.

## 🛠️ Hardware Requirements

To build this project, you will need:
* Microcontroller (e.g., ESP32, Arduino Nano/Uno, etc.)
* 1x 128x64 I2C OLED Display (SSD1306)
* 3x Tactile Push Buttons (Left, Select, Right)
* 1x Piezo Buzzer (For Tetris audio)
* Breadboard & Jumper Wires (or custom PCB)

### Pin Configuration

Wire your components to the microcontroller using the following digital pins (pull-up resistors are enabled in software):

| Component | Pin | Function |
| :--- | :--- | :--- |
| **OLED Display** | I2C Pins (SDA/SCL) | Screen Output |
| **Button 1** | `D9` | Move Left |
| **Button 2** | `D10` | Select / Rotate / Jump |
| **Button 3** | `D11` | Move Right |
| **Piezo Buzzer** | `D12` | Audio Output (Tetris Only) |
<img width="600" height="475" alt="image" src="https://github.com/user-attachments/assets/8bc0bb00-fe16-4eb1-bd12-40f873991282" />


## 🕹️ Controls

**Tetris**
* **Left Button:** Move piece left.
* **Right Button:** Move piece right.
* **Select Button (Tap):** Rotate piece 90 degrees.
* **Select Button (Hold):** Fast-drop piece.

**Flappy Bird**
* **Select Button:** Start game / Flap wings.

## 💻 Software & Libraries

This project is written in C++ for the Arduino IDE. Ensure you have the following libraries installed via the Arduino Library Manager:
* `Wire.h` (Built-in)
* `Adafruit_GFX.h`
* `Adafruit_SSD1306.h`

*Note: You will also need the custom `games_bitmaps.h` file included in this repository, which contains the PROGMEM byte arrays for all the custom sprites and splash screens (cover art, pipes, birds, game over screens).*

## 🧠 Technical Highlights
* **Non-Blocking Audio Engine:** The Tetris sketch utilizes `millis()` instead of `delay()` to process the game loop and the music array simultaneously. This ensures the "Korobeiniki" theme plays smoothly without pausing the falling blocks.
* **Matrix Collision Detection:** Tetris uses a custom algorithm to check tetromino coordinates against the 2D grid array before allowing movement or rotation, locking pieces only when a valid move is impossible.
* **Memory Optimization:** Heavy visual assets and multidimensional arrays for the Tetris shapes are stored in Flash memory using `PROGMEM` to preserve limited dynamic memory (SRAM).

---
*Developed by [hack_updt]*
