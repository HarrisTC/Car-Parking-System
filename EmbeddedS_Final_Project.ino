/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int ir =3;
int x=1;
int flag=0;
void setup() {
  Serial.begin(9600);
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object
  pinMode(3,INPUT);
}

void loop() {
  x= digitalRead(ir);
  Serial.println(x);

  //Open the gate
  if(x==0 && flag==0){
    flag=1;
    for (pos = 90; pos >= 0; pos -= 1) { 
      myservo.write(pos);             
      delay(10);                   
    }
  }

  //Close the gate
  if(x==1 && flag==1){
    flag=0;
    delay(2500);
    for (pos = 0; pos <= 90; pos += 1) { 
      myservo.write(pos);             
      delay(10);                    
    }
  }

  delay(300);
}
