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
#define DRINK_INTERVAL    120000 // 2hours
#define CLEAN_INTERVAL    480000 // 8hours
#define PLACE_INTERVAL    120000//  2hours
#define INACTIVE_INTERVAL 60000 //  1hour

//define high and low
#define LOW 0
#define HIGH 1

unsigned long current_time = millis(), time, previous_drink_time = 0, previous_place_time = 0, previous_clean_time = 0, previous_inactive_time = 0;
unsigned long elapsed_drink_time, elapsed_clean_time, elapsed_place_time;
unsigned long elapsed_drink_time1, elapsed_clean_time1, elapsed_place_time1;
uint8_t flag = 1;

enum state
{
  IDLE_FROST = 1,
  ACTIVE,
  DND,
};


CRGB leds[NUM_LEDS];
void setup()
{
  //INPUT PINS
  pinMode(LDR_PIN, INPUT);
  pinMode(IR_PIN, INPUT_PULLUP);

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

void loop() {
  current_time = millis();
  //previous_drink_time = current_time;

  switch (state) {
    case IDLE_FROST:
    Serial.print("| IDLE_MODE |");
      if (Read_LDR_Sensor()) {
        state = DND;
      } else if (active_condition()) {
        state = ACTIVE;
      }
      break;
    case ACTIVE:
      Serial.print("| ACTIVE_MODE |");
      if (Warning_type == 1 && flag == 1) {
        elapsed_drink_time = current_time - previous_drink_time;
        Serial.print("elapsed time = ");
        elapsed_drink_time1 = elapsed_drink_time / 60000;
        Serial.print(elapsed_drink_time);
        Serial.print("hours");
        previous_drink_time = current_time;
        Serial.println("| DRINK_WATER |");
        drink_water_mode();
        FastLED.clear();
      } else if (Warning_type == 2 && flag == 1) {
        elapsed_clean_time = current_time - previous_clean_time;
        Serial.print("elapsed time = ");
        elapsed_clean_time1 = elapsed_clean_time / 60000;
        Serial.print(elapsed_clean_time);
        Serial.print("hours");
        previous_clean_time = current_time;
        Serial.println("| CLEAN_BOTTLE |");
        clean_bottle_mode();
        FastLED.clear();
      } else if (Warning_type == 3 && flag == 1) {
        elapsed_place_time = current_time - previous_place_time;
        Serial.print("elapsed time = ");
        elapsed_place_time1 = elapsed_place_time / 60000;
        Serial.print(elapsed_place_time);
        Serial.print("hours");
        previous_place_time = current_time;
        Serial.println("| PLACE_BOTTLE |");
        place_bottle_mode();
        FastLED.clear();
      } else if (current_time - previous_inactive_time >= INACTIVE_INTERVAL) {
        previous_inactive_time = current_time;
        state = IDLE_FROST;
      }
      break;
    case DND:
      dnd_mode();
      break;
  }
}

unsigned char Read_IR_Sensor() {
  int ir_read = analogRead(IR_PIN);

  if (ir_read >= 50) {
    //Serial.println(ir_read);
    return LOW;
  } else {
    //Serial.println(ir_read);
    return HIGH;
  }
}

unsigned char Read_LDR_Sensor() {
  int ldr_read = analogRead(LDR_PIN);
  //Serial.print("LDR Value:");
  //Serial.print(ldr_read);
  Serial.print(" |");

  if (ldr_read <= 1023 && ldr_read >= 500) {
    Serial.println("DARK |");
    return HIGH;
  } else {
    Serial.println("LIGHT |");
    return LOW;
  }
}

void buzzer_frost(unsigned long on_interval, unsigned long off_interval) {
  unsigned long i;
  unsigned long running_time_on = current_time + on_interval;

  for (i = current_time; i <= running_time_on; i = millis()) {
    digitalWrite(BUZZER_PIN, HIGH);
  }

  unsigned long running_time_off = current_time + off_interval;

  for (i = current_time; i <= running_time_off; i = millis()) {
    digitalWrite(BUZZER_PIN, LOW);
  }
}


unsigned int active_condition()
{

  if ((current_time - previous_drink_time >= DRINK_INTERVAL) && Read_IR_Sensor())            Warning_type = 1;  //2hrs and ir high // DRINK WATER
  else if ((current_time - previous_clean_time >= CLEAN_INTERVAL) && (Read_IR_Sensor()))    Warning_type = 2;  //8hrs and ir low // CLEAN
  else if ((current_time - previous_place_time >= PLACE_INTERVAL) && !(Read_IR_Sensor()))    Warning_type = 3;  //2hrs and ir low // PLACe BOTTLE
  else Warning_type = 0;
  flag = 1;
  return Warning_type;
}



void drink_water_mode() 
{
 Serial.print("current_time:");
  Serial.print(current_time);
  Serial.print(" previous_drink_time:");
  Serial.print(previous_drink_time );
  buzzer_frost(2000, 1000);
   leds[0] = CRGB(255, 0,  0);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;

}

void clean_bottle_mode() {
  Serial.print("current_time:");
  Serial.print(current_time);
  Serial.print(" previous_clean_time:");
  Serial.print(previous_clean_time );
  buzzer_frost(3000, 1000);
  leds[1] = CRGB(0,  255,  0);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;
}

void place_bottle_mode() {
  Serial.print("current_time:");
  Serial.print(current_time);
  Serial.print(" previous_place_time:");
  Serial.print(previous_place_time );
  buzzer_frost(2000, 1000);
  leds[2] = CRGB(0,  0,  255);
  FastLED.show();
  digitalWrite(A5, HIGH);
  flag = 0;
}

void dnd_mode() {
  buzzer_frost(4000, 1000);

Serial.print("DND_MODE |");

  if (!Read_LDR_Sensor()) {
    state = IDLE_FROST;
  }
}