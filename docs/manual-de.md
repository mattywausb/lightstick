# Personal K-Pop Lightstick - Anleitung

Mit dem "Personal K-Pop Lightstick" hat man seine eigene Lichtshow verfügbar. Im Gegensatz zu kommerziellen Produkten funktioniert er unabhängig von einer Konzertveranstaltung. Die Einstellung und Steuerung erfolgt über eine Webbrowseroberfläche. Er bietet folgende Funktionen 
* Synchronisation mit dem Beat und Musikverlauf über eingebauten Taster
* Direkte Auswahl eines Musters und Farbzusammenstellung
* Abruf von vorbereiteten Abläufen für bestimmte Lieder
* Individuelle, taktgenaue Programmierung eines Ablaufs

## Einschalten

Der Stick wird eingeschaltet, indem man den USB Stecker aus dem unteren Stabende herausholt und mit einer passenden Stromversorgung verbindet. Sofern diese kompakt genug ist, kann man sie direkt im Stick deponieren und den Deckel wieder verschließen.

Sowie der Lightstick Strom hat sucht er die Verbindung zum zuletzt kontaktierten WLAN. In dieser Zeit leuchtet er schwach in Orange. Kann er das WLAN nicht erreichen versucht er Kontakt mit einem fest codierten WLAN aufzunehmen. 
War der Verbindungsaufbau erfolgreich, wird dies durch ein schwaches grünes Blinken mitgeteilt.

Kann er kein WLAN erreichen, eröffnet der Stick eine eigene WLAN Zelle. (SSID:"Lightstick-xxxxxxxx-AP" Passwort:"letsparty" (xxxxxx=Seriennummer des eingebauten Controllers)) und ist dort unter der Adress 192.168.4.10 erreichbar.

Wenn beim Einschalten/Einstecken der USB Verbindung der Taster gehalten wird, umgeht der Stick den Versuch ein WLAN zu erreichen und baut sofort seine eigene WLAN Zelle auf.

Das ein eigenes WLAN eingerichtet wurde, wird durch ein schwaches blaues Blinken mitgetilt.

Sollte der Stick auch keinen eigenen AP aufbauen können, blink er rot und ist dann leider nicht bedienbar.

## Zugriff auf die Bedienoberfläche
Für den Zugriff benötigt man einen Browser auf einem beliebigen Gerät, das mit dem gleichen Netzwerk verbunden ist, wie der Stick.

Die im Browser einzugebende IP Adresse besteht aus 4 Zahlen, die jeweils von einem Punkt getrennt sind (z.B. 192.168.4.10) und wird vom Stick wie folgt als sich wiederholender Ablauf verschiedener Muster dargestellt.
* 8 Beats schwach grün/Blau = WLAN Modus (siehe oben)
* 2 Beats dunkel weiß>pink = Startzeichen für erste Zahl
* n rotierende Schritte in rot (n= Ziffer 1)
* 2 Beats dunkel pink 
* n rotierende Schritte in gelb (n= Ziffer 2)
* 2 Beats dunkel pink 
* n rotierende Schritte in blau (n= Ziffer 3)
* 2 Beats dunkel pink 
* 2 Beats dunkel weiß>pink = Startzeichen für zweite Zahl
* n rotierende Schritte in rot (n= Ziffer 1)
...

Um eine Null als Ziffer darzustellen wird mit einem kräftigen Pink einmal aufgeblendet. Die Ziffern 2 und 3 werden nicht genutzt, wenn sie für die Zahl nicht notwendig sind.

## Auswahl eines Pattern (Musters)
Ein Pattern definiert, welche der eingebauten LED wann und wie lange leuchten. Dies erfolgt bezogen auf den laufenden Takt, d.h. das Pattern passt sein Lauftempo der Beatzahl(BPM) an, die über den Taster ermittelt wird oder von einem vorigen Song noch eingestellt ist. 

## Zusammenstellung einer Farbpalette
Die Farbe der LED wird in der Regel durch die eingestellte Farbpalette festgelegt. Dabei wird vom eingestellten Pattern bestimmt ob mehrere Farben der Palette gleichzeitig genutzt werden und in welchem Tempo zum nächsten Platz der Palette geschaltet wird. 

Mit Auswahl einer Farbe auf der Weboberfläche wird die Farbpalette geleert und der erste Platz der Palette auf diese Farbe gesetzt. Das laufende Pattern ist dann einfarbig (Whipe wird dann nur diese Farbe zeigen). Wird das '+' Zeichen rechts neben der Farbe gewählt, so wird diese Farbe an die Palette angehängt und die Pattern greifen das entsprechend auf.

Je nach Pattern kann es interessant sein, die gleiche Farbe mehrfach hintereinander in die Palette  einzubauen. (z.B. Gelb, Blau, Blau,Blau bei "Fade", leuchtet alle 4 Beats einmal Gelb und ansonsten Blau)

