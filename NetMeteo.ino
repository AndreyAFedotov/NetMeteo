#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <GyverNTP.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include <TimerMs.h>
#include <Arduino_JSON.h>

// Статическая конфигурация 
const char* ssid = "SSID";
const char* password = "PASSW";
const char* latitude = "XX.XXXXXX";
const char* longitude = "XX.XXXXXX";
const char* timezone = "Europe/Moscow";
const int ntpGmt = 3;

TFT_eSPI tft = TFT_eSPI();
uint16_t digit1 = tft.color565(29, 51, 74);
uint16_t digit2 = tft.color565(71, 74, 81);
uint16_t digit3 = tft.color565(29, 30, 51);
uint16_t digit4 = tft.color565(39, 49, 63);
uint16_t green = tft.color565(0, 165, 80);
uint16_t yellow = tft.color565(230, 226, 9);
uint16_t red = tft.color565(255, 102, 102);
GyverNTP ntp = GyverNTP();

//NTP GMT будет загружен из конфигурации
int ntpRefresh = 60 * 60;
//URL метео будет строиться динамически
//Таймеры
TimerMs myClock(1000, 1, 0);
TimerMs refreshClock(1000 * 60 * 1, 1, 0);
TimerMs refreshMeteo(1000 * 60 * 1, 1, 0);
TimerMs health(1000 * 60 * 1, 1, 0);
//ЛОГИРОВАНИЕ
boolean doLog = true;

struct myData {
  int hour;
  int minute;
  int second;
  int day;
  int month;
  int year;
  double temp0;
  int humi0;
  int press0;
  int osad0;
  double temp13;
  int humi13;
  int press13;
  int osad13;
  double temp24;
  int humi24;
  int press24;
  int osad24;
} data;

struct myDataRefresh {
  boolean date = false;
  boolean time = false;
  boolean block0 = false;
  boolean block13 = false;
  boolean block24 = false;
} dataRefresh;

int lastMin = 0;

struct startStruct {
  boolean wifi;
  boolean ntp;
  boolean meteo;
} startState;

struct healthStruct {
  int wifi;
  int ntp;
  int meteo;
} healthData;

Datime nowDateTime;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(16, OUTPUT);
  doLED(false, false, true);
  initDisplay();
  initSerial();
  
  // Валидация статической конфигурации
  if (!validateConfiguration()) {
    logMsg("Configuration validation failed, using defaults", "w");
  }
  
  initWiFi();
  initNtp();

  if (refreshMeteoData()) {  
    tft.println("Refresh meteo: OK");
    logMsg("Test meteo: OK");
    startState.meteo = true;
  } else {
    tft.println("Refresh meteo: ERROR");
    logMsg("Test meteo: Error", "e");
    startState.meteo = false;
  }

  if (startState.wifi && startState.ntp && startState.meteo) {
    tft.println("System started!");
  } else {
    tft.println("Starting ERROR. Attempting recovery...");
    doLED(true, false, false);
    logMsg("Starting Error. Attempting recovery...", "e");
    
    // Попытка восстановления перед перезагрузкой
    attemptRecovery();
    
    // Проверка, помогло ли восстановление
    if (WiFi.isConnected()) {
      tft.println("Recovery successful!");
      logMsg("Recovery successful, continuing...");
    } else {
      tft.println("Recovery failed. Restarting...");
      logMsg("Recovery failed. Restarting...", "e");
      delay(5000);
      ESP.restart();
    }
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK, false);
  doLED(false, true, false);
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  doLED(false, false, false);
  logMsg("Started!");
  initGui();
  myClock.force();
  refreshClock.force();
  refreshMeteo.force();
  health.force();
  tickHealth();
}

void loop() {
  // Мониторинг производительности
  monitorPerformance();
  
  ntp.tick();
  if (myClock.tick()) {
    tickMyClock();
    guiMeteoOutput();
  }
  if (refreshClock.tick()) {
    tickNtp();
  }
  if (refreshMeteo.tick()) {
    refreshMeteoData();
  }
  if (health.tick()) {
    tickHealth();
  }
}

void tickMyClock() {
  int min = nowDateTime.minute;
  int day = nowDateTime.day;
  nowDateTime.addSeconds(1);

  guiTimeCheck(min, day, nowDateTime);
  guiTimeOutput(nowDateTime);
  guiMeteoOutput();
}

void tickNtp() {
  int count = 0;
  while (!ntp.updateNow() || count == 30) {
    delay(500);
    count++;
    tickHealth();
  }
  Datime dt(ntp);
  setTimeData(dt);

  if (count == 30) {
    healthData.ntp++;
    logMsg("Ntp update timeout");
  } else {
    healthData.ntp = 0;
    logMsg("Ntp updated");
  }
}

void tickHealth() {
  healthCheck(healthData.wifi, healthData.ntp, healthData.meteo);
}
