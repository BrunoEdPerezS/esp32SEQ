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
int scrollTIME = 500;
long timeB1,timeB2,timeB3,timeB4,timeB5;
long ltimeB1,ltimeB2,ltimeB3,ltimeB4,ltimeB5;
int seqBANK1[16] = {0,0,0,1,0,0,1,0,0,1,1,1,0,0,1,1}; 
int seqBANK2[16] = {1,1,1,0,1,1,0,1,1,0,0,0,1,1,0,0};
int bankFLAG = 0;
int seqNOTE[16][2], arpBANK1[16][2], arpBANK2[16][2];

int nota = 0;
int octava = 0;




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
  //Inicializamos matriz de notas
  for (int i=0; i <= 15;i++)
  {
    for (int j=0; j <= 1;j++)
  {
    seqNOTE[i][j]=0;
    arpBANK1[i][j]=0;
    arpBANK2[i][j]=0;
  }
  }


  //Visualizar pantalla de inicio para secuenciador
  seqPANTALLA_init();
  delay(1000);


//INICIALIZACION DE LOS DISPLAY
//screenONDA(0);
//screenBPM(bpm);
mainDISPLAY();
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
    oled.clearDisplay();
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
    //MAIN DISPLAY
    mainDISPLAY();
    }
}
//-----------------MODO SECUENCIA---------------------




//****************MODO EDITOR*************************
//Trigger del EDITOR
if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > debTIME)
    {
    //---------------------------
    modoEDITOR = true;
    //Iniciamos step monitor y la secuencia 1 predeterminada
    oled.clearDisplay();
    editorDISPLAY();
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
    storeMATRIX(arpBANK1,seqNOTE);
    bankFLAG = 0;
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
    storeMATRIX(arpBANK2,seqNOTE);
    bankFLAG = 1;
    Serial.print("FUERA:Cambiando a bank 2 \n");
    //---------------------------
    ltimeB5 = timeB5;
    }
    }


//STEP EDITOR-----------------------------------------------------------
//Trigger del STEP EDITOR
if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > debTIME)
    {
    //---------------------------
    stepedSTART = true;
    Serial.print("Step editor iniciado \n");
    step = 0;
    seqSCROLLING(step);
    stepDISPLAY();
    //---------------------------
    ltimeB2 = timeB2;
    }
    }

while(stepedSTART)
{
  //Serial.print("STEP EDITOR \n");
  //step editor code below

  //BUTTON 2 SCROLL
  if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > scrollTIME)
    {
    //---------------------------
    //Contador de steps
    if (step == 15)
    {
    step = 0;
    }
    else
    {
    step = step+1;
    }
    seqSCROLLING(step);
    //---------------------------
    ltimeB2 = timeB2;
    }
    }

  //BUTTON 3 STEP TOGGLE
  if(digitalRead(BUTTON3))
   {
    timeB3 = millis();
    if (timeB3 - ltimeB3 > debTIME)
    {
    //---------------------------
    //Toggle de steps
    seqVECTOR[step]=toggleSTEPS(seqVECTOR,step);
    //---------------------------
    ltimeB3 = timeB3;
    }
    }






  //Exit&Save del step editor
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
    if (bankFLAG==0){
    storeVECTOR(seqVECTOR,seqBANK1);}
    else{
    storeVECTOR(seqVECTOR,seqBANK2);}
    editorDISPLAY();
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
    step = 0;
    seqSCROLLING(step);
    //Parametros para editor
    arpDISPLAY();
    noteDISPLAY(seqNOTE[step][0],seqNOTE[step][1]);
    nota =seqNOTE[step][0];
    octava =seqNOTE[step][0];
    //---------------------------
    ltimeB3 = timeB3;
    }
    }

