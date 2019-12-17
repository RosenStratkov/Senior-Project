/* YourDuino Electronic Brick Test: 
 Light Sensor Analog + Digital AB-890202
 terry@yourduino.com */

/*-----( Declare Constants )-----*/
#define ANALOG_SENSOR_PIN_PINK   A0
#define ANALOG_SENSOR_PIN_GREEN  A1

/*-----( Declare Variables )-----*/
int  switch_state_pink;  /* Holds the last digital value */
int  switch_state_green;
int  LightAnalogValue; /* Holds the last analog value */
int switchState = 0;

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);          // Enable the Serial data output
  Serial.println("YourDuino Light Sensor Test 1.10 ");  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

}/*--(end setup )---*/

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/


// This module is ACTIVE LOW when a reflection is seen
{
  LightAnalogValue = analogRead(ANALOG_SENSOR_PIN_PINK);  //Read the voltage from sensor
  Serial.print("Analog Value (0 to 1023)"); 
  Serial.println(LightAnalogValue,DEC);      // Send result to Serial Monitor
  delay(500);

  switch_state_pink = digitalRead(ANALOG_SENSOR_PIN_PINK);
  switch_state_green = digitalRead(ANALOG_SENSOR_PIN_GREEN);  
  
  if (switch_state_pink == LOW && switch_state_green == LOW)
  {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    Serial.println("Digital Signal ON ");
  }  
  else if (switch_state_pink == HIGH && switch_state_green == LOW)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
  else if (switch_state_pink == LOW && switch_state_green == HIGH)
  {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
  }
  else
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
  }


}/* --(end main loop )-- */

/* ( THE END ) */
