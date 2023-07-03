#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Servo.h>

#define VCC2 5  // define new vcc pin
#define GND2 2  // define new gnd pin

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position
int ir = 7;     // ir pin
int x;          // ir value
int flag = 0;   // check ir status

void Greeting(){
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("Have a good day.");
}

void GoodBye(){
  lcd.setCursor(0,0);
  lcd.print("Goodbye");
  lcd.setCursor(0,1);
  lcd.print("See you again.");
}

void setup() {
  Serial.begin(9600);

  lcd.init();       // lcd setup
  lcd.backlight();
  
  myservo.attach(6);  // attaches the servo on pin 6 to the servo object
  pinMode(ir,INPUT);   // pin 7 to ir value
  
  pinMode(VCC2,OUTPUT);  // define a digital pin as output
  digitalWrite(VCC2,HIGH);  // set the above pin as high

  pinMode(GND2,OUTPUT);  // define a digital pin as output
  digitalWrite(GND2,LOW);  // set the above pin as low
}

void loop() {

  // The Exit with IR Sensor
  x = digitalRead(ir);
  Serial.println(x);

  //Open the gate
  if( x==0 && flag==0 ) {
    flag=1;
    GoodBye();
    delay(200);
    for (pos = 90; pos >= 0; pos -= 1) { 
      myservo.write(pos);             
      delay(10);                   
    }
  }

  //Close the gate
  if( x==1 && flag==1 ){
    flag=0;
    delay(2500);
    for (pos = 0; pos <= 90; pos += 1) { 
      myservo.write(pos);             
      delay(10);                    
    }
  }

  // RFID System
  
  delay(300);
}
