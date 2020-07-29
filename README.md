# ElBanquos K-Pop Lightstick
A DIY Lightstick with multiple light effects and song specific light show presets

# Motivation
My daughter and her friends are huge fans of K-POP music and wanted to have these fancy light sticks, that are common in the concerts of K-POP groups. To be independent from an provider and restricted to one group, we decided to make our own with the following features
* Lot's of different multicolor light patterns to choose from
* Song specific presets, to have pattern changes aligned with the music 
* Smartphone compatible webinterface for song selection and pattern programming
* BPM Detection and synchronization with integrated button
* on the fly programming of new light shows over web interface
* easy extention of firmware with new songs and pattern

# Material
* ESP8266 NodeMCU Board
* Neopixel Jewel
* Pushbutton
* Small USB Power Pack or long USB Cable
* Tube that can hold NODEMCU and power pack
* small facette crystal
* transparent sphere

# Software Architecture
The software is split up into the following modules
* Main Module: Coordination of processing time, Translation of input events (Button, Serial Input) into changes of pattern, Mangement of pattern sequences
* Input Module: Detection of button changes and serial input
* WebUi Module: Redering of the WebUI and translation of WebUI Form Inputs to the main main module
* Lamp Module: Class to model a Hue/Saturation/Value Lamp and translate color into RGB 
* Output Module: Controlling the Lamps and Pixsel, Stepping and looping through a defined pattern until instructed otherwise. Contains Functions für generic chase patterns and defines the available pattern presets (Chase mode, beat fraction tempo, color palette)
* Song Module: Contains the preprogrammed songs pattern sequences

  
# What you will find in the repository
* Main arduino sketch for the firmware
* Additional sketches for separate  developtment of light pattern and web interface
* Additional sketches for simple demos of light control and web interface
* Fritzing sketch for wireing used in the implementation
* Manuals


