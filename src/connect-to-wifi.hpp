#pragma once

#include <WiFi.h>
#include <LittleFS.h>
#include <WebServer.h>

#ifdef LED_BUILTIN
#define LED_PIN LED_BUILTIN
#else
#define LED_PIN 2
#endif

#define CRED_FILE "/wifiCreds.txt"
#define AP_SSID "ConfigAP"

struct Credentials
{
    String ssid;
    String password;
};

WebServer server(80);
std::vector<String> ssidList;
bool inAPMode = false;

// Load WiFi credentials from file
bool loadCredentials(Credentials &cred)
{
    if (!LittleFS.exists(CRED_FILE))
    {
        Serial.println("[WiFiManager] No credentials file found. Creating empty one.");
        File file = LittleFS.open(CRED_FILE, "w");
        if (file)
        {
            file.println();
            file.println();
            file.close();
        }
        return false;
    }

    File file = LittleFS.open(CRED_FILE, "r");
    if (!file)
    {
        Serial.println("[WiFiManager] Failed to open credentials file.");
        return false;
    }
    cred.ssid = file.readStringUntil('\n');
    cred.ssid.trim();
    cred.password = file.readStringUntil('\n');
    cred.password.trim();
    file.close();
    return cred.ssid.length() > 0;
}

// Save WiFi credentials to file
void saveCredentials(const Credentials &cred)
{
    File file = LittleFS.open(CRED_FILE, "w");
    if (!file)
    {
        Serial.println("[WiFiManager] Failed to open credentials file for writing.");
        return;
    }
    file.println(cred.ssid);
    file.println(cred.password);
    file.close();
    Serial.println("[WiFiManager] Credentials saved.");
}

// Scan for WiFi networks
void scanNetworks()
{
    ssidList.clear();
    int n = WiFi.scanNetworks();
    if (n <= 0)
    {
        Serial.println("[WiFiManager] No networks found.");
        return;
    }

    Serial.printf("[WiFiManager] Found %d networks:\n", n);
    int bestRSSI = -1000;
    String bestSSID;

    for (int i = 0; i < n; ++i)
    {
        String s = WiFi.SSID(i);
        int32_t r = WiFi.RSSI(i);
        Serial.printf("  %2d: %s (RSSI: %d)\n", i, s.c_str(), r);
        ssidList.push_back(s);
        if (r > bestRSSI && s.length() > 0)
        {
            bestRSSI = r;
            bestSSID = s;
        }
    }

    // Move strongest SSID to front
    if (bestSSID.length())
    {
        auto it = std::find(ssidList.begin(), ssidList.end(), bestSSID);
        if (it != ssidList.end())
        {
            ssidList.erase(it);
            ssidList.insert(ssidList.begin(), bestSSID);
        }
    }
}

// Generate configuration HTML page
String generateHTML()
{
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<style>body{font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}";
    html += ".container{padding:20px;border-radius:8px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
    html += "input, button{width:100%;padding:10px;margin:5px 0;border:1px solid #ccc;border-radius:4px;}";
    html += "button{background:#28a745;color:#fff;border:none;cursor:pointer;}button:hover{background:#218838;}";
    html += "</style><title>WiFi Config</title></head><body><div class='container'><h2>Configure WiFi</h2>";
    html += "<form action='/save' method='POST'>";
    html += "<label>SSID:</label>";
    html += "<input list='ssids' name='ssid' required value='" + (ssidList.size() ? ssidList[0] : String()) + "'>";
    html += "<datalist id='ssids'>";
    for (auto &s : ssidList)
        html += "<option value='" + s + "'/>";
    html += "</datalist>";
    html += "<label>Password:</label>";
    html += "<input type='password' name='password' required>";
    html += "<button type='submit'>Save & Connect</button></form></div></body></html>";
    return html;
}

// Handle root request
void handleRoot()
{
    scanNetworks(); // Refresh SSID list every time
    server.send(200, "text/html", generateHTML());
}

// Handle credentials saving
void handleSave()
{
    if (server.hasArg("ssid") && server.hasArg("password"))
    {
        Credentials cred;
        cred.ssid = server.arg("ssid");
        cred.password = server.arg("password");
        saveCredentials(cred);
        server.send(200, "text/html", "<html><body><h3>Saved. Rebooting...</h3></body></html>");
        delay(2000);
        ESP.restart();
    }
    else
    {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Setup WiFi and either connect or start AP
void connectToWifi()
{
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);
    Serial.println("[WiFiManager] Starting...");

    // Mount LittleFS
    if (!LittleFS.begin())
    {
        Serial.println("[WiFiManager] LittleFS mount failed, formatting...");
        if (!LittleFS.format() || !LittleFS.begin())
        {
            Serial.println("[WiFiManager] LittleFS mount failed after format. Halting.");
            digitalWrite(LED_PIN, HIGH);
            while (true)
                delay(1000);
        }
    }

    WiFi.mode(WIFI_STA);
    scanNetworks();

    Credentials cred;
    if (loadCredentials(cred))
    {
        Serial.printf("[WiFiManager] Connecting to: %s\n", cred.ssid.c_str());
        WiFi.begin(cred.ssid.c_str(), cred.password.c_str());

        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20)
        {
            delay(500);
            Serial.print(".");
            retries++;
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("\n[WiFiManager] Connected! IP: " + WiFi.localIP().toString());
            digitalWrite(LED_PIN, LOW);
            return;
        }
        else
        {
            Serial.println("\n[WiFiManager] Failed to connect. Falling back to AP.");
        }
    }

    // Fallback to Access Point mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    inAPMode = true;

    Serial.println("[WiFiManager] AP Mode started. Connect to SSID: " AP_SSID);
    Serial.println("[WiFiManager] Visit http://192.168.4.1 to configure.");

    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();

    while (inAPMode)
    {
        server.handleClient();
    }
}