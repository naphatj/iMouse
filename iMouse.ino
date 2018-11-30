#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const int LEFT_BUTTON = 14;
//const int RIGHT_BUTTON= 4;
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
double x,y;

// ----- CHANGE IP ADDRESS HERE------------
String IPv4 = "192.168.1.4";
// ----------------------------------------

String ip= "http://"+IPv4+":3000";

void setup() {

  pinMode(LEFT_BUTTON,INPUT);
//  pinMode(RIGHT_BUTTON,INPUT);
 
    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

     Wire.begin();
     mpu.initialize();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("THE_NUT","045243008");
    

}

void loop() {
     
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {
        //USE_SERIAL.println("Connected to Wifi");

        HTTPClient http;


        
      //-------------------------------------LEFT-CLICK-------------------------------------------------------------------------
      
        if(digitalRead(LEFT_BUTTON)==HIGH){
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(ip+"/left-click");
        http.addHeader("Content-Type","application/x-www-form-urlencoded");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        String lc ="lc";
        int httpCode = http.POST(lc);

        http.end();
    }
    //----------------------------------------------------------------------------------------------------------------------------


  //-------------------------------------RIGHT-CLICK-------------------------------------------------------------------------
      
       /* if(digitalRead(RIGHT_BUTTON)==HIGH){
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(ip+"/right-click");
        http.addHeader("Content-Type","application/x-www-form-urlencoded");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        String rc ="rc";
        int httpCode = http.POST(rc);

        http.end();
    }*/
    
    //----------------------------------------------------------------------------------------------------------------------------
    

    //----------------------------------------------------------- MOVEMENT -------------------------------------------------------

        int x_threshold=4; //if (abs(x) or abs(y) <= threshold) then ignore the value used for canceling out small unwanted values;
        int y_threshold=4; 
        
        double x_sensitivity=2; // adjust sensitivity here
        double y_sensitivity=1;
      
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        
        http.begin(ip+"/move-by");
        http.addHeader("Content-Type","application/x-www-form-urlencoded");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header

        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  

     
       // Convert Acceleration to Gyro  
       // Default Constant = *0.000122f*9.80665f 
       //y = -(ax)*0.000122f*9.80665f; 
       //x = -(ay)*0.000122f*9.80665f;


      
      x = (az)*0.00122f*9.80665f; 
      y = -(ax)*0.00122f*9.80665f;
       


      if (abs(x)<x_threshold*10) {
        x=0;
      }else {
        x*=x_sensitivity;
      }

      if (abs(y)<y_threshold*10) {
        y=0;
      }else {
         y*=y_sensitivity;
      }
    
        String strx = String(x);
        String stry = String(y);
        String str = String();

        str = "x=" + strx +"&y=" + stry;
        int httpCode = http.POST(str); 

        http.end();
   
    //------------------------------------------------------------------------------------------------------------------------------------
    
    }
    
}
