# Personal K-Pop Lightstick - Anleitung

Mit dem "Personal K-Pop Lightstick" hat man seine eigene Lichtshow verfügbar. Im Gegensatz zu kommerziellen Produkten funktioniert er unabhängig von einer Konzertveranstaltung. Die Einstellung und Steuerung erfolgt über eine Webbrowseroberfläche. Er bietet folgende Funktionen 
* Synchronisation mit dem Beat und Musikverlauf über eingebauten Taster
* Direkte Auswahl eines Musters und Farbzusammenstellung
* Abruf von vorbereiteten Abläufen für bestimmte Lieder
* Individuelle, taktgenaue Programmierung eines Ablaufs

## Einschalten

Der Stick wird eingeschaltet, indem man den Deckel am unteren Stabende öffnet und den USB Stecker mit einer passenden Stromversorgung verbindet. Sofern diese kompakt genug ist, kann man sie direkt im Stick deponieren und den Deckel wieder verschließen.

Sowie der Lightstick Strom hat sucht er die Verbindung zum WLAN. In dieser Zeit leuchtet er schwach in Orange. 

Hat er den Kontakt zum WLAN aufgenommen, blinkt er schwach grün und ist bereit.

Wie Webseite ist auf der IP Adresse des Stick zu erreichen (Momentan in unserem Netz unter http://192.168.178.82)

## Auswahl eines Pattern (Musters)
Ein Pattern definiert, welche der eingebauten LED wann und wie lange leuchten. Dies erfolgt bezogen auf den laufenden Takt, d.h. das Pattern passt sein Lauftempo der Beatzahl(BPM) an, die über den Taster ermittelt wird oder von einem vorigen Song noch eingestellt ist. 

## Zusammenstellung einer Farbpalette
Die Farbe der LED wird in der Regel durch die eingestellte Farbpalette festgelegt. Dabei wird vom eingestellten Pattern bestimmt ob mehrere Farben der Palette gleichzeitig genutzt werden und in welchem Tempo zum nächsten Platz der Palette geschaltet wird. 

Mit Auswahl einer Farbe auf der Weboberfläche wird die Farbpalette geleert und der erste Platze  diese Farbe gesetzt. Wird das '+' Zeichen rechts neben der Farbe gewählt, so wird diese Farbe an die Palette angehängt. 

Je nach Pattern kann es interessant sein, die gleiche Farbe mehrfach hintereinander in die Palette  einzubauen. (z.B. Gelb, Blau, Blau,Blau bei Whipe, leuchtet alle 4 Beats einmal Gelb und ansonsten Blau)

### Die Pattern:
    Pulse - Kurzes Aufblitzen auf dem Beat
    Wipe - leds innerhalb eines Beats nacheinander auf Folgefarbe schalten 
    Wave - leds nacheinander innerhalb eines Beats ein-  und im Folgebeat wieder ausschalten 
    Orbit - einfarbig rotierende Lichter. Die mittige Led blinkt dazu in einer langsameren Frequenz
    Disco - Wie Orbit, allerdings werden 3 einträge der Farbpalette auf die 3 aktiven Led verteilt

Folgenden Pattern ignorieren die Farbpalette 

    Rainbow - Volles Farbspektrum 
    Quater - Ausschnitt aus dem Spektrum der sich dann verschiebt
    Flat - es wird nur eine Farbe des Spektrums gezeigt  und weitergeschaltet

## Synchronisationstaster
Mit dem Taster werden folgende Funktionen ausgelöst.
* 1xTippen - Beatposition festlegen / korrigieren 
* 4x regelmäßig Tippen - Tempo messen und einstellen (unregelmäßiges Tippen führt zu keiner Änderungen) 

## Songprogramm abspielen
Die fest eingebauten Songs werden auf der Weboberfläche als eigene Schaltflächen angezeigt. Mit der Auswahl eines Songs wird dessen Programm sofort aktiviert, dies umfasst das korrekte Tempo (BPM) und eine Abfolge von Mustern und Farben passend zum Songverlauf.
Das Programm beginnt mit einem Muster als Wartschleife, d.h. die Abfolge wird noch nicht abgespielt sondern verweilt auf diesem ersten Muster bis der Taster gedrückt wird.

In der Regel muss der Taster auf Beat 17 (die 1 nach zweimal 8 zählern) des laufenden Songs gedrückt werden um den Ablauf passend zu starten. Die Bedienabfolge wäre also:
* Songprogramm starten (Warteschleife beginnt)
* Song im Abspielprogramm deiner Wahl starten 
* Bis Beat 16 zählen und auf dem Folgebeat den Taster bedienen

### Korrigieren
Mit einem einmaligen kurzen Druck auf den Taster kann der Beat neu synchronisiert werden. Sollte es dazu kommen, dass das Programm insgesamt gegenüber dem Ablauf verschoben ist kann mit etwas Geschickt die Position korrigiert werden.
* Lichtshow ist dem Song voraus: 2-3 Takte vor einem markanten Lichtwechsel (z.B. Refrain) den Taster drücken und halten und mit dem Beat an dem der Wechsel erfolgen soll loslassen
* Lichtshow hängt hinterher: Taster 2,5 Sekunden halten und sofort loslassen. Das Programm springt eine Position vor und ist vermutlich jetzt dem Song voraus. Deshalb gleich wieder den Taster halten und warten bis der nächste Wechsel ansteht

### Wann ist Beat 17 und warum der ?
Manche Songs haben einen Auftakt, was bei unverzüglichem Zählen der Beat zu einem Versatz in den üblichen 8er Gruppen führt. Deshalb gilt folgende Definition:

**Beat 17 ist der erste Beat nach zwei vollständigen 8er Gruppen**. 

Oder auch Beat 1 zum zählen ist der *erste Beat der ersten vollständigen 8er Gruppe*. Vortakte (z.B. die 4 Vortaktbeats von "DNA") zählen also nicht.

Beat 17 war die beste Wahl, da man so über die zwei 8er Gruppe gute Orientierung bekommt und auch bei plötzlichen Songstarts(z.B. aus einer Playlist) noch zeit hat das Programm zu wechseln während man im Kopf noch mitzählt.

Die Warteschleife und ihre Position im Song ist eine Festlegung des jeweiligen Song Programms. Für die leichtere Bedienung wurde Beat 17 festgelegt. Eigene Programme können auch andere Zählpositionen festlegen.

## Eigene Songprogamme eingeben und abspielen
Auf der Weboberfläche gibt es zwei Eingabefelder um das Songprogramm selbst festzulegen. Diese werden bei der Auswahl eines vorbereiteten Songs mit den Programmangaben für diesen Song belegt. Die beiden Textschnipsel kann man aber auch selbst dort editiern oder von einer externen Quelle hineinkopieren. Ein Songprogramm besteht aus zwei Teilen:
* Song Parts: Definieren verschiedene Sätze von Farbpaletten und Mustern die der Song nutzt
* Song Sequence: Legt das Tempo fest und regelt , welcher Song Part für wieviele Beats abgespielt werden soll

### Song Parts
Die Angabe eines Song Parts erfolgt in folgender Syntax:
```
	'<Part Letter><Pattern preset>/<Speed>:<color>
	
	Beispiel:"A10/4:0  B42/8:2  R30/8:0"
```
* Part Letter: Ein Buchstabe unter dem dieser Part in der Song Sequence angesprochen wird
* Pattern Preset: Die Nummer eines Patternpresets (Legt Pattern und Farbstepping fest)
* Speed: Tempo des Steppings innerhalb des Beats (2=2 Beats(Halbe Note), 4= 1 Beat(4tel Note), 8=1/2 Beat(8tel Note), 16, 32,64
* Color: Die Nummer einer der fest programmierten Farbpaletten

### Song Sequence:
Die Song Sequence repräsentiert die Abfolge des Songs. Sie wird mit der BPM Angabe eröffnet und mit einer Angabe für jeden Songabschnitt fortgesetzt. Verschiedene Sonderzeichen markieren spezielle übergeordnete Positionen
```
	<bpm> <Part Letter> <Beats><Beats>... <Part Letter>...
	Beispiel: "120 A88# A88 B88 A8888 B88 R8888 8888 >A8888 B88 R8888 8888"
```

* Part Letter: Muss einer der Buchstaben aus der "Song Parts" Definition sein
* Beates: Die Beats werden als einstellige Zahlen angegeben und als solche dann für den Abschnitt aufaddiert bis ein neuer Buchstabe angegeben wird. (Beispiele: 8888=32 Beats 248 =14 Beats)
* Sonderzeichen #: Der Abschnitt in dem dieses Zeichen Steht ist die Warteschleife. Mit dem Tastendruck springt das Programm dann zum nächsten Abschnitt. Die Anzahl an Beats in dem Abschnitt sind dabei irrelevant, sollten aber so angegeben werden, dass man ablesen kann, welcher Beat für den einsatz gewählt werden soll (88 = 16 Beats, Also Beat 17 für den Wechsel)
* Sonderzeichen >: Nach dem Songende wird an diese Stelle zurückgesprungen. So kann eine Endlosschleife einer Folge konstruiert werden. Ohne diese Angabe springt das Programm wieder an den Anfang zurück und in die ggf. dort definierte Warteschleife 

### Fehlerbehandlung
Die Angaben für Song Parts und Song Sequence werden intern bzgl. plausibilität geprüft. Formatfehler führen dazu, dass Teile ignoriert oder auch falsch zugeordnet werden. *Es gibt bisher keine Rückmeldung* ob alle Angaben korrekt waren. Syntaktisch falsche oder inkonsistente Angaben(z.B. Buchstabe doppelt als Song Part genutzt oder nicht als Songpart definiert, unbekannte Farbpalette, unbekannter Pattern Speed) führen zu verschiedenen Phänomenen: Falsche Farbpalette, Falsche Pattern, Falsche Anzahl an Beats bis zum Wechsel

### Farbpaletten
	// Code r=red o=orange y=yellow g=green c=cyan sb=sky blue b=blue pu=purple pi=pink
	// prefix "l" = light   prefix"w" = White (unsaturated)
	0: // Prime Colors and Yellow
	1: // Police USA (red,white (over blue) ,blue,white over red)
	2: // Red blue variant (w,sb,r,b,o,sb,r,b) "not today"
	3: // b g y
	4: // Yellow pulse with blue and green change
	5: // y g b c
	20: // blue green
	21: // blue white cyan geen
	22: // blue white(on blue) cyan geen
	23: // blue sky lblue cyan
	40: // yellow, skyblue, orange, pink
	41: // red, orange, rose
	42: // pu pi wpu pi
	43: // o wo o wo r wo r wo
	44: // fire
	45: // r wr
	46: // r r r wr r r
	80: // r
	81: // wr
	100: // blue green
	101: // r wr o w+o
	102: // o wo r wo wo
	120: // pu pu pi pi wpu wpu pi pi
	
Weiß wird durch eine entsättigte Farbe dargestellt. Bei Pattern, die die Sättigung verändern (z.Zt. "Pulse", der von Weiß in den gesättigten Ton überführt) hat dies eine Relevanz, da dann die LED zwischen Weiß und der entsprechenden Farbe wechselt.
	
### Pattern Presets

	0-3: Pulse
	4-7: Heartbeat Pulse
	8-9: Dimmed Pulse, Heartbeat
	10: Whipe
	11: Wave
	20-27: Orbit
	30-37: Disco, color shift 1
	40-47: "    , color shift 2
	50-57: "    , color shift 3
	60-67: Full Rainbow
	70-77: Half  Rainbow
	80-87: Quater  Rainbow
	90-97: Flat (Varies in distance to next color)

Die Pattern innerhalb einer Gruppe (z.B. 0-3), varieren über die Anzahl Steps bis zum Weiterschalten einer Farbe. Dabei wird in der Regel in 2er Potenzen skaliert. So schaltet 20 bei jedem Step eine Farbe weiter, 21 jeden 2. Step, 22 jeden 4., 23 jeden 8.  Ein Song Part 20/4 schaltet also jeden Beat die Farbe voran. Ebenso 21/8 und 22/16, wobei das Muster sich aber jeweils schneller  in 1/4, 1/8 und 1/16 Noten bewegt. Mit 22/4 Schaltet die Farbe jeden 4. Beat voran, also auf jeden Takt.
