#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ORBI29";
const char* password = "be3b96f4";
WiFiClient wifiClient;

const char* mqttBroker = "192.168.1.53";
const char* mqttClientName = "teamX";
const String mqttUser = "hello"; // MQTT User Authentification
const String mqttPass = "world"; // MQTT Password Authentification
const char* topicToSub = "test/in";
const char* topicToPub = "test/out";
PubSubClient mqttClient(wifiClient);

const int buttPin = 6;
bool buttState;


const int motorPin0_0 = 19;         // the PWM pin the LED is attached to
const int motorPin0_1 = 21;         // the PWM pin the LED is attached to
const int motorPin1_0 = 14;         // the PWM pin the LED is attached to
const int motorPin1_1 = 32;         // the PWM pin the LED is attached to

int motorPower0 = 0;
int motorPower1 = 0;


void assignMotorPower(int pin0, int pin1, int power){
  power = (power>511)?511:power;
  power = (power<0)?0:power;
  
  if (power<256){
      analogWrite(pin0, power);
      analogWrite(pin1, 0);
  }
  else {
      analogWrite(pin0, 0);
      analogWrite(pin1, power-255);
  }
}



void mqttConnect() {
  
  while (!mqttClient.connected()) {
  
    Serial.print("Attempting MQTT connection...");
  
    if (mqttClient.connect(mqttClientName)) {
  
      Serial.println("connected");
      mqttClient.publish("hello", mqttClientName);
      
      // Topic(s) subscription
      mqttClient.subscribe(topicToSub);
  
    } else {
      
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
    
  String receivedMsg;
  long fullPower = 0;
  
  for (int i = 0; i < length; i++) {
     receivedMsg += (char)message[i];
  }
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(receivedMsg);

  if (String(topic) == topicToSub) {
    // convert message to int 
    fullPower = receivedMsg.toInt();
    int motorPower0 = fullPower%1000;
    int motorPower1 = fullPower/1000;
    Serial.print("MotorPower0: ");
    Serial.print(motorPower0);
    Serial.print(" MotorPower1: ");
    Serial.print(motorPower1);
    assignMotorPower(motorPin0_1, motorPin0_0, motorPower0);  
    assignMotorPower(motorPin1_1, motorPin1_0, motorPower1);  





  
    // Add here the code to act on input --------------
    // Arduino String obejct reference :
    // https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/
    // if (receivedMsg == ("1")) {
    
    //   Serial.println("on");
    //   digitalWrite(LED_BUILTIN, HIGH);
      
    
    // } else if (receivedMsg == "0") {
      
    //   Serial.println("off");
    //   digitalWrite(LED_BUILTIN, LOW);
    
    // }
    //-------------------------------------------------
  }
}

void setup() {
  
  pinMode(motorPin0_0, OUTPUT);
  pinMode(motorPin0_1, OUTPUT);
  pinMode(motorPin1_0, OUTPUT);
  pinMode(motorPin1_1, OUTPUT);

  Serial.begin(115200);

  // Connect to wifi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // MQTT setup
  mqttClient.setServer(mqttBroker, 1883);
  mqttClient.setCallback(callback);

  // Led setup
  // pinMode(LED_BUILTIN, OUTPUT);
}

unsigned long lastMsg = 0;
char msg[50];

void loop() {
    
  // Check the current button state
  bool currentState = digitalRead(buttPin);

  // Check if we are still connected to the MQTT broker
  if (!mqttClient.connected()) {
    mqttConnect();
  }

  // Let PubSubClient library do his magic
  mqttClient.loop();

  // Add your publish code here --------------------
  // // Publish motor current data on a timer
  // // Only publish every 5 seconds
  // unsigned long now = millis();
  // if (now - lastMsg > 5000) {
    
  //   lastMsg = now;
  //   snprintf (msg, 50, "%ld - hello again!", now);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
    
  //   mqttClient.publish(topicToPub, msg);
  // }
  //-----------------------------------------------
}
