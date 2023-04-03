#include <RunningMedian.h>      // mediana krocząca
#include <LiquidCrystal_I2C.h>  // funkcje I2C

// utworzenie median kroczących
RunningMedian zeros = RunningMedian(100);
RunningMedian samplesX = RunningMedian(15);
RunningMedian samplesY = RunningMedian(15);
RunningMedian samplesZ = RunningMedian(15);

const int i2c_addr = 0x27;              //definicja adresu I2C
LiquidCrystal_I2C lcd(i2c_addr, 16, 2); //utworzenie obiektu wyświetlacza
 
// piny enkodera
const int SIA = 4; 
const int SIB = 6;
const int button = 5;

// zmienne obslugi enkodera
int counter = 0; 
int SIAVal;
int prevSIAVal; 

// piny akcelerometru
const int xpin = A3;                
const int ypin = A2;                
const int zpin = A1; 

// zmienne kalibracji akcelerometru
int xCal;
int yCal;
int zCal;
int zeroVal[3] = {0}; //xAcc, yAcc, zAcc
 
double xg;    // przyspieszenie na osi x [m/s^2]
double xVel;  // prędkość na osi x [m/s]

double yg;
double yVel;

double zg;
double zVel;
double zDist; // przebyty dystans na osi z [m]

double hVel;  // prędkość horyzontalna
double hDist;

double tVel;  // prędkość całkowita
double tDist;
 
unsigned long timer; //licznik czasu

 void setup() { 
  lcd.begin();          // inicjalizacja biblioteki LiquidCrystal_I2C
  Serial.begin (19200); // inicjalizacja monitora

  // definiowanie trybów pinów
  pinMode (SIA,INPUT);
  pinMode (SIB,INPUT);
  pinMode (button, INPUT);
  pinMode (xpin,INPUT);
  pinMode (ypin,INPUT);
  pinMode (zpin,INPUT);
   
  prevSIAVal = digitalRead(SIA);  // wczytanie początkowej wartości na SIA
  timer = micros();               // inicjalizacja licznika czasu

  // wyświetlenie powiadomienia o kalibracji
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calibrating");
  lcd.setCursor(0,1);
  lcd.print("Please wait...");

  delay(1000); // oczekiwanie na ustabilizowanie czujnika

  // kalibracja czujnika
  zeroVal[0] = getZero(xpin);
  zeroVal[1] = getZero(ypin);
  zeroVal[2] = getZero(zpin);
  zeroVal[2] += 71; // rekompensata -g na osi z w pozycji kalibracji
 } 
 void loop() {

  double dt = (double)(micros() - timer) / 1000000; // obliczanie dt z konwersją na sekundy
  timer = micros();

  //================AKCELEROMETR=========================
  // oś x
  xCal = analogRead(xpin)-zeroVal[0];  // pomiar przyspieszenia na osi x
  samplesX.add(xCal);                  // dodanie pomiaru do zbioru probek filtru medianowego
  int filteredX = samplesX.getMedian();// wyznaczanie wyniku filtracji
  xg = filteredX*9.80665/73;           // konwertowanie wyniku na postac m/s^2
  if(abs(xg)<=0.2) {xg=0; xVel=0;}     // przedzial nieczulosci
  xVel += integralR(xg, dt);           // wyznaczanie predkosci        

  // oś y
  yCal = analogRead(ypin)-zeroVal[1];
  samplesY.add(yCal);
  int filteredY = samplesY.getMedian();
  yg = filteredY*9.80665/72;
  if(abs(yg)<=0.2) {yg=0; yVel=0;}
  yVel += integralR(yg, dt);

  // oś z
  zCal = analogRead(zpin)-zeroVal[2];
  samplesZ.add(zCal);
  int filteredZ = samplesZ.getMedian();
  zg = (filteredZ*9.80665/71)+9.80665; // niwelowanie wpływu przyspieszenia ziemskiego
  if(abs(zg)<=0.2) {zg=0;zVel=0;}
  zVel += abs(integralR(zg, dt));
  zDist += abs(integralR(zVel, dt));   // wyznaczanie dystansu przebytego w pionie

  // prędkość i dystans w poziomie
  hVel=sqrt(sq(xVel)+sq(yVel));
  hDist += abs(integralR(hVel,dt));

  // całkowite prędkość i dystans
  tVel=sqrt(sq(hVel)+sq(zVel));
  tDist=sqrt(sq(hDist)+sq(zDist));
  
  //=====================ENKODER=============================
   SIAVal = digitalRead(SIA);           // aktualizacja wartości SIA
   if (SIAVal != prevSIAVal){           // zmiana wartości SIA = wystąpienie obrotu
       
     if (digitalRead(SIB) != SIAVal) {  // obrót enkodera przeciwnie do ruchu wskazówek zegara
       counter --;
       
     } else {                           // obrót enkodera zgodnie z ruchem wskazówek zegara
       counter ++;
       
     }
   }
  // zapętlenie licznika
     if (counter >= 6) counter = 0;
     if (counter <= -2) counter = 4;

   prevSIAVal = SIAVal; 
   
   //============================RESETOWANIE DYSTANSU I PRĘDKOŚCI==================
   static bool sReset;                        // flaga resetu dystansu
   static bool gotTime;                       // zezwolenie na pobór aktualnego czasu
   static const unsigned resetTime = 2000;    // wymagany czas przytrzymania przycisku
   static unsigned long holdTime;             // aktualny czas przytrzymania przycisku
   if(digitalRead(button)==0) {xVel=0; yVel=0; zVel=0; hVel=0; tVel=0; sReset=true;}

   // jednorazowy pobór aktualnego czasu
   if(sReset==true){
    if(gotTime==false){
      holdTime=millis();
      gotTime=true;
    }

    // resetowanie dystansu po przytrzymaniu przycisku przez określony czas
    if(millis() - holdTime >= resetTime){
        zDist=0;
        hDist=0;
        tDist=0;
        gotTime=false;
    }

    // anulowanie zliczania czasu
    if(digitalRead(button)==1){sReset=false; gotTime=false;}
  }
  
   //=====================OPÓŹNIENIE LCD===============================
  static const unsigned refreshTime = 200;  // okres aktualizowania LCD
  static unsigned long lastRefreshTime = 0; // czas od ostatniej aktualizacji LCD

 // aktualizowanie LCD po upłynięciu określonego czasu
 if(millis() - lastRefreshTime >= refreshTime)
   {
        lastRefreshTime += refreshTime;     // przedłużenie czasu o kolejny okres
              refreshDisplay();             // wywołanie funkcji aktualizacji LCD
  }
Serial.print(tDist);
Serial.print("  ");
Serial.print(hDist);
Serial.print("  ");
Serial.print(zDist);
Serial.print("  ");
Serial.print(tVel);
Serial.print("  ");
Serial.print(hVel);
Serial.print("  ");
Serial.print(zVel);
Serial.print("  ");
Serial.print(xg);
Serial.print("  ");
Serial.print(yg);
Serial.print("  ");
Serial.println(zg);

  delay(10);
 }
