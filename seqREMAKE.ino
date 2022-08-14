//Pre-codigo para secuencia en pantalla oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "wavetables.h"


//Definir parametros del OLED---------------------
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Pines-------------------------------------------
#define BUTTON1 25
#define BUTTON2 33
#define BUTTON3 32
#define BUTTON4 14
#define BUTTON5 27
#define clockOUT 23


//Variables---------------------------------------
int miliSecs;
int bpm = 100;
bool modoSECUENCIA=false;
bool modoEDITOR= false;
bool seqSTART=false;
bool stepedSTART=false;
bool arpedSTART=false;
//int seqBANK1[16], seqBANK2[16];
int step;
int debTIME = 250;
long timeB1,timeB2,timeB3,timeB4,timeB5;
long ltimeB1,ltimeB2,ltimeB3,ltimeB4,ltimeB5;
int seqBANK1[16] = {0,0,0,1,0,0,1,0,0,1,1,1,0,0,1,1}; 
int seqBANK2[16] = {1,1,1,0,1,1,0,1,1,0,0,0,1,1,0,0};


//Secuencia
int seqVECTOR[16];


void setup()
{
//**************************SEQUENCER*****************************************

  pinMode(BUTTON1,INPUT_PULLDOWN);
  pinMode(BUTTON2,INPUT_PULLDOWN);
  pinMode(BUTTON3,INPUT_PULLDOWN);
  pinMode(BUTTON4,INPUT_PULLDOWN);
  pinMode(BUTTON5,INPUT_PULLDOWN);
  
  pinMode(clockOUT,OUTPUT);

  //Iniciar monitor serial
  Serial.begin(9600);
  // Inicializar la pantalla OLED con el adress -> 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }
  delay(1); 
  oled.setCursor(0, 0);
  //Calculo del tempo inicial----------------------------
  miliSecs = (60000/bpm/4)/2; //calcular duracion de cada step
  //Inicializar matriz de secuencia
  for (int i=0; i <= 15;i++)
  {
  seqVECTOR[i] = 0;
  }



  //Visualizar pantalla de inicio para secuenciador
  seqPANTALLA_init();
  delay(1000);


//INICIALIZACION DE LOS DISPLAY
screenONDA(0);
screenBPM(bpm);
}

void loop()
{







//-----------------MODO SECUENCIA---------------------
//Trigger del MODO SECUENCIA
if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    modoSECUENCIA = true;
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK1);
    storeVECTOR(seqBANK1,seqVECTOR);
    Serial.print("Modo secuencia iniciado \n");
    //---------------------------
    ltimeB1 = timeB1;
    }
    }

//Modo secuencia
while(modoSECUENCIA){


//Selectores de secuencia
if(digitalRead(BUTTON4))
   {
    timeB4 = millis();
    if (timeB4 - ltimeB4 > debTIME)
    {
    //---------------------------
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK1);
    storeVECTOR(seqBANK1,seqVECTOR);
    Serial.print("FUERA:Cambiando a bank 1 \n");
    //---------------------------
    ltimeB4 = timeB4;
    }
    }

if(digitalRead(BUTTON5))
   {
    timeB5 = millis();
    if (timeB5 - ltimeB5 > debTIME)
    {
    //---------------------------
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK2);
    storeVECTOR(seqBANK2,seqVECTOR);
    Serial.print("FUERA:Cambiando a bank 2 \n");
    //---------------------------
    ltimeB5 = timeB5;
    }
    }


//Iniciar secuenciador
if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > debTIME)
    {
    //---------------------------
    seqSTART = true;
    step = 0;
    seqTRIANGULOS_init();
    //---------------------------
    ltimeB2 = timeB2;
    }
    }


