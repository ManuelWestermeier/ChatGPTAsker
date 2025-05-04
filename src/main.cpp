#include "./connect-to-wifi.hpp"
#include "./get-answer.hpp"

void setup()
{
  Serial.begin(115200);

  connectToWifi();

  const String baseURL = "https://progressive-kaela-manuelwestermeier-e8db173d.koyeb.app/?q=";
  const String query = "some text";

  getAnswer(baseURL, query);
}

void loop()
{
  // Nothing here
}
