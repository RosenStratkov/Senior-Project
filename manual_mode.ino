int button_state_green = 0;         // variable for reading the pushbutton status
int button_state_pink = 0;         // variable for reading the pushbutton status

const int button_pin_green = 2;     // the number of the pushbutton pin
const int button_pin_pink = 3;     // the number of the pushbutton pin

const int led_pin_green =  12;      // the number of the LED pin
const int led_pin_pink =  13;      // the number of the LED pin

int switch_state = 0; //shows whether the blinds are open or closed
                      //we start assuming the blinds are closed

void blinds_open()
{
    digitalWrite(8,HIGH); //direction of the stepper motor
  
    for(int Index = 0; Index < 500; Index++) //movement of the stepper motor
    {
      digitalWrite(9,HIGH);
      delayMicroseconds(500);
      digitalWrite(9,LOW);
      delayMicroseconds(500);
    }
}

void blinds_close()
{
    digitalWrite(8,LOW); //direction of the stepper motor
  
    for(int Index = 0; Index < 500; Index++) //movement of the stepper motor
    {
      digitalWrite(9,HIGH);
      delayMicroseconds(500);
      digitalWrite(9,LOW);
      delayMicroseconds(500);
    }
}

void setup() {
  Serial.begin(9600);
  Serial.print("\tArduino Automated Blinds");
  pinMode(6, OUTPUT); //Enable
  pinMode(9, OUTPUT); //Step
  pinMode(8, OUTPUT); //Direction

  digitalWrite(6,LOW);

  pinMode(button_pin_green, INPUT);
  pinMode(button_pin_pink, INPUT);
  pinMode(led_pin_green, OUTPUT);
  pinMode(led_pin_pink, OUTPUT);
}

void loop() {
  button_state_green = digitalRead(button_pin_green); //read button pressed
  button_state_pink = digitalRead(button_pin_pink); //read button pressed

  if (switch_state == 1 && button_state_pink == HIGH) 
  {
      switch_state = 0;
      Serial.print("\nClosing blinds...");
      digitalWrite(led_pin_pink, HIGH); // turn LED on:
  
      blinds_close(); //close blinds
      digitalWrite(led_pin_pink, LOW); // turn LED off:
      Serial.print("Blinds CLOSED\n");
    }
    else if (switch_state == 0 && button_state_green == HIGH) {
      switch_state = 1;
      Serial.print("\nOpening blinds...");
      digitalWrite(led_pin_green, HIGH); // turn LED on:
      
      blinds_open(); //open blinds
      digitalWrite(led_pin_green, LOW); // turn LED off:
      Serial.print("Blinds OPEN\n");
    }

}