//Secuencia ON
while(seqSTART)
{
  //Cambio de secuencia
  if(digitalRead(BUTTON4))
   {
    timeB4 = millis();
    if (timeB4 - ltimeB4 > debTIME)
    {
    //---------------------------
    //Cargamos BANK1
    stepMONITOR_init(seqBANK1);
    storeVECTOR(seqBANK1,seqVECTOR);
    Serial.print("Cambiando a bank 1 \n");
    Serial.print("\n");
    //---------------------------
    ltimeB4 = timeB4;
    }
  }

  if(digitalRead(BUTTON5))
   {
    timeB5 = millis();
    if (timeB5 - ltimeB5 > debTIME)
    {
    //---------------------------
    //Cargamos BANK2
    stepMONITOR_init(seqBANK2);
    storeVECTOR(seqBANK2,seqVECTOR);
    Serial.print("Cambiando a bank 2 \n");
    Serial.print("\n");
    //---------------------------
    ltimeB5 = timeB5;
    }
  }

  //scrolling....
  seqSCROLLING(step);
  //Salidas para los estados de steps
  if(seqVECTOR[step] == 1)
  {
      digitalWrite(clockOUT,HIGH);
      delay(miliSecs);
      digitalWrite(clockOUT,LOW);
      delay(miliSecs);
  }
  else
  {
      digitalWrite(clockOUT,LOW);
      delay(2*miliSecs);
  }
  //Contador de steps
  if (step == 15)
  {
  step = 0;
  }
  else
  {
  step = step+1;
  }


//Secuencia OFF
if(digitalRead(BUTTON3))
   {
    timeB3 = millis();
    if (timeB3 - ltimeB3 > debTIME)
    {
    //---------------------------
    seqSTART = false;
    seqTRIANGULOS_init();
    //---------------------------
    ltimeB3 = timeB3;
    }
    }
}

//Salida del modo secuencia
if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    modoSECUENCIA = false;
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    seqSTEPS_init();
    Serial.print("Saliendo del modo secuencia... \n");
    //---------------------------
    ltimeB1 = timeB1;
    }
    }
}
//-----------------MODO SECUENCIA---------------------




//****************MODO EDITOR*************************
//Trigger del EDITOR DE SECUENCIA
if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > debTIME)
    {
    //---------------------------
    modoEDITOR = true;
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK1);
    storeVECTOR(seqBANK1,seqVECTOR);
    Serial.print("Modo editor iniciado \n");
    //---------------------------
    ltimeB2 = timeB2;
    }
    }

//Loop del modo editor
while(modoEDITOR)
{
Serial.print("MODO EDITOR \n");
if(digitalRead(BUTTON4))
   {
    timeB4 = millis();
    if (timeB4 - ltimeB4 > debTIME)
    {
    //---------------------------
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK1);
    storeVECTOR(seqBANK1,seqVECTOR);
    Serial.print("FUERA:Cambiando a bank 1 \n");
    //---------------------------
    ltimeB4 = timeB4;
    }
    }

if(digitalRead(BUTTON5))
   {
    timeB5 = millis();
    if (timeB5 - ltimeB5 > debTIME)
    {
    //---------------------------
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqBANK2);
    storeVECTOR(seqBANK2,seqVECTOR);
    Serial.print("FUERA:Cambiando a bank 2 \n");
    //---------------------------
    ltimeB5 = timeB5;
    }
    }


//STEP EDITOR-----------------------------------------------------------
//Trigger del EDITOR DE STEPS
if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > debTIME)
    {
    //---------------------------
    stepedSTART = true;
    Serial.print("Step editor iniciado \n");
    //---------------------------
    ltimeB2 = timeB2;
    }
    }

while(stepedSTART)
{
  //Serial.print("STEP EDITOR \n");
  //step editor code below









  //Exit del step editor
  if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    stepedSTART = false;
    //Iniciamos step monitor y la secuencia seleccionada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqVECTOR);
    Serial.print("Saliendo del step editor \n");
    //---------------------------
    ltimeB1 = timeB1;
    }
    }  
}



//arp EDITOR-----------------------------------------------------------
if(digitalRead(BUTTON3))
   {
    timeB3 = millis();
    if (timeB3 - ltimeB3 > debTIME)
    {
    //---------------------------
    arpedSTART = true;
    Serial.print("Arp editor iniciado \n");
    //---------------------------
    ltimeB3 = timeB3;
    }
    }

while(arpedSTART)
{
  //Serial.print("ARP EDITOR \n"); 
  //arp editor code below









  //Exit del arp editor
  if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    arpedSTART = false;
    //Iniciamos step monitor y la secuencia seleccionada
    seqTRIANGULOS_init();
    stepMONITOR_init(seqVECTOR);
    Serial.print("Saliendo del arp editor \n");
    //---------------------------
    ltimeB1 = timeB1;
    }
    }  
}




//Exit del modo editor
if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    modoEDITOR = false;
    //Iniciamos step monitor y la secuencia 1 predeterminada
    seqTRIANGULOS_init();
    seqSTEPS_init();
    Serial.print("Saliendo del modo editor \n");
    //---------------------------
    ltimeB1 = timeB1;
    }
    }  
}


