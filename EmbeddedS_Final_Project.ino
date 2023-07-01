#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position
int ir = 3;     // ir pin
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
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object
  pinMode(3,INPUT);   // pin 3 to ir value
  lcd.init();
  lcd.backlight();
}

void loop() {

  // The Exit with IR Sensor
  x = digitalRead(ir);
  Serial.println(x);

  //Open the gate
  if( x==0 && flag==0 ) {
    flag=1;
    GoodBye();
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
