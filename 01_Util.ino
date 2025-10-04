void setTimeData(Datime dt) {
  logMsg("New time data: " + dt.toString());

  if (nowDateTime.hour != dt.hour) {
    nowDateTime.hour = dt.hour;
    dataRefresh.time = true;
  }
  if (nowDateTime.minute != dt.minute) {
    nowDateTime.minute = dt.minute;
    dataRefresh.time = true;
  }
  if (nowDateTime.day != dt.day) {
    nowDateTime.day = dt.day;
    dataRefresh.date = true;
  }
  if (nowDateTime.month != dt.month) {
    nowDateTime.month = dt.month;
    dataRefresh.date = true;
  }
  if (nowDateTime.year != dt.year) {
    nowDateTime.year = dt.year;
    dataRefresh.date = true;
  }
  if (nowDateTime.second != dt.second) {
    nowDateTime.second = dt.second;
  }
}

String getNormalDate(Datime dt) {
  String month = "";
  String day = "";

  if (dt.month < 10) {
    month = "0" + String(dt.month);
  } else {
    month = dt.month;
  }

  if (dt.day < 10) {
    day = "0" + String(dt.day);
  } else {
    day = dt.day;
  }

  //return day + "." + month + "." + dt.year;
  return day + "." + month;
}

String getNormalTime(Datime dt) {
  String hour = "";
  String minute = "";

  if (dt.hour < 10) {
    hour = "0" + String(dt.hour);
  } else {
    hour = dt.hour;
  }

  if (dt.minute < 10) {
    minute = "0" + String(dt.minute);
  } else {
    minute = dt.minute;
  }

  return hour + ":" + minute;
}

void doLED(boolean red, boolean green, boolean blue) {
  if (red) {
    digitalWrite(4, LOW);
  } else {
    digitalWrite(4, HIGH);
  }
  if (green) {
    digitalWrite(16, LOW);
  } else {
    digitalWrite(16, HIGH);
  }
  if (blue) {
    digitalWrite(17, LOW);
  } else {
    digitalWrite(17, HIGH);
  }
}

void cPrint(uint16_t color, int x, int y, String message) {
  tft.setTextColor(color, TFT_BLACK, false);
  tft.setCursor(x, y);
  tft.print(message);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, false);
}

void digitPrint(int x, int y, String message, uint16_t color) {
  tft.setTextColor(TFT_WHITE, TFT_SILVER, false);
  tft.setCursor(x, y);
  tft.print(message);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, false);
}

void cPrint(int x, int y, String message) {
  tft.setCursor(x, y);
  tft.print(message);
}

void logMsg(String msg) {
  logMsg(msg, "I");
}

void logMsg(String msg, String typeM) {
  if (doLog) {
    String typeStr = typeM;
    typeStr.toUpperCase();
    if (typeStr == "I") {
      typeStr = "INFO";
    } else if (typeStr == "W") {
      typeStr = "WARN";
    } else if (typeStr == "E") {
      typeStr = "ERRO";
    }

    Serial.println("[" + nowDateTime.toString() + "][" + typeStr + "] " + msg);
  }
}

// ============================================================================
// Дополнительные утилитарные функции для производительности и надежности
// ============================================================================


// Безопасные операции со строками для предотвращения переполнения буферов
void safeStringCopy(char* dest, const char* src, size_t destSize) {
  if (dest && src && destSize > 0) {
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
  }
}

// Валидация числового ввода
bool isValidNumber(const String& str) {
  if (str.length() == 0) return false;
  
  for (size_t i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i)) && str.charAt(i) != '.' && str.charAt(i) != '-') {
      return false;
    }
  }
  return true;
}

// Валидация диапазонов координат
bool isValidCoordinate(const String& coord, bool isLatitude) {
  if (!isValidNumber(coord)) return false;
  
  float value = coord.toFloat();
  if (isLatitude) {
    return value >= -90.0 && value <= 90.0;
  } else {
    return value >= -180.0 && value <= 180.0;
  }
}

