#include <FastLED.h>
#include <stdint.h>
#define LOW    0
#define HIGH   1
#define ON     1
#define NUM_LEDS 3
#define LED_PIN A5

//define analog pins to components
const int  LDR_PIN = A0;
const int  IR_PIN = A2;
const int  BUZZER_PIN = A3;

unsigned char Warning_type = 0;
uint8_t state = 1;

// Define time intervals (in milliseconds) 
#define  DRINK_INTERVAL    10000 // 20 seconds
#define  CLEAN_INTERVAL    20000 // 80 seconds
#define  PLACE_INTERVAL    30000 // 20 seconds
#define  INACTIVE_INTERVAL 10000 //10 seconds

//define high and low
#define LOW 0
#define HIGH 1

unsigned long current_time = millis(), previous_drink_time = 0, previous_place_time = 0, previous_clean_time = 0, previous_inactive_time = 0;
uint8_t flag = 1;

enum state
{
  IDLE_FROST=1,
  ACTIVE,
  DND,
};


CRGB leds[NUM_LEDS];  
void setup() 
{
  //INPUT PINS
  pinMode(A0, INPUT);   //LDR to analog pin 0 
  pinMode(A2, INPUT);   //IR SENSOR to analog pin 2

  //OUTPUT PINS 
  pinMode(A3, OUTPUT);  //BUZZER to analog pin 3 
  pinMode(A5, OUTPUT);  //LEDs to analog pin 5 
  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<WS2812, LED_PIN, BRG>(leds, NUM_LEDS);   //3-wire led controllers
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);         //Set the maximum power to be used, given in volts and milliamps.
  FastLED.clear();   // To clear the leds,
  FastLED.show();    // To update the led colours
  Serial.begin(9600); 

  
}

void loop()
{
  current_time = millis();
  previous_drink_time = current_time; 
  switch(state)
  {
    //Initially in IDLE_FROST state
    case IDLE_FROST:
      if(Read_LDR_Sensor())     state = DND;                        	      //when light is not detected(dark) it moves to dnd state                                 
      else if(active_condition()) state = ACTIVE;                           //when any of the condition is satisfied it moves to active state    
      break;

    case ACTIVE:
    Serial.print(" ACTIVE_MODE ");
      if(Warning_type == 1 && flag ==1)      //every 2hours  and ir high                  
      {
        previous_drink_time = current_time;    //updating previous drink time for next cycle
        drink_water_mode();                   //drink water function
      }         
      else if(Warning_type == 2 && flag ==1)    //every 8hours and ir low
      {
        previous_clean_time = current_time;        //updating previous clean time for next cycle
        clean_bottle_mode();                 //clean bottle function
      }                         
      else if(Warning_type == 3 && flag ==1)  //every 2hours and ir low
      {
        previous_place_time = current_time;      //updating previous place time for next cycle
        place_bottle_mode();                //place bottle function               
      }
      else if(current_time - previous_inactive_time >= INACTIVE_INTERVAL)   //every 1hour
      {
        previous_inactive_time = current_time; //updating previous inactive time for next cycle
        state = IDLE_FROST;
      }
      break;
    case DND:
      dnd_mode();
      break;
  }
}



//FUNCTIONS DEFINITION


// The function checks whether the sensor is detecting an object or not by comparing the analog READING with a reference value of 1023.
unsigned char Read_IR_Sensor() 
{
  int ir_read = analogRead(IR_PIN);
  if (ir_read == 1023)             // Object detected
  {
   // Serial.write("No Dock \n");
    Serial.println(ir_read) ;
    return HIGH;                        
  }
  else                             //no object detected
  {
    //Serial.write("Dock \n");
    Serial.println(ir_read) ;
    return LOW;                          
  }
}





/* function checks If the analog value is equal to the maximum value of 1023, 
it means that the sensor is detecting low light levels, and the function will return a value of LOW.*/

unsigned char Read_LDR_Sensor()
{
  int ldr_read = analogRead(LDR_PIN) ;
  Serial.print(" LDR Value = ");
  Serial.println(ldr_read);
  if(ldr_read <= 1023 && ldr_read >= 500)               //DARK
  {
        Serial.println(" DARK");           

    return HIGH ;        
  }
  else                                                 ////LIGHT
  {
        Serial.println(" LIGHT");           

    return LOW ;                 
  }
}


//function to turn ON/OFF buzzer for specified interval
void buzzer_frost(unsigned long on_interval, unsigned long off_interval)   
{
  // get the current time                      
  unsigned long i;                                       //ex: i = 1000
  //adding buzzer on interval to current time
  unsigned long running_time_on = current_time + on_interval;                     // a = 1000+4000 = 5000
  //condition to swtich on buzzer for specified interval
  for(i=current_time; i <= running_time_on; i=millis())            //for(i = 1000; 1000 <= 5000)          after  four seconds(i = 5000; 5000<=5000 )
  {
    digitalWrite(BUZZER_PIN, HIGH);
    //Serial.write("Buzzer ON");               
  }                                                                 

  //adding buzzer off interval to current time                             
  unsigned long running_time_off = current_time + off_interval;       // same ex:  5000+8000=13000(b) 
  //condition to switch off buzzer for specified interval         
  for(i=current_time; i <= running_time_off; i=millis())             // (i = 5000 ; 5000<=13000)         after  eight seconds(i = 13000; 13000<=13000)   
  {
    digitalWrite(BUZZER_PIN, LOW);
   // Serial.write("Buzzer OFF");
  } 
}

unsigned int active_condition()
{
  Serial.print(" current_time= ");
  Serial.println(current_time);
  Serial.print(" previous_drink_time= ");
  Serial.println(previous_drink_time);
  if((current_time - previous_drink_time >= DRINK_INTERVAL) && Read_IR_Sensor())            Warning_type = 1;   //2hrs and ir high // DRINK WATER
  else if((current_time - previous_clean_time >= CLEAN_INTERVAL) && !(Read_IR_Sensor()))    Warning_type = 2;   //8hrs and ir low // CLEAN
  else if((current_time - previous_place_time >= PLACE_INTERVAL) && !(Read_IR_Sensor()))    Warning_type = 3;   //2hrs and ir low // PLACe BOTTLE
  else Warning_type = 0;
  flag = 1;
  return Warning_type;
  }



void drink_water_mode()
{
  buzzer_frost(2000, 1000);                   //buzzer on for 2seonds                                   
  Serial.print("drink water");
  leds[0] = CRGB(255, 0,  0);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;
  //Serial.write("LED ON");
}
       
void clean_bottle_mode()
{
  buzzer_frost(3000, 1000);                  //buzzer on for 3seconds   
  Serial.print("Clean bottle");
  leds[1] = CRGB(0,  255,  0);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;
  //Serial.write("LED ON");
}

void place_bottle_mode()
{
  buzzer_frost(2000, 2000);                 //buzzer on for 2seconds   
  Serial.print("Place bottle");
  leds[2] = CRGB(0,  0,  255);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;
  //Serial.write("LED ON");
}

void dnd_mode()
{
  buzzer_frost(4000, 1000);                //buzzer on for 4seconds    
  Serial.print(" DND_MODE ");
  if(!Read_LDR_Sensor()) state = IDLE_FROST;  //when light is  detected it moves from dnd state to idle state

}
