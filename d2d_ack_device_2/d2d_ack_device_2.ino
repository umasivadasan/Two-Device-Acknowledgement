//#include <ArduinoJson.h>
//#include <ArduinoJson.hpp>

#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0
#include <ArduinoJson.h>

const char* ssid = "IoT@228";
const char* password = "ese481@IoT";

//AWS IoT Core--> Settings (Device data endpoint)
const char* awsEndpoint = "az9w5umcnbrge-ats.iot.us-east-1.amazonaws.com"; 

const char* pubTopic = "iot/inTopic"; //changed this to inTopic from outTopic??
const char* subTopic = "iot/outTopic";

//topics for ack
const char* PBpubTopic = "iot/readTopic";
const char* PBsubTopic = "iot/ackTopic";

StaticJsonDocument<256> jsonBuffer;

// xxxxxxxxxx-certificate.pem.crt
static const char certificate_pem_crt[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUKyfF3/jiG3TWumtCzfUZ3e06JFIwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTAwMTEyMTgw
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANaz6LSep6nZa+mNEra+
FuuSY8K3ulIzwy0KeUEdPCB0NB/l/d/C0pk5RcMA9qwdlo7m9fqymHK0iDjep8SP
mfMrgmERANPV2R4b96ZrtC0lfkk1/3UByXbDZEA9lsuMGOiNTzvbuHBxWS1zt5u/
HKi64CuHY+qoaxcAuO690MOxIPJIEZReScSO5zcEQ4BRyAth2h84hC+zdIBQJaiL
BPL8rZZ4IPPrqK/FCpkr2ifmISGZ638F1cAIqJpItEZl99GbWLBeWBGANWrXiH91
Gl+ZRSAmceIuv36KMnHTjsjtlSyzq8jltRToljIsRUT60AcqS73ffPeksFuwAAMY
dOECAwEAAaNgMF4wHwYDVR0jBBgwFoAUMOYSsuaYz9FFTf+lLYSG7nasjw4wHQYD
VR0OBBYEFJjRYyI5zJ3gcDfAGiuwAixSJW+6MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQANDpdZPj9jMfQozTaKwiPyfrS0
gaVt6CSXJImLSjWlSppBgBrFfons4JSsVO9eAO/txY7AIc5wu3m4RKUFYkg8EYP1
x1lTPAuKVjqxxEDQdmmoekFEc8r4lv9qAHiTbv9O/+6WA7h/v4T464B60HMe4ZdZ
SuarvvG8KtNRn/CTnN7PXQDW67CA1Hg/R9Fs0AuR5IOf05/86oiqyBMuFJxiGJwa
EOWjBDMp7EReDqy11MpZkDb6vEq9sz16RBM/hOFeaqRV6WFHJkGfNjPpI3q5MEWp
hiRL9ian/lxuetskKHPWHwjk7JhUYLy4JvsX8w0aWgqpG/bM4pEURH23GYzP
-----END CERTIFICATE-----

)KEY";

