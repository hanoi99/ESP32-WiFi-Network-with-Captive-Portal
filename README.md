
# ESP32 – High Speed Wi-Fi Captive Portal

## Was macht es?
Der ESP32 startet einen offenen Access Point namens `High Speed Wi-Fi`.
DNS-Anfragen werden auf `192.168.4.1` umgeleitet. Das Portal zeigt eine
Landingpage mit dem Button „Jetzt online gehen“. Nach dem Klick wird dein
eigenes Video geladen und abgespielt.

> Nur auf eigenen Geräten bzw. mit Zustimmung testen. Das WLAN ist offen und
> bietet absichtlich keinen Internetzugang. Das Portal täuscht keine echte
> Internetverbindung vor und sammelt keine Zugangsdaten.

## 1. Voraussetzungen
- ESP32 Dev Module / NodeMCU
- VS Code mit PlatformIO
- USB-Datenkabel

## 2. Video eintragen
Öffne `data/script.js` und ersetze `VIDEO_URL` durch eine direkte HTTPS-Adresse
zu einer MP4-Datei. Ein YouTube- oder TikTok-Seitenlink funktioniert an dieser
Stelle nicht als direkte Videodatei. Die Videodatei wird nicht auf dem ESP32
gespeichert; sie wird von der angegebenen Adresse geladen. Ohne Internetzugang
auf dem Handy ist ein extern gehostetes Video möglicherweise nicht erreichbar.

## 3. Firmware bauen und hochladen
1. Öffne diesen Projektordner in VS Code.
2. PlatformIO installiert die ESP32-Plattform automatisch.
3. Verbinde den ESP32.
4. Wähle das passende COM-Gerät.
5. PlatformIO: **Upload** (Firmware hochladen).

## 4. Webdateien in LittleFS hochladen
In PlatformIO die Aufgabe **PlatformIO: Upload Filesystem Image** ausführen.
Damit werden `data/index.html`, `data/style.css` und `data/script.js` auf LittleFS
geschrieben.

Falls der Befehl nicht angezeigt wird: PlatformIO-Erweiterung aktualisieren und
prüfen, dass `board_build.filesystem = littlefs` in `platformio.ini` steht.

## 5. Testen
1. Seriellen Monitor mit 115200 Baud öffnen.
2. Mit `High Speed Wi-Fi` verbinden.
3. Falls das Portal nicht automatisch erscheint, im Browser `http://192.168.4.1`
   öffnen.
4. „Jetzt online gehen“ antippen.

## Hinweise
- iOS/Android entscheiden selbst, ob sie das Captive-Portal-Fenster anzeigen.
- Betriebssysteme können das Portal-Fenster schließen oder einschränken.
- HTTPS-Webseiten lassen sich durch DNS-Umleitung nicht transparent ersetzen;
  deshalb wird das lokale Portal über HTTP aufgerufen.
- Das ESP32-WLAN stellt keine Internetverbindung bereit.
- Ein Video mit Ton braucht normalerweise einen echten Tap. Auch danach kann
  ein eingeschränkter Captive-Portal-Browser die Wiedergabe blockieren.
- Für ein Video ohne Internet müsste man die Videodatei lokal auf einem
  leistungsfähigeren Server bereitstellen; ESP32-Flash und RAM sind dafür
  meist unpraktisch.