while(arpedSTART)
{
  //Serial.print("ARP EDITOR \n"); 
  //arp editor code below

  //BUTTON 2 SCROLL
  if(digitalRead(BUTTON2))
   {
    timeB2 = millis();
    if (timeB2 - ltimeB2 > scrollTIME)
    {
    //---------------------------
    //Contador de steps
    if (step == 15)
    {
    step = 0;
    }
    else
    {
    step = step+1;
    }
    seqSCROLLING(step);
    noteDISPLAY(seqNOTE[step][0],seqNOTE[step][1]);
    //Inicializar nota y octava post scroll
    nota =seqNOTE[step][0];
    octava =seqNOTE[step][0];
    //---------------------------
    ltimeB2 = timeB2;
    }
    }
 
  //BUTTON 3 EDIT NOTE
  if(digitalRead(BUTTON3))
   {
    timeB3 = millis();
    if (timeB3 - ltimeB3 > debTIME)
    {
    //---------------------------
    //Contador de notas
    if (nota == 11)
    {
    nota = 0;
    }
    else
    {
    nota = nota+1;
    }
    seqNOTE[step][0] = nota;
    noteDISPLAY(seqNOTE[step][0],seqNOTE[step][1]);
    //---------------------------
    ltimeB3 = timeB3;
    }
    }
  //BUTTON 4 EDIT NOTE
  if(digitalRead(BUTTON4))
   {
    timeB4 = millis();
    if (timeB4 - ltimeB4 > debTIME)
    {
    //---------------------------
    //Contador de notas
    if (octava == 5)
    {
    octava = 0;
    }
    else
    {
    octava = octava+1;
    }
    seqNOTE[step][1] = octava;
    noteDISPLAY(seqNOTE[step][0],seqNOTE[step][1]);
    //---------------------------
    ltimeB4 = timeB4;
    }
    }






  //Exit&save del arp editor
  if(digitalRead(BUTTON1))
   {
    timeB1 = millis();
    if (timeB1 - ltimeB1 > debTIME)
    {
    //---------------------------
    arpedSTART = false;
    //Iniciamos step monitor y la secuencia seleccionada
    oled.fillRect(0, 15, 40, 15, BLACK);
    seqTRIANGULOS_init();
    stepMONITOR_init(seqVECTOR);
    if (bankFLAG==0){
    storeMATRIX(seqNOTE,arpBANK1);}
    else{
    storeMATRIX(seqNOTE,arpBANK2);}
    editorDISPLAY();
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
    //MAIN DISPLAY
    mainDISPLAY();
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

//Store del vector de steps
void storeVECTOR(int origen[16], int destino[16]) {
    memcpy(destino, origen, sizeof(int)*16);
}

//Store matriz de arpegio
void storeMATRIX(int origen[16][2], int destino[16][2]) {
    memcpy(destino, origen, sizeof(int)*16*2);
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
   int step = (_step);
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

//---------------------------------------------------------------------
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
//---------------------------------------------------------------------

//De numeroNota a stringNota
String traductorNOTA(int n)
{
  String NOTA;
  if (n == 0) NOTA = "C";
  if (n == 1) NOTA = "C#";
  if (n == 2) NOTA = "D";
  if (n == 3) NOTA = "D#";
  if (n == 4) NOTA = "E";
  if (n == 5) NOTA = "F";
  if (n == 6) NOTA = "F#";
  if (n == 7) NOTA = "G";
  if (n == 8) NOTA = "G#";
  if (n == 9) NOTA = "A";
  if (n == 10) NOTA = "A#";
  if (n == 11) NOTA = "B";
  return NOTA;
}







//Displays para las funcionalidades
void editorDISPLAY()
{
  oled.fillRect(0, 0, 120, 10, BLACK);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("EDITOR ENABLED");
  oled.display();
}

void arpDISPLAY()
{
  oled.fillRect(90, 0, 30, 10, BLACK);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(90,0);
  oled.println("ARP");
  oled.display();
}

void stepDISPLAY()
{
  oled.fillRect(90, 0, 30, 10, BLACK);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(90,0);
  oled.println("STEP");
  oled.display();
}
   
void mainDISPLAY()
{
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("MAIN");
  oled.display();
}

void noteDISPLAY(int note, int octave)
{
  String NOTA;
  NOTA = traductorNOTA(note);
  oled.fillRect(0, 15, 40, 15, BLACK);
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(0,15);
  oled.println(NOTA);
  oled.setCursor(25,15);
  oled.println(octave);
  oled.display();
}