// xxxxxxxxxx-private.pem.key
static const char private_pem_key[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA1rPotJ6nqdlr6Y0Str4W65Jjwre6UjPDLQp5QR08IHQ0H+X9
38LSmTlFwwD2rB2Wjub1+rKYcrSION6nxI+Z8yuCYREA09XZHhv3pmu0LSV+STX/
dQHJdsNkQD2Wy4wY6I1PO9u4cHFZLXO3m78cqLrgK4dj6qhrFwC47r3Qw7Eg8kgR
lF5JxI7nNwRDgFHIC2HaHziEL7N0gFAlqIsE8vytlngg8+uor8UKmSvaJ+YhIZnr
fwXVwAiomki0RmX30ZtYsF5YEYA1ateIf3UaX5lFICZx4i6/fooycdOOyO2VLLOr
yOW1FOiWMixFRPrQBypLvd9896SwW7AAAxh04QIDAQABAoIBAQDLvbgzRmdWr8NO
wxeiIKsBa06N5enCveoV4N2F4+c96OfZ6VfMMkXFL2s7DLwIQp+wBg4gk8BFAXpP
dUMBLUBrWLuFvyvo2gMKJoIRBB3bxbucX8iGUWbL88k7iPaDO/QbIpWw+OWClp2I
m0n+WxIoMAYwaG9xq9bL4lyb+baVK9EHKTNLd79nSTiodXjgtezINRsYJGKLv6cC
0CMYIxoe60AdxYpJ8Pvk0TlVeGyaK+d3pvctLhhN1V8lqalJTvIAOxu32n9IDZXU
PU7whckgGxf20BdlVDElob4YWhaMQ4JNiaeY6Fg6vXjNRGtDPXp/kW4P2tAaDfJm
yqml8vctAoGBAPQEfieApJSK7TRFUl5UYXznf+h9+swuq63KdJFgMKakYsSp6tCn
zRJuaEyFigCux6tvsYQSlU5Y48Yx625lAgtOjm8Fa4GStQoZEvt4KfDzms756xKX
YhpZ0FVayTt4ePt92Zzrxkh9akHSrjLJK0sOqTbpRQ7BactbY1BlHYKjAoGBAOE+
55Q7XqzioDixpNleirETspNltmpjwZGXUFTpB6QZhPMCTeFkgwjpuYQk+KaUiTGw
xzXf3XQs/EjwBxjtBruGz/VCczOc9XPuvGyqm1qYoon7eJXsNAZ5rgOHMZVgS4Ix
5m5m5sD6A/TaqkaIPSv+310iDYthfy/h/qV66iarAoGAfnLpkLzYDGWln+WgVmNx
lVE9dhP6Z95zdn6oQ0FOVLxwtiq7FN8vpLJz99tndQ0dSRh7K86Zcl0bw3PQ5YcQ
NR3DL4Lmf+GvUQlHTeqqCUOjYEgf6aVq3+7Ux7Zg+yFlyCUiJHAFc0ESVAPB421d
suppA5yD/a56jTfRUjLjxPMCgYBalIRmXPW2yrAp/G5Rp65YhZZTphyUbidimBoK
+2dCO6z2WJ/fGx+CzxL2rDN+Mc92NyP1a2BQHuMZpcUE5hTGBvHMLHvNbrpac9A8
VuOArE749DLK/6DwVbB/5Lcw3feflNeY4DK9ug0rQ2AkWxdWgPlsJ4yQ15VhOR3B
iEtvXwKBgQCMEb4eHPdZJjb4XsWEBEooEQwDnv/KGkFzyQyKpCFTAHshnu5yyZnk
2LdC+r1qQyudwQnuCv/QqvgIDaN4tzCBzjXJCiycAMrgHIbf3E8kSJnZWddGeZvI
BzcQ2NQBHd1Y16boRQdL/uHcqS3g7lUr/HtAjTLQBQ8OSGiJlES3Rg==
-----END RSA PRIVATE KEY-----

)KEY";

/* root CA found at:
  https://www.amazontrust.com/repository/AmazonRootCA1.pem
  fungible between regions.  Open cert, free for public consumption
*/
static const char rootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

char light_p[256];
float temp_p;
char data[256];

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void msgReceived(char* topic, byte* payload, unsigned int length) 
{
  if(strcmp(topic, PBsubTopic) == 0){
    Serial.print("Acknowledgement received on Device 1\n");
    Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
    for (int i = 0; i < length; i++) 
    {
      Serial.print((char)payload[i]);
    }

    for(int i = 2; i < 12; i++){
      
      digitalWrite(i, HIGH); //turn all leds off
    
    }
      //RGB OFF
      digitalWrite(14, HIGH);
      digitalWrite(15, HIGH);
      digitalWrite(16, HIGH);

      delay(2000); //delay 2s

      //turn everything on
      for(int i = 2; i < 12; i++){
      
      digitalWrite(i, LOW); //turn all leds off
    
    }
      //RGB OFF
      digitalWrite(14, LOW);
      digitalWrite(15, LOW);
      digitalWrite(16, LOW);
    
      Serial.println();
  }
  else if(strcmp(topic, subTopic) == 0){
    Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
    
    char buffer[512];
    for (int i = 0; i < length; i++) 
    {
      Serial.print((char)payload[i]);
      buffer[i] = (char)payload[i];
    }

   buffer[length] = '\0';

  //deserializeJSON("Incoming data: "); Serial.println(data);
    deserializeJson(jsonBuffer, buffer);
    JsonObject root = jsonBuffer.as<JsonObject>();

    long int uptime_p = root["uptime"];
    float mcp9700a_mv_p = root["mcp9700a_mv"];
    float mcp9700a_tempc_p = root["mcp9700a_tempc"];
    temp_p = root["mcp9700a_tempf"];
    strcpy(light_p, root["light_level"]);

    Serial.println();
  }
}

void pubSubCheckConnect() 
{
  if ( ! pubSubClient.connected()) 
  {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) 
    {
      Serial.print(".");
      pubSubClient.connect("ESPDeviceUniqueIdentifier2"); //this is you clientID
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe(subTopic);
    pubSubClient.subscribe(PBsubTopic); //subscribe to D1's pub for PB ack
  }
  pubSubClient.loop();
}
  const uint8_t MY_LED_RED = 14;
  const uint8_t MY_LED_GREEN = 15;
  const uint8_t MY_LED_BLUE = 16;
  
