#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>


//**************************************************
//*********** GuinaAndonMQTTPI CONFIG **************
//************* PI = Pessoa Idosa ******************
//**************************************************
const char *mqtt_server = "ioticos.org";
const int mqtt_port = 1883;
const char *mqtt_user = "xxxxxxxxxxxxxxx";
const char *mqtt_pass = "xxxxxxxxxxxxxxx";
const char *root_topic_subscribe = "xxxxxxxxxxx/input";
const char *root_topic_publish = "xxxxxxxxxxxxx/output";


//Usuário e senha do wi-fi
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxxxxxx";

//Pino do botão
const int pushButton = 15;

//Pinos do RGB
#define greenLED 25
#define yellowLED 26
#define redLED 27

//Variáveis Globais
WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;
String str;


//Funções
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  pinMode(pushButton, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  delay (1000);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  int pushButtonState = digitalRead(pushButton);

  if (pushButtonState == HIGH)
  {
    str = "help me";
    str.toCharArray(msg,25);
    client.publish(root_topic_subscribe,msg);
    triggerGreenLED();
    delay(1000);
    triggerYellowLED();
    delay(1000);
    triggerRedLED();
    delay(1000);
    triggerHIGHLEDs();
    delay(1000);
    triggerHIGHLEDs();
    count++;
    Serial.println(count);
  } 

  if (count == 2) 
  {
    triggerLOWLEDs();
    str = "forget it";
    str.toCharArray(msg,25);
    client.publish(root_topic_subscribe,msg);
    count = 0;
  }

  client.loop();
}

//Validação do wi-fi
void setup_wifi(){
  delay(10);
  // estamos conectado na nossa rede Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a rede WiFi!");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);
  triggerGreenLED();
  delay(1000);
  triggerLOWLEDs();
}



//*****************************
//***    MQTT CONNECTION     ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Tentando conexão Mqtt...");
    // Criando um cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Inscrição ok");
      }else{
        Serial.println("falha na Inscrição");
      }
    } else {
      Serial.print("falha :( com erro -> ");
      Serial.print(client.state());
      Serial.println(" Tentando de novo em 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensagem recebida -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensagem -> " + incoming);

  if (incoming == "ALL_LOW"){
    triggerLOWLEDs();
    count = 0;
  }
}

//Rotina do RGB
void triggerGreenLED(){
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  str = "";  
}

void triggerYellowLED(){
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, HIGH);
  digitalWrite(redLED, LOW);
  str = "";  
}

void triggerRedLED(){
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, HIGH);  
  str = "";  
}

void triggerLOWLEDs(){
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);  
  str = "";
}

void triggerHIGHLEDs(){
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, HIGH);
  digitalWrite(redLED, HIGH);  
  str = "";  
}
