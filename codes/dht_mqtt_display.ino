
// From https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide

#include <WiFi.h>
#include "PubSubClient.h"
#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 4 
#define DHTTYPE DHT11

#define MODULO "eletronica_3"


// configurações da rede wifi
#define WIFI_SSID "has"
#define WIFI_PASSWORD  "wdev1234"
// define canal de conexao wifi mais rapido
#define WIFI_CHANNEL 6

WiFiClient espClient;
PubSubClient client(espClient);

// broker mqtt
const char* mqttServer = "192.168.1.101";
const int mqttPort = 1883;
const char* mqttUser = "homeiot";
const char* mqttPassword = "homeiot";
#define MODULO "eletronica_3"
const char* mqttTopic = MODULO "/led";

int port = 1883;
String stMac;
char mac[50];
char clientId[50];



const int ledPin = 23;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
 
  Serial.begin(115200);
  randomSeed(analogRead(0));


  dht.begin();

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  wifiConnect();

   Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);

  
// iniciando conexao mqtt
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  long r = random(1000);
  sprintf(clientId, "clientId-%ld", r);

while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");

    if (client.connect(clientId, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
      // assina o topico
        client.subscribe("IFSP/led1");
    } else {
      Serial.print("MQTT connection failed, retrying in 5 seconds...");
      delay(5000);
    }
  }

    Serial.println(" iniciando display ");
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha na alocação SSD1306"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("iniciando..");
  display.display(); 



  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

    

  Serial.println(F("DHTxx test!"));

  
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
 
    if (client.connect(clientId, mqttUser, mqttPassword)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("IFSP/led1");
        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebida no topico: ");
  Serial.print(topic);
  Serial.print(". Messagem: ");
  String stMessage;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "IFSP/led1") {
    Serial.print("Saida alterada para ");
    if(stMessage == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
       display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(10, 10);
        display.println("Ligou poste 1");
        display.display(); 
        delay(2000);
        display.clearDisplay();
        display.display();
    }
    else if(stMessage == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
       display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(10, 10);
        display.println("Desligou poste 1");
        display.display(); 
        delay(2000);
        display.clearDisplay();
        display.display();
    }
  }
  
}

unsigned long millisTarefa1 = millis();

void loop() {
  delay(10);
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  
  if((millis() - millisTarefa1) >10000){
    millisTarefa1 = millis();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("Umidade:");
    display.setCursor(10, 25);
    display.print(h);
    display.print ("%");
    display.setCursor(10, 35);
    display.println("Temperatura:");
    display.setCursor(10, 45);
    display.print(t);
    display.print (" C");


    display.display();

    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    /// publica no topio mqtt
const char* mqttTopic = MODULO "/led";

    client.publish (MODULO "/umidade", String(h).c_str());
    client.publish(MODULO "/temperatura", String(t).c_str());


  }
}
