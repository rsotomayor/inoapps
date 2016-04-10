#include <SoftwareSerial.h>


#define TSAMPLE          60000UL  // millisegundos

// Software serial #1: RX = digital pin 8, TX = digital pin 9
#define PIN_RX 8
#define PIN_TX 9
#define PIN_OUTPUT_LED1 13
#define PIN_OUTPUT_LED2 12

#define PIN_PWM 10

SoftwareSerial portOne(PIN_RX, PIN_TX);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

// configurando los pines como salidas digitales
  pinMode(PIN_OUTPUT_LED1, OUTPUT);
  pinMode(PIN_OUTPUT_LED2, OUTPUT);
  
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

  val = digitalRead(PIN_OUTPUT_LED1);
  estado_g |= ( val << 0 ) ;

  val = digitalRead(PIN_OUTPUT_LED2);
  estado_g |= ( val << 1 ) ;



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
    
    if ( strCommand == "LED1ON" ) {
      digitalWrite(PIN_OUTPUT_LED1, HIGH); 
    } else if ( strCommand == "LED1OFF" ) {
      digitalWrite(PIN_OUTPUT_LED1, LOW); 
    } else if ( strCommand == "LED2ON" ) {
      digitalWrite(PIN_OUTPUT_LED2, HIGH); 
    } else if ( strCommand == "LED2OFF" ) {
      digitalWrite(PIN_OUTPUT_LED2, LOW); 
    } else if ( strCommand[0] == 'S' ) {
      strCommand = strCommand.substring(1,(strRx.length()-1));
      sPwm_g = atoi(strCommand.c_str());  
      analogWrite(PIN_PWM,sPwm_g); 
      
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
