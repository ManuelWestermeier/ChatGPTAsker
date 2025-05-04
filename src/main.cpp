#include "./connect-to-wifi.hpp"
#include "./get-answer.hpp"

void setup()
{
  Serial.begin(115200);

  connectToWifi();

  const String baseURL = "https://manuelwestermeier.github.io/?q=";
  const String query = "some text";

  getAnswer(baseURL, query);
}

void loop()
{
  // Nothing here
}
