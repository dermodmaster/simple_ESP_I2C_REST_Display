# Simple ESP I2C REST Display
## Introduction
This is a simple project I created, because I didn't found any simple project like that, that actually works for me.

## Description
This is a rest server for Arduino ESP8266 (tested with nodemcu v2/v3) to display some text on a 16x2 LCD Display with I2C via REST.

## Wiring it up
- Connect D1 to SCL
- Connect D2 to SDA
- Connect the I2C to a power source
- Connect the ESP to a power source

**Make sure** the LCD is connected to an 5V power source! The ESP8266 usually provides 3,3V wich is not sufficient.

# Functionalities
- Display text with POST on /display (Payload (form-data):   Key: body | Value: {"line1":"YOURTEXT", "line2":"YOURTEXT"}
- Getting the actual displayed text with GET on /display
- Turn backlight on with GET on /backlight/1
- Turn backlight off with GET on /backlight/0

Have fun! ;-)
