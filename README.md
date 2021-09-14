# GrrCON 0xA 10th Anniversary Badge

This badge was created for the 10th anniversay event for GrrCON at the DeVos Center in Grand Rapids, MI September 16-17, 2021.

# Description
Think of this project simply as a skull shaped Arduino. The goal was to create the classic GrrCON skull branding in the form of a digital badge and hackable platform. The front of the badge takes the look of the circuit traces and parts to a level of exposing some non-conductive copper to be coated with LF HASL for a reflective light effect.

Surrounding the skull in bright orange LEDs gives it an effect of shooting fire from behind. The pattern is based upon motion events detected by the LIS3DH acceleromoter. One of seven functions will be selected, including a secret message. The eyes have no mask to expose the FR4  

To extend the capabilities and program the GrrCON0xA digital badge, follow these instructions/references:

* [Using the ATmega328 - 3.3V/8MHz](https://learn.sparkfun.com/tutorials/arduino-comparison-guide/atmega328-boards)
  * Data sheet for [ATmega328P 8MHz 3.3V](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
* SparkFun [FTDI Basic Breakout 3.3V](https://www.sparkfun.com/products/9873)
  * FTDI (Future Technology Devices International Ltd) Drivers [How to Install FTDI Drivers](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers)
      * [Windows](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/windows---in-depth)
      * [Linux](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/linux)
      * [Mac](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/mac)
      * [Going further](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/resou)
   * [SparkFun USB to serial UART Boards Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-usb-to-serial-uart-boards-hookup-guide)
* [USB mini-B cable](https://www.sparkfun.com/products/13243)
* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
  * Board Library for generic AVR programming [MiniCore](https://github.com/MCUdude/MiniCore)
  * Bootloader Flashing [Fuse Calculator](https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p)
  * Thank you Nick Gammon for your [Arduino Sketches](https://github.com/nickgammon/arduino_sketches)

Please refer to the manufacturer's installations instructions.

# Build of Materials (BOM)

Quick badge BOM:
1. BATTERY RETAINER COIN 20MM SMD *Mfg Part No.:* S8211-46R	Ref: BT1	Q: 1
2. CAP Cer 0.1uf 50v Y5V 0603 SMD	Mfg Part No.: CL10F104ZB8NNNC	Ref: C1 C4 C5	Q: 3
3. 22pF ±5% 50V C0G 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT RoHS	Mfg Part No.: CL21C220JBANNNC	Ref: C2 C3	Q: 2
4. CAP CER 10UF 6.3V X5R 0603	Mfg Part No.: CL10A106KQ8NNWC	Ref: C6	Q: 1
5. Standard LEDs - SMD SMD 0603 RED	Mfg Part No.: 599-0010-007F	Ref: D1 D2	Q: 2
6. LED BLUE CLEAR CHIP SMD	Mfg Part No.: LTST-C191TBKT	Ref: D3	Q: 1
7. INFRARED EMITTING DIODE (DNP)	Mfg Part No.: APT2012F3C	Ref: D4	Q: 1
8. SENSOR PHOTODIODE 940NM 2SMD (DNP)	Mfg Part No.: APT2012PD1C	Ref: D5	Q: 1
9. LED ORANGE CLEAR 0603 SMD	Mfg Part No.: LTST-C190KFKT	Ref: D6 - D17	Q: 12
10. RES SMD 0 OHM 5% 1/10W 0603	Mfg Part No.: RC0603JR-070RL	Ref: D18	Q: 1
11. RES SMD 10K OHM 5% 1/10W 0603	Mfg Part No.: RMCF0603JT10K0	Ref: R1 R3 R6	Q: 3
12. RES SMD 470 OHM 5% 1/10W 0603	Mfg Part No.: RMCF0603JT470R	Ref: R2	Q: 1
13. RES SMD 100 OHM 5% 1/10W 0603	Mfg Part No.: CR0603-JW-101ELF	Ref: R4 R5 R7-R12	Q: 8
14. IC MCU 8BIT 32KB FLASH 32TQFP	Mfg Part No.: ATMEGA328P-AU	Ref: U1	Q: 1
15. ACCEL 2-16G I2C/SPI 16LGA	Mfg Part No.: LIS3DHTR	Ref: U2	Q: 1
16. Crystals 16MHz 22pF -10C 60C	Mfg Part No.: LFXTAL027945Reel	Ref: Y1	Q: 1



