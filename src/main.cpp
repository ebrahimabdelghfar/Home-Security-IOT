#include <Arduino.h>
#include <ThingerESP32.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#define LumSenesor 39
#define TempSensor 33
#define GasSensor 34

#define Alarm 13
#define Red_led 25
#define Green_led 27
/*define the credintals for the cloud*/
#define USERNAME "ebrahim"
#define DEVICE_ID "TestEsp"
#define DEVICE_CREDENTIAL "meMJRwn9h04l_yCQ"
/*end*/

/*wifi info*/
#define SSID "ASU"
#define SSID_PASSWORD "12345678"
/*end*/

/*define the phone number and the api key*/
String phoneNumber = "+201551385201";
String apiKey = "4023861";
/*end*/
int number[4];
int password[4]={21,6,1,9};
int DoorClosed=0;
bool Activate;
bool ACK;
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(Red_led, OUTPUT);
  pinMode(Green_led, OUTPUT);
  pinMode(Alarm, OUTPUT); 
  thing.add_wifi(SSID, SSID_PASSWORD);  
  thing["Lumunisty"] >> outputValue(analogRead(LumSenesor));
  thing["TemperatureSensor"]>>outputValue(analogRead(TempSensor));
  thing["GasSensor"]>>outputValue(analogRead(GasSensor));  
  thing["MotionDetected"]>>outputValue(analogRead(GasSensor));  
  thing["WindowState"]>>outputValue(analogRead(GasSensor));  
  thing["DoorState"]>>outputValue(analogRead(GasSensor));  
  /*used to acknowledge alarm*/
    thing["SendMessag"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in = (bool) ACK;
      }
      else{
          // This code is called whenever the "led" resource change
          ACK = in;
      }
  };

  thing["activate"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in = (bool) Activate;
      }
      else{
          // This code is called whenever the "led" resource change
          Activate = in;
      }
  };

  thing["pin1"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in =number[0];
      }
      else{
          // This code is called whenever the "led" resource change
          number[0] = in;
      }
  };

    thing["pin2"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in =number[1];
      }
      else{
          // This code is called whenever the "led" resource change
          number[1] = in;
      }
  };

    thing["pin3"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in =number[2];
      }
      else{
          // This code is called whenever the "led" resource change
          number[2] = in;
      }
  };

    thing["pin4"] << [](pson& in){
      if(in.is_empty()){
          // We send back the pin value to thinger platform
          in =number[3];
      }
      else{
          // This code is called whenever the "led" resource change
          number[3] = in;
      }
  };
}
void loop() {
  thing.handle();
  if(number[0]==password[0] && number[1]==password[1] && number[2]==password[2] && number[3]==password[3]){
    thing["DoorState"] >> outputValue(DoorClosed=1);
  }
  else{
    thing["DoorState"] >> outputValue(DoorClosed=0);
  }
  if(Activate){
    /*send a whatsapp message when temperature increase above certain level*/
    if(analogRead(TempSensor)>50){
      /*send message to the user until the user press acknowledge BUTTON PRESSED SO STOP SENDING MESSAGE*/
      if(ACK){
      sendMessage("Temperature is above normal");
      }
      /*operate buzzer*/
      tone(Alarm, 1000,1000);
    }
    if (analogRead(GasSensor)){
      if(ACK){
      sendMessage("Gas leckage detected");
      }
      /*operate buzzer*/
      tone(Alarm, 1000,1000);
    }
    if(analogRead(LumSenesor)>80){
      if(ACK){
      sendMessage("change in light intensity detected");
      }
      /*operate buzzer*/
      thing["motion"] >> outputValue(0);
      tone(Alarm, 1000,1000);
    }
    if(analogRead(LumSenesor)<80){
      thing["motion"] >> outputValue(1);
    }
  }
  delay(100);
}
