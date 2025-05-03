#pragma once

#include <HTTPClient.h>

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

const String getAnswer(const String &baseURL, const String &query)
{
    Serial.println("Building URL...");
    String url = baseURL + urlencode(query);
    Serial.print("Request URL: ");
    Serial.println(url);

    HTTPClient http;
    Serial.println("Starting HTTP GET request...");
    http.begin(url);

    int httpCode = http.GET();
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        String response = http.getString();
        Serial.println("Response received:");
        Serial.println(response);
        http.end();
        return response;
    }
    else
    {
        String error = http.errorToString(httpCode).c_str();
        Serial.print("HTTP GET failed: ");
        Serial.println(error);
        http.end();
        return error;
    }
}