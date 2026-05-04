#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";


const char* mqtt_server = "7bc1afb0b62846e7b25cdf25f99bf007.s1.eu.hivemq.cloud"; 
const char* mqtt_user = "usuario1";    
const char* mqtt_pass = "Senha123";     

WiFiClientSecure espClient;
PubSubClient client(espClient);


#define DHTPIN 27      // Alterado para o Pino D27
#define DHTTYPE DHT11  // Mude para DHT22 se for o modelo branco
DHT dht(DHTPIN, DHTTYPE);


#define TRIG_PIN 32    // Alterado para o Pino D32
#define ECHO_PIN 33    // Alterado para o Pino D33

unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    
    String clientId = "ESP32Estufa-";
    clientId += String(random(0, 1000));
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  dht.begin();
  
  setup_wifi();
  
  espClient.setInsecure();
  
  client.setServer(mqtt_server, 8883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  

  if (now - lastMsg > 5000) { 
    lastMsg = now;

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(hum) || isnan(temp)) {
      Serial.println("Falha ao ler do sensor DHT!");
      return;
    }

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    float dist = duration * 0.034 / 2;
    char tempStr[8];
    char humStr[8];
    char distStr[8];
    
    dtostrf(temp, 1, 2, tempStr);
    dtostrf(hum, 1, 2, humStr);
    dtostrf(dist, 1, 2, distStr);

    client.publish("fiap/estufa/temperatura", tempStr);
    client.publish("fiap/estufa/umidade", humStr);
    client.publish("fiap/estufa/nivel_agua", distStr);

    Serial.print("Temp: "); Serial.print(tempStr); Serial.print(" °C | ");
    Serial.print("Umid: "); Serial.print(humStr); Serial.print(" % | ");
    Serial.print("Nível: "); Serial.print(distStr); Serial.println(" cm");
  }
}
