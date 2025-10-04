int meteoRespCode;
WiFiClient wifiClient;

String buildMeteoUrl() {
  String url = "http://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(latitude);
  url += "&longitude=" + String(longitude);
  url += "&timezone=" + String(timezone);
  url += "&hourly=temperature_2m,relative_humidity_2m,surface_pressure,precipitation_probability";
  url += "&forecast_hours=25&temperature_unit=celsius";
  return url;
}

boolean refreshMeteoData() {
  logMsg("Refresh Meteo: start");
  String jsonBuffer;
  String serverPath = buildMeteoUrl();
  jsonBuffer = GET_Request(serverPath.c_str());
  
  // Валидация длины ответа
  if (jsonBuffer.length() < 100) {
    logMsg("Refresh Meteo: response too short", "e");
    return false;
  }
  
  JSONVar myObject;
  boolean status = true;

  if (!safeJsonParse(jsonBuffer, myObject)) {
    logMsg("Refresh Meteo: parsing failed", "e");
    return false;
  } else {

    if (meteoRespCode == 200) {
      logMsg("Refresh Meteo: code is " + String(meteoRespCode));

      JSONVar timeArray = myObject["hourly"]["time"];
      String timeData[25];
      for (int i = 0; i < timeArray.length(); i++) {
        timeData[i] = (const char*)timeArray[i];
      }

      JSONVar tempArray = myObject["hourly"]["temperature_2m"];
      double tempData[25];
      for (int i = 0; i < tempArray.length(); i++) {
        tempData[i] = (double)tempArray[i];
      }

      JSONVar humiArray = myObject["hourly"]["relative_humidity_2m"];
      int humiData[25];
      for (int i = 0; i < humiArray.length(); i++) {
        humiData[i] = (int)humiArray[i];
      }

      JSONVar pressArray = myObject["hourly"]["surface_pressure"];
      double pressData[25];
      for (int i = 0; i < pressArray.length(); i++) {
        pressData[i] = (double)pressArray[i];
      }

      JSONVar osadArray = myObject["hourly"]["precipitation_probability"];
      int osadData[25];
      for (int i = 0; i < osadArray.length(); i++) {
        osadData[i] = (int)osadArray[i];
      }

      if (osadArray.length() == 25 && tempArray.length() == 25 && humiArray.length() == 25 && pressArray.length() == 25) {
        // Валидация диапазонов данных
        bool dataValid = true;
        for (int i = 0; i < 25; i++) {
          if (!validateMeteoData(tempData[i], humiData[i], pressData[i], osadData[i])) {
            logMsg("Refresh Meteo: invalid data range at index " + String(i), "w");
            dataValid = false;
            break;
          }
        }
        
        if (dataValid) {
          logMsg("Refresh Meteo: all data block is OK. First: " + String(timeData[0]) + " and last: " + String(timeData[24]));
          guiMeteoCheck(timeData, tempData, humiData, pressData, osadData);
        } else {
          logMsg("Refresh Meteo: data validation failed", "e");
          status = false;
        }
      } else {
        logMsg("Refresh Meteo: bad data block", "e");
        status = false;
      }

    } else {
      logMsg("Refresh Meteo: code is " + String(meteoRespCode), "e");
      status = false;
    }
  }
  logMsg("Refresh Meteo: end");
  return status;
}

String GET_Request(const char* server) {
  HTTPClient http;
  String payload = "{}";
  int httpResponseCode = -1;
  int retryCount = 0;
  const int maxRetries = 3; // Максимальное количество попыток
  
  while (retryCount < maxRetries) {
    http.begin(wifiClient, server);
    http.setTimeout(10000); // 10 секунд таймаут
    httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      payload = http.getString();
      if (httpResponseCode == 200) {
        healthData.meteo = 0;
        break; // Успех
      } else {
        logMsg("HTTP Error " + String(httpResponseCode) + " (attempt " + String(retryCount + 1) + "): " + payload, "w");
        healthData.meteo++;
      }
    } else {
      logMsg("HTTP Request failed (attempt " + String(retryCount + 1) + "): " + String(httpResponseCode), "e");
      healthData.meteo++;
    }
    
    http.end();
    retryCount++;
    
    if (retryCount < maxRetries) {
      delay(2000); // Ждем 2 секунды перед повтором
    }
  }
  
  tickHealth();
  meteoRespCode = httpResponseCode;
  return payload;
}

void guiMeteoCheck(String timeData[25], double tempData[25], int humiData[25], double pressData[25], int osadData[25]) {
  // 0
  if(data.temp0 != tempData[0]) {
    data.temp0 = tempData[0];
    dataRefresh.block0 = true;
  }
  if(data.humi0 != humiData[0]) {
    data.humi0 = humiData[0];
    dataRefresh.block0 = true;
  }
  int press0 = pressData[0] * 0.750063755419211; // из hPa в мм рт ст 
  if(data.press0 != press0) {
    data.press0 = press0;
    dataRefresh.block0 = true;
  }
  if(data.osad0 != osadData[0]) {
    data.osad0 = osadData[0];
    dataRefresh.block0 = true;
  }
  // 13
  if(data.temp13 != tempData[13]) {
    data.temp13 = tempData[13];
    dataRefresh.block13 = true;
  }
  if(data.humi13 != humiData[13]) {
    data.humi13 = humiData[13];
    dataRefresh.block13 = true;
  }
  int press13 = pressData[13] * 0.750063755419211; // из hPa в мм рт ст 
  if(data.press13 != press13) {
    data.press13 = press13;
    dataRefresh.block13 = true;
  }
  if(data.osad13 != osadData[13]) {
    data.osad13 = osadData[13];
    dataRefresh.block13 = true;
  }
  // 24
    if(data.temp24 != tempData[24]) {
    data.temp24 = tempData[24];
    dataRefresh.block24 = true;
  }
  if(data.humi24 != humiData[24]) {
    data.humi24 = humiData[24];
    dataRefresh.block24 = true;
  }
  int press24 = pressData[24] * 0.750063755419211; // из hPa в мм рт ст 
  if(data.press24 != press24) {
    data.press24 = press24;
    dataRefresh.block24 = true;
  }
  if(data.osad24 != osadData[24]) {
    data.osad24 = osadData[24];
    dataRefresh.block24 = true;
  }
}

void guiTimeCheck(int min, int day, Datime now) {
  if (day != now.day) {
    dataRefresh.date = true;
  }
  if (min != now.minute) {
    dataRefresh.time = true;
  }
}

void healthCheck(int wifi, int ntp, int meteo) {
  if (!WiFi.isConnected()) {
    wifi++;
  } else {
    wifi = 0;
  }

  if (wifi > 2) {
    logMsg("WIFI connection failed. Restarting...");
    ESP.restart();
  }

  doHealthGui(setHealthColor(wifi), setHealthColor(ntp), setHealthColor(meteo));
}

uint16_t setHealthColor (int val) {
  if (val == 0) {
    return green;
  } else if (val == 1) {
    return yellow;
  } else {
    return red;
  }
}