//============================LCD=================================
void refreshDisplay(){ // funkcja aktualizacji LCD
 switch(counter){ // sprawdzanie położenia enkodera
    case 0: 
   // wyświetlenie parametrów ruchu poziomego
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Horizontal");
   lcd.setCursor(0,1);
   lcd.print("s=");
   lcd.print(hDist,2); // 2 liczby po przecinku
   lcd.print(" ");
   lcd.print("v=");
   lcd.print(hVel,2);
   break;
   case 2:
   // wyświetlenie parametrów ruchu pionowego
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Vertical");
   lcd.setCursor(0,1);
   lcd.print("s=");
   lcd.print(zDist,2);
   lcd.print(" ");
   lcd.print("v=");
   lcd.print(zVel,2);
   break;
   case 4:
   // wyświetlenie parametrów całkowitego ruchu
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Total");
   lcd.setCursor(0,1);
   lcd.print("s=");
   lcd.print(tDist,2);
   lcd.print(" ");
   lcd.print("v=");
   lcd.print(tVel,2);
   break;
  } 
  }

 //========================CAŁKOWANIE===============================
 double integralR(double meas, double delta){
  double result = meas*delta;
  return result;
 }
 
//========================================WYZNACZANIE PUNKTU ZEROWEGO============================
int getZero(int pin){
  int result;
  // pobranie 100 próbek
    for (int i=0; i<100; i++){
    int meas = analogRead(pin);
    zeros.add(meas);          //dodanie próbki do zbioru
    delay(10);
  }
  result = zeros.getMedian(); //wyznaczenie mediany z próbek
  return result;
}
