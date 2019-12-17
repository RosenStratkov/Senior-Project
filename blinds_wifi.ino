#include <SoftwareSerial.h>
 
#define DEBUG true

int button_state_green = 0;         // variable for reading the pushbutton status
int button_state_pink = 0;         // variable for reading the pushbutton status

const int button_pin_green = 2;     // the number of the pushbutton pin
const int button_pin_pink = 3;     // the number of the pushbutton pin

int switch_state = 0; //shows whether the blinds are open or closed
                      //we start assuming the blinds are closed

SoftwareSerial esp8266(2,3); // making RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // connecting the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

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
                             
void setup()
{
  Serial.begin(57600);
  esp8266.begin(57600); // your esp's baud rate might be different
  
  pinMode(1,OUTPUT);
  digitalWrite(11,LOW);
  
  pinMode(2,OUTPUT);
  digitalWrite(12,LOW);
  
  pinMode(10,OUTPUT);
  digitalWrite(10,LOW);
   
  sendCommand("AT+RST\r\n",2000,DEBUG); // reset module
  sendCommand("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point
  sendCommand("AT+CWJAP=\"AAAA\",\"123\"\r\n",3000,DEBUG);
  delay(10000);
  sendCommand("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendCommand("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendCommand("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  
  Serial.println("Server Ready");

  Serial.print("\tArduino Automated Blinds");
  pinMode(6, OUTPUT); //Enable
  pinMode(9, OUTPUT); //Step
  pinMode(8, OUTPUT); //Direction

  digitalWrite(6,LOW);

  pinMode(button_pin_green, INPUT);
  pinMode(button_pin_pink, INPUT);
}
 
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
 
    if(esp8266.find("+IPD,"))
    {
     delay(1000); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
          
     esp8266.find("pin="); // advance cursor to "pin="
          
     int pinNumber = (esp8266.read()-48); // get first number i.e. if the pin 13 then the 1st number is 1

     // build string that is send back to device that is requesting pin toggle
     String content;
     content = "Blinds ";
     content += pinNumber;
     content += " are ";

     /*check if the blinds are open
     *check the command sent by the application
     check if a button is pressed*/
     if(switch_state == 0 && (digitalRead(pinNumber) == 1) || button_state_pink == HIGH)
     {
       switch_state = 1;
       content += "OPEN";
       blinds_open(); //open blinds
     }
     else if (switch_state == 1 && (digitalRead(pinNumber) == 2) || button_state_green == HIGH)
     {
       switch_state = 0;
       content += "CLOSED";
       blinds_close(); //close blinds
     }
     else
     {
      content += "already in this state"; 
     }
     
     sendHTTPResponse(connectionId,content);
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendCommand(closeCommand,1000,DEBUG); // close connection
    }
  }
}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    int dataSize = command.length();
    char data[dataSize];
    command.toCharArray(data,dataSize);
           
    esp8266.write(data,dataSize); // send the read character to the esp8266
    if(debug)
    {
      Serial.println("\r\n====== HTTP Response From Arduino ======");
      Serial.write(data,dataSize);
      Serial.println("\r\n========================================");
    }
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
 
/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content)
{
     
     // build HTTP response
     String httpResponse;
     String httpHeader;
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     httpResponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpResponse);
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   sendCommand(cipSend,1000,DEBUG);
   sendData(data,1000,DEBUG);
}
 
/*
* Name: sendCommand
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
    String response = "";
           
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
