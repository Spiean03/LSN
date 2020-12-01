//LSN1 Steuerprogramm
// Update December 2019: Serial Readout over TX(0) and RX(1) ports. In order to send/receive data over RX/TX pins, you will have to change all occurencies of "Serial" to "Serial1" 



#include <Arduino.h>

unsigned long  Leistung;
float Strom, Spannung, I, U;
unsigned long time, starttime, stoptime;
const int START = 2;              //Interrupt 1
const int STOP = 3;               //Interrupt 0
const int Switch_100_25 = 4;      //Schalter 100%-25%, "0"=100%
const int Switch_cont_1h = 5;     //Schalter 1h-cont,  "0"=cont.
const int Rel_100_25 = 6;         //Relais, "0"=25%
const int LED_D200 = 7;           //"1"=D200, LED und Stromred.
const int NEX_angeschl = 8;       //"0"=Pumpe angeschlossen
const int LED_Strom = 11;         //"0"=Strom EIN
const int Vent = 12;              //"1"=Lüfter EIN
const int Switch_D100_D200 = 13;  //Schalter D100-D200, "0"=D100
const int LED_remote = A5;        //"0"=remote LED
const int I_PIN = A0;
const int U_PIN = A1;

void setup() {
  analogReference(EXTERNAL);
  pinMode(START, INPUT_PULLUP); digitalWrite(START, HIGH);
  pinMode(STOP, INPUT_PULLUP); digitalWrite(STOP, HIGH);
  pinMode(Switch_100_25, INPUT_PULLUP); digitalWrite(Switch_100_25, HIGH);
  pinMode(Switch_cont_1h, INPUT_PULLUP); digitalWrite(Switch_cont_1h, HIGH);
  pinMode(Switch_D100_D200, INPUT_PULLUP); digitalWrite(Switch_D100_D200, HIGH);
  pinMode(NEX_angeschl, INPUT_PULLUP); digitalWrite(NEX_angeschl, HIGH);
  pinMode(LED_remote, OUTPUT); digitalWrite(LED_remote, HIGH);
  pinMode(Rel_100_25, OUTPUT);
  pinMode(LED_D200, OUTPUT);
  pinMode(LED_Strom, OUTPUT); digitalWrite(LED_Strom, HIGH);
  attachInterrupt(1, starten, FALLING);
  attachInterrupt(0, stoppen, FALLING);
  Serial1.begin(9600);
}
void loop() {
  Pumpentyp();
  Power();
  timer();
  Imessen();
  Umessen();
  Prechnen();
  Monitor();
  delay (1000);
}

void Pumpentyp() {
  int val;
  val = digitalRead(Switch_D100_D200);
  if (val == LOW) {
    digitalWrite(LED_D200, LOW);
  }
  else
  {
    digitalWrite(LED_D200, HIGH);
  }
}

void Power() {
  int val;
  val = digitalRead(Switch_100_25);
  if (val == HIGH) {
    digitalWrite(Rel_100_25, LOW);
  }
  else
  {
    digitalWrite(Rel_100_25, HIGH);
  }
}

void starten() {
  int val;
  val = digitalRead(NEX_angeschl);
  if (val == LOW) {
    time = millis();
    stoptime = time + 3600000;   //1 Stunde
    digitalWrite(LED_Strom, LOW);
    digitalWrite(Vent, HIGH);
  }
}

void stoppen() {
  digitalWrite(LED_Strom, HIGH);
  digitalWrite(Vent, LOW);
}

void timer() {
  int val;
  time = millis();
  val = digitalRead(Switch_cont_1h);
  if (val == LOW) {
    stoptime = time + 3600000;
  }
  if (time > stoptime) {
    digitalWrite(LED_Strom, HIGH);
    digitalWrite(Vent, LOW);
  }
}

void Imessen() {
  unsigned int Wert10bit;
  Wert10bit = ADC10bit(I_PIN);
  Strom = Wert10bit * 9.65;  //mA !
  I = Strom / 1000;           //A !
}

void Umessen() {
  unsigned int Wert10bit;
  Wert10bit = ADC10bit(U_PIN);
  Spannung = Wert10bit * 15;
  Spannung = Spannung - (Strom / 10); //mV !
  U = Spannung / 1000;                //V !
}

void Prechnen() {
  Leistung = Spannung * Strom / 1000000 ; //Watt
}

int ADC10bit (int kanal) {
  unsigned int Wert10bit, ADCwert, i = 0, summe = 0;
  while (i < 10) {
    ADCwert = analogRead(kanal);
    summe = summe + ADCwert;
    delay(1);
    i++;
  }
  return (summe / 10);
}

void Monitor() {
  Serial1.print(I, 1);
  Serial1.print("A ");
  Serial1.print(U, 1);
  Serial1.print("V ");
  Serial1.print(Leistung);
  Serial1.print("W");
  Serial1.println();
}
