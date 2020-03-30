#include<SoftwareSerial.h>

SoftwareSerial beagle(4, 5);
//int dioda1 = 8;
//int dioda2 = 12;
int diody[] = {8, 12};
//int but_start = 7;
//int przerwanie1 = 3;
//int przerwanie2 = 2;
int przerwania[] = {3, 2};
bool swiec_single = false;
bool swiec_double = false;
bool double_licznik = false;
bool wczyt = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  beagle.begin(9600);
  pinMode(diody[0], OUTPUT);
  pinMode(diody[1], OUTPUT);
  //  pinMode(but_start, INPUT);
  pinMode(przerwania[0], INPUT);
  pinMode(przerwania[1], INPUT);
  
  wczyt = true;
}

int czekaj, czas, czasp, czasp0, czasp1;
int dioda;
int proba, czasy[2];
int licznik[2];
int wygrana[2];
int odczyt;

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println(digitalRead(przerwanie1));
  //  Serial.println(digitalRead(przerwanie2));
  //  Serial.println(digitalRead(but_start));
  //  Serial.println(" ");
  //  delay(300);
  if(wczyt){
    //Serial.println("pozapetla");
    if(beagle.available()){
      //Serial.println("wpteli1");
      odczyt = beagle.read();
      Serial.println(odczyt);
      //Serial.println("wpteli2");
      switch(odczyt){
        case 49: start_single();
        break;
        case 50: start_double();
        break;
        case 51: start_double_licznik();
        break;
      }
      wczyt = false;
    }
  }
  else if (swiec_single) {
    proba++;
    dioda = random(2);
    czekaj = random(1000, 5000);
    delay(czekaj);

    swiec_single = false;
    //      Serial.println(digitalRead(przerwanie1));

    beagle.println("toggle"+String(dioda));
    zapal_diode(dioda);
    
    czas = millis();
    //      Serial.println("czesc");
    //      delay(20);
    attachInterrupt(digitalPinToInterrupt(przerwania[dioda]), interrup_single, RISING);


  }
  else if (swiec_double) {
    proba++;
//    Serial.println("przed oczekiwaniem");
    czekaj = random(1000, 5000);
    //delay(czekaj);
    delay(czekaj);
//    Serial.println(czekaj);
//    Serial.println("po oczekiwaniu");
    swiec_double = false;

    beagle.println("toggle");
    zapal_diode(0);
    zapal_diode(1);
    czas = millis();

//    noInterrupts();
        attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double0, RISING);
        attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double1, RISING);
//    interrupts();
  }
  else if(double_licznik){
    if(millis() - czas >= 5000){
//      noInterrupts();
      interrupt_double_licznik_end();
//      interrupts();
    }
  }


}

void start_single() {
  delay(100);
  czasy[0] = 0;
  proba = 0;
  swiec_single = true;
}

void start_double() {
//  noInterrupts();
  delay(100);
  wygrana[0] = 0;
  czasy[0] = 0;
  wygrana[1] = 0;
  czasy[1] = 0;
  proba = 0;
//  attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double0, RISING);
//  attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double1, RISING);
  swiec_double = true;
//  wczyt=true;
}

void end_double() {
  detachInterrupt(digitalPinToInterrupt(przerwania[0]));
  detachInterrupt(digitalPinToInterrupt(przerwania[1]));

  Serial.println(wygrana[0]);
  Serial.println(wygrana[1]);
  if (wygrana[0] > wygrana[1]) {
    
      beagle.println(String(float(czasy[0]) / wygrana[0])); //wysylanie sredniej wygranwgo
  }
  else {
    
      beagle.println(String(float(czasy[1]) / wygrana[1]));
  }

//    start_single();
  wczyt = true;
//  interrupts();
}

void start_double_licznik() {
  delay(1000);
  swiec_single = false;
  swiec_double = false;
  czasy[0] = 0;
  czasy[1] = 0;
  licznik[0] = 0;
  licznik[1] = 0;

//    noInterrupts();
  double_licznik = true;
  beagle.println("start");
  zapal_diode(0);
  zapal_diode(1);
  czas = millis();
//  Timer1.initialize(5000000);
//  Timer1.restart();
//  Timer1.attachInterrupt(interrupt_double_licznik_end);

//  TCCR0A = 0;// set entire TCCR1A register to 0
//  TCCR0B = 0;// same for TCCR1B
//  TCNT0  = 0;//initialize counter value to 0
//  // set compare match register for 1hz increments
//  OCR0A = 3124;// = (16*10^6) / (5*1024) - 1 (must be <65536)
//  // turn on CTC mode
//  TCCR0B |= (1 << WGM01);
//  // Set CS12 and CS10 bits for 1024 prescaler
//  TCCR0B |= (1 << CS01) | (1 << CS00);  
//  // enable timer compare interrupt
//  TIMSK0 |= (1 << OCIE0A);

  attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double_licznik0, RISING);
  attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double_licznik1, RISING);

  czasy[0] = czasy[1] = millis();
