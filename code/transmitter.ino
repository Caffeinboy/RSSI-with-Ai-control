// ==========================
// PUBLISHER (TRANSMITTER)
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

char ssid[] = "wifi_userid";
char pass[] = "wifi_password";

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define SLIDER_MIN 0
#define SLIDER_MAX 5

int sliderValue = 0;
int v4Status = 0;
int currentRSSI = -100;

int ai_value = 0;        // V5
int max_people = 0;      // V6

unsigned long previousMillis = 0;
unsigned long wifiRetryMillis = 0;
unsigned long blynkRetryMillis = 0;

// ---------------- WIFI BARS ----------------
void drawWiFiBars(int rssi)
{
  int bars = 0;

  if (rssi > -55) bars = 4;
  else if (rssi > -65) bars = 3;
  else if (rssi > -75) bars = 2;
  else if (rssi > -85) bars = 1;

  int xStart = 100;
  int baseY = 18;

  for (int i = 0; i < 4; i++)
  {
    int h = (i + 1) * 4;

    if (i < bars)
      display.fillRect(xStart + i * 6, baseY - h, 4, h, SSD1306_WHITE);
    else
      display.drawRect(xStart + i * 6, baseY - h, 4, h, SSD1306_WHITE);
  }
}

// ---------------- BOOT ANIMATION ----------------
void bootAnimation()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(8, 16);
  display.println("ESP32 BOOT");

  for (int i = 0; i <= 100; i += 10)
  {
    display.drawRect(14, 48, 100, 10, SSD1306_WHITE);

    int w = map(i, 0, 100, 0, 98);
    display.fillRect(15, 49, w, 8, SSD1306_WHITE);

    display.display();
    delay(120);
  }

  delay(300);
  display.clearDisplay();
  display.display();
}

// ---------------- WIFI CONNECT SCREEN ----------------
void connectWiFi()
{
  WiFi.begin(ssid, pass);

  int anim = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(18, 10);
    display.println("Connecting WiFi");

    for (int i = 0; i < 4; i++)
    {
      if (i <= anim)
        display.fillRect(30 + i * 15, 38 - (i * 5), 8, (i + 1) * 5, SSD1306_WHITE);
      else
        display.drawRect(30 + i * 15, 38 - (i * 5), 8, (i + 1) * 5, SSD1306_WHITE);
    }

    display.display();

    anim++;
    if (anim > 3) anim = 0;

    delay(300);
    Serial.println("Connecting WiFi...");
  }

  Serial.println("WiFi Connected");
}

// ---------------- BLYNK ----------------
BLYNK_WRITE(V3)
{
  if (v4Status == 1)
  {
    sliderValue = param.asInt();
    sliderValue = constrain(sliderValue, SLIDER_MIN, SLIDER_MAX);

    Serial.print("V3 Updated: ");
    Serial.println(sliderValue);
  }
}

BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");

  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
}

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  bootAnimation();
  connectWiFi();

  Blynk.config(BLYNK_AUTH_TOKEN);

  configTime(19800, 0, "pool.ntp.org");

  Serial.println("System Ready");
}

// ---------------- LOOP ----------------
void loop()
{
  // -------- WiFi reconnect every 5 sec --------
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

  // -------- Blynk reconnect every 5 sec --------
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

  // -------- Main task every 1 sec --------
  if (millis() - previousMillis >= 1000)
  {
    previousMillis = millis();

    currentRSSI = WiFi.RSSI();

    // -------- V4 Logic --------
    if (currentRSSI > -60 && v4Status == 0)
    {
      v4Status = 1;
      Serial.println("V4 ACTIVE");
    }
    else if (currentRSSI <= -60 && v4Status == 1)
    {
      sliderValue = 0;
      v4Status = 0;

      Serial.println("V4 INACTIVE -> Slider Reset");
    }

    Blynk.virtualWrite(V4, v4Status);
    Blynk.virtualWrite(V3, sliderValue);

    // Example AI values
    Blynk.virtualWrite(V5, ai_value ? 1 : 0);
    Blynk.virtualWrite(V6, max_people);

    // -------- OLED --------
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

    drawWiFiBars(currentRSSI);

    display.setTextSize(1);

    display.setCursor(0, 28);
    display.print("V4:");
    display.println(v4Status ? "ACT" : "INA");

    display.setCursor(64, 28);
    display.print("V3:");
    display.println(sliderValue);

    display.setCursor(0, 40);
    display.print("AI:");
    display.println(ai_value ? "ON" : "OFF");

    display.setCursor(64, 40);
    display.print("P:");
    display.println(max_people);

    display.setCursor(0, 52);
    display.print("WiFi:");
    display.println(WiFi.status() == WL_CONNECTED ? "OK" : "OFF");

    display.setCursor(64, 52);
    display.print("Blynk:");
    display.println(Blynk.connected() ? "ON" : "OFF");

    display.display();
  }
}
