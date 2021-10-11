#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>

int buzzer = 8;
int state = 0;
//const int sensor = 10 ;
int sensorstate = 0;

//bool gpsReading = true;

int smokeA0 = A1;
//int GASA3 = A3;
// Your threshold value
int sensorThres = 150;
//int sensorThres2 = 280;


String lattitude, longitude;
SoftwareSerial gpsSerial(6,5);
TinyGPSPlus gps;

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2 i.e 2 is the arduino receiving pin

void setup()
{
  pinMode(buzzer, OUTPUT);
//  pinMode(sensor, INPUT);
   pinMode(smokeA0, INPUT);
//    pinMode(smokeA3, INPUT);

//    bool gpsReading = false;
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  // gps setup
  gpsSerial.begin(9600);
  Serial.println("GPS starting up");
  delay(1000);
  
}

void loop()
{
//   if ( (analogSensor > sensorThres ){
//          digitalWrite(buzzer, HIGH);
//          delay(100);
//            digitalWrite(buzzer, LOW);
//   }
    
        smsLoop();
    
}

void gpsLoop() {
  while(1){
    while(gpsSerial.available() > 0){
      gps.encode(gpsSerial.read());
     
    }
    if(gps.location.isUpdated()){
      lattitude = String(gps.location.lat(),6);
      longitude = String(gps.location.lng(),6);
    
      break;
    }
  }
  
  
  Serial.print(" Lattitude= ");
  Serial.print(lattitude);
  Serial.print(" Longitude= ");
  Serial.println(longitude);
                                  
  delay(5000);
  
}
void smsLoop(){
  
        gpsLoop();
       
       int analogSensor = analogRead(smokeA0);
//       int analogSensor2 = analogRead(smokeA3);
        Serial.println(analogSensor);
        
   if ( (analogSensor > sensorThres )&& (state == 0)) {

           Serial.println("rrrrrrruuuuuuuuunnnnnnnn");
          digitalWrite(buzzer, HIGH);
           Serial.println("GAS DETECTED");
          mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
          updateSerial();
        
          mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
          updateSerial();
          mySerial.println("AT+CMGS=\"+2348069002832\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.print("Gas Leakage Alert\n\r" ); //text content
          updateSerial();

          String PPM = String(analogSensor);
          mySerial.print("PPM: " + PPM + "\n\r" ); //text content
          updateSerial();
          
          mySerial.print("https://www.google.com/maps/?q=" + lattitude + "," + longitude ); //text content
          updateSerial();
          mySerial.write(26);
           Serial.println("MESSAGE SENT");
          state = 1;
   }
   if (analogSensor < sensorThres) {
      state = 0;
      digitalWrite(buzzer, LOW);
    }
  
}
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