//    interrupts();
  
  
  
}

void zapal_diode(int i) {
  //if(beagle.available()){
//    beagle.println("toggle" + String((i+1)%2));
//    Serial.println("dd");
//    beagle.println(i); // wysylanie diody do zapalenia (D0 lub D1)
  //}
  digitalWrite(diody[i], HIGH);
}

void interrup_single() {
  detachInterrupt(digitalPinToInterrupt(przerwania[dioda]));
  //  noInterrupts();
  czasp = millis() - czas;
  if (czasp < 5) {
    attachInterrupt(digitalPinToInterrupt(przerwania[dioda]), interrup_single, RISING);
    //    interrupts();
    return;
  }

  czas = czasp;
  beagle.println(String(czas)); //wysylanie czasu reakcji
  digitalWrite(diody[0], LOW);
  digitalWrite(diody[1], LOW);
  Serial.println(czas);
  
  czasy[0] += czas;
  if (proba < 5) {
    swiec_single = true;
  }
  else {
    Serial.println(float(czasy[0]) / proba);
    beagle.println(String(float(czasy[0]) / proba)); //wysyaleni sredniej
//        start_double();
    wczyt = true;
  }
}

void interrupt_double0() {
    detachInterrupt(digitalPinToInterrupt(przerwania[0]));
    detachInterrupt(digitalPinToInterrupt(przerwania[1]));
//  noInterrupts();
  czasp0 = millis() - czas;
  if ( czasp0 < 5 ) {
        attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double0, RISING);
        attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double1, RISING);
//    interrupts();
    return;
  }

//  czas = czasp0;
  beagle.println(String(0) + String(czasp0)); //do zmiany
  digitalWrite(diody[0], LOW);
  digitalWrite(diody[1], LOW);
  Serial.print("int0 ");
  Serial.println(czasp0);
  
  //beagle.println(String(czas)); // wysyaleni czasu reakcji (1 linia nr gracza, druga czas)
  
  czasy[0] += czasp0;
  wygrana[0]++;
  if (proba < 5) {
    swiec_double = true;
  }
  else {
    end_double();
  }
}

void interrupt_double1() {
    detachInterrupt(digitalPinToInterrupt(przerwania[0]));
    detachInterrupt(digitalPinToInterrupt(przerwania[1]));
//  noInterrupts();
  czasp1 = millis() - czas;
  if ( czasp1 < 5) {
        attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double0, RISING);
        attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double1, RISING);
//    interrupts();
    return;
  }

//  czas = czasp1;
  beagle.println(String(1) + String(czasp1)); //do zaminay
  digitalWrite(diody[0], LOW);
  digitalWrite(diody[1], LOW);
  Serial.print("int1 ");
  Serial.println(czasp1);
  
  
  //beagle.println(String(czas)); // wysyaleni czasu reakcji (1 linia nr gracza, druga czas)
  
  czasy[1] += czasp1;
  wygrana[1]++;
  if (proba < 5) {
    swiec_double = true;
  }
  else {
    end_double();
  }
}

void interrupt_double_licznik0() {
  detachInterrupt(digitalPinToInterrupt(przerwania[0]));
  if (millis() - czasy[0] >= 100) {
    
      beagle.println("0"); //wcisnal 0 gracz
      Serial.println(0);
    licznik[0]++;
    czasy[0] = millis();
  }
  attachInterrupt(digitalPinToInterrupt(przerwania[0]), interrupt_double_licznik0, RISING);
  
} 
void interrupt_double_licznik1() {
  detachInterrupt(digitalPinToInterrupt(przerwania[1]));
  if (millis() - czasy[1] >= 100) {
    
      beagle.println("1"); //wcisnal 1 gracz
      Serial.println(1);
    licznik[1]++;
    czasy[1] = millis();
  }
  attachInterrupt(digitalPinToInterrupt(przerwania[1]), interrupt_double_licznik1, RISING);
}

void interrupt_double_licznik_end() {
  //  noInterrupts();
//  Timer1.stop();
  detachInterrupt(digitalPinToInterrupt(przerwania[0]));
  detachInterrupt(digitalPinToInterrupt(przerwania[1]));
//  Timer1.detachInterrupt();

  beagle.println("END"); //wysylany sygnal END
  digitalWrite(diody[0], LOW);
  digitalWrite(diody[1], LOW);

  double_licznik = false;
  Serial.println(licznik[0]);
  Serial.println(licznik[1]);
  
  //  interrupts();
  wczyt = true;
}
//
//ISR(TIMER1_COMPA_vect){
//  detachInterrupt(digitalPinToInterrupt(przerwania[0]));
//  detachInterrupt(digitalPinToInterrupt(przerwania[1]));
//  Timer1.detachInterrupt();
//
//  digitalWrite(diody[0], LOW);
//  digitalWrite(diody[1], LOW);
//
//  Serial.println(licznik[0]);
//  Serial.println(licznik[1]);
//  beagle.println(-1); //wysylany sygnal END
//  //  interrupts();
//}
