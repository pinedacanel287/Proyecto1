/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "Poncho_287"
#define IO_KEY       "Llave de adafruit"


#define WIFI_SSID "Javier"
#define WIFI_PASS "Constraseña del Wifi"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
