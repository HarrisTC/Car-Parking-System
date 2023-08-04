#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define MAX_SLOT 3    // Max capacity of the car park

#define RST_PIN  9    // Pin RST for MFRC522
#define SS_PIN   10   // Pin SS for MFRC522
#define ir_close A0   // ir pin close gate
#define ir_open  7    // ir pin open gate

#define VCC_RFID     5    // define new vcc for ir open gate pin
#define GND_RFID     4    // define new gnd for ir open gate pin
#define VCC_IR_CLOSE A2   // define new vcc for ir close gate pin
#define GND_IR_CLOSE A1   // define new gnd for ir close gate pin
#define GND_IR_OPEN  8    // define new gnd for ir open gate pin

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27,16,2);   // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo openGate, closeGate;   // create servo object to control a servo

int slot;   // number of slot in present
int pos = 0;    // variable to store the servo position
int x_open = 1;   // ir open value
int x_close = 1;  // ir close value
int flag_open = 0;   // check ir open status
int flag_close = 0;  // check ir close status

// Print to LCD default line
void Default(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Hello World!");
  lcd.setCursor(2,1);
  lcd.print("Welcome home");
}

// Print to LCD to greet the vehicle coming in the park
void Greeting() {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("Have a good day.");
}

// Print to LCD to notify the vehicle using wrong card 
void WrongCard() {
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Wrong Card!");
  lcd.setCursor(0,1);
  lcd.print("Use another card!");
}

// Print to LCD to notify that the car park is full
void FullPark(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("The park is full");
  lcd.setCursor(2,1);
  lcd.print("Come later!");
}

// Open the gate
void Open(Servo myservo) {
  for (pos = 90; pos >= 0; pos -= 1) { 
    myservo.write(pos);             
    delay(10);                   
  }
}

// Close the gate
void Close(Servo myservo) {
  for (pos = 0; pos <= 90; pos += 1) { 
    myservo.write(pos);             
    delay(10);                    
  }
}

void setup() {
  Serial.begin(9600);
  slot = 0;
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.init();       // lcd setup
  lcd.backlight();
  
  closeGate.attach(6);  // attaches the servo on pin 6 to the servo object
  closeGate.write(90);  // servo at close start position
  openGate.attach(3);   // attaches the servo on pin 3 to the servo object
  openGate.write(90);   // servo at open start position

  pinMode(ir_close,INPUT);  // pin 7 to ir value
  pinMode(ir_open,INPUT);   // pin 8 to ir value

  pinMode(VCC_RFID,OUTPUT);  // define a digital pin as output
  digitalWrite(VCC_RFID,HIGH);  // set the above pin as high
  pinMode(GND_RFID,OUTPUT);  // define a digital pin as output
  digitalWrite(GND_RFID,LOW);  // set the above pin as low
  pinMode(VCC_IR_CLOSE,OUTPUT);  // define a digital pin as output
  digitalWrite(VCC_IR_CLOSE,HIGH);  // set the above pin as high
  pinMode(GND_IR_CLOSE,OUTPUT);  // define a digital pin as output
  digitalWrite(GND_IR_CLOSE,LOW);  // set the above pin as low
  pinMode(GND_IR_OPEN,OUTPUT);  // define a digital pin as output
  digitalWrite(GND_IR_OPEN,LOW);  // set the above pin as low

  Default();  // print default line to LCD
  Serial.println("Put your card to the reader...");
  Serial.println();
}

void loop() {
  // Print to LCD default line when there isn't any vehicle at the open gate
  if( x_open==1 ) {
    Default();  // print default line to LCD
  }

  // Check if the open gate haven't closed since the vehicle passed
  if( x_open==1 && flag_open==1 && openGate.read()==0 ) {
    flag_open=0;
    delay(1000);
    Close(openGate);
    // delay(300);
    // Default();  // print default line to LCD
    slot++;
  }

// EXIT GATE
  x_close = digitalRead(ir_close);  // ir close value
  
  // Open the gate
  if( x_close==0 && flag_close==0 ) {
    flag_close=1;
    delay(200);
    Open(closeGate);
  }
  // Close the gate
  if( x_close==1 && flag_close==1 ) {
    flag_close=0;
    slot--;
    if(slot < 0) slot = 0;
    delay(2000);
    Close(closeGate);
  }


// OPEN GATE
  x_open = digitalRead(ir_open);  // ir open value
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  if (mfrc522.PICC_IsNewCardPresent()) {
    Default();  // print default line to LCD
  }
  // Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";  // UID of new card

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
  
  // Check UID of vehicle's card
  if (content.substring(1) == "23 7A 69 04"|| content.substring(1) == "E3 B1 20 00") { //change here the UID of the cards that you want to give access
    if(slot < MAX_SLOT) {
      if( x_open==0 && flag_open==0 ) {
        flag_open=1;
        Serial.println("Authorized access");
        Serial.println();
        Greeting();  // Print greeting line to LCD
        Open(openGate);   // Open the open gate
      }
    }
    if(slot == MAX_SLOT) {
      FullPark();
    }
  }
  else {
    Serial.println(" Access denied");
    WrongCard();  // Print wrong card line to LCD
  }
  
  delay(300);
}
