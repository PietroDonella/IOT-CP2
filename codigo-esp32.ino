#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ==========================================
// CONFIGURAÇÕES DE REDE E MQTT
// ==========================================
const char* ssid = "NOME_DA_SUA_REDE";
const char* password = "SENHA_DA_REDE";

const char* mqtt_server = "broker.hivemq.com"; 
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ==========================================
// CONFIGURAÇÕES DOS SENSORES
// ==========================================
// Sensor de Temperatura e Umidade (DHT)
#define DHTPIN 4       // Pino digital conectado ao pino de dados do DHT
#define DHTTYPE DHT11  // Mude para DHT22 se for o modelo branco
DHT dht(DHTPIN, DHTTYPE);

// Sensor Ultrassônico (HC-SR04)
#define TRIG_PIN 5     // Pino conectado ao TRIG do sensor
#define ECHO_PIN 18    // Pino conectado ao ECHO do sensor

unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Estufa-";
    clientId += String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa os pinos do Ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Inicializa o DHT
  dht.begin();
  
  // Inicializa conexões
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) { // Faz a leitura e publica a cada 5 segundos
    lastMsg = now;

    // --- 1. LEITURA DO DHT (Temperatura e Umidade) ---
    float hum = dht.readHumidity();
    float temp = dht.readTemperature(); // Em Celsius

    // Verifica se a leitura falhou
    if (isnan(hum) || isnan(temp)) {
      Serial.println("Falha ao ler do sensor DHT!");
      return; // Pula este ciclo se der erro
    }

    // --- 2. LEITURA DO ULTRASSÔNICO (Distância/Nível do Reservatório) ---
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Lê o tempo de retorno do pulso em microssegundos
    long duration = pulseIn(ECHO_PIN, HIGH);
    
    // Calcula a distância em centímetros (Velocidade do som = 343 m/s)
    float dist = duration * 0.034 / 2;

    // --- 3. PREPARAÇÃO E ENVIO (MQTT) ---
    char tempStr[8];
    char humStr[8];
    char distStr[8];
    
    dtostrf(temp, 1, 2, tempStr);
    dtostrf(hum, 1, 2, humStr);
    dtostrf(dist, 1, 2, distStr);

    client.publish("fiap/estufa_pietro/temperatura", tempStr);
    client.publish("fiap/estufa_pietro/umidade", humStr);
    client.publish("fiap/estufa_pietro/nivel_agua", distStr);

    // Log no Serial Monitor para você acompanhar na bancada
    Serial.print("Temp: "); Serial.print(tempStr); Serial.print(" °C | ");
    Serial.print("Umid: "); Serial.print(humStr); Serial.print(" % | ");
    Serial.print("Dist/Nível: "); Serial.print(distStr); Serial.println(" cm");
  }
}