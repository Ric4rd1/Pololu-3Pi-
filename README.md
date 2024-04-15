# Pololu 3pi+ Robot Library 🤖
Welcome to the Pololu 3pi+ Robot Library repository! This repository contains everything you need to get started with programming and using the Pololu 3pi+ robot with Arduino IDE. The Pololu 3pi+ is a versatile programmable robot designed for educational and hobbyist purposes, featuring various sensors, motors, and a programmable microcontroller.
![image](https://github.com/Ric4rd1/Pololu-3Pi-/assets/111295737/e1a7be12-eda1-4cc3-8ce4-bd47cda7a02b)
## Pololu3piPlus32U4 library 📚
- &lt;Pololu3piPlus32U4.h>
- https://pololu.github.io/pololu-3pi-plus-32u4-arduino-library/

## Getting Started ⚡
To get started with using the Pololu 3pi+ Robot Library, follow these steps:
1. **Driver Setup:**
- Download the A-Star Windows Drivers [https://www.pololu.com/file/0J1240/a-starwindows-1.3.0.0.zip] (7k zip) and extract the ZIP file to a temporary folder on your computer
2. **Arduino IDE Setup:** 
- In the Preferences dialog, find the “Additional Boards Manager URLs” text box (highlighted in the picture below). Copy and paste the following URL into this box:
- https://files.pololu.com/arduino/package_pololu_index.json
- In the Tools > Board menu, select “Boards Manager…” (at the top of the menu).
- In the Boards Manager dialog, search for “Pololu A-Star Boards”.
- Select the “Pololu A-Star Boards” entry in the list, and click the “Install” button
- Use blink.ino to check if setup is done correctly
