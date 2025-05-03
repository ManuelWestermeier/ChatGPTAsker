#include <WiFi.h>
#include <HTTPClient.h>

#include "./private.hpp"

// API endpoint
const String baseURL = "https://my-api.de/answers?q=";
const String query = "some text";

// Minimal URL encoder
String urlencode(const String &str)
{
  String encoded = "";
  char c;
  char hex[] = "0123456789ABCDEF";
  for (size_t i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (isalnum(c))
    {
      encoded += c;
    }
    else
    {
      encoded += '%';
      encoded += hex[(c >> 4) & 0xF];
      encoded += hex[c & 0xF];
    }
  }
  return encoded;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.println("Sending request...");

  String url = baseURL + urlencode(query);

  HTTPClient http;
  http.begin(url); // HTTPS supported

  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.printf("HTTP Response code: %d\n", httpCode);
    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  }
  else
  {
    Serial.printf("GET failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void loop()
{
  // Nothing in loop
}
