
#include <IotWebConf.h>
#include <ThingsBoard.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "CO2 CRIF";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "educamadrid";

#define STRING_LEN 128

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "003"

char thingsboardServer[] = "tb.iotopentech.io";

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Callback method declarations.
void wifiConnected();
void configSaved();
boolean formValidator();


DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;
WiFiClient net;
ThingsBoard tb(net);

//char mqttServerValue[STRING_LEN];
char mqttUserNameValue[STRING_LEN];
//char mqttUserPasswordValue[STRING_LEN];

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
//IotWebConfParameter mqttServerParam = IotWebConfParameter("MQTT server", "mqttServer", mqttServerValue, STRING_LEN);
IotWebConfParameter mqttUserNameParam = IotWebConfParameter("MQTT user", "mqttUser", mqttUserNameValue, STRING_LEN);
//IotWebConfParameter mqttUserPasswordParam = IotWebConfParameter("MQTT password", "mqttPass", mqttUserPasswordValue, STRING_LEN, "password");

boolean needMqttConnect = false;
boolean needReset = false;

unsigned long lastReport = 0;
unsigned long lastMqttConnectionAttempt = 0;

int status = WL_IDLE_STATUS;
unsigned long lastSend;

//////////////////////////////////////////////////////////////////////////
// set SCD30 driver debug level (only NEEDED case of errors)            //
// Requires serial monitor (remove DTR-jumper before starting monitor)  //
// 0 : no messages                                                      //
// 1 : request sending and receiving                                    //
// 2 : request sending and receiving + show protocol errors             //
//////////////////////////////////////////////////////////////////////////
#define scd_debug 0
#define SCD30WIRE Wire
#include "paulvha_SCD30.h"

int co2;
float temperatura;
float humedad;
bool parpadeo = true;

bool res;

SCD30 airSensor;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define educamadrid_width 48
#define educamadrid_height 45
static unsigned char educamadrid_bits[] = {
  0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0xff,
  0xff, 0xff, 0xe0, 0x00, 0x03, 0xff, 0x80, 0x0f, 0xf0, 0x00, 0x07, 0xfc, 0x1c, 0x03, 0xf8, 0x00,
  0x0f, 0xf8, 0x3e, 0x01, 0xfc, 0x00, 0x0f, 0xe0, 0x3f, 0x00, 0xfc, 0x38, 0x1f, 0xc0, 0x3f, 0x00,
  0x7e, 0x7c, 0x3f, 0xc0, 0x3f, 0x80, 0x7e, 0xfc, 0x3f, 0x80, 0x3f, 0xc0, 0x3e, 0xfe, 0x3f, 0x00,
  0x3f, 0xc6, 0x3e, 0x7e, 0x7f, 0x00, 0x3f, 0xfe, 0x3e, 0x7e, 0x7e, 0x00, 0x3f, 0xfe, 0x3e, 0x7f,
  0x7e, 0x00, 0x7d, 0xfe, 0x7e, 0x3f, 0xfc, 0x03, 0xfc, 0xfc, 0x7c, 0x3f, 0xfc, 0x3f, 0xfc, 0xe0,
  0xfc, 0x3f, 0xfc, 0x7f, 0xfc, 0x03, 0xf8, 0x3f, 0xfc, 0xff, 0xe0, 0x07, 0xf0, 0x3f, 0xfc, 0xff,
  0x80, 0x0f, 0xe0, 0x3f, 0xfc, 0x7f, 0xe0, 0x1f, 0x80, 0x3f, 0xfc, 0x3f, 0xf8, 0x0f, 0xc0, 0x3f,
  0xfc, 0x0f, 0xfc, 0x07, 0xc0, 0x3f, 0xfc, 0x03, 0xf8, 0xc7, 0xc0, 0x3f, 0xfc, 0x00, 0x78, 0xf3,
  0xe0, 0x3f, 0xfe, 0x00, 0x79, 0xff, 0xf0, 0x3e, 0x7e, 0x00, 0x7f, 0xff, 0xf0, 0x7e, 0x7f, 0x00,
  0x7f, 0xff, 0xf8, 0x7e, 0x7f, 0x00, 0x7f, 0xdf, 0xf8, 0x7c, 0x3f, 0x80, 0x7f, 0x87, 0xf8, 0xfc,
  0x3f, 0xc0, 0x7f, 0x80, 0xf1, 0xfc, 0x1f, 0xe0, 0x7f, 0x00, 0x03, 0xf8, 0x1f, 0xf0, 0x7e, 0x00,
  0x07, 0xf0, 0x0f, 0xf8, 0x7c, 0x00, 0x0f, 0xf0, 0x07, 0xfc, 0x38, 0x00, 0x1f, 0xe0, 0x03, 0xff,
  0x80, 0x00, 0x7f, 0xc0, 0x01, 0xff, 0xe0, 0x01, 0xff, 0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x07, 0xff, 0xff,
  0xe0, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00
};

