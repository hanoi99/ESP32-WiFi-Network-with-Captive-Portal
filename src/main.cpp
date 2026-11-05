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
    Serial.println("[VIDEO] Video angefordert");
    handleAsset("/video.mp4", "video/mp4");
});


  // Häufig verwendete Captive-Portal-Erkennungsadressen von Smartphones.
  server.on("/generate_204", HTTP_GET, redirectToPortal);             // Android
  server.on("/gen_204", HTTP_GET, redirectToPortal);
  server.on("/hotspot-detect.html", HTTP_GET, sendPortalPage);         // Apple
  server.on("/library/test/success.html", HTTP_GET, redirectToPortal);
  server.on("/connecttest.txt", HTTP_GET, redirectToPortal);           // Windows
  server.on("/ncsi.txt", HTTP_GET, redirectToPortal);

  server.onNotFound(redirectToPortal);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS konnte nicht eingebunden werden.");
  }

  WiFi.mode(WIFI_AP);
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
}
