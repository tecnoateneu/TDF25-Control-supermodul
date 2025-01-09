//
//  Tecnoateneu de Vilablareix
//  Base 42
//  Realització per el Temps de Flors 2025
//  Girona 2024
//
//  Programació realitzada per Antoni Martorano i Gomis
//

#define LCDdebug           // a fi de debugar i veure el que fa
//#define CASA               // Si treball a casa canvio l'entorn


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Configuració del display
#define LCD_ADDR 0x27 // Adreça I2C del display
#define LCD_COLS 20   // Nombre de columnes
#define LCD_ROWS 4    // Nombre de files

#include <lcdi2c.h>

#define addressi 0x20

// Configuració de la xarxa WiFi
const char* ssid = "TDFMQTT0";
const char* password = "T3mpsD3F1ors";
// Configuració de la xarxa WiFi
//const char* ssid = "NOM_DE_LA_TEVA_XARXA";
//const char* password = "CONTRASENYA";
WiFiUDP udp;
unsigned int localUdpPort = 4210;  // Port UDP local
char incomingPacket[51];  // Buffer per rebre 50 caràcters més '\0'
IPAddress remoteIp;
unsigned int remotePort;

//
int val1[3];
int value = 0;
int addrp = 0;
int addrh = 0;
int j     = 0;
int f     = 0;
int pannell;
int lmax;

char buffer[20];
//
char nibble[512];       // per missatges rebuts
//
char packet[512];
int npaq   = 0;
int pks    = 0;
int cnt    = 0;
int estat  = 0;
int dada   = 0;
int modul  = 0;
//
int angle1 = 0;
int angle2 = 80;
int angle3 = 160;
int cicle  = 50;

byte  Servos[ 25] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  90, 90, 90, 90, 90};  // valors inicials
byte  spos[ 25];

// Configuració dels servos
uint8_t servoValues[25];  // Array per emmagatzemar els valors dels servos

// Inicialitza els dos mòduls PCA9685 amb les seves adreces I2C
Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40); // Mòdul 1 a l'adreça I2C 0x40
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41); // Mòdul 2 a l'adreça I2C 0x41

// Definicions dels límits del servo
// #define SERVO_MIN 150  // Pols mínim per a la posició 0 graus
//#define SERVO_MAX 600  // Pols màxim per a la posició 180 graus
// Definicions dels límits del servo
#define SERVO_MIN 150  // Pols mínim per a la posició 0 graus
#define SERVO_MAX 600  // Pols màxim per a la posició 180 graus

int c = 0;

//
// Llegir adreça
//
int Llegir_addr()
{
    // Escriu els valors inicials al PCF8574
    Wire.beginTransmission(addressi);
    Wire.write(B00001111); // Exemple de dades a enviar
    Wire.write(B00000000);
    if (Wire.endTransmission() != 0) { // Comprova errors de transmissió
        Serial.println("Error en la transmissió I2C");
        return 256;
    }

    // Sol·licita 2 bytes del dispositiu
    if (Wire.requestFrom(addressi, 2) != 2) { // Comprova si arriben 2 bytes
        Serial.println("No s'han rebut els bytes esperats");
        return 256;
    }

    // Llegeix els bytes rebuts
    for (int j = 0; j < 2; j++) {
        if (Wire.available()) {
            val1[j] = Wire.read();
        }
    }

    // Processa els valors rebuts
    addrp = val1[0] & 0x0F;
    addrh = val1[1] & 0xFF;

    // Depuració
  //  Serial.print("addrp: ");
  //  Serial.println(addrp, HEX);
  //  Serial.print("addrh: ");
  //  Serial.println(addrh, HEX);

    return addrp;
}

// Envia el vector de servos
void envia_servos(){
  for (int i = 0; i < 12; i++){
    pca1.setPWM(i, 0, map(Servos[i],    0, 180, SERVO_MIN, SERVO_MAX));
    pca2.setPWM(i, 0, map(Servos[i+12], 0, 180, SERVO_MIN, SERVO_MAX));
    delay(5);   
  }
}

