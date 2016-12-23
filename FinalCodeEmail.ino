#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <TextFinder.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Temboo.h>
#include "TembooAccount.h"

//ethernet
byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x07, 0x6F}; //ethernet shield number
IPAddress ip(192, 168, 1, 122); //change last digit to station# + 1
EthernetClient client;
TextFinder  finder( client );

//bluetooth
SoftwareSerial BTSerial(2,3);

//weather
char server[] = "weather.yahooapis.com";
char place[50];
char sunrise[7];
char sunset[7];
int sunrisemin;
int sunsetmin;
int conditions = finder.getValue();

//email
int numRuns = 1;
String MessageBodyValue = "";

void setup() {
  BTSerial.begin(9600);
  Serial.begin(9600);
  //start the Ethernet connection:
  Ethernet.begin(mac, ip);

  //set time (CHANGE THIS TO CURRENT TIME)
  setTime(17,20,00,07,12,2015);
}

void loop(){
  //email
  ((finder.getString("<title>Conditions for ", " ",place,50)!=0));
     Serial.print("City:  ");
     Serial.println(place);
  if(finder.find("code=")) {
    int conditions = finder.getValue();
    Serial.print("Conditions: ");
    Serial.println(conditions);
    if (conditions == 32 || conditions == 34 || conditions == 36) {
    MessageBodyValue = "It's sunny outside; don't forget your sunglasses!";
    Serial.println("It's sunny outside; don't forget your sunglasses!");
    }
    else if (conditions == 27 || conditions == 4 || conditions == 9 || 
    conditions == 11 || conditions == 12 || conditions == 37 || conditions == 38 || 
    conditions == 39 || conditions == 40 || conditions == 45 || conditions == 47) {
    MessageBodyValue = "It's rainy outside; don't forget your umbrella!";
    Serial.println("It's rainy outside; don't forget your umbrella!");
    }
  if (numRuns <= 10) {
    Serial.println("Running SendEmail - Run #" + String(numRuns++));
    TembooChoreo SendEmailChoreo(client);
    SendEmailChoreo.begin();
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
    SendEmailChoreo.addInput("MessageBody", MessageBodyValue);
    String SubjectValue = "WeatherSmart Lights";
    SendEmailChoreo.addInput("Subject", SubjectValue);
    String UsernameValue = "eselabpenn@gmail.com";
    SendEmailChoreo.addInput("Username", UsernameValue);
    String PasswordValue = "wwtbtnasedwhvqzj";
    SendEmailChoreo.addInput("Password", PasswordValue);
    String FromAddressValue = "eselabpenn@gmail.com";
    SendEmailChoreo.addInput("FromAddress", FromAddressValue);
    String ToAddressValue = "eselabpenn@yahoo.com";
    SendEmailChoreo.addInput("ToAddress", ToAddressValue);
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
    SendEmailChoreo.run();
    while(SendEmailChoreo.available()) {
      char c = SendEmailChoreo.read();
      Serial.print(c);
    }
    SendEmailChoreo.close();
  }
  }
  
  //get time and convert to minutes
  int CThour;
  CThour = hour();
  Serial.println(CThour);
  long CThourmin = 0;
  CThourmin = CThour * 60;
  Serial.println(CThourmin);
  int CTmin;
  CTmin = minute();
  Serial.println(CTmin);
  long currenttime;
  currenttime = CThourmin + CTmin;
  Serial.println(currenttime);
  
  //connect to weather
  if (client.connect(server, 80)) {
    Serial.println("Connecting to Yahoo Weather...");
    client.println("GET /forecastrss?w=12765491&u=f HTTP/1.1");
    client.println("HOST:weather.yahooapis.com\n\n");
    client.println();
  }
  else {
    Serial.println("Connection failed");
  }

  if (client.connected()) {
    //Sunrise
   (finder.getString("<yweather:astronomy sunrise=\"", "\"",sunrise,7)!=0);
     Serial.print("Sunrise:  ");
     Serial.println(sunrise);
     //convert sunrise time to minutes
     char Rhourc = sunrise[0];
     int Rhouri;
     Rhouri = Rhourc- '0';
     int Rhourmin;
     Rhourmin = Rhouri * 60;
     char Rmin1c = sunrise[2];
     int Rmin1i;
     Rmin1i = Rmin1c- '0';
     int Rminmin1;
     Rminmin1 = Rmin1i * 10;
     char Rmin2c = sunrise[3];
     int Rmin2i;
     Rmin2i = Rmin2c- '0';
     sunrisemin = Rhourmin + Rminmin1 + Rmin2i;
     Serial.println(sunrisemin);
     
           //Sunset
           (finder.getString("sunset=\"", "\"",sunset,7)!=0);
              Serial.print("Sunset:  ");
              Serial.println(sunset);
              //convert sunset time to minutes
              char Shourc = sunset[0];
              int Shouri;
              Shouri = Shourc- '0';
              int Shourmin;
              Shourmin = Shouri * 60;
              char Smin1c = sunset[2];
              int Smin1i;
              Smin1i = Smin1c- '0';
              int Sminmin1;
              Sminmin1 = Smin1i * 10;
              char Smin2c = sunset[3];
              int Smin2i;
              Smin2i = Smin2c- '0';
              sunsetmin = 720 + Shourmin + Sminmin1 + Smin2i;
              Serial.println(sunsetmin);
              
                  //Place
                  (finder.getString("<title>Conditions for ", " ",place,50)!=0);
                    Serial.print("City:  ");
                    Serial.println(place);
                  //Conditions
                  finder.find("code=");
                    int conditions = finder.getValue();
                    Serial.print("Conditions: ");
                    Serial.println(conditions);
   }
   else {
     Serial.println("Disconnected");
     }
   if (currenttime < sunrisemin || currenttime > sunsetmin) {
     Serial.println("Lights On");
     BTSerial.println("H");
     }
     else {
         if (conditions == 24 || conditions == 32 || conditions == 34
            || conditions == 36 || conditions == 44) {
           Serial.println("Lights Off");
           BTSerial.println("L");
           }
         else {
             Serial.println("Lights On");
             BTSerial.println("H");
             }
     }

  client.stop();
  client.flush();
  delay(3600000); //should check every hour
}