void setup() 
{
  Serial.begin(115200); //setting the baud rate to 115.2kbaud

  delay(1000);

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
  
  //push button
  pinMode(A7, INPUT_PULLUP);

  pinMode(MY_LED_RED, OUTPUT);
  digitalWrite(MY_LED_RED, HIGH);

  pinMode(MY_LED_GREEN, OUTPUT);
  digitalWrite(MY_LED_GREEN, HIGH);

  pinMode(MY_LED_BLUE, OUTPUT);
  digitalWrite(MY_LED_BLUE, HIGH);

  for(int i = 2; i < 12; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  
}
unsigned long lastPublish;

void loop() 
{
  pubSubCheckConnect();

  /**********Light and Temp values************/

  //must change on both MCs but how do we change on the first one?
  //light levels
  if(strcmp(light_p, "dark") == 0){
    digitalWrite(MY_LED_GREEN, HIGH);
    digitalWrite(MY_LED_RED, HIGH);
    digitalWrite(MY_LED_BLUE, LOW);
  }
  else if(strcmp(light_p, "ambient") == 0){
    digitalWrite(MY_LED_BLUE, HIGH);
    digitalWrite(MY_LED_RED, HIGH);
    digitalWrite(MY_LED_GREEN, LOW);
  }
  else if(strcmp(light_p, "bright") == 0){
    digitalWrite(MY_LED_GREEN, HIGH);
    digitalWrite(MY_LED_BLUE, HIGH);
    digitalWrite(MY_LED_RED, LOW);
  }

  //temperature
  if(temp_p <0){
    //turn LEDs off
    for(int i = 2; i < 12; i++){
      digitalWrite(i, HIGH); //turn all leds off
    }
  }
  else if (temp_p >=0 && temp_p < 10){

    for(int i = 2; i < 12; i++){

      if(i == 2){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 10 && temp_p <20){
    for(int i = 2; i < 12; i++){

      if(i == 2 || i == 3){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 20 && temp_p < 30){
    for(int i = 2; i < 12; i++){

      if(i == 2 || i == 3 || i == 4){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 30 && temp_p < 40){
    for(int i = 2; i < 12; i++){

      if(i == 2 || i == 3 || i == 4 || i == 5){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 40 && temp_p < 50){
    for(int i = 2; i < 12; i++){

      if(i == 2 || i == 3 || i == 4 || i == 5 || i == 6){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 50 && temp_p < 60){
    for(int i = 2; i < 12; i++){

      if(i == 2 || i == 3 || i == 4 || i == 5 || i == 6 || i == 7){
        digitalWrite(i, LOW);
      }
      else {
      digitalWrite(i, HIGH); //turn leds off
      }
    }
  }
  else if (temp_p >= 60 && temp_p < 70){
    for(int i = 2; i < 12; i++){

      if(i == 9 || i == 10 || i == 11){
        digitalWrite(i, HIGH); //leds off
      }
      else {
      digitalWrite(i, LOW); //turn leds on
      }
    }
  }
  else if (temp_p >= 70 && temp_p < 80){
    for(int i = 2; i < 12; i++){

      if(i == 10 || i == 11){
        digitalWrite(i, HIGH); //leds off
      }
      else {
      digitalWrite(i, LOW); //turn leds on
      }
    }
  }
  else if (temp_p >= 80 && temp_p < 90){
    for(int i = 2; i < 12; i++){

      if(i == 11){
        digitalWrite(i, HIGH); //leds off
      }
      else {
      digitalWrite(i, LOW); //turn leds on
      }
    }
  }
  else if(temp_p >= 90 && temp_p < 100){
    for(int i = 2; i < 12; i++){

      digitalWrite(i, LOW); //turn leds on
    }
  }
//If PB is pressed (message was read on device 2)

  if(analogRead(A7)==1){
//turn bargraph off
    for(int i = 2; i < 12; i++){

        digitalWrite(i, HIGH); //leds off
    }

    delay(2000); //turn off for 2 seconds

    for(int i = 2; i < 12; i++){

      digitalWrite(i, LOW); //turn leds on

    }

    //send message to d1 --> subscribe to D1's pub and D1 subscribes to D2's pub
      boolean rc = pubSubClient.publish(PBpubTopic, "Message read on Device 2");
      Serial.println("Published to Device 1, rc="); 
      Serial.print((rc ? "OK: " : "FAILED"));

  }
}