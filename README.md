# NetMeteo - Метеостанция на ESP32

Проект метеостанции на ESP32 с TFT-дисплеем, с данными о погоде в реальном времени от API Open-Meteo.
В качестве устройства использовался ESP32-3248S035 (3.5 дюйма).
Пример лота на Ali: https://aliexpress.ru/item/1005008398513238.html?sku_id=12000044859754216&spm=a2g2w.productlist.search_results.6.2bb33d60f5oPXJ

<img width="871" height="582" alt="esp1" src="https://github.com/user-attachments/assets/c4acaf56-a1a4-4d84-bd8c-1088df0339d3" />
<img width="886" height="631" alt="esp2" src="https://github.com/user-attachments/assets/df85f00b-fb29-4478-8bfe-d65f39b3ef69" />

## Возможности

### Основной функционал
- **Отображение погоды в реальном времени**: Температура, влажность, давление и вероятность осадков
- **Прогноз**: На 12 и 24 часа
- **Синхронизация времени NTP**: Автоматическое обновление времени с настраиваемым часовым поясом
- **Статическая конфигурация**: Простая настройка через редактирование кода
- **Автовосстановление**: Автоматическое переподключение и восстановление после ошибок

## Установка

1. **Установите необходимые библиотеки**:
   - TFT_eSPI
   - GyverNTP
   - Arduino_JSON
   - TimerMs

## Конфигурация

### Настройка параметров
Для настройки устройства отредактируйте следующие константы в файле `NetMeteo.ino`:

```cpp
const char* ssid = "SSID";                    // Имя вашей WiFi-сети
const char* password = "PASS";                // Пароль WiFi
const char* latitude = "XX.XXXXX";            // Широта (-90 до 90)
const char* longitude = "XX.XXXXX";           // Долгота (-180 до 180)
const char* timezone = "Europe/Moscow";       // Часовой пояс
const int ntpGmt = 3;                         // Смещение от GMT (-12 до +12)
```

Координаты можно найти на https://www.latlong.net/






