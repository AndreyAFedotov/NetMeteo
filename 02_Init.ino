void initSerial() {
  Serial.begin(115200);
  delay(10);
  logMsg("Serial started");
  tft.println("Serial started");
}

void initDisplay() {
  tft.begin();
  tft.setRotation(0);
  // Инициализация файловой системы
  if (!SPIFFS.begin()) {
    while (1) yield();
  }

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, false);
  tft.loadFont("ClearSansBold22");
  logMsg("Display started");
  tft.println("Display started");
}

void initNtp() {
  logMsg("Init ntp...");
  tft.print("Starting NTP");
  ntp.setPeriod(ntpRefresh);
  //ntp.asyncMode(false);
  ntp.ignorePing(true);
  ntp.begin(ntpGmt);

  TimerMs ntpTimerStart(500, 1, 0);
  int attempts = 0;
  while (!ntp.updateNow()) {
    if (ntpTimerStart.tick()) {
      tft.print(".");
      attempts++;
      if (attempts > 60) { // 30 секунд
        logMsg("NTP connection failed. Restarting...");
        ESP.restart();
      }
    }
  }
  tft.println("");
  tft.println("NTP started"); 

  Datime dt (ntp);
  tft.println("Date: " + getNormalDate(dt));
  tft.println("Time: " + getNormalTime(dt));
  startState.ntp = true;
  setTimeData(dt);
  logMsg("Ntp started");
}

void initWiFi() {
  logMsg("Starting wi-fi...");
  tft.print("Starting wi-fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  TimerMs wifiTimerStart(500, 1, 0);
  int wifiAttempts = 0;
  while (!WiFi.isConnected() || WiFi.localIP().toString() == "0.0.0.0") {
    if (wifiTimerStart.tick()) {
      tft.print(".");
      wifiAttempts++;
      if (wifiAttempts > 60) { // 30 секунд
        logMsg("Wi-fi connection failed. Restarting...");
        ESP.restart();
      }
    }
  }
  tft.println("");

  startState.wifi = true;
  logMsg("Wi-fi started");
  tft.println("Wi-Fi started");
}

void initGui() {
  initGuiBlock(5, 55, 0);
  initGuiBlock(5, 195, 13);
  initGuiBlock(5, 335, 24);

  cPrint(5, 460, "wi-fi:");
  cPrint(130, 460, "ntp:");
  cPrint(225, 460, "meteo:");
}

void initGuiBlock(int x, int y, int block) {
  tft.drawRoundRect(x, y, 310, 105, 8, TFT_SILVER);           // Основная рамка для периода
  tft.drawRoundRect(x + 82, y - 22, 150, 30, 8, TFT_SILVER);  // Рамка наименования периода
  tft.fillRect(x + 83, y - 1, 148, 2, TFT_BLACK);

  String name = "";
  int xName = x;
  switch (block) {
    case 0:
      name = "Сейчас";
      xName = xName + 120;
      break;
    case 13:
      name = "12 часов";
      xName = xName + 110;
      break;
    case 24:
      name = "24 часа";
      xName = xName + 116;
      break;
  }

  cPrint(xName, y - 16, name);

  cPrint(x + 15, y + 15, "темп");
  cPrint(x + 88, y + 15, "влаж");
  cPrint(x + 166, y + 15, "давл");
  cPrint(x + 241, y + 15, "осад");

  fillDigitBlocks(x, y);
}