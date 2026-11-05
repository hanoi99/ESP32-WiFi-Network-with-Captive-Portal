// Hier die direkte MP4-Adresse deines eigenen Videos eintragen.
// Beispiel: https://deine-domain.de/video.mp4
// Ein normaler YouTube-/TikTok-Seitenlink ist keine direkte MP4-Datei.
const VIDEO_URL = "/video.mp4";

const goButton = document.getElementById("goButton");
const welcome = document.getElementById("welcome");
const videoPanel = document.getElementById("videoPanel");
const video = document.getElementById("surpriseVideo");

goButton.addEventListener("click", async () => {

    // Klick an den ESP32 melden
    fetch("/api/button-hit", {
    method: "POST",
    cache: "no-store"
    }).catch(error => {
    console.warn("Logging fehlgeschlagen:", error);
    });
    // WLAN-Oberfläche ausblenden
    document.body.classList.add("playing");

    welcome.hidden = true;
    videoPanel.hidden = false;

    // Video konfigurieren
    video.src = VIDEO_URL;
    video.loop = true;
    video.controls = false;
    video.muted = false;
    video.playsInline = true;

    video.setAttribute("playsinline", "");
    video.setAttribute("webkit-playsinline", "");

    // Wiedergabe direkt durch den Klick starten
    try {
        await video.play();
    } catch (error) {
        console.error("Videowiedergabe fehlgeschlagen:", error);
    }
});