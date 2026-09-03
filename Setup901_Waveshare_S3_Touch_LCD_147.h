// Waveshare ESP32-S3-Touch-LCD-1.47 -- 172x320 IPS, JD9853 controller, SPI.
// Pins from the CircuitPython board port (adafruit/circuitpython#10689).
// ST7789_DRIVER is used only for the plumbing: JD9853 shares the standard
// MIPI-DCS drawing path (CASET/RASET/RAMWR/MADCTL/COLMOD). The vendor init
// sequence differs and is pushed by jd9853Init() in the sketch.

#define USER_SETUP_ID 901

#define ST7789_DRIVER
#define TFT_WIDTH  172
#define TFT_HEIGHT 320
#define CGRAM_OFFSET            // 172-wide panel sits at column 34

#define TFT_RGB_ORDER TFT_RGB   // JD9853 init sets MADCTL 0x00 -> RGB
// Inversion off: the vendor sequence leaves INVON (0x21) commented out.

#define TFT_MOSI 39
#define TFT_SCLK 38
#define TFT_CS   21
#define TFT_DC   45
#define TFT_RST  40             // GPIO47 is TOUCH reset, not panel reset
#define TFT_BL   46
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// Required on ESP32-S3. TFT_eSPI overloads SPI_PORT for two different numbering
// schemes: the SPIClass bus index (S3: FSPI=0, HSPI=1) and the raw register
// macros SPI_CMD_REG(n) (n=2 -> SPI2, 3 -> SPI3). The S3 default (SPI_PORT=FSPI=0)
// therefore aims register writes at SPI0, the flash controller, while binding
// `spi` to the global SPI object on SPI3. USE_HSPI_PORT makes both mean SPI3.
#define USE_HSPI_PORT

#define SPI_FREQUENCY 27000000
