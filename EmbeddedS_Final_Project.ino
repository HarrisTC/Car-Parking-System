#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define VCC2 5  // define new vcc pin
#define GND2 2  // define new gnd pin

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position
int ir = 7;     // ir pin
int x=1;          // ir value
int flag = 0;   // check ir status

void Greeting(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("Have a good day.");
}

void GoodBye(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Goodbye");
  lcd.setCursor(1,1);
  lcd.print("See you again.");
}

void WrongCard(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Wrong Card!");
  lcd.setCursor(0,1);
  lcd.print("Use another card!");
}

void Open(){
  for (pos = 90; pos >= 0; pos -= 1) { 
    myservo.write(pos);             
    delay(10);                   
  }
}

void Close(){
  for (pos = 0; pos <= 90; pos += 1) { 
    myservo.write(pos);             
    delay(10);                    
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.init();       // lcd setup
  lcd.backlight();
  
  myservo.attach(6);  // attaches the servo on pin 6 to the servo object
  myservo.write(90);  //servo start position
  pinMode(ir,INPUT);   // pin 7 to ir value
  
  pinMode(VCC2,OUTPUT);  // define a digital pin as output
  digitalWrite(VCC2,HIGH);  // set the above pin as high

  pinMode(GND2,OUTPUT);  // define a digital pin as output
  digitalWrite(GND2,LOW);  // set the above pin as low
  Serial.println("Put your card to the reader...");
  Serial.println();
}

void loop() {

//OPEN GATE
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "23 7A 69 04"|| content.substring(1) == "4A BA C6 23") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    Greeting();
    Open();
    delay(4000);
    Close();
    delay(500);
  }
 
  else   {
    Serial.println(" Access denied");
    WrongCard();
  }


//EXIT GATE
  // The Exit with IR Sensor
  x = digitalRead(ir);
  //Open the gate
  if( x==0 && flag==0 ) {
    flag=1;
    GoodBye();
    delay(200);
    Open();
  }
  //Close the gate
  if( x==1 && flag==1 ){
    flag=0;
    delay(2500);
    Greeting();
    Close();
  }
  delay(300);
}
