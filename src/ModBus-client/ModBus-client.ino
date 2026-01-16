#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#define SSID "ESP32-Network"
#define PASS "HelloWorld"

#define MODBUS_PORT 502

#define LED_PIN 18

struct ModbusTCP_Master
{
  uint16_t transactionID;
  uint16_t protocolID;
  uint16_t length;
  uint8_t unitID;
  uint8_t code;
  uint16_t regAddr;
  uint16_t regTotal;
};

struct ModbusTCP_Slave
{
  uint16_t transactionID;
  uint16_t protocolID;
  uint16_t length;
  uint8_t unitID;
  uint8_t code;
  uint8_t count;
  uint16_t reg1;
  uint16_t reg2;
};

Adafruit_BME280 bme280;

WiFiClient client;

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  Wire.begin();
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);

  Serial.println("Подключение к WiFi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nПодключено к WiFi");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  Serial.println("Подключение к серверу");

  while (!client.connect("192.168.4.1", MODBUS_PORT))
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Подключено к серверу");

  if (!bme280.begin(0x76))
    Serial.println("Датчик не найден, проверьте соединение");
}

void loop()
{
  if (!client.connected())
    setup();

  float temp = bme280.readTemperature();
  float hum = bme280.readHumidity();

  if (isnan(temp) || isnan(hum))
    return;

  Serial.print("Температура: ");
  Serial.println(temp);
  Serial.print("Влажность: ");
  Serial.println(hum);

  ModbusTCP_Master m;

  client.read((uint8_t *)&m, sizeof(m));
  Serial.print("Принято ");
  Serial.print(sizeof(m));
  Serial.println(" байт");

  if (m.regAddr == 0)
  {
    ModbusTCP_Slave s;

    s.transactionID = m.transactionID;
    s.protocolID = m.protocolID;
    s.length = 7;
    s.unitID = m.unitID;
    s.code = m.code;
    s.count = 4;
    s.reg1 = temp;
    s.reg2 = hum;

    client.write((uint8_t *)&s, sizeof(s));
    Serial.print("Отправлено ");
    Serial.print(sizeof(s));
    Serial.println(" байт");

    delay(1000);
  }
  else if (m.regAddr == 4)
  {
    for (int i = 0; i < 6; i++)
    {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(100);
    }
  }
}