//****************MODO EDITOR SEQ*************************






}
//******************************FUNCIONES******************************
//Iniciar pantalla
void seqPANTALLA_init()
{
  oled.clearDisplay();
  for (int i = 0; i<=8; i++)
  { 
    if ((i == 0)||(i==4))
    {
      //BEATS
      oled.drawRect( i*16, 42, 15, 7, WHITE);
      oled.drawRect( i*16, 56, 15, 7, WHITE);
    }
    else
    {
      //offbeat steps
      oled.drawRoundRect( i*16, 42, 15, 7,8, WHITE);
      oled.drawRoundRect( i*16, 56, 15, 7,8, WHITE);
    }
          //Triangulos
      oled.drawTriangle(5+i*16, 36, 10 + i*16, 36, 7+ i*16, 42, WHITE);
      oled.drawTriangle(5+i*16, 50, 10 + i*16, 50, 7+ i*16, 56, WHITE);
      oled.display();
  }
}

//Reinicial el monitor de secuencia (TRIANGULOS)
void seqTRIANGULOS_init()
{
  for (int i = 0; i<=8; i++)
  { 
      //Triangulos
      oled.fillTriangle(5+i*16, 36, 10 + i*16, 36, 7+ i*16, 42, BLACK);
      oled.drawTriangle(5+i*16, 36, 10 + i*16, 36, 7+ i*16, 42, WHITE);
      oled.fillTriangle(5+i*16, 50, 10 + i*16, 50, 7+ i*16, 56, BLACK);
      oled.drawTriangle(5+i*16, 50, 10 + i*16, 50, 7+ i*16, 56, WHITE);
      oled.display();
  }
}

//Reinicial el monitor de STEPS (cuadrados)
void seqSTEPS_init()
{
  for (int i = 0; i<=8; i++)
  { 
    if ((i == 0)||(i==4))
    {
      //BEATS
      oled.fillRect( (i)*16, 42, 15, 7, BLACK);
      oled.drawRect( i*16, 42, 15, 7, WHITE);
      oled.fillRect( (i)*16, 56, 15, 7, BLACK);
      oled.drawRect( i*16, 56, 15, 7, WHITE);
    }
    else
    {
      //offbeat steps
      oled.fillRect( (i)*16, 42, 15, 7, BLACK);
      oled.drawRoundRect( i*16, 42, 15, 7,8, WHITE);
      oled.fillRect( (i)*16, 56, 15, 7, BLACK);
      oled.drawRoundRect( i*16, 56, 15, 7,8, WHITE);
    }
  }
  oled.display();
}

//Funcion para el "scrolling"
void seqSCROLLING(int _step)
{
       int step = _step;


//Secuencia
    //8 steps de arriba-------------
    //(Esta wea es el reset del step 8 de abajo)
    if (step == 0)
    {
    oled.fillTriangle(5+(7)*16, 50, 10 + (7)*16, 50, 7+ (7)*16, 56, BLACK);
    oled.drawTriangle(5+(7)*16, 50, 10 + (7)*16, 50, 7+ (7)*16, 56, WHITE);
    }
    if (step<=7)
    {
    oled.fillTriangle(5+step*16, 36, 10 + step*16, 36, 7+ step*16, 42, WHITE);
    //Borrar anterior
    if (step > 0)
    {
    oled.fillTriangle(5+(step-1)*16, 36, 10 + (step-1)*16, 36, 7+ (step-1)*16, 42, BLACK);
    oled.drawTriangle(5+(step-1)*16, 36, 10 + (step-1)*16, 36, 7+ (step-1)*16, 42, WHITE);
    }
    }
    //8 steps de abajo-------------
    else
    {
    oled.fillTriangle(5+(step-8)*16, 50, 10 + (step-8)*16, 50, 7+ (step-8)*16, 56, WHITE);
    //Borrar el anterior
    if (step == 8)
    {
    oled.fillTriangle(5+(step-1)*16, 36, 10 + (step-1)*16, 36, 7+ (step-1)*16, 42, BLACK);
    oled.drawTriangle(5+(step-1)*16, 36, 10 + (step-1)*16, 36, 7+ (step-1)*16, 42, WHITE);
    }
    if (step > 8)
    {
    oled.fillTriangle(5+(step-9)*16, 50, 10 + (step-9)*16, 50, 7+ (step-9)*16, 56, BLACK);
    oled.drawTriangle(5+(step-9)*16, 50, 10 + (step-9)*16, 50, 7+ (step-9)*16, 56, WHITE);
    }
    }
    oled.display();
}

