#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define SS_PIN 10 //RX slave select
#define RST_PIN 9
#define GreenLed 4 //define green LED pin
#define RedLed 3 //define red LED
#define Buzzer 8 //buzzer pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myServo; //define servo name

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(7,6); //SIM800L Tx & Rx is connected to Arduino #7 & #6

byte card_ID[4]; //card UID size 4byte
byte Name1[4]={0xD5,0x30,0xE6,0xA9};//first UID card
byte Name2[4]={0xA5,0x9F,0x78,0x89};//second UID card

//if you want the arduino to detect the cards only once
int NumbCard[2];//this array content the number of cards. in my case i have just two cards.
int j=0;        

String Name;//user name
long Credit;//user number
long Bno; //number of bus borded
int n ;//The number of card you want to detect (optional)  

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  mySerial.begin(9600); //Begin serial communication with Arduino and SIM800L
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  
  Serial.println("CLEARSHEET");                 // clears starting at row 1
  Serial.println("LABEL,Date,Time,Name,Credit,Bno");// make four columns (Date,Time,[Name:"user name"]line 48 & 52,[Credit:"user number"]line 49 & 53)

  Serial.println("Initializing...");
  delay(1000);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+256772497630\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.println("User 1: Your travel balance is "); //text content
  mySerial.print(Credit);
  updateSerial();
  mySerial.write(26);
  
  myServo.attach(2);
  myServo.write(0);
  pinMode(RedLed,OUTPUT);
  pinMode(GreenLed,OUTPUT);
  pinMode(Buzzer,OUTPUT);

   }
    
void loop() {
  //look for new card
   if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;//got to start of loop if there is no card present
 }
 // Select one of the cards
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
 }
 
 for (byte i = 0; i < mfrc522.uid.size; i++) {
     card_ID[i]=mfrc522.uid.uidByte[i];

       if(card_ID[i]==Name1[i]){
       Name="Passenger 1";//user name
       Credit=30;//user number
       Bno=005;
       j=0;//first number in the NumbCard array : NumbCard[j]
      }
      else if(card_ID[i]==Name2[i]){
       Name="Passenger 2";//user name
       Credit=64;//user number
       Bno=007;
       j=1;//Second number in the NumbCard array : NumbCard[j]
      }
      else{
          digitalWrite(GreenLed,LOW);
          digitalWrite(RedLed,HIGH);
          tone(Buzzer, 300);
          Serial.println(" Access Denied");
          delay(1000);
          digitalWrite(RedLed, LOW);
          noTone(Buzzer);
          goto cont;//go directly to line 85
     }
}
      if(NumbCard[j] == 1){//to check if the card already detect
      //if you want to use LCD
      //Serial.println("Already Exist");
      }
      else{
      NumbCard[j] = 1;//put 1 in the NumbCard array : NumbCard[j]={1,1} to let the arduino know if the card was detecting 
      n++;//(optional)
      Serial.print("DATA,DATE,TIME," + Name);//send the Name to excel
      Serial.print(",");
      Serial.println(Credit); //send the Number to excel
      digitalWrite(GreenLed,HIGH);
      tone(Buzzer, 500);
      delay(300);
      noTone(Buzzer);
      myServo.write(180);
      delay(5000);
      myServo.write(0);
      digitalWrite(GreenLed,LOW);
      digitalWrite(RedLed,LOW);
      delay(30);
      Serial.println("SAVEWORKBOOKAS,Names/WorkNames");
      }
      delay(1000);
cont:
delay(2000);
digitalWrite(GreenLed,LOW);
digitalWrite(RedLed,LOW);

//if you want to close the Excel when all card had detected and save Excel file in Names Folder. in my case i have just 2 card (optional)
//if(n==2){
    
  //  Serial.println("FORCEEXCELQUIT");
 //   }
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

    
