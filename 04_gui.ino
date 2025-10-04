void guiTimeOutput(Datime now) {
    if (dataRefresh.date) {
    dataRefresh.date = false;
    tft.fillRect(5, 1, 70, 25, TFT_BLACK);
    cPrint(10, 4, getNormalDate(now));
  }
  if (dataRefresh.time) {
    dataRefresh.time = false;
    tft.fillRect(250, 1, 80, 25, TFT_BLACK);
    cPrint(255, 4, getNormalTime(now));
  }
}

void guiMeteoOutput() {
  if (dataRefresh.block0) {
    int temp = round(data.temp0);
    meteoBlockOutput(5, 55, temp, data.humi0, data.press0, data.osad0);
    dataRefresh.block0 = false;
    logMsg("GUI - Block NOW updated");
  }
  if (dataRefresh.block13) {
    int temp = round(data.temp13);
    meteoBlockOutput(5, 195, temp, data.humi13, data.press13, data.osad13);
    dataRefresh.block13 = false;
    logMsg("GUI - Block 12 updated");
  }
  if (dataRefresh.block24) {
    int temp = round(data.temp24);
    meteoBlockOutput(5, 335, temp, data.humi24, data.press24, data.osad24);
    dataRefresh.block24 = false;
    logMsg("GUI - Block 24 updated");
  }
}

void meteoBlockOutput(int x, int y, int temp, int humi, int press, int osad) {
  fillDigitBlocks(x, y);
  
  String myTemp = String(temp);
  String myHumi = String(humi);
  String myOsad = String(osad);
  int xTemp = x;
  int xHumi = x;
  int xOsad = x;

  switch (myTemp.length()) {
    case 0:
    case 1:
      xTemp = xTemp + 32;
      break;
    case 2:
      xTemp = xTemp + 21;
      break;
    case 3:
      xTemp = xTemp + 8;
      break;
  }

  switch (myHumi.length()) {
    case 0:
    case 1:
      xHumi = xHumi + 107;
      break;
    case 2:
      xHumi = xHumi + 96;
      break;
    case 3:
      xHumi = xHumi + 83;
      break;
  }

  switch (myOsad.length()) {
    case 0:
    case 1:
      xOsad = xOsad + 257;
      break;
    case 2:
      xOsad = xOsad + 246;
      break;
    case 3:
      xOsad = xOsad + 233;
      break;
  }

  tft.unloadFont();
  tft.loadFont("Digi48");

  digitPrint(xTemp, y + 53, myTemp, digit1);
  digitPrint(xHumi, y + 53, myHumi, digit2);
  digitPrint(x + 157, y + 53, String(press), digit3);
  digitPrint(xOsad, y + 53, myOsad, digit4);

  tft.unloadFont();
  tft.loadFont("ClearSansBold22");
}

void fillDigitBlocks(int x, int y) {
  tft.fillRect(x + 8, y + 40, 70, 55, digit1);
  tft.fillRect(x + 83, y + 40, 70, 55, digit2);
  tft.fillRect(x + 158, y + 40, 70, 55, digit3);
  tft.fillRect(x + 233, y + 40, 70, 55, digit4);
}

void doHealthGui(uint16_t wifi, uint16_t ntp, uint16_t meteo) {
  tft.fillCircle(65, 469, 8, wifi);
  tft.fillCircle(180, 469, 8, ntp);
  tft.fillCircle(305, 469, 8, meteo);
}