//
// Rutina d'inici de la WiFi
//
void setup_wifi() {
//
  delay(100);
//
// Assigna una adreça estàtica per cada rusc
#ifdef CASA
   IPAddress local_IP(192, 168, 2, 10+addrp);   // adreça IP (depèn del rusc)
   IPAddress gateway( 192, 168, 2, 1);          // IP del router
#else
   IPAddress local_IP(192, 168, 10, 10+addrp);  // adreça IP
   IPAddress gateway( 192, 168, 10, 1);         // IP del router
#endif
   IPAddress subnet(255, 255, 255, 0);          // màscara
//
#ifdef LCDdebug
  lcd_setCursor(12, 1);  lcd_print("                ");
  lcd_setCursor(12, 1);  lcd_print(ssid);   // visualitza ssid
  delay(500);
#endif
//
  WiFi.mode(WIFI_STA);                          // modus estació (AP)
//
  wifi_set_sleep_type(NONE_SLEEP_T);            // Per que no esperi (Jordi F.)
//                                              // Aportació de Jordi Fàbregas
  // Configura adreça IP estàtica
  if (!WiFi.config(local_IP, gateway, subnet)) {
#ifdef LCDdebug
    lcd_setCursor(0, 2);  lcd_print("                ");
    lcd_setCursor(0, 2);  lcd_print(WiFi.localIP().toString().c_str());    // visualitza IP
#endif
  }
//
  WiFi.begin(ssid, password);                     // Engega i es connecta a WiFi
//
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
#ifdef LCDdebug
    lcd_setCursor(0, 0);  lcd_print("Con.Err,Reboot."); // connexió fallida (reboot)
#endif
    delay(5000);  // en cas d'error al cap de 5 segons ho reintenta
    ESP.restart();
  }
//
Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
#ifdef LCDdebug
    if (f == 0) {f = 1;  lcd_setCursor(14, 0);  lcd_print(" *");} // Mentre intenta connectar
    else        {f = 0;  lcd_setCursor(14, 0);  lcd_print("* ");}
#endif
 }
//
#ifdef LCDdebug
  lcd_setCursor(0, 2);  lcd_print("                ");
  lcd_setCursor(0, 2);  lcd_print(WiFi.localIP().toString().c_str());
#endif
  delay(1500);
//
//  Begin listening to UDP port
  udp.begin(localUdpPort);
//  
}   //  setup_wifi
//
//
//


void udp_pkt()
{
int packetSize = udp.parsePacket();  
  if (packetSize) {  
    pks++;      // conta el nombre de paquets
//
//
    int len = udp.read(packet, 392);
//
    if (len > 0)
    {
      packet[len] = '\0';
      dada = 1;
    }
//
    lmax = len;
//
#ifdef LCDdebug
  lcd_setCursor( 10, 3);  lcd_print(String(pks).c_str());   // mostra nombre de paquets rebuts
#endif
//
    for (int i = 0; i < len; i++) {
      nibble[i] = (char)packet[i];
//
      if ((nibble[i] >= '0') && (nibble[i] <= '9')) {
        nibble[i] = nibble[i] - 48;
      }
      else if ((nibble[i] >= 'A') && (nibble[i] <= 'F')) {
        nibble[i] = nibble[i] - 55;
      }
      else if ((nibble[i] >= 'a') && (nibble[i] <= 'f')) {
        nibble[i] = nibble[i] - 87;
      }
      else {
        dada = 0;
      }
    }  // for
  }    // if(packetSize)
//
//
}


// diverses proves de test de servos
void descodifica1()
{

  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);
  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle1, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle1, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);
  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);
  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle3, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle3, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);
  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle2, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);

}

// diverses proves de test de servos
void descodifica2()
{

  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle1, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle1, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);
  for (int i = 0; i<16; i++){
    pca1.setPWM(i, 0, map(angle3, 0, 180, SERVO_MIN, SERVO_MAX));
  }
  for (int i = 0; i<16; i++){
    pca2.setPWM(i, 0, map(angle3, 0, 180, SERVO_MIN, SERVO_MAX));
  }
    delay(cicle);

}

