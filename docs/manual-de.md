# Personal K-Pop Lightstick - Anleitung

Mit dem "Personal K-Pop Lightstick" hat man seine eigene Lichtshow verfügbar. Er funktioniert unabhängig von einer Konzertveranstaltung. Die Einstellung und Steuerung erfolgt über eine Webbrowseroberfläche. Er bietet folgende Funktionen 
* Synchronisation mit dem Beat und Musikverlauf über eingebauten Taster
* Direkte Auswahl eines Musters und Farbzusammenstellung
* Abruf von vorbereiteten Abläufen für bestimmte Lieder
* Individuelle, taktgenaue Programmierung eines Ablaufs

##Einschalten

Der Stick wird eingeschaltet, indem man den Deckel am unteren Stabende öffnet und den USB Stecker mit einer passenden Stromversorgung verbindet. Sofern diese kompakt genug ist, kann man sie direkt im Stick deponieren und den Deckel wieder verschließen.

Sowie der Lightstick Strom hat sucht er die Verbindung zum WLAN. In dieser Zeit leuchtet er schwach in Orange.

Nach Abruf der Webseite wird ein regelmäßiges grünes Blinken angezeigt.

(noch nicht implementiert>>)
Konnte er sich verbinden, leuchtet er konstant grün und teilt er seine IP Adresse per Blinkzeichen in einzelnen Ziffern mit: 
    1-5 grüne Blitzer=1-5
    1-5 blaue Blitzer =6-9+0
    Weißer Blitzer = Punkt
    4 rote Blitzer = Markiert den Anfang 
(<<noch nicht implementiert)


## Auswahl eines Pattern (Musters)
Ein Pattern definiert, welche der eingebauten LED wann und wie lange leuchtet. Dies erfolgt bezogen auf den laufenden Takt, d.h. das Pattern passt sein Lauftempo der Beatzahl an, die über den Taster ermittelt wird. 

## Zusammenstellung einer Farbpalette
Die Farbe der LED wird in der Regel durch die eingestellte Farbpalette festgelegt. Das Pattern bestimmt aber ob mehrere Farben der Palette gleichzeitig genutzt werden und in welchem Tempo zum nächsten Platz der Palette geschaltet wird. 

Mit Auswahl einer Farbe auf der Weboberfläche wird die Farbpalette auf jeweils diese Farbe gesetzt. Wird das '+' Zeichen rechts neben der Farbe gewählt, so wird diese Farbe an die Palette drangehängt. 
Je nach Muster macht es auch Sinn, die gleiche Farbe mehrfach hintereinander in die Palette  einzubauen.

### Die Pattern:
    Pulse - Kurzes Aufblitzen auf dem Beat
    Wipe - leds nacheinander auf Folgefarbe schalten 
    Wave - leds nacheinander erst ein und danach wieder ausschalten 
    Orbit - einfarbig rotierende Lichter mit Taktpuls
    Disco - mehrfarbig rotierende Lichter mit Taktpuls

Folgenden Pattern ignorieren die Farbpalette 

    Rainbow - Volles Farbspektrum 
    Quater - Ausschnitt aus dem Spektrum der sich dann verschiebt
    Flat - es wird nur eine Farbe des Spektrums gezeigt  und weitergeschaltet

## Synchronisationstaster
Mit dem Taster werden folgende Funktionen ausgelöst.
* 1xTippen - Beatposition festlegen / korrigieren 
* 4x regelmäßig Tippen - Tempo messen und einstellen (unregelmäßiges Tippen führt zu keiner Änderungen) 
* Erstes Tippen nach Songstart - Beatposition festlegen und "Warteschleife beenden"
* Lange (>2s) halten  -  Liedfortschritt aufhalten
* Loslassen nach halten -   Beatposition festlegen und auf nächsten Liedabschnitt wechseln 







