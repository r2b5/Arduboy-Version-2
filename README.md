# Arduboy Version 2

Arduboy Version 2 is a custom-built handheld game console based on the original **DIY Mini Retro Game Console** by **MaDDy Regal** (Uttar Pradesh, India), published on Instructables.

Unlike the original design, this version integrates the Arduino Nano's **ATmega32U4** microcontroller directly onto a custom PCB. It also features a USB Type-C connector for power and programming, an integrated Li-ion battery charging circuit, and a larger OLED display.

---

## Hardware

- **Microcontroller:** ATmega32U4-AU
- **Display:** 2.42" 128×64 SPI OLED (Joy-it COM-OLED2.42, SSD1309 controller)
- Integrated Li-ion battery charging circuit MCP73831
- Custom-designed PCB

---

## Software

- Arduino IDE
- CadSoft Eagle

> **Note:**  
> The game source code is **not my own work**. It consists of example projects that have been adapted to run on this hardware implementation.

---

## Flashing the Bootloader

An **Arduino Uno** can be used as an ISP programmer to burn the bootloader.

The board provides the standard **6-pin Arduino ISP header**. Before burning the bootloader, select **Arduino Leonardo** as the target board in the Arduino IDE.

| Arduino Uno | Arduboy (Leonardo) |
|--------------|--------------------|
| D12 | MISO |
| 5V | 5V |
| D13 | SCK |
| D11 | MOSI |
| D10 | RESET |
| GND | GND |

After wiring the programmer, use **Tools → Burn Bootloader** in the Arduino IDE.

---

## Required Changes to `Arduboy2Core.cpp`

Since this hardware differs from the original Arduboy, a small modification to `Arduboy2Core.cpp` is required for the SSD1309 display to render correctly.

Replace the column and page address initialization with:

```cpp
// Set column address range
0x21, 0x00, COLUMN_ADDRESS_END,

// Set page address range
0x22, 0x00, PAGE_ADDRESS_END
```

---

## Project Goals

The primary objective of this project is to teach apprentices and interns the fundamentals of microcontroller programming while providing a practical understanding of how embedded electronic systems work.

In addition, the project serves as a foundation for developing more advanced retro gaming consoles and other embedded systems.

---

## Credits

- Original DIY Mini Retro Game Console concept by **MaDDy Regal**
- Original game source code by **different authors**
- Hardware redesign and PCB development by **Jesus Suarez**