### Die Pattern:
* Pulse - Kurzes Aufblitzen auf dem Beat (nutzt 2 Paletteneinträge)
* Whipe - leds innerhalb eines Beats nacheinander auf Folgefarbe schalten 
* Wave - leds nacheinander innerhalb eines Beats ein-  und im Folgebeat wieder ausschalten, dann zum nächsten Paletteneintrag wechseln
* Orbit - einfarbig rotierende Lichter. Die mittige Led blinkt auf Step 1 und 2 von 8)
* Disco - Wie Orbit, allerdings werden 3 einträge der Farbpalette auf die 3 aktiven Led verteilt
* Sparkle - Aufblitzen zufälliger LED mit einer Zufallsfarbe aus 3 Paletteneinträgen

Folgenden Pattern ignorieren die Farbpalette 

 Rainbow - Volles Farbspektrum 
 Quater - Ausschnitt aus dem Spektrum der sich dann verschiebt
 Flat - es wird nur eine Farbe des Spektrums gezeigt  und weitergeschaltet

## Synchronisationstaster
Mit dem Taster werden folgende Funktionen ausgelöst.
* 1xTippen - Beatposition festlegen / korrigieren 
* 4x regelmäßig Tippen - Tempo messen und einstellen (unregelmäßiges Tippen führt zu keiner Änderungen) 
* Halten - Aktuelles Pattern wird festgehalten. Mit Loslassen des Tasters wird das nächste Pattern gestartet

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
<Part Letter><Pattern preset>/<Speed>:<Color Ppalette>

Beispiel:"A10/4:0  B42/8:2  R30/8:0"
```
* Part Letter: Ein Buchstabe unter dem dieser Part in der Song Sequence angesprochen wird. Gute Praxis sind A B C für Strophenabschnitte R S T für Refrainabschnitte, I J L M für Intro Abschnitte
* Pattern Preset: Die Nummer eines Patternpresets (Legt Pattern und Farbstepping fest)
* Speed: Tempo des Steppings innerhalb des Beats (2=2 Beats(halbe Note), 4= 1 Beat(4tel Note), 8=1/2 Beat(8tel Note), 16, 32,64
* Color Palette: Die Nummer einer der fest programmierten Farbpaletten

### Song Sequence:
Die Song Sequence repräsentiert die Abfolge des Songs. Sie beginnt mit der  BPM Angabe der dann die Songabschnitt folgen. Verschiedene Sonderzeichen markieren spezielle übergeordnete Positionen
```
<bpm> <Part Letter> <Beats><Beats>... <Part Letter>...

