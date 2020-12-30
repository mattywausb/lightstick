# ElBanquos K-Pop Lightstick
A DIY "ightstick" with multiple light effects and song specific light show presets

# Motivation
My daughter and her friends are huge fans of K-POP music and wanted to have these fancy light sticks, that are common in the concerts of K-POP groups. To be independent from an provider and restricted to one group, we decided to make our own with the following features
* Lot's of different multicolor light patterns to choose from
* Song specific presets, to have pattern changes aligned with the music 
* Smartphone compatible webinterface for song selection and pattern programming
* BPM Detection and synchronization with integrated button
* on the fly programming of new light shows over web interface
* save and load songs to/from the flash memory
* connects to existing WLAN or creates its own Access Point

# Material
* ESP8266 NodeMCU Board or D1 Mini
* Neopixel Jewel
* Pushbutton
* Small USB Power Pack or long USB Cable
* Tube that can hold NODEMCU and power pack
* small facette crystal
* transparent sphere

For more information check out my [arduino project page]( https://create.arduino.cc/projecthub/mattywausb/k-pop-lightstick-personal-lightshow-641dac)

# Software Architecture
The software is split up into the following modules
* Main Module: Coordination of processing time, Translation of input events (Button, Serial Input) into changes of pattern, Mangement of pattern sequences
* Input Module: Detection of button changes (and serial input if activated)
* WebUi Module: Redering of the WebUI and translation of WebUI Form Inputs to the main main module and management of network connection
* Lamp Module: Class to model a Hue/Saturation/Value Lamp and translate color into RGB 
* Output Module: Controlling the Lamps and Pixels, Stepping and looping through a defined pattern until instructed otherwise. Contains functions for generic chase patterns and defines the available pattern presets (Chase mode, beat fraction tempo, color palette)
* Song Catalog Module: Manages reading and saving of songs patterns to the flash memory file system

# What you will find in the repository
* Main Arduino sketch for the firmware (the_light_stick)
* Arduino sketch to format and prepare the flash memory file system (song_catalog_init)
* Additional sketches for separate developtment of light pattern(Arduino UNO compatible) and web interface(only on ESP8266)
* Additional sketches for simple demos of light control and web interface
* Documentation
  * Fritzing sketch for wireing used in the implementation
  * Manuals
  * Schematic


