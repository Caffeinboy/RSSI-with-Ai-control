// ==========================
// RECEIVER
// ==========================

#define BLYNK_TEMPLATE_ID "TMPL3y7-name"
#define BLYNK_TEMPLATE_NAME "publisher"
#define BLYNK_AUTH_TOKEN "vo8Nm7akPFDS"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

char ssid[] = "wifi_username";
char pass[] = "wifi_password";

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- RELAY PINS ----------------
#define RELAY1 5
#define RELAY2 18
#define RELAY3 19
#define RELAY4 23
#define RELAY5 25

int sliderValue = 0;
int v4Status = 0;
int v5AIStatus = 0;
int v6HeadCount = 0;

unsigned long previousMillis = 0;
unsigned long wifiRetryMillis = 0;
unsigned long blynkRetryMillis = 0;

// ---------------- RELAY CONTROL ----------------
void updateRelays()
{
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);
  digitalWrite(RELAY5, LOW);

  if (v4Status == 0) return;

  switch (sliderValue)
  {
    case 1: digitalWrite(RELAY1, HIGH); break;
    case 2: digitalWrite(RELAY2, HIGH); break;
    case 3: digitalWrite(RELAY3, HIGH); break;
    case 4: digitalWrite(RELAY4, HIGH); break;
    case 5: digitalWrite(RELAY5, HIGH); break;
  }
}

// ---------------- BLYNK ----------------
BLYNK_WRITE(V3)
{
  sliderValue = param.asInt();
  Serial.print("V3 Received: ");
  Serial.println(sliderValue);

  updateRelays();
}

BLYNK_WRITE(V4)
{
  v4Status = param.asInt();
  Serial.print("V4 Received: ");
  Serial.println(v4Status);

  updateRelays();
}

BLYNK_WRITE(V5)
{
  v5AIStatus = param.asInt();

  Serial.print("V5 AI Status: ");
  Serial.println(v5AIStatus);
}

BLYNK_WRITE(V6)
{
  v6HeadCount = param.asInt();

  Serial.print("V6 Headcount: ");
  Serial.println(v6HeadCount);
}

BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");

  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
}

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(RELAY5, OUTPUT);

  updateRelays();

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  configTime(19800, 0, "pool.ntp.org");

  Serial.println("Receiver Ready");
}

// ---------------- LOOP ----------------
void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - wifiRetryMillis > 5000)
    {
      wifiRetryMillis = millis();

      Serial.println("Reconnecting WiFi...");
      WiFi.disconnect();
      WiFi.begin(ssid, pass);
    }
  }

  if (WiFi.status() == WL_CONNECTED && !Blynk.connected())
  {
    if (millis() - blynkRetryMillis > 5000)
    {
      blynkRetryMillis = millis();

      Serial.println("Reconnecting Blynk...");
      Blynk.connect(1000);
    }
  }

  if (Blynk.connected())
    Blynk.run();

  if (millis() - previousMillis >= 1000)
  {
    previousMillis = millis();

    display.clearDisplay();

    struct tm tinfo;

    if (getLocalTime(&tinfo))
    {
      display.setTextSize(2);
      display.setCursor(0, 0);

      display.printf("%02d:%02d:%02d",
                      tinfo.tm_hour,
                      tinfo.tm_min,
                      tinfo.tm_sec);
    }

    display.setTextSize(1);

    display.setCursor(0, 28);
    display.print("V4:");
    display.println(v4Status ? "ACT" : "INA");

    display.setCursor(64, 28);
    display.print("V3:");
    display.println(sliderValue);

    display.setCursor(0, 40);
    display.print("AI:");
    display.println(v5AIStatus ? "ON" : "OFF");

    display.setCursor(64, 40);
    display.print("Heads:");
    display.println(v6HeadCount);

    display.setCursor(0, 52);
    display.print("WiFi:");
    display.println(WiFi.status() == WL_CONNECTED ? "OK" : "OFF");

    display.setCursor(64, 52);
    display.print("Blynk:");
    display.println(Blynk.connected() ? "ON" : "OFF");

    display.display();
  }
}