#define crif_width 50
#define crif_height 48
static unsigned char crif_bits[] = {
  // 'crif', 50x48px
  0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00,
  0x00, 0x00, 0xf0, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x01, 0xf8, 0x00, 0xc0, 0x07, 0xe0, 0x00, 0x01,
  0xf8, 0x00, 0xc0, 0x07, 0xe0, 0x00, 0x01, 0xf8, 0x00, 0xc0, 0x07, 0xe0, 0x00, 0x01, 0xf8, 0x00,
  0xc0, 0x07, 0xe0, 0x00, 0x00, 0xfc, 0x00, 0xc0, 0x0f, 0xc0, 0x00, 0x00, 0x1e, 0x00, 0xc0, 0x1e,
  0x00, 0x00, 0x00, 0x0e, 0x00, 0xc0, 0x1c, 0x00, 0x00, 0x00, 0x0f, 0x00, 0xc0, 0x3c, 0x00, 0x00,
  0x00, 0x07, 0x00, 0xc0, 0x38, 0x00, 0x00, 0x00, 0x07, 0x00, 0xc0, 0x38, 0x00, 0x00, 0x00, 0x03,
  0x80, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x03, 0x80, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x03, 0x80, 0xc0,
  0x70, 0x00, 0x00, 0x00, 0x03, 0x80, 0xc0, 0x70, 0x00, 0x00, 0x7c, 0x03, 0x80, 0xc0, 0x70, 0x0f,
  0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x7e, 0x00, 0x01, 0xe0, 0x00, 0x1f, 0x80, 0x3c, 0x00, 0x00,
  0xc0, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xc0, 0x3e, 0x00, 0x00, 0x00, 0x0f, 0xc0,
  0xc0, 0xfc, 0x00, 0x00, 0x00, 0x07, 0xfd, 0xef, 0xf8, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00
};
#define wifi1_icon16x16_width 16
#define wifi1_icon16x16_height 16
unsigned char wifi1_icon16x16[] =
{
  0b00000000, 0b00000000, //
  0b00000111, 0b11100000, //      ######
  0b00011111, 0b11111000, //    ##########
  0b00111111, 0b11111100, //   ############
  0b01110000, 0b00001110, //  ###        ###
  0b01100111, 0b11100110, //  ##  ######  ##
  0b00001111, 0b11110000, //     ########
  0b00011000, 0b00011000, //    ##      ##
  0b00000011, 0b11000000, //       ####
  0b00000111, 0b11100000, //      ######
  0b00000100, 0b00100000, //      #    #
  0b00000001, 0b10000000, //        ##
  0b00000001, 0b10000000, //        ##
  0b00000000, 0b00000000, //
  0b00000000, 0b00000000, //
  0b00000000, 0b00000000, //
};

#define arrow_up_icon16x16_width 16
#define arrow_up_icon16x16_height 16
unsigned char arrow_up_icon16x16[] =
{
  0b00000001, 0b10000000, //        ##
  0b00000011, 0b11000000, //       ####
  0b00000111, 0b11100000, //      ######
  0b00001111, 0b11110000, //     ########
  0b00011110, 0b01111000, //    ####  ####
  0b00111100, 0b00111100, //   ####    ####
  0b01111000, 0b00011110, //  ####      ####
  0b11111100, 0b00111111, // ######    ######
  0b11111100, 0b00111111, // ######    ######
  0b01111100, 0b00111110, //  #####    #####
  0b00011100, 0b00111000, //    ###    ###
  0b00011100, 0b00111000, //    ###    ###
  0b00011100, 0b00111000, //    ###    ###
  0b00011111, 0b11111000, //    ##########
  0b00011111, 0b11111000, //    ##########
  0b00001111, 0b11110000, //     ########
};

#define cancel_icon16x16_width 16
#define cancel_icon16x16_height 16
unsigned char cancel_icon16x16[] =
{
  0b00000000, 0b00000000, //
  0b00000000, 0b00000000, //
  0b00111000, 0b00001110, //   ###       ###
  0b00111100, 0b00011110, //   ####     ####
  0b00111110, 0b00111110, //   #####   #####
  0b00011111, 0b01111100, //    ##### #####
  0b00001111, 0b11111000, //     #########
  0b00000111, 0b11110000, //      #######
  0b00000011, 0b11100000, //       #####
  0b00000111, 0b11110000, //      #######
  0b00001111, 0b11111000, //     #########
  0b00011111, 0b01111100, //    ##### #####
  0b00111110, 0b00111110, //   #####   #####
  0b00111100, 0b00011110, //   ####     ####
  0b00111000, 0b00001110, //   ###       ###
  0b00000000, 0b00000000, //
};
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  pinMode(12, OUTPUT);//VERDE
  pinMode(13, OUTPUT);//ROJO
  pinMode(14, OUTPUT);//AZUL

  //iotWebConf.addParameter(&mqttServerParam);
  iotWebConf.addParameter(&mqttUserNameParam);
  //iotWebConf.addParameter(&mqttUserPasswordParam);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.setupUpdateServer(&httpUpdater);
  iotWebConf.setApTimeoutMs(120000);



  // -- Initializing the configuration.
  boolean validConfig = iotWebConf.init();
  if (!validConfig)
  {
    //mqttServerValue[0] = '\0';
    mqttUserNameValue[0] = '\0';
    //mqttUserPasswordValue[0] = '\0';
  }

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", [] { iotWebConf.handleConfig(); });
  server.onNotFound([]() {
    iotWebConf.handleNotFound();
  });

  Serial.println("Ready.");
  lastSend = 0;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

  // Clear the buffer.
  display.clearDisplay();
  display.drawBitmap(8, 1,  educamadrid_bits, 48, 45, 1);
  display.display();
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  delay(1000);
  analogWrite(12, 220);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  delay(1000);
  display.clearDisplay();
  display.drawBitmap(7, 0,  crif_bits, 50, 48, 1);
  display.display();
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  delay(1000);
  analogWrite(12, 220);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  delay(1000);
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);



  airSensor.setDebug(scd_debug);

  //This will cause readings to occur every two seconds
  if (! airSensor.begin(SCD30WIRE))
  {
    Serial.println(F("The SCD30 did not respond. Please check wiring."));
    while (1);
  }

  // display device info
  DeviceInfo();
}

