#include <SoftwareSerial.h>

// Software serial #1: RX = digital pin 8, TX = digital pin 9
#define PIN_TX 9
#define PIN_RX 8

// Las entradas análogas se comportan como salidas digitales A0, A1, A2 y A3
#define RELAY1  A0
#define RELAY2  A1
#define RELAY3  A2
#define RELAY4  A3

// Optoacopladores en las entradas digitales D4, D5, D6 y D7
#define OPTO1   7
#define OPTO2   6
#define OPTO3   5
#define OPTO4   4

#define PIN_PWM 10

SoftwareSerial portOne(PIN_RX, PIN_TX);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

// configurando los pines como salidas digitales
  pinMode(RELAY1, OUTPUT);  
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  
// configurando los pines como entradas digitales
  pinMode(OPTO1, INPUT);  
  pinMode(OPTO2, INPUT); 
  pinMode(OPTO3, INPUT); 
  pinMode(OPTO4, INPUT);
 
//Obs: Si tienes la versión de la placa 1.06, para que funcione correctamente debes configurar una resistencia
// Pull-UP en el Pin de Salida (TX) del Arduino (RX del Bluetooth), ya que esta versión cuenta con un Diodo,
// el cual se debe polarizar para la transmisión de datos.
// En el bloque de código setup() debes configurar una resistencia Pull-Up (Pin TX de Arduino en estado alto,
// HIGH) de la siguiente manera. Pin 15 Arduino Mega, o Pin 0 Arduino UNO (Depende del puerto serial de 
// Arduino mega que estén usando).

//  pinMode(PIN_TX, INPUT);
//  digitalWrite(PIN_TX, HIGH); 
  
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
      digitalWrite(RELAY1, HIGH); 
    } else if ( strCommand == "R2ON" ) {
      digitalWrite(RELAY2, HIGH); 
    } else if ( strCommand == "R3ON" ) {
      digitalWrite(RELAY3, HIGH); 
    } else if ( strCommand == "R4ON" ) {
      digitalWrite(RELAY4, HIGH); 
    } else if ( strCommand == "R1OFF" ) {
      digitalWrite(RELAY1, LOW); 
    } else if ( strCommand == "R2OFF" ) {
      digitalWrite(RELAY2, LOW); 
    } else if ( strCommand == "R3OFF" ) {
      digitalWrite(RELAY3, LOW); 
    } else if ( strCommand == "R4OFF" ) {
      digitalWrite(RELAY4, LOW); 
    } else if ( strCommand == "S0" ) {
      analogWrite(PIN_PWM,0); // Señal PWM a 0% en el PIN
    } else if ( strCommand == "S25" ) {
      analogWrite(PIN_PWM,63); // Señal PWM a 25% en el PIN
    } else if ( strCommand == "S50" ) {
      analogWrite(PIN_PWM,127); // Señal PWM a 50% en el PIN
    } else if ( strCommand == "S75" ) {
      analogWrite(PIN_PWM,190); // Señal PWM a 75% en el PIN
    } else if ( strCommand == "S100" ) {
      analogWrite(PIN_PWM,255); // Señal PWM a 100% en el PIN
    } else {
      flagCommandValido = false ;
    }
    
    if ( flagCommandValido ) {
      strResponse = strCommand + " => OK\r\n";
    } else {
      strResponse = strCommand + " => ERROR\r\n";
    }
    Serial.print(strResponse);      
    portOne.write(strResponse.c_str());
    
  }


}
