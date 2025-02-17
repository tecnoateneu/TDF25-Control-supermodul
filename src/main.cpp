//
//  Tecnoateneu de Vilablareix
//  Base 42
//  Realització per el Temps de Flors 2025
//  Girona 2024
//
//  Programació realitzada per Antoni Martorano i Gomis
//
//  DMX
//
//

#define LCDdebug           // a fi de debugar i veure el que fa

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Configuració del display
#define LCD_ADDR 0x27 // Adreça I2C del display
#define LCD_COLS 20   // Nombre de columnes
#define LCD_ROWS 4    // Nombre de files

#include <lcdi2c.h>
#include <DMX/DMX.h>


#define addressi 0x20 // Adreça LCD

// Configuració de la xarxa WiFi
const char* ssid = "TDFMQTT0";
const char* password = "T3mpsD3F1ors";

// Defineix els pins TX i Output Enable
const int TX_PIN = 4;
const int OUTPUT_ENABLE_PIN = 16;

ESP8266DMXShield dmxShield(TX_PIN, OUTPUT_ENABLE_PIN);

// UDP
WiFiUDP udp;
unsigned int localUdpPort = 4210;  // Port UDP local
char incomingPacket[51];  // Buffer per rebre 50 caràcters més '\0'
IPAddress remoteIp;
unsigned int remotePort;

//
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
int cicle  = 50;

byte  DMX[520];  // Matriu total dmx
byte  dmx[ 40];  // matriu rebuda dmx

int   c = 0;

// Envia el vector de servos
void envia_dmx(){
  for (int i = 0; i < 512; i++){
    dmxShield.sendByte(i, DMX[i]);
    delay(10);   
  }
}

//
// Rutina d'inici de la WiFi
//
void setup_wifi() {
//
  int i = 0;
  delay(100);
//
// Assigna una adreça estàtica per cada rusc
   IPAddress local_IP(192, 168, 10, 30);        // adreça IP
   IPAddress gateway( 192, 168, 10, 1);         // IP del router

   IPAddress subnet(255, 255, 255, 0);          // màscara
//
#ifdef LCDdebug
  lcd_setCursor(12, 1);  lcd_print("                ");
  lcd_setCursor(12, 1);  lcd_print(ssid);       // visualitza ssid
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
    if (i == 0) {i = 1;  lcd_setCursor(14, 0);  lcd_print(" *");} // Mentre intenta connectar
    else        {i = 0;  lcd_setCursor(14, 0);  lcd_print("* ");}
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
  lcd_setCursor( 10, 3);
  lcd_print(String(pks).c_str());   // mostra nombre de paquets rebuts
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
}

void descodifica() {
// descodifica els missatge rebuts a callback() per llargada de missatge
// per DMX, els 4 primers bytes són l'adreça, els 30 següents són els valors dels registres
//  
  if (dada == 1) { // Si ha arribat un missatge ... processar
//  
  switch(lmax) {
//
// Rusc Clear
//
  case 34:    // 34 bytes   
      {
         modul  = int(nibble[0]) * 2048 + int(nibble[1]) * 256 + int(nibble[2]) * 16 + int(nibble[3]);
         modul  = modul & 0x01FF;

         for (int i = 0; i < 30; i++){
           dmx[i/2] = int(nibble[4 + i * 2]) * 16 + int(nibble[5 + i * 2]);
           i++;
         }

         for (int i = 0; i < 15; i++){
           DMX[i + modul] = dmx[i] ;
         }

         // cal transferir les dades al arrai DMX
         envia_dmx();
//
         break;

     }         // if lmax == 34
//
//
//  
  default:
      break;
//
  }            // switch-case
//
  dada = 0;    // acabat el processat, espera el següent
//  
  }  // if dada==1
}  // descodificam
//
//

void setup() {
//  Inicialment esborra el vector
  for (int i = 0;i < 512; i++) {
    DMX[i] = 0;
  }
  Serial.begin(115200);
  Wire.begin();    // Inicia la comunicació I2C
#ifdef LCDdebug
  lcd_init();      // Inicia el display
  lcd_print("Temps de flors 2025"); // Escriu un missatge
#endif
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println("TDF 2025");

  dmxShield.begin();

  delay(10);                           // Espera perquè la configuració es faci efectiva

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

  udp_pkt();      // recull o rep paquets
  descodifica();  // descodifica els missatges
}
