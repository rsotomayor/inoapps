#include <SoftwareSerial.h>


#define TSAMPLE          30000UL  // millisegundos

// Software serial #1: RX = digital pin 8, TX = digital pin 9
#define PIN_TX 9
#define PIN_RX 8

// Las entradas análogas se comportan como salidas digitales A0, A1, A2 y A3
#define PIN_RELAY1  A0
#define PIN_RELAY2  A1
#define PIN_RELAY3  A2
#define PIN_RELAY4  A3
#define PIN_OUTPUT1 13

// Optoacopladores en las entradas digitales D4, D5, D6 y D7
#define PIN_INPUT1   7
#define PIN_INPUT2   6
#define PIN_INPUT3  5
#define PIN_INPUT4  4

#define PIN_PWM 10

SoftwareSerial portOne(PIN_RX, PIN_TX);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

// configurando los pines como salidas digitales
  pinMode(PIN_RELAY1, OUTPUT);  
  pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_RELAY3, OUTPUT);
  pinMode(PIN_RELAY4, OUTPUT);
  pinMode(PIN_OUTPUT1, OUTPUT);
  
// configurando los pines como entradas digitales
  pinMode(PIN_INPUT1, INPUT);  
  pinMode(PIN_INPUT2, INPUT); 
  pinMode(PIN_INPUT3, INPUT); 
  pinMode(PIN_INPUT4, INPUT);
 
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
int    contadorPrevio=0;
unsigned long previousMillis_a = 0 ;
unsigned long currentMillis_a;
unsigned long deltaT;
unsigned long estado_g ;
int sPwm_g=0;

void readEstado() {

  estado_g      = 0x00000000;
  int val;

  val = digitalRead(PIN_RELAY1);
  estado_g |= ( val << 0 ) ;

  val = digitalRead(PIN_RELAY2);
  estado_g |= ( val << 1 ) ;

  val = digitalRead(PIN_RELAY3);
  estado_g |= ( val << 2 ) ;

  val = digitalRead(PIN_RELAY4);
  estado_g |= ( val << 3 ) ;

  val = digitalRead(PIN_OUTPUT1);
  estado_g |= ( val << 4 ) ;

  val = digitalRead(PIN_INPUT1);
  estado_g|= !( val << 5 ) ;

  val = digitalRead(PIN_INPUT2);
  estado_g |= !( val << 6 ) ;

  val = digitalRead(PIN_INPUT3);
  estado_g |= !( val << 7 ) ;

  val = digitalRead(PIN_INPUT4);
  estado_g |= !( val << 8 ) ;


}


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
    strCommand.toUpperCase(); // Convierta a mayuscula
    strCommand.trim();  // Elimina espacios

    flagRxCommand     = false ;
    strRx             = "";
    flagCommandValido = true ;
    
    if ( strCommand == "R1ON" ) {
      digitalWrite(PIN_RELAY1, HIGH); 
    } else if ( strCommand == "R2ON" ) {
      digitalWrite(PIN_RELAY2, HIGH); 
    } else if ( strCommand == "R3ON" ) {
      digitalWrite(PIN_RELAY3, HIGH); 
    } else if ( strCommand == "R4ON" ) {
      digitalWrite(PIN_RELAY4, HIGH); 
    } else if ( strCommand == "R1OFF" ) {
      digitalWrite(PIN_RELAY1, LOW); 
    } else if ( strCommand == "R2OFF" ) {
      digitalWrite(PIN_RELAY2, LOW); 
    } else if ( strCommand == "R3OFF" ) {
      digitalWrite(PIN_RELAY3, LOW); 
    } else if ( strCommand == "R4OFF" ) {
      digitalWrite(PIN_RELAY4, LOW); 
    } else if ( strCommand == "O1ON" ) {
      digitalWrite(PIN_OUTPUT1, HIGH); 
    } else if ( strCommand == "O1OFF" ) {
      digitalWrite(PIN_OUTPUT1, LOW); 
    } else if ( strCommand == "S0" ) {
      sPwm_g = 0 ;
      analogWrite(PIN_PWM,sPwm_g); // Señal PWM a 0% en el PIN
    } else if ( strCommand == "S25" ) {
      sPwm_g = 63 ;
      analogWrite(PIN_PWM,sPwm_g); // Señal PWM a 25% en el PIN
    } else if ( strCommand == "S50" ) {
      sPwm_g = 127 ;
      analogWrite(PIN_PWM,sPwm_g); // Señal PWM a 50% en el PIN
    } else if ( strCommand == "S75" ) {
      sPwm_g = 190 ;
      analogWrite(PIN_PWM,sPwm_g); // Señal PWM a 75% en el PIN
    } else if ( strCommand == "S100" ) {
      sPwm_g = 255 ;
      analogWrite(PIN_PWM,sPwm_g); // Señal PWM a 100% en el PIN
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


  currentMillis_a = millis();
  if ( (currentMillis_a - previousMillis_a) > 0  ) {
    deltaT = currentMillis_a - previousMillis_a;
  } else {
    deltaT = previousMillis_a - currentMillis_a;
  }

  if (  deltaT > TSAMPLE  ) {
    readEstado();

    previousMillis_a = currentMillis_a;
    strResponse = "E:" + String(estado_g,BIN) + ",S:" + String(sPwm_g) + "\r\n";
    Serial.print(strResponse);      
    portOne.write(strResponse.c_str());
  }
  


}
