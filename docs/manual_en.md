# Personal K-Pop Lightstick - Manual
With the "Personal K-Pop Lightstick" you have your own light show available. 
In contrast to commercial products, it works independently of a concert event. The configuration and controlling is done  via a web browser interface.
It offers the following features
* Synchronization with the beat and music history via built-in button
* Direct selection of a pattern and color combination
* Running perfect color and animation sequences for predefined songs
* Individual  programming of a color and animation sequence

## Power on

The stick is activated by opening the cover at the lower end of the stick and connecting the USB plug to a suitable power supply. 
If the powersupply is a compact power bank, you can deposit it directly in the stick and close the lid again.

As soon as the Lightstick has power, it searches for a connection to the WLAN. During this time it glows faintly in orange.

If it has made contact with the WLAN (currently hard-coded access keys ), it flashes weakly green and is ready. 
The website can be reached on the IP address of the stick (currently in our network at http://192.168.178.82)

If the button is held while switching on/plugging in the USB connection, the stick bypasses the attempt to reach a WLAN.

If the stick could not / should not dial into the WLAN, the stick establishes its own WLAN cell. 
(SSID: "Lightstick-xxxxxxxx" Password: "letsparty" (xxxxxx = serial number of the built-in controller)).
In this case it flashes blue. To use it, you have to connect to the stick's WLAN and then find the website at http://192.168.4.22.

If the stick cannot set up its own AP, it flashes red and is then unfortunately not operable.

## Selection of a pattern
A pattern defines which of the built-in LEDs light up when and for how long. 
This is done in relation to the current song tempo, so the pattern adapts its running speed to the number of beats per minute(BPM)  
that is determined by the button or is still set from a previous song.

## Defining a color palette
The color of the LED is usually determined by the loaded color palette. Depending on the pattern one or more 
colors of the palette are used at the same time. Also the pattern traverses through the palette after some beats.

When you select a color on the web interface, the color palette is emptied and the first place in the palette is set to this color. 
The current pattern is then monochrome (Pattern "Whipe" will then only show this color). When you use '+' sign to the right of the color,
this color is appended to the palette and the patterns take up this accordingly.

Depending on the pattern, it can be interesting to add the same color multiple times into the palette. 
(e.g. yellow, blue, blue, blue for "Wave", 1 step lights up yellow 3 steps blue)

### The Pattern:
* Pulse - Flashes on the beat (uses 2 palette entries)
* Whipe - Change LEDs to the following color one after the other within a beat
* Wabe - light leds on one after the other within a beat and turn them off again in the next beat, then switch to the next color palette entry
* Orbit - single color rotating lights. The central LED flashes on step 1 and 2 of 8.
* Disco - Like Orbit, but 3 entries of the color palette are distributed over the 3 active LEDs
* Sparkle - flashing random LED with a random color from 3 palette entries

The following patterns ignore the color palette

 Rainbow - full color spectrum
 Quater - section of the spectrum which then shifts
 Flat - only one color of the spectrum is shown and shifted in the color circle

## Synchronization button
The following functions are triggered with the button.
* 1x tap - set / correct beat position
* 4x regular tapping - measure and set the pace (irregular tapping does not result in any changes)

## Play song program
The built-in songs are displayed as separate buttons on the web interface. 
With the selection of a song, its program is activated immediately, this includes the correct tempo (BPM) and a sequence of patterns
and colors matching the course of the song. The program starts with a pattern as a waiting loop. It will not proceed to the 
next pattern until the button is pressed.

In order to synchronize the sequence appropriately, the button must be pressed  on beat 17 of the current song (the 1st beat after counting to 8 twice)
The operating sequence would be:
* Start the song program on the stick(waiting loop begins)
* Start the song in the player of your choice
* Count to beat 16 and press the button on the next beat

### Correcting the timing
The beat can be re-synchronized with a single short press of the button. Should it occour, that the program is shifted compared to the song progress, 
small deviation can be fixed.

2-3 bars before a significant light change (e.g. chorus) press and hold the button and release it on the beat at which the change should take place.

### Determining Beat 17 and why this number?
Some songs have a prelude which, if the beat is counted immediately, leads to an offset to the usual groups of 8 beats. Therefore the following definition applies:

** Beat 17 is the first beat after two complete groups of 8 **.

Or beat 1 for counting is the * first beat of the first complete group of 8 *. Pre-notes (e.g. the 4 pre-beats from "DNA") do not count.

Beat 17 was the best choice, because the two groups of 8 give you a good orientation and even if a song starts suddenly (e.g. from a playlist)
you might have time to change the program and be ready to synchronize whithout starting it over.

The waiting loop and its position in the song is a definition of the song program. Sticking to Beat 17 was set for easier operation.
Own programs can also define other counting positions, but I would not reccomend that.

## Enter your own song programs and play them
On the web interface there are two input fields to define the song program by yourself. 
When you select a prepared song, these will be set to the programming strings for this song. 
The two text snippets can also be edited there or copied from an external source. A song program consists of two components:
* Song Parts: Define different sets of color palettes and patterns that the song uses
* Song Sequence: Determines the tempo and controls which song part should be played for how many beats

### Song Parts
A song part is specified in the following syntax:
```
<Part Letter> <Pattern preset> / <Speed>: <Color P Palette>

Example: "A10/4:0 B42/8:2 R30/8:0"
```
* Part Letter: A letter which identifies this part in the song sequence. Good practice is A B C for verse sections, R S T for chorus sections, I J L M for intro sections
* Pattern Preset: The number of a pattern preset (defines pattern and color stepping, see list in Appendix)
* Speed: tempo of the stepping within the beat (2 = 2 beats (half note), 4 = 1 beat (fourth note), 8 = 1/2 beat (eighth note), 16, 32.64
* Color Palette: The number of one of the permanently programmed color palettes (see list in appendix)

### Song Sequence:
The song sequence represents the sequence of the song. It begins with the BPM declaration, which is then followed by the song sections.
Various special characters mark special locations in the sequence
```
<bpm> <Part Letter> <Beats> <Beats> ... <Part Letter> ...

Example: "120 A88 # A88 B88 A8888 B88 R8888 8888> A8888 B88 R8888 8888"
```

* Part Letter: Must be one of the letters from the "Song Parts" definition
* Beats: The beats are given as single-digit numbers and as such then added up for the current section until a new letter declares the next section. (Examples: 8888 = 32 beats 248 = 14 beats)
* Special character #: The section in which this character appears is the waiting loop. When you press the button, the program then jumps to the next section. The number of beats in the section is irrelevant, but should be specified in such a way that you can read which beat should be selected for the application (88 = 16 beats, i.e. beat 17 for the change)
* Special characters > : After the end of the song, it jumps back to this point. In this way an endless loop of a sequence can be constructed. Without this information, the program jumps back to the beginning and into the waiting loop that may have been defined there.

### Error handling
The information for Song Parts and Song Sequence is checked internally for plausibility. Format errors lead to parts being ignored or incorrectly assigned.
* Up to now there is no message in the UI that will inform you  whether all information was correct *. 
Syntactically incorrect or inconsistent information (e.g. letters used twice as song part or not defined as song part, unknown color palette,
unknown pattern speed) lead to various phenomena: wrong color palette, wrong pattern, wrong number of beats until the change. So if something weird happens, 
check the programming strings

## Appendix

###Permanently programmed color palettes###

```
// Code r = red o = orange y = yellow g = green c = cyan sb = sky blue b = blue pu = purple pi = pink
// prefix "l" = light prefix "w" = White (unsaturated)
0: // Prime Colors and Yellow (Same as 12740)
1: // Police USA
2: // Red blue variant (w, sb, r, b, o, sb, r, b) "not today"
3: // b g y
4: // Yellow pulse with blue and green change
5: // y g b c
20: // blue green
21: // blue white cyan geen
22: // blue white (on blue) cyan geen
23: // blue sky lblue cyan
40: // yellow, skyblue, orange, pink
41: // red, orange, rose
42: // pu pi wpu pi
43: // o where o where r where r where
44: // fire
45: // r wr
46: // r r r wr r r
80: // r
81: // wr
100: // blue green
101: // r wr o w + o
102: // o where r where where
120: // pu pu pi pi wpu wpu pi pi
```
White is represented by a * desaturated color *. This is relevant for patterns that change the saturation ("Pulse" and "Sparkle"),
since the LED then changes between white and the corresponding color.

### Generic color palettes ###
For generic color palettes, the colors are determined using a one digit color palette number:
```
0 = red     5 = cyan
1 = orange  6 = sky
2 = yellow  7 = blue
3 = lemon   8 = purple
4 = green   9 = pink
```

#### 4 step pallets (10000-19999)
Always creates 4 entries in the palette. The last four digits of the number determine the color sequence.

Example: 12779 gives the color palette; yellow blue blue pink

#### pallet generators for 1-3 colors (20000-99000)
Palette generators use the colors to create more complex color sequences which may also contain white.
The last three digits define the colors to be used (x y z). If these are all the same, a single-color generator is used, 
if the last two digits are the same, a two-color generator is addressed, otherwise a three-color generator.

The first two digits of the pallet number determine the specific generator within the 1, 2 and 3 color generators.
There are currently the following options implemented
```
One color: 20-24, 29
Two color: 20-36, 40-43, 70-73
Three color: 20-21, 25-29, 40-49, 70-73

Examples:
20 One color: x wx
20 Two color: x y y
20 Three color: x y z
20333 becomes> lemon white over lemon
20377 becomes> lemon blue blue
20370 becomes> lemon blue red

25 Two color: wx x y y
25 Three color: wx y x z

43 Two color: wx x x x wx x x x wy y y y wy y y y
43 Three color: x y y y x y y y x z z z x z z z
```
A complete catalog can only be obtained from the code in the output.ino module

### Pattern Presets
```
0-3: pulse
4-7: Heartbeat Pulse
8-9: Dimmed Pulse, Heartbeat
10: Whipe
11: Wave
20-27: orbit
30-37: Disco, color shift 1
40-47: ", color shift 2
50-57: ", color shift 3
60-67: full rainbow
70-77: Half Rainbow
80-87: Quater Rainbow
90-97: Flat (Varies in distance to next color)
100-107: Sparkle shift 1
110-117: Sparkle shift 2
120-127: Sparkle shift 3
```
The patterns within a group (e.g. 20-27) vary over the number of steps until the next palette color is taken. 
This is usually scaled in powers of 2. So 20 advances one color with each step, 21 advances each second step, 22 each 4th, 23 each 8th. 
A song part 20/4 advances the color each beat. 21/8 and 22/16 will do the same, but the pattern moves faster in 1/4, 1/8 and 1/16 notes.
With 22/4 the color advances every 4th beat, i.e. every measure.
