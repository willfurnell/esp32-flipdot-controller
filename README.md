# `esp32-flipdot-controller`

## Replacement controller for Hannover Flipdot Displays

This is a fork of the original https://github.com/themainframe/esp32-flipdot-controller, but is designed to allow for more than one panel (my display has three), and works with the Platform.IO build, and Arduino ecosystems.

It currently gets the time from an NTP server and displays it. The code obviosuly needs more work, but it's good enough for now. 

Also included in this repository are KiCad schematics for my version of the PCB, which has hardware (but not yet software) support for the built in LED strip and other personal customisations.

Based on the [blog post on damow.net](https://damow.net/fun-with-flip-dot-displays/)!