// Безопасный доступ к массиву с проверкой границ
template<typename T>
bool safeArrayAccess(T* array, int index, int arraySize, T& value) {
  if (array && index >= 0 && index < arraySize) {
    value = array[index];
    return true;
  }
  return false;
}

// Оценка качества сети
int getNetworkQuality() {
  if (!WiFi.isConnected()) return 0;
  
  int rssi = WiFi.RSSI();
  if (rssi > -30) return 5;      // Отлично
  else if (rssi > -50) return 4; // Очень хорошо
  else if (rssi > -70) return 3; // Хорошо
  else if (rssi > -80) return 2; // Удовлетворительно
  else if (rssi > -90) return 1; // Плохо
  else return 0;                 // Очень плохо
}

// Оценка состояния системы
String getSystemHealth() {
  int wifiQuality = getNetworkQuality();
  int freeMemory = ESP.getFreeHeap();
  int uptime = millis() / 1000;
  
  String health = "Good";
  if (wifiQuality < 2 || freeMemory < 10000) {
    health = "Poor";
  } else if (wifiQuality < 3 || freeMemory < 20000) {
    health = "Fair";
  }
  
  return health;
}

// Механизмы восстановления ошибок
void attemptRecovery() {
  logMsg("Attempting system recovery...", "w");
  
  // Попытка переподключения к WiFi
  if (!WiFi.isConnected()) {
    logMsg("Reconnecting to WiFi...");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (!WiFi.isConnected() && attempts < 30) {
      delay(1000);
      attempts++;
    }
    
    if (WiFi.isConnected()) {
      logMsg("WiFi reconnected successfully");
    } else {
      logMsg("WiFi reconnection failed", "e");
    }
  }
  
  // Очистка зависших таймеров
  myClock.restart();
  refreshClock.restart();
  refreshMeteo.restart();
  health.restart();
  
  logMsg("Recovery attempt completed");
}

// Мониторинг производительности
void monitorPerformance() {
  static unsigned long lastCheck = 0;
  static int loopCount = 0;
  
  unsigned long now = millis();
  loopCount++;
  
  if (now - lastCheck > 60000) { // Каждую минуту
    float loopsPerSecond = loopCount / 60.0;
    logMsg("Performance: " + String(loopsPerSecond, 1) + " loops/sec");
    
    // Проверка проблем производительности
    if (loopsPerSecond < 10) {
      logMsg("Performance warning: Low loop frequency", "w");
    }
    
    if (ESP.getFreeHeap() < 10000) {
      logMsg("Memory warning: Low free heap", "w");
    }
    
    loopCount = 0;
    lastCheck = now;
  }
}

// Вспомогательные функции валидации данных
bool validateMeteoData(double temp, int humi, double press, int osad) {
  return (temp >= -50 && temp <= 60 &&
          humi >= 0 && humi <= 100 &&
          press >= 800 && press <= 1200 &&
          osad >= 0 && osad <= 100);
}

// Безопасный парсинг JSON с обработкой ошибок
bool safeJsonParse(const String& jsonString, JSONVar& result) {
  if (jsonString.length() == 0) {
    logMsg("JSON parsing failed: Empty string", "e");
    return false;
  }
  
  result = JSON.parse(jsonString);
  if (JSON.typeof(result) == "undefined") {
    logMsg("JSON parsing failed: Invalid JSON", "e");
    return false;
  }
  
  return true;
}

// Валидация конфигурации (упрощено для статической конфигурации)
bool validateConfiguration() {
  // Проверка разумности значений статической конфигурации
  if (strlen(ssid) == 0 || strlen(password) == 0) {
    logMsg("Configuration validation failed: WiFi credentials missing", "e");
    return false;
  }
  
  if (!isValidCoordinate(String(latitude), true) || 
      !isValidCoordinate(String(longitude), false)) {
    logMsg("Configuration validation failed: Invalid coordinates", "e");
    return false;
  }
  
  if (ntpGmt < -12 || ntpGmt > 12) {
    logMsg("Configuration validation failed: Invalid NTP GMT offset", "e");
    return false;
  }
  
  return true;
}