Beispiel: "120 A88# A88 B88 A8888 B88 R8888 8888 >A8888 B88 R8888 8888"
```

* Part Letter: Muss einer der Buchstaben aus der "Song Parts" Definition sein
* Beats: Die Beats werden als einstellige Zahlen angegeben und als solche dann für den aktuellen Abschnitt aufaddiert bis ein neuer Buchstabe den nächsten Abschnitt deklariert. (Beispiele: 8888=32 Beats 248 =14 Beats)
* Sonderzeichen #: Der Abschnitt in dem dieses Zeichen steht ist die Warteschleife. Mit dem Tastendruck springt das Programm dann zum nächsten Abschnitt. Die Anzahl an Beats in dem Abschnitt sind dabei irrelevant, sollten aber so angegeben werden, dass man ablesen kann, welcher Beat für den einsatz gewählt werden soll (88 = 16 Beats, Also Beat 17 für den Wechsel)
* Sonderzeichen >: Nach dem Songende wird an diese Stelle zurückgesprungen. So kann eine Endlosschleife einer Folge konstruiert werden. Ohne diese Angabe springt das Programm wieder an den Anfang zurück und in die ggf. dort definierte Warteschleife 

### Fehlerbehandlung
Die Angaben für Song Parts und Song Sequence werden intern bzgl. plausibilität geprüft. Formatfehler führen dazu, dass Teile ignoriert oder auch falsch zugeordnet werden. *Es gibt bisher keine Rückmeldung* ob alle Angaben korrekt waren. Syntaktisch falsche oder inkonsistente Angaben(z.B. Buchstabe doppelt als Song Part genutzt oder nicht als Songpart definiert, unbekannte Farbpalette, unbekannter Pattern Speed) führen zu verschiedenen Phänomenen: Falsche Farbpalette, Falsche Pattern, Falsche Anzahl an Beats bis zum Wechsel

## Songprogramme speichern
Ein Songprogramm kann fest in dem Stick gespeichert und dann über die Auswahlliste wieder abgerufen werden. Hierzu dienenen die Eingabefelder "Filename" und "Operation" sowei die Schaltfläche "Execute"

### Neuen Song hinzufügen
* Gewünschte Part- und Sequencedefinition müssen eingegeben und gestartet worden sein
* In "Filename" den Namen des Liedes eingeben (Buchstaben, Zahlen sowie Klammern und Bindestriche sind gestattet). Der Name wird automatisch von dem zulestzt geladenen Song vorbelegt.
* In Operation "Create" auswählen.
* Execute betätigen
Der Song sollte nun unter seinem neuen Namen in der Songprogrammliste erscheinen. Gibt es schon einen Song mit dem Namen, wird dies mit einer Meldung angezeigt und die Daten sind nicht gespeichert.

### Vorhandenen Song anpassen
* Song auswählen
* Part- und Sequencedefintionen anpassen und starten
* Der Filename sollte noch mit dem Namen des geladenen Songs besetzt sein
* In Operation "Update" auswählen
* Execute betätigen

### Vorhandenen Song löschen
* Song auswählen
* Der Filename wird mit dem Namen des geladenen Songs besetzt
* In Operation "Delete" auswählen
* Execute betätigen
Achtung: Diese Operation kann nicht Rückgängig gemacht werden
 
## WLAN Netzwerk konfigurieren
Mit den Feldern "SSID", "Password" und der Schaltfläche "connect to wifi" kann der Stick mit einem vorhandenen WLAN verbunden werden. Beim Verbindungsaufbau geht die alte Verbindung zum Stick verloren und es muss für die weitere Steuerung ein Browser im nun verbunden WLAN verwendet werden.
Sollte die Verbindung fehlschlagen und das fest einprogrammierte WLAN nicht erreichbar sein, baut der Stick wieder sein eigenes WLAN auf.
Die Darstellung von Status und IP Adresses ist identische mit der Beschreibung in "Zugriff auf die Bedienoberfläche".
 
## Anhang

### Fest programmierte Farbpaletten

```
// Code r=red o=orange y=yellow g=green c=cyan sb=sky blue b=blue pu=purple pi=pink
// prefix "l" = light   prefix"w" = White (unsaturated)
0: // Prime Colors and Yellow
1: // Police USA 
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
```
Weiß wird durch eine *entsättigte Farbe* dargestellt. Bei Pattern, die die Sättigung verändern ("Pulse" und "Sparkle") hat dies eine Relevanz, da dann die LED zwischen Weiß und der entsprechend unterlegten Farbe wechselt.
	

### Generische Farbpaletten
Bei generischen Farbpaletten werden die Farben mit der Farbpalettennummer bestimmt. Dabei gilt folgende Farbcodierung:
```
0=rot       5=cyan
1=orange    6=sky
2=gelb      7=blau
3=lemon     8=lila
4=grün      9=pink
```

#### 4 Step Paletten (10000-19999 )
Legen immer 4 Einträge in der Palette an. Die vier letzten Ziffern der Nummer bestimmen die Farbfolge.

12779 z.b. ergibt die Farbpalette;gelb blau blau pink

#### Palettengeneratoren für 1-3 Farben (20000-99000)
Palettengeneratoren erzeugen aus den Farben komplexere Farbfolgen die auch Weiß enthalten. 
Die letzten drei Ziffern definieren die zu verwendenen Farben (x y z). Sind diese all gleich, wird ein einfarbiger Generator genutzt, sind die letzten beiden Ziffern gleich, wird ein zweifarbiger Generator angesprochen, ansonsten ein Dreifarbiger.

Die ersten beiden Ziffern der Palettennummer bestimmen innerhalb der 1er,2er und,3er Farbgeneratoren den konkreten Generator aus. 
Momentan gibt es folgende Möglichkeiten
```
Einfarbig: 20-24,29
Zweifarbig: 20-36,40-43,70-73
Dreifarbig: 20-21,25-29,40-49,70-73

Beispiele:
20 Einfarbig: x wx
20 Zweifarbig: x y y
20 Dreifarbig: x y z
20333 wird zu>lemon weiß über lemon
20377 wird zu>lemon blau blau
20370 wird zu>lemon blau rot

25 Zweifarbig: wx x y y
25 Dreifarbig: wx y x z

43 Zweifarbig: wx x x x wx x x x wy y y y wy y y y
43 Dreifarbig: x y y y x y y y x z z z x z z z
```
Ein vollständiger Katalog kann nur dem Code entommen werden

### Pattern Presets
```
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
100-107: Sparkle shift 1
110-117: Sparkle shift 2
120-127: Sparkle shift 3
```
Die Pattern innerhalb einer Gruppe (z.B. 0-3), varieren über die Anzahl Steps bis zum Weiterschalten einer Farbe. Dabei wird in der Regel in 2er Potenzen skaliert. So schaltet 20 bei jedem Step eine Farbe weiter, 21 jeden 2. Step, 22 jeden 4., 23 jeden 8.  Ein Song Part 20/4 schaltet also jeden Beat die Farbe voran. Ebenso 21/8 und 22/16, wobei das Muster sich aber jeweils schneller  in 1/4, 1/8 und 1/16 Noten bewegt. Mit 22/4 Schaltet die Farbe jeden 4. Beat voran, also auf jeden Takt.
