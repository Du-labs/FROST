int buzzerPin=A3;
int irsensorPin=A2;
void setup() 
{
pinMode(buzzerPin, OUTPUT);
pinMode(irsensorPin, INPUT);
Serial.begin(9600);
}
void loop(){
int irsensorValue=digitalRead(irsensorPin);
if(irsensorValue==HIGH)
{
digitalWrite(buzzerPin, HIGH);
Serial.println(irsensorValue);
}
else
{
digitalWrite(buzzerPin, LOW);
Serial.println(irsensorValue);
}

}