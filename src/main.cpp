#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>

const char* AP_SSID = "High Speed Wi-Fi";
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;
WebServer server(80);

// ==========================================
// Logging / Statistiken
// ==========================================

uint32_t totalWifiConnections = 0;
uint32_t totalClicks = 0;
uint32_t totalVideoRequests = 0;

// ==========================================
// WiFi Event Logging
// ==========================================

void onWiFiEvent(arduino_event_id_t event) {

    switch (event) {

        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:

            totalWifiConnections++;

            Serial.println();
            Serial.println("================================");
            Serial.println("[WLAN] Neues Gerät verbunden!");
            Serial.printf(
                "[WLAN] Verbindungen insgesamt: %lu\n",
                (unsigned long)totalWifiConnections
            );
            Serial.println("================================");

            break;


        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:

            Serial.println("[WLAN] Ein Gerät hat die Verbindung getrennt.");

            break;


        default:
            break;
    }
}

// ==========================================
// Button Logging
// ==========================================

void handleButtonHit() {

    totalClicks++;

    Serial.println();
    Serial.println("################################");
    Serial.println("[BUTTON] Jemand hat geklickt!");
    Serial.printf(
        "[BUTTON] Klicks insgesamt: %lu\n",
        (unsigned long)totalClicks
    );
    Serial.println("################################");

    server.send(
        200,
        "application/json",
        "{\"status\":\"success\"}"
    );
}

void sendPortalPage() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain; charset=utf-8",
                "Portal-Datei fehlt. Bitte LittleFS hochladen.");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleAsset(const char* path, const char* contentType) {
  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain; charset=utf-8", "Datei nicht gefunden");
    return;
  }
  File file = LittleFS.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}

void redirectToPortal() {
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("Location", String("http://") + apIP.toString() + "/", true);
  server.send(302, "text/plain", "Weiterleitung zum WLAN-Portal");
}

void setupRoutes() {
  server.on("/", HTTP_GET, sendPortalPage);
  server.on("/style.css", HTTP_GET, []() {
    handleAsset("/style.css", "text/css");
  });
  server.on("/script.js", HTTP_GET, []() {
    handleAsset("/script.js", "application/javascript");
  });

// Lokales Video aus LittleFS bereitstellen
  server.on("/video.mp4", HTTP_GET, []() {

    totalVideoRequests++;

    Serial.println();
    Serial.println("[VIDEO] Video wurde angefordert!");
    Serial.printf(
        "[VIDEO] Anfragen insgesamt: %lu\n",
        (unsigned long)totalVideoRequests
    );

    handleAsset("/video.mp4", "video/mp4");
});


  // Häufig verwendete Captive-Portal-Erkennungsadressen von Smartphones.
  server.on("/generate_204", HTTP_GET, redirectToPortal);             // Android
  server.on("/gen_204", HTTP_GET, redirectToPortal);
  server.on("/hotspot-detect.html", HTTP_GET, sendPortalPage);         // Apple
  server.on("/library/test/success.html", HTTP_GET, redirectToPortal);
  server.on("/connecttest.txt", HTTP_GET, redirectToPortal);           // Windows
  server.on("/ncsi.txt", HTTP_GET, redirectToPortal);
  server.on("/api/button-hit", HTTP_POST, handleButtonHit);
  server.onNotFound(redirectToPortal);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS konnte nicht eingebunden werden.");
  }

  WiFi.mode(WIFI_AP);
  // WiFi-Events registrieren
  WiFi.onEvent(onWiFiEvent);
  WiFi.softAPConfig(apIP, apIP, netMsk);

  // Offener Access Point – nur für ein eigenes Testgerät bzw. mit Zustimmung nutzen.
  if (!WiFi.softAP(AP_SSID)) {
    Serial.println("Access Point konnte nicht gestartet werden.");
    return;
  }

  dnsServer.start(DNS_PORT, "*", apIP);
  setupRoutes();
  server.begin();

  Serial.println();
  Serial.println("Captive Portal gestartet.");
  Serial.print("WLAN: ");
  Serial.println(AP_SSID);
  Serial.print("Portal: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {

    dnsServer.processNextRequest();
    server.handleClient();

    // Aktuelle WLAN-Verbindungen kontrollieren
    static unsigned long lastCheck = 0;
    static int lastClientCount = -1;

    if (millis() - lastCheck >= 500) {

        lastCheck = millis();

        int clients = WiFi.softAPgetStationNum();

        if (clients != lastClientCount) {

            lastClientCount = clients;

            Serial.printf(
                "[STATUS] Aktuell verbunden: %d Gerät(e)\n",
                clients
            );
        }
    }
}
