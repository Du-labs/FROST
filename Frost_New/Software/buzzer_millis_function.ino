#include <stdint.h>

#define HIGH 1
#define LOW 0


const int BUZZER_PIN =A3;




void setup() {


pinMode(A3, OUTPUT);  //BUZZER

// initialize serial communication
  Serial.begin(9600);
}

void loop() {
  buzzer_frost(3000, 1000);
}


//function to turn ON/OFF buzzer for specified interval
void buzzer_frost(unsigned long on_interval, unsigned long off_interval)   
{
    // get the current time
  unsigned long previousMillis = 0;
  unsigned long i, currentMillis = millis();  
  unsigned long a = currentMillis + on_interval;      //adding buzzer on interval to current time
  // check if it's time to execute the timer
  for(i=currentMillis; i <= a; i=millis())           //condition to swtich on buzzer for specified interval
  {
    digitalWrite(A3, HIGH);
    //Serial.println("Buzzer ON");               
    //Serial.println("Timer executed!");
  }

  unsigned long b = currentMillis + off_interval;         //adding buzzer off interval to current time 
  for(i=currentMillis; i <= b; i=millis())                //condition to switch off buzzer for specified interval
  {
    digitalWrite(A3, LOW);
    //Serial.println("Buzzer OFF");
  }  
}


// void buzzer_frost(unsigned long buzzer_interval)   //function to turn ON buzzer for specified interval
// {
//     // get the current time
//   unsigned long currentMillis = millis();   

//   // check if it's time to execute the timer
//   if (currentMillis - previousMillis >= buzzer_interval)      
//   {
//     digitalWrite(A3, HIGH);
//     Serial.write("Buzzer ON");
//     previousMillis = currentMillis;             // save the current time for the next execution   
//     // execute the timer code here
//     Serial.println("Timer executed!");
//   }
//   digitalWrite(A3, LOW);
//   Serial.write("Buzzer OFF");  
// }


/*

void buzzer_frost(unsigned long buzzer_interval)   //function to turn ON buzzer for specified interval
{
    // get the current time
  unsigned long i,currentMillis = millis();   

  // check if it's time to execute the timer
  for(i=currentMillis; i<=(currentmilliss+buzzer_interval); i=millis)     
  {
    digitalWrite(A3, HIGH);
    Serial.write("Buzzer ON");
    // execute the timer code here
    Serial.println("Timer executed!");
  }
  digitalWrite(A3, LOW);
  Serial.write("Buzzer OFF");  
}
*/