// diverses proves de test de servos
void descodifica()
{
for (int j = 0; j<160; j=j+4){
    for (int i = 0; i<16; i++){
      pca1.setPWM(i, 0, map(j, 0, 180, SERVO_MIN, SERVO_MAX));
    }
    for (int i = 0; i<16; i++){
      pca2.setPWM(i, 0, map(j, 0, 180, SERVO_MIN, SERVO_MAX));
    }
      delay(cicle);
  }

for (int j = 160; j>0; j=j-4){
    for (int i = 0; i<16; i++){
      pca1.setPWM(i, 0, map(j, 0, 180, SERVO_MIN, SERVO_MAX));
    }
    for (int i = 0; i<16; i++){
      pca2.setPWM(i, 0, map(j, 0, 180, SERVO_MIN, SERVO_MAX));
    }
      delay(cicle);
  }

}

void descodificam() {
// descodifica els missatge rebuts a callback() per llargada de missatge
//
//  
  if (dada == 1) { // Si ha arribat un missatge ... processar
//  
  switch(lmax) {
//
// Rusc Clear
//
  case 50:    // 50 bytes   
      {
         modul  = int(nibble[0]) * 16 + int(nibble[1]);

         for (int i = 0; i < 24; i++){
           spos[i] = int(nibble[2 + i * 2]) * 16 + int(nibble[3 + i * 2]);
           Servos[i] = map(spos[i],0,255,20,160);
         }
         envia_servos();
//
          break;
     } // if lmax == 2
//
//
//  
  default:
      break;
//
  }  // switch-case
//
  dada = 0;  // acabat el processat, espera el següent
//  
  }  // if dada==1
}    // descodificam
//
//



void setup() {
  Serial.begin(115200);
  Wire.begin(); // Inicia la comunicació I2C
#ifdef LCDdebug
  lcd_init();    // Inicia el display
  lcd_print("Temps de flors 2025"); // Escriu un missatge
#endif
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println("TDF 2025");
  // Inicia els mòduls PCA9685
  pca1.begin();
  pca1.setPWMFreq(50); // Estableix la freqüència PWM a 50 Hz per als servos

  pca2.begin();
  pca2.setPWMFreq(50); // Estableix la mateixa freqüència per al segon mòdul

  delay(10); // Espera perquè la configuració es faci efectiva

  //                                   // Llegeix l'adreça de rusc dels interruptors
  Wire.beginTransmission(addressi);   // Transmit to device number 44 (0x2C)
  Wire.write(B00001111);
  Wire.write(B00000000);
  Wire.endTransmission();            // Stop transmitting
//
  Wire.requestFrom(addressi, 2);      // Solicitar 2 bytes del esclau #20 .. #27
    while(Wire.available()) {
        val1[j] = Wire.read();       // Receive a byte as character
        j++;
    }
  addrp = val1[0] & 0x0F;
  addrh = val1[1] & 0xFF;
  Wire.endTransmission();
//
  pannell = addrp & 0x07;        // adreça del rusc
//
#ifdef LCDdebug
  sprintf(buffer, "Hex: 0x%X", addrp);
  lcd_setCursor(0, 1);
  lcd_print(buffer);
#endif
//
  setup_wifi();   // Setup WiFi  *****
//
Serial.println("exit setup");
}


void loop() {
  c++;
#ifdef LCDdebug
  lcd_setCursor(0, 3); // Mou el cursor a la segona fila
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "Lmax: %d", lmax);
  lcd_print(buffer);   // Mostra els segons transcorreguts
#endif
int a = Llegir_addr();
//  Serial.println(a);
#ifdef LCDdebug
    sprintf(buffer, "Hex: 0x%X", a);
    lcd_setCursor(0, 1);
    lcd_print(buffer);
#endif

  udp_pkt();      // Paquets
  descodificam();  // descodifica els missatges

   // Aquí pots afegir més moviments segons les teves necessitats
}