//Store de la matriz
void storeVECTOR(int origen[16], int destino[16]) {
    memcpy(destino, origen, sizeof(int)*16);
}

//Inicializar STEPS matrix (CUADRADOS)
void stepMONITOR_init(int seqVECTOR[16])
{
  //Reiniciar display de secuencia
  seqSTEPS_init();
  //Pintar steps
  for (int i = 0; i<=8; i++)
  { 
    if ((i == 0)||(i==4))
    {
      //Pintar arriba
      if (seqVECTOR[i] == 1)
      {
        oled.fillRect( i*16, 42, 15, 7, WHITE);
      }
      //Pintar abajo
      if (seqVECTOR[i+8] == 1)
      {
        oled.fillRect( i*16, 56, 15, 7, WHITE);
      }
    }
    else
    {
      if (seqVECTOR[i] == 1)
      {
        oled.fillRoundRect(i*16, 42, 15, 7, 8, WHITE);
      }
      //Pintar abajo
      if (seqVECTOR[i+8] == 1)
      {
        oled.fillRoundRect(i*16, 56, 15, 7, 8, WHITE);
      }
    }
  }
  oled.display();
}

//Toggle de los steps
int toggleSTEPS(int seqVECTOR[16], int _step)
{
   int output;
   int step = (_step-1);
   //Pintar los steps qlos
   //stepMONITOR_init(seqMATRIX);
//***********************************Caso de step prendio***********************************
   if (seqVECTOR[step] == 0)
   {
    //Pintar arriba
    if (step <=7)
    {
      //Pintar cuadrado
      if((step == 0)&&(step == 4))
      {
      oled.fillRect((step)*16, 42, 15, 7, WHITE);
      }
      //Pintar redondo
      else
      {
      oled.fillRoundRect((step)*16, 42, 15, 7,8, WHITE);
      }
    }
    //Pintar abajo
    if((step) >=8)
    {
      //Pintar cuadrado
      if(((step-8) == 0)&&((step-8) == 4))
      {
      oled.fillRect((step-8)*16, 56, 15, 7, WHITE);
      }
      //Pintar redondo
      else
      {
      oled.fillRoundRect((step-8)*16, 56, 15, 7, 8, WHITE);  
      }
    }
    //oled.display();
    //Tiramos un 1 a la salida
    output = 1;
   }
//***********************************Caso de step apagado***********************************
   if (seqVECTOR[step] == 1)
   {
    //Despintar arriba
    if (step <=7)
    {
      //Despintar cuadrado
      if((step == 0)||(step == 4))
      {
      oled.fillRect((step)*16, 42, 15, 7, BLACK);
      oled.drawRect((step)*16, 42, 15, 7, WHITE);
      }
      //Despintar redondo
      else
      {
      oled.fillRoundRect( (step)*16, 42, 15, 7,8, BLACK);
      oled.drawRoundRect( (step)*16, 42, 15, 7,8, WHITE);
      }

    }
    //Despintar abajo
    if(step >=8)
    {
      //Despintar cuadrado
      if(((step-8) == 0)||((step-8) == 4))
      {
      oled.fillRect((step-8)*16, 56, 15, 7, BLACK);
      oled.drawRect((step-8)*16, 56, 15, 7, WHITE);
      }
      //Despintar redondo
      else
      {
      oled.fillRoundRect((step-8)*16, 56, 15, 7,8, BLACK);
      oled.drawRoundRect((step-8)*16, 56, 15, 7,8, WHITE);
      }
    }
    //oled.display();
    //Tiramos un 0 a la salida
    output = 0;
   }
   oled.display();
   return output;
}

//Variar BPM ????
int variarBPM(int lectura)
{
  int bpm;
  bpm = (int)(lectura*(0.0488));
  if (bpm < 60)
  {
    bpm = 60;
    return bpm;
  }
  else
  return bpm;
}

//Funcion que displayea la onda
void screenONDA(int count)
{
  oled.fillRect(0, 10, 100, 10, BLACK);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,10);
if (count ==0)
{
oled.println("SINE");
}
if (count ==1)
{
oled.println("SAW");
}
if (count ==2)
{
oled.println("TRIANGLE");
}
if (count ==3)
{
oled.println("SQUARE");
}
oled.display();
}

//Funcion que displayea los bpm
void screenBPM(int bpm)
{
  String print = "BPM ";
  oled.fillRect(0, 23, 100, 10, BLACK);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,23);
  oled.println(print + String(bpm));
  oled.display();
}







   
