//#include <RestClient.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define OPEN 0
#define CLOSE 1
#define DEBUG

char inputGet[255];
byte sensorStatus[8] = {1};
byte sensorOld[8] = {1};
byte sensorWeb[8] = {1};
byte alarmSet[8] = {0};
byte inbyte[8];

const char* ssid     = "12345678";
const char* password = "12345678";
const String BASE_URL = "http://us-central1-myworkspace-4ef9e.cloudfunctions.net/witchwork/api/";

void setAlarm(int sensor){
  if(alarmSet[sensor] != 1){
    Serial.printf("A%d",sensor);
    alarmSet[sensor] = 1;
  }
  
}
void unSetAlarm(int sensor){
    if(alarmSet[sensor] != 0){
    Serial.printf("D%d",sensor);
    alarmSet[sensor] = 0;
  }
}
void UnsetAllAlarm(){
  Serial.printf("D0D1D2D3D4D5D6D7");
}

void sendrequest(){
  DynamicJsonBuffer jsonBuffer;
  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(BASE_URL+"embedded");  //Specify request destination
  Serial1.print("GET ");
  Serial1.println(BASE_URL+"embedded");
  int httpCode = http.GET();                                                                  //Send the request
  if (httpCode > 0) {
    //Serial.println(httpCode);
    if(httpCode != 200){
      Serial1.println("GET ERROR "+http.errorToString(httpCode));
      return;}
    String payload = http.getString();   //Get the request response payload
    Serial1.println("GET Complete");
    //Serial.println(payload);                     //Print the response payload
    http.end();
    JsonObject& root = jsonBuffer.parseObject(payload);
    JsonArray& itemsAr = root["items"];
    for (int j=0;j<itemsAr.size();j++){
      JsonObject& aDoor = itemsAr[j];
      String adv = aDoor["action"];
      String adn = aDoor["status"];
      //ALARM SECTION
      //Serial.print(adv+" ");
      //Serial.println(adn+" ");
      if(aDoor["action"] == "ring"){
        setAlarm(j);
      }
      else{
        unSetAlarm(j);
      }
      //STATUU UPDATE
      if(aDoor["status"] == "open"){
        sensorWeb[j] = OPEN;
      }
      else{
        sensorWeb[j] = CLOSE;
      }
      //Serial.println(sensorWeb[i]);
    }
    Serial1.println("PRASE DONE");
  }
}


void openDoor(int dn){
  char c = '0'+(dn);
  HTTPClient http;
  http.begin(BASE_URL+"embedded/open/d"+c);
  //Serial.println(BASE_URL+"embedded/open/d"+c);
  /*http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-length","0");
  Serial1.print("POST");
  Serial1.println(BASE_URL+"embedded/open/d"+c);
  int httpCode = http.POST("");*/

  Serial1.print("GET");
  Serial1.println(BASE_URL+"embedded/open/d"+c);
  int httpCode = http.GET();
  Serial1.println("GETTING");
  
  if (httpCode != 200){
    Serial1.printf("ERROR %d\n",httpCode);
    Serial1.println(http.errorToString(httpCode));
    Serial.printf("J%d",dn);
  }
  Serial.printf("O%d",dn);
  Serial1.printf("OPEN %d/n",dn);
  //http.end();
  
}
void closeDoor(int dn){
  char c = '0'+(dn);
  HTTPClient http;
  http.begin(BASE_URL+"embedded/close/d"+c);
  /*http.addHeader("Content-Type","application/x-www-form-urlencoded");
  http.addHeader("Content-length","0");
  Serial1.print("POST");
  Serial1.println(BASE_URL+"embedded/close/d"+c);
  int httpCode = http.POST("");
  Serial1.println("POSTING");*/

  Serial1.print("GET");
  Serial1.println(BASE_URL+"embedded/close/d"+c);
  int httpCode = http.GET();
  Serial1.println("GETTING");
  
  if (httpCode != 200){
    Serial1.printf("ERROR %d\n",httpCode);
    Serial1.println(http.errorToString(httpCode));
    Serial.printf("J%d",dn);
  }
  Serial.printf("C%d",dn);
  Serial1.printf("CLOSE %d/n",dn);
  //http.end();
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.println("PREInitializing");
  //Serial.setDebugOutput(true);
  Serial.println("Please connect to STM32 in 15 second");
  //Serial.swap();
  delay(15000);
  Serial.print("I0N0I0T0");
  Serial1.println("Initializing");

   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
   {
      delay(250);
      Serial.printf("W%d",WiFi.status());
   }

   Serial1.println("WiFi connected");  
   Serial1.println("IP address: ");
   Serial1.println(WiFi.localIP());
   Serial1.print("MAC address: ");
   Serial1.println(WiFi.macAddress());



 Serial.print("W1");
 Serial.print("S1");

 sendrequest();

/*String response = "";
 RestClient client = RestClient("http://us-central1-myworkspace-4ef9e.cloudfunctions.net/witchwork/api");
 int statusCode = client.get("/embedded",&response);
 Serial.println(statusCode);
 Serial.println(response);*/

 /* HTTPClient http;
  http.begin(BASE_URL+"/embedded");
  int httpCode = http.GET();
  if (httpCode != 200){
    Serial.printf("ERROR %d",httpCode);
  }
  Serial.printf("C%d",dn);
  //http.end();*/

}

void loop() {
  // put your main code here, to run repeatedly:
 if (Serial.available() > 7){
  Serial.readBytes(inbyte, 8);
  Serial1.printf("Got data\n");
  Serial1.printf("%d %d %d %d\n",inbyte[0],inbyte[1],inbyte[2],inbyte[3]);  
  memcpy(sensorStatus,inbyte,sizeof(inbyte));
  //Serial.printf("%d %d %d %d\n",sensorStatus[0],sensorStatus[1],sensorStatus[2],sensorStatus[3]);
  for (int i = 0; i < 8; i++) {
    if (sensorStatus[i] != sensorWeb[i]) {
      if (sensorStatus[i] == OPEN){
        openDoor(i);
      } else {
        closeDoor(i);
      }
    }
  }
  //memcpy(sensorOld,sensorStatus,sizeof(sensorStatus));
 }
 else{
  Serial.printf("N0");
 }
 sendrequest();
 delay(250);
 while(Serial.available()){
  char getData = Serial.read();
 }
 delay(250);

}
