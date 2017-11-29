#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LinkedList.h>
#include <ArduinoJson.h>

const int LED = D4;
const char* ssid = "Shopan";
const char* password = "chay1436";

void setup() {
 pinMode(LED, OUTPUT);
 Serial.begin(38400); /* Define baud rate for serial communication */

 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println();
    Serial.print("Connecting..");
  }
   Serial.println("Setuped");
}

void loop() {

 if (Serial.available())  /* If data is available on serial port */
    {
      String data_received;
      data_received = Serial.readString();  /* Data received from bluetooth */
      data_received.trim();
      String deviceID = data_received.substring(2,data_received.length()-1);
      
      if (data_received.charAt(0)=='1')   //Login
      {
        Serial.println("Student login (Code: "+deviceID+")");
        Connect(deviceID, "Login");
        digitalWrite(LED, HIGH);
        Serial.println("Login");        
      }
      else if (data_received.charAt(0)=='2')  //Logout
      {
        Serial.println("Student login (Code: "+deviceID+")");
        Connect(deviceID, "Logout");
        digitalWrite(LED, LOW);
        Serial.println("Logout");
      }
      else
      {
        Serial.println("Select either 1 or 2 :"+data_received);
      }
    }
}
void Connect(String studentID, String command)
{
    HTTPClient http;  //Declare an object of class HTTPClient
    
    //Specify request destination
    String URL = "http://studentattendancechecker-iot.azurewebsites.net/Student/Student"+command+"/?deviceID="+studentID;
    Serial.println(URL);
    http.begin(URL); 
    int httpCode = http.GET();//Send the request
    Serial.println("Start Connect...");
    
//    if (httpCode < 0)  //Check the returning code
//    {
      String payload = http.getString();//Get the request response payload
      const size_t bufferSize = JSON_OBJECT_SIZE(3) + ((payload.length()-30)*20);
      DynamicJsonBuffer jsonBuffer(bufferSize);
      
      JsonObject& root = jsonBuffer.parseObject(const_cast<char*>(payload.c_str()));
      if (!root.success()) 
      {
        Serial.println("parseObject() failed");
        return;
      }
      else
      {
        int Code = root["Code"];
        if(0 <= Code)   //Get request Success
        {
          String Bluetooth = root["loginedBltMacAddress"].asString();
          Serial.print("Succsess: "+Code);
        }
        else
        {
          Serial.print("Error occor when GetRequest: ");
          Serial.println(Code);
        }
      }
 
    http.end();   //Close connection
}
