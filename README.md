# Sunrise

A simple program for an Arduino to simulate a sunrise.

## Concept

I've always had trouble waking up in the morning.  With a new job starting soon, my sleep schedule had to change quickly.  Solution: make my own very basic sunrise!

## Implementation

The arduino has its time seeded from a USB-connected computer with the following command (OSX):

```
echo T$(($(date +%s)+60*60*-8)) > /dev/tty.usbmodem1411  #The device will be different on linux, etc. 
```

Sunrise starts half an hour before your alarmTime, reaching full brightness at alarmTime and stays on until half an hour afterwards.  This code assumes using NeoPixel LED strips.

