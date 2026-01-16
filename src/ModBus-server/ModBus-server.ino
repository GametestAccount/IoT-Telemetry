#include <WiFi.h>
#include <WebSocketsServer.h>

#define SSID "ESP32-Network"
#define PASS "HelloWorld"

#define MODBUS_PORT 502

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

WiFiServer server(MODBUS_PORT);
WebSocketsServer ws(40001);

WiFiClient client;

uint16_t transactionID = 0;

void ws_callback(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if ((type == WStype_TEXT) && (length > 0))
  {
    ModbusTCP_Master m;

    m.transactionID = transactionID;
    m.protocolID = 0;
    m.length = 6;
    m.unitID = 1;
    m.code = 3;
    m.regAddr = 4;
    m.regTotal = 1;

    client.write((uint8_t *)&m, sizeof(m));
    Serial.print("[Реле] Отправлено ");
    Serial.print(sizeof(m));
    Serial.println(" байт");

    transactionID++;
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.softAP(SSID, PASS);
  server.begin();

  Serial.print("Сервер запущен: ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(MODBUS_PORT);

  ws.begin();

  ws.onEvent(ws_callback);
}

void loop()
{
  client = server.accept();

  if (client)
  {
    Serial.println("Новый ModbusTCP клиент подключен");

    int32_t active = 5;

    while (client.connected())
    {
      ws.loop();

      ModbusTCP_Master m;

      m.transactionID = transactionID;
      m.protocolID = 0;
      m.length = 6;
      m.unitID = 1;
      m.code = 3;
      m.regAddr = 0;
      m.regTotal = 2;

      client.write((uint8_t *)&m, sizeof(m));
      Serial.print("Отправлено ");
      Serial.print(sizeof(m));
      Serial.println(" байт");

      if (active <= 0)
        break;

      active--;

      transactionID++;

      if (client.available())
      {
        active = 5;

        ModbusTCP_Slave s;

        client.read((uint8_t *)&s, sizeof(s));
        Serial.print("Принято ");
        Serial.print(sizeof(s));
        Serial.println(" байт");

        Serial.print("Температура: ");
        Serial.println(s.reg1);
        Serial.print("Влажность: ");
        Serial.println(s.reg2);

        char buffer[128];

        size_t len = snprintf(buffer, 128, "{\"gas\": 0, \"photoresistor\": 0, \"motion\": false, \"temp\": %hu, \"humidity\": %hu}", s.reg1, s.reg2);

        ws.broadcastTXT((uint8_t *)buffer, len);
      }

      delay(1000);
    }

    client.stop();
    ws.disconnect();
    Serial.println("ModbusTCP клиент отключен");
  }
}