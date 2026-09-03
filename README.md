# External Display for WAHOO KICKR BIKE SHIFT
Code is written for LILYGO T-Display-S3 ESP32-S3, LILYGO TTGO T-Display ESP32, or Waveshare ESP32-S3-Touch-LCD-1.47 (see [below](#waveshare-esp32-s3-touch-lcd-147)) and displays the following Bluetooth Low Energy (BLE) data:

- Gearing ratio (as set in the wahoo app) and what gears you are currently in. Information (text and graphic) will update when shifting.
- Cycling power (watts and watts per kilogram). Displayed information can be changed (instantaneous power, 3s moving average, 10s moving average) via a button (BOOT) on the device.
- Cycling cadence (RPM - revolutions per minute)
- Cyclist weight (as set in the wahoo app) - Not visible on the display but used to calculate W/kg

![in use example](wahoo-kickr-bike-shift-display.jpeg)

## Setup & Requirements
You need to install the Arduino IDE to compile and upload the code to the microcontroller. A very good HOWTO can be found here:
https://github.com/teastainGit/LilyGO-T-display-S3-setup-and-examples/blob/main/T-DisplayS3_Setup.txt

### ESP32 Core for Arduino IDE Version 
Code is written for the latest stable version (v3.1.0) and does not work with former 2.x.x as there are some changes to the BLE library.
Documentation on how to install the ESP32 Core can be found here:
https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


### TFT_eSPI
Comment and un-comment the following lines in the "..\libraries\TFT_eSPI\User_Setup_Select.h" file depending on which board you use.

> #include <User_Setups/Setup25_TTGO_T_Display.h>
>
> #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

### Waveshare ESP32-S3-Touch-LCD-1.47

This board is **not** the same as the non-touch `ESP32-S3-LCD-1.47` — different
LCD controller, different pin map. Do not mix up their setup files.

| | |
|---|---|
| MCU | ESP32-S3R8, 16MB flash, 8MB octal PSRAM |
| LCD | **JD9853**, 172x320 IPS, SPI. Column offset 34, row offset 0 |
| LCD pins | SCK 38, MOSI 39, DC 45, CS 21, RST 40, backlight 46 (active HIGH) |
| Touch | AXS5106L on I2C — SCL 41, SDA 42, RST 47, IRQ 48 (unused by this sketch) |
| SD | CMD 15, CLK 16, D0 17, D1 18, D2 13, D3 14 |

Note that GPIO47 is the *touch* reset, not the panel reset — some third-party
pinouts get this wrong.

Instead of the two `User_Setup_Select.h` lines above, use the setup file
included in this repo:

1. Copy `Setup901_Waveshare_S3_Touch_LCD_147.h` into
   `..\libraries\TFT_eSPI\User_Setups\`
2. In `..\libraries\TFT_eSPI\User_Setup_Select.h`, comment out the LilyGO /
   TTGO includes and add, **next to the other setup includes** (the driver
   dispatch further down the file runs before the end of it, so an include
   appended at the bottom is too late):

> #include <User_Setups/Setup901_Waveshare_S3_Touch_LCD_147.h>

There is no Arduino board variant for this board. Build it with the
**LilyGo T-Display-S3** board selection (`esp32:esp32:lilygo_t_display_s3`) —
same chip, flash and PSRAM configuration, and USB CDC serial works.

#### Two things worth knowing

**TFT_eSPI needs `USE_HSPI_PORT` on the ESP32-S3.** It is already in the
included setup file. TFT_eSPI 2.5.43 overloads `SPI_PORT` for two incompatible
numbering schemes: the `SPIClass` bus index (on S3 the core defines `FSPI 0`,
`HSPI 1`) and the raw register macros `SPI_CMD_REG(n)`, where
`REG_SPI_BASE(i)` is `((i)>=2) ? (DR_REG_SPI2_BASE + (i-2)*0x1000) : 0`. The S3
default of `SPI_PORT = FSPI` is therefore 0, `REG_SPI_BASE(0)` returns 0, and
every register write lands near address 0:

> Guru Meditation Error: Core 1 panic'ed (StoreProhibited), EXCVADDR: 0x00000010

with a backtrace through `begin_tft_write` -> `TFT_eSPI::init` -> `begin`.
`USE_HSPI_PORT` makes both meanings resolve to SPI3. Fixed in TFT_eSPI master,
so this workaround can go once a release ships with it.

**TFT_eSPI has no JD9853 driver, and does not need one.** The drawing path is
standard MIPI-DCS and identical to the ST7789, so the setup file selects
`ST7789_DRIVER` with `CGRAM_OFFSET` and the sketch pushes the vendor
initialisation sequence itself in `jd9853Init()`, called right after
`tft.begin()`. That sequence and the pin map above come from the CircuitPython
board port, [adafruit/circuitpython#10689](https://github.com/adafruit/circuitpython/pull/10689).

## A few things to consider

1. If you have changed the Bluetooth name of your bike (in the wahoo app) you will need to change it also in the code:

> // BLE Server name
> 
> #define bleServerName "KICKR BIKE SHIFT 720C"

2. Set Display Resolution "170 x 320" for LILYGO T-Display-S3 ESP32-S3, "172 x 320" for Waveshare ESP32-S3-Touch-LCD-1.47, or "135 x 240" for LILYGO TTGO T-Display ESP32
   
> #define RESOLUTION_X 320
> 
> #define RESOLUTION_Y 170

3. The graphic should scale depending on the gearing setting, so it will fit the screen. (There are variables to change the rendering if necessary.)

> int myGearWidth = (IWIDTH - 35) / (myFrontGears + myRearGears);
> 
> #define myGearHeight (RESOLUTION_Y - 60)
> 
> #define myGearSpacing -1
> 
> #define myGearX 15
> 
> #define myGearY 30

## Development Notes

1. Debugging: All Status messages during initiation and during connecting process will be displayed on the TFT and in the serial monitor window of the IDE. I tried to comment the code so that it is easier to understand.

2. To identify BLE GATT servies and characteristics use https://www.bluetooth.com/specifications/assigned-numbers/ and if you look at GATT Specification Supplement 5 it describes how to intepret the bytes: https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-5/
   
3. Scan BLE devices: https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile

Enter the upload mode for Sketch upload manually.
- Connect the board via the USB cable
- Press and hold the BOOT button (next to RST) , While still pressing the BOOT button, press RST
- Release the RST
- Release the BOOT button
- Upload sketch

### Cycling Power Measurement (Power & Cadence)
As mentioned in the GATT Specification Supplement document the Service 0x1818 holds the characteristic 0x2A63. The first UINT16 (unsigned 16-bits integer) holds the FLAGS. In our Case "3C00" (as little-endian) and so we get "00000000 00111100". Section "3.59.2.1 Flags field" in the document tells us which values are beeing submitted:

- Pedal Power Balance Present: False
- Pedal Power Balance Reference: Unknown
- __Accumulated Torque Present: True 16__
- __Accumulated Torque Source: Crank based__
- __Wheel Revolution Data Present: True 32 & 16__
- __Crank Revolution Data Present: True 16 & 16__
- treme Force Magnitudes Present: False
- Extreme Torque Magnitudes Present: False
- Extreme Angles Present: False
- Top Dead Spot Angle Present: False
- Bottom Dead Spot Angle Present: False
- Accumulated Energy Present: False
- Offset Compensation Indicator : False

So the values in the characteristic are:

| Flags field | Instantaneous Power | Accumulated Torque | Cumulative Wheel Revolutions | Last Wheel Event Time | Cumulative Crank Revolutions | Last Crank | Event Time | 
| --- | --- | --- | --- | --- | --- | --- | --- | 
| Mandatory | Mandatory | Optional | Optional | Optional | Optional | Optional | Optional | 
| 0 - uint16 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 
| [0][1] | [2][3] | [4][5] | [6][7] | [8][9] | [10][11] | [12][13] | [14][15] | 

### FONT for ESP32 LCD
- Install selected Google Font
- Install Processing - https://processing.org/download
   - Select "Toools" - "Create Font" and after selecting the font the ".vlw" file will be created in the "\data" folder.
- Covert ".vlw" to binary - https://tomeko.net/online_tools/file_to_hex.php?lang=en
   - copy to clipboard an create a ".h" file and add constructor
   > const uint8_t  FontName[] PROGMEM = { BINARY };
