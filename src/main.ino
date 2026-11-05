#include <WiFi.h>
#include <PubSubClient.h>

// Configuracoes do Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configuracoes do broker MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Identificador do cliente MQTT
const char* mqtt_client_id = "ESP32_WOKWI_01";

// Topico de publicacao
const char* mqtt_topic = "curso/esp32/potenciometro";

// Pinos utilizados
const int LED_WIFI = 2;
const int LED_MQTT = 15;
const int PINO_POT = 34;

// Cliente TCP
WiFiClient WOKWI_client;

// Cliente MQTT
PubSubClient client(WOKWI_client);

// Conecta o ESP32 ao Wi-Fi
void connect_wifi() {
  Serial.println();
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado!");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

// Conecta ou reconecta ao broker MQTT
void reconnect_MQTT() {
  while (!client.connected()) {
    Serial.println("Tentando conectar ao broker MQTT...");

    if (client.connect(mqtt_client_id)) {
      Serial.println("Cliente MQTT conectado!");
    } else {
      Serial.print("Falha na conexao. Estado MQTT: ");
      Serial.println(client.state());

      Serial.println("Nova tentativa em 5 segundos.");
      delay(5000);
    }
  }
}

// Le o potenciometro e publica o valor
void publica_dados() {
  int valor_pot = analogRead(PINO_POT);

  char payload[12];
  snprintf(payload, sizeof(payload), "%d", valor_pot);

  Serial.print("Valor do potenciometro: ");
  Serial.println(valor_pot);

  if (client.publish(mqtt_topic, payload)) {
    Serial.println("Dado publicado com sucesso.");
  } else {
    Serial.println("Falha ao publicar o dado.");
  }

  delay(1000);
}

// Atualiza o LED de Wi-Fi
void wifi_on() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_WIFI, HIGH);
  } else {
    digitalWrite(LED_WIFI, LOW);
  }
}

// Atualiza o LED de MQTT
void broker_on() {
  if (client.connected()) {
    digitalWrite(LED_MQTT, HIGH);
  } else {
    digitalWrite(LED_MQTT, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_MQTT, OUTPUT);
  pinMode(PINO_POT, INPUT);

  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_MQTT, LOW);

  connect_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Verifica e recupera a conexao Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_MQTT, LOW);

    connect_wifi();
  }

  // Verifica e recupera a conexao MQTT
  if (!client.connected()) {
    digitalWrite(LED_MQTT, LOW);
    reconnect_MQTT();
  }

  // Processa a comunicacao MQTT
  client.loop();

  // Publica a leitura do potenciometro
  publica_dados();

  // Atualiza os indicadores de conexao
  wifi_on();
  broker_on();
}