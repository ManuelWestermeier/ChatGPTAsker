#include "./private.hpp" // Make sure this defines: const char* ssid; const char* password;

#include "./connect-to-wifi.hpp"
#include "./get-answer.hpp"

void setup()
{
  Serial.begin(115200);
  delay(1000);

  connectToWifi();

  const String baseURL = "https://my-api.de/answers?q=";
  const String query = "some text";

  getAnswer(baseURL, query);
}

void loop()
{
  // Nothing here
}