void loop()
{
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
  tb.loop();
  if (airSensor.dataAvailable())
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    co2 = airSensor.getCO2();
    temperatura = airSensor.getTemperature();
    humedad = airSensor.getHumidity();
    display.print("CO2: ");
    display.println(co2);
    display.setCursor(0, 11);
    display.print("Temp: ");
    display.println(temperatura, 1);
    display.setCursor(0, 22);
    display.print("Hum: ");
    display.println(humedad, 1);
    if (iotWebConf.getState() == IOTWEBCONF_STATE_ONLINE ) {
      display.drawBitmap(5, 31, wifi1_icon16x16, 16, 16, 1);
    } else if (iotWebConf.getState() != IOTWEBCONF_STATE_ONLINE ) {
      display.drawBitmap(5, 31, cancel_icon16x16, 16, 16, 1);
    }
    if ( needMqttConnect ) {
      if (tb.connected())
      {
        Serial.println("MQTT connected");
        needMqttConnect = false;

      } else if ((iotWebConf.getState() == IOTWEBCONF_STATE_ONLINE) && (!tb.connected()))
      {
        Serial.println("MQTT reconnect");
        reconnect();
      }
    }
    if (tb.connected()) {
      if ( millis() - lastSend > 30000 ) { // Update and send only after 1 seconds
        display.drawBitmap(43, 31, arrow_up_icon16x16, 16, 16, 1);
        display.display();
        myIoT();
        lastSend = millis();
      }
    }
    display.display();
  }
  if (co2 < 1000) {
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    digitalWrite(14, LOW);

  } else if (co2 < 2000) {
    if (parpadeo) {
      analogWrite(12, 220);
      digitalWrite(13, HIGH);
      digitalWrite(14, LOW);
    } else {
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      digitalWrite(14, LOW);
    }
    parpadeo = !parpadeo;
    delay(500);
  } else {

    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    digitalWrite(14, LOW);
  }


  if (needReset)
  {
    Serial.println("Rebooting after 1 second.");
    iotWebConf.delay(1000);
    ESP.restart();
  }

  unsigned long now = millis();

}

/**
   Handle web requests to "/" path.
*/
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 06 MQTT App</title></head><body>MQTT App demo";
  s += "<ul>";
  s += "<li>MQTT server: ";
  s += "tb.iotopentech.io";
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void wifiConnected()
{
  Serial.println("Conectado por WiFi");
  needMqttConnect = true;
}

void configSaved()
{
  Serial.println("Configuration was updated.");
  needReset = true;
}

boolean formValidator()
{
  Serial.println("Validating form.");
  boolean valid = true;
  /*
    int l = server.arg(mqttServerParam.getId()).length();
    if (l < 3)
    {
      mqttServerParam.errorMessage = "Please provide at least 3 characters!";
      valid = false;
    }
  */
  return valid;
}

void myIoT()
{
  Serial.println("Enviando datos a myIoT.");
  tb.sendTelemetryInt("CO2", co2);
  tb.sendTelemetryFloat("temperatura", temperatura);
  tb.sendTelemetryFloat("humedad", humedad);

}
void DeviceInfo()
{
  uint8_t val[2];
  char buf[(SCD30_SERIAL_NUM_WORDS * 2) + 1];

  // Read SCD30 serial number as printed on the device
  // buffer MUST be at least 33 digits (32 serial + 0x0)
  if (airSensor.getSerialNumber(buf))
  {
    Serial.print(F("SCD30 serial number : "));
    Serial.println(buf);
  }

  // read Firmware level
  if ( airSensor.getFirmwareLevel(val) ) {
    Serial.print("SCD30 Firmware level: Major: ");
    Serial.print(val[0]);

    Serial.print("\t, Minor: ");
    Serial.println(val[1]);
  }
  else {
    Serial.println("Could not obtain firmware level");
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, mqttUserNameValue) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
