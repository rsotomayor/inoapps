#include <SoftwareSerial.h>

int digPinPWM = 10; // pin digital 10 Para PM

// Optoacopladores en las entradas digitales D4, D5, D6 y D7
int Opto1 = 7;     
int Opto2 = 6;
int Opto3 = 5;
int Opto4 = 4;

// Las entradas análogas se comportan como salidas digitales A0, A1, A2 y A3
int Relay1 =  A0;   
int Relay2 =  A1;
int Relay3 =  A2;
int Relay4 =  A3;

// Software serial #1: RX = digital pin 8, TX = digital pin 9
SoftwareSerial portOne(8, 9);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

// configurando los pines como salidas digitales
  pinMode(Relay1, OUTPUT);  
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  
// configurando los pines como entradas digitales
  pinMode(Opto1, INPUT);  
  pinMode(Opto2, INPUT); 
  pinMode(Opto3, INPUT); 
  pinMode(Opto4, INPUT); 
  portOne.begin(9600);
}

String strRx;
String strCommand;
String strResponse;

int    counterCRLF=0;
bool   flagRxCommand=false ;
bool   flagCommandValido ;


void loop() {
  
  portOne.listen();
//  Serial.println("Data from port one:");

  while (portOne.available() > 0) {
    char inByte = portOne.read();
    strRx = strRx + inByte;
    if ( inByte == '\n' ) {
      counterCRLF++; 
    } else if ( inByte == '\r' ) {
      counterCRLF++; 
    } else {
      counterCRLF = 0 ;  
    }
    if ( counterCRLF == 2 ) {
      flagRxCommand = true ;
      break;  
    }
  }
  
  if ( flagRxCommand == true ) {
    strCommand = strRx.substring(0,(strRx.length()-2));
    strCommand.toUpperCase();


    flagRxCommand     = false ;
    strRx             = "";
    flagCommandValido = true ;
    
    if ( strCommand == "R1ON" ) {
      digitalWrite(Relay1, HIGH); 
    } else if ( strCommand == "R2ON" ) {
      digitalWrite(Relay2, HIGH); 
    } else if ( strCommand == "R3ON" ) {
      digitalWrite(Relay3, HIGH); 
    } else if ( strCommand == "R4ON" ) {
      digitalWrite(Relay4, HIGH); 
    } else if ( strCommand == "R1OFF" ) {
      digitalWrite(Relay1, LOW); 
    } else if ( strCommand == "R2OFF" ) {
      digitalWrite(Relay2, LOW); 
    } else if ( strCommand == "R3OFF" ) {
      digitalWrite(Relay3, LOW); 
    } else if ( strCommand == "R4OFF" ) {
      digitalWrite(Relay4, LOW); 
    } else if ( strCommand == "S0" ) {
      digitalWrite(Relay4, LOW); 
      analogWrite(digPinPWM,0); // Señal PWM a 0% en el PIN
    } else if ( strCommand == "S25" ) {
      analogWrite(digPinPWM,63); // Señal PWM a 25% en el PIN
    } else if ( strCommand == "S50" ) {
      analogWrite(digPinPWM,127); // Señal PWM a 50% en el PIN
    } else if ( strCommand == "S75" ) {
      analogWrite(digPinPWM,190); // Señal PWM a 75% en el PIN
    } else if ( strCommand == "S100" ) {
      analogWrite(digPinPWM,255); // Señal PWM a 100% en el PIN
    } else {
      flagCommandValido = false ;
    }
    
    if ( flagCommandValido ) {
      strResponse = strCommand + " -> OK\r\n";
    } else {
      strResponse = strCommand + " -> ERROR\r\n";
    }
    Serial.print(strResponse);      
    portOne.write(strResponse.c_str());
    
  }


}
