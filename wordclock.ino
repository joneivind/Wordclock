#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)


int ledpin = 3;
int lysstyrke = 0;
int lyssensor = A6;


// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      160

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, ledpin, NEO_GRB + NEO_KHZ800);


//#include <CapacitiveSensor.h>

//CapacitiveSensor   cs_4_6 = CapacitiveSensor(4,6);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
//CapacitiveSensor   cs_4_7 = CapacitiveSensor(4,7);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
//CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil


int meny = 0;
int timer = 0;
int t = 0;
float celsius1 = 0;
int celsius = 0;
int temperature_error = -3; // temperature calibration in degrees celsius

boolean sleep = 0; //Sleep mode on off
int sleep_sensitivity = 30; //0 - 1023 - Setter nedre verdi

int max_brightness = 80; //Max brightness
int min_brightness = 15; //Min brightness

//Fargemiks på klokken
int rod = 0;
int gronn = 1;
int blaa = 1;

int meny_0_delay = 30000; //Klokke
int meny_1_delay = 600; //Temp
int meny_2_delay = 1000; //Binærklokke






void setup() {
  
  Serial.begin(115200);

  //Synker klokkemodul eksternt
  setSyncProvider(RTC.get);
  if(timeStatus() == timeSet){
    Serial.println("RTC Sync!");
  }
  else{
    Serial.println("External sync failed!");
  }

  //Kapasative knapper
  //cs_4_6.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //cs_4_7.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //cs_4_8.set_CS_AutocaL_Millis(0xFFFFFFFF);

  //Starter led lys
  pixels.begin(); // This initializes the NeoPixel library.

  //Lager temp-variabel
  t = RTC.temperature();

  Serial.println("Still klokken med format hh,mm,ss");
    
}






void loop() {

  lysstyrke = map(analogRead(lyssensor), 0, 1024, min_brightness, max_brightness); //Måler lysstyrke og setter til brightness mellom max og min
  

  tmElements_t tm;
  time_t time_var;
  
  //Henter klokke fra seriell input, format (hh,mm,ss) 12 timers format
  if(Serial.available()){
    tm.Year = 16;
    tm.Month = 1;
    tm.Day = 1;
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();

    time_var = makeTime(tm);
    RTC.set(time_var);
    RTC.write(tm);
    setTime(time_var);

    RTC.read(tm);
    Serial.print("Klokken er satt til ");
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.println(second());

    theaterChase(pixels.Color(1*lysstyrke, 1*lysstyrke, 1*lysstyrke), 30);
    
    for(int i=0;i<160;i++){ //Slukker alle leds
      pixels.setPixelColor(i, 0,0,0);
    }
    pixels.show();
    
  }

  /*
  int knapp1 =  cs_4_7.capacitiveSensor(30);
  int knapp2 =  cs_4_7.capacitiveSensor(30);
  int knapp3 =  cs_4_7.capacitiveSensor(30);
  
  if(knapp1 > 400 && meny == 0){
      meny = 1;
      Serial.println("Meny 1");
      for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
      delay(300);
  }
  else if(knapp1 > 400 && meny == 1){
      meny = 2;
      Serial.println("Meny 2");
      for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
      delay(300);
  }
  else if(knapp1 > 400 && meny == 2){
      meny = 0;
      Serial.println("Meny 3");
      for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
      delay(300);
  }
  
  //Serial.println(knapp1);
  */
    
  //digitalClockDisplay(); //Viser klokke i seriedisplay for debug
  
  
  
  if(meny != 1){ //Måler ikke temperatur når temperatur vises
    t = RTC.temperature();
    }
  celsius1 = t / 4.0;
  celsius = celsius1 + temperature_error;
    
    
  
  minutter(); //Minuttvisere rundt klokken (Prikker) 
  
  
  
    //Moduser
    
    if(meny == 0){
      klokke();
    }
    
    if(meny == 1){
      temperatur();
    }
    
    if(meny == 2){
      binaerklokke(1,1,1,lysstyrke); //Setter farge på tallene i binærmodus
    }
    
    
    //Debug info
    //Serial.println(analogRead(lyssensor));
    //Serial.println(lysstyrke);
    //Serial.println(timer);
    //Serial.println(meny);
    //Serial.println(celsius);
    
     
      timer++; //Teller opp til neste meny
      
      
      //Tid meny 0
      if(timer == meny_0_delay && meny == 0){
        meny = 1;
        timer = 0;
        //theaterChase(pixels.Color(10, 10, 10), 50); // White
        for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
        }
      
      //Tid meny 1
      if(timer == meny_1_delay && meny == 1){
        meny = 0;
        timer = 0;
        //theaterChase(pixels.Color(10, 10, 10), 50); // White
        for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
        }
        
        /*
      //Tid meny 2
      if(timer == meny_2_delay && meny == 2){
        meny = 0;
        timer = 0;
        //theaterChase(pixels.Color(10, 10, 10), 50); // White
        for(int i=0;i<156;i++){ //Slukker alle leds
          pixels.setPixelColor(i, 0,0,0);
         }
        }
        */
        
        
        //Sovemodus på
        if(analogRead(lyssensor) < sleep_sensitivity){
           sleep = 1;
           for(int i=0;i<160;i++){ //Slukker alle leds
            pixels.setPixelColor(i, 0,0,0);
           }
           for(int i=78;i<83;i++){
              pixels.setPixelColor(i, pixels.Color(10,10,10));
            }
            pixels.show();
            while(analogRead(lyssensor) < (sleep_sensitivity + 100));
        }
        
        //Sovemodus av
        if(sleep == 1 && analogRead(lyssensor) >= sleep_sensitivity){
          sleep = 0;
          for(int i=0;i<160;i++){ //Slukker alle leds
            pixels.setPixelColor(i, 0,0,0);
            }
        }
        
     
     pixels.show(); //Sender pixler til skjermen
    
    delay(10);
      
}








/*
###################################
########### Overganger ############
###################################
*/


void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / 155) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < 155; i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();
     
      delay(wait);
     
      for (int i=0; i < 155; i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < 156; i=i+3) {
          pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        pixels.show();
       
        delay(wait);
       
        for (int i=0; i < 156; i=i+3) {
          pixels.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}









/*
###################################
############ Temperatur ###########
###################################
*/



void temperatur(){

  
  temperaturen(rod,gronn,blaa,lysstyrke);
  er(rod,gronn,blaa,lysstyrke);
  grader(rod,gronn,blaa,lysstyrke);
    
  
  if(celsius >= 10 && celsius < 20)
    tiende_grad(1,1,1,lysstyrke);
    
  else if(celsius >= 20 && celsius < 30)
    tjuende_grad(1,1,1,lysstyrke);
    
  else if(celsius >= 30 && celsius < 40)
    trettiende_grad(1,1,1,lysstyrke);
       
    
  if(celsius == 0 || celsius == 10 || celsius == 20 || celsius == 30)
    null_grad(1,1,1,lysstyrke);
    
  else if(celsius == 1 || celsius == 11 || celsius == 21 || celsius == 31)
    en_grad(1,1,1,lysstyrke);
    
  else if(celsius == 2 || celsius == 12 || celsius == 22 || celsius == 32)
    to_grad(1,1,1,lysstyrke);
    
  else if(celsius == 3 || celsius == 13 || celsius == 23 || celsius == 33)
    tre_grad(1,1,1,lysstyrke);
    
  else if(celsius == 4 || celsius == 14 || celsius == 24 || celsius == 34)
    fire_grad(1,1,1,lysstyrke);
    
  else if(celsius == 5 || celsius == 15 || celsius == 25 || celsius == 35)
    fem_grad(1,1,1,lysstyrke);
    
  else if(celsius == 6 || celsius == 16 || celsius == 26 || celsius == 36)
    seks_grad(1,1,1,lysstyrke);
    
  else if(celsius == 7 || celsius == 17 || celsius == 27 || celsius == 37)
    syv_grad(1,1,1,lysstyrke);
    
  else if(celsius == 8 || celsius == 18 || celsius == 28 || celsius == 38)
    aatte_grad(1,1,1,lysstyrke);
    
  else if (celsius == 9 || celsius == 19 || celsius == 29 || celsius == 39)
    ni_grad(1,1,1,lysstyrke);
    
}











/*
###################################
######### Vanlig klokke ###########
###################################
*/


void klokke(){  
  
  //klokken er
    
  klokken(rod,gronn,blaa,lysstyrke);
  er(1,1,1,lysstyrke);
  
  //minutter
  
  if((minute()>=5 && minute()<10) || (minute()>=25 && minute()<30) || (minute()>=35 && minute()<40) || (minute()>=55 && minute()<=59))
    fem_1(1,1,1,lysstyrke);
  else 
    fem_1(0,0,0,lysstyrke);
  
  if((minute()>=10 && minute()<15) || (minute()>=20 && minute()<25) || (minute()>=40 && minute()<45) || (minute()>=50 && minute()<55))
    ti_1(1,1,1,lysstyrke);
  else 
    ti_1(0,0,0,lysstyrke);
  
  if((minute()>=15 && minute()<20) || (minute()>=45 && minute()<50))
    kvart(1,1,1,lysstyrke);
  else 
    kvart(0,0,0,lysstyrke);
  
  if((minute()>=5 && minute()<20) || (minute()>=35 && minute()<45))
    over(1,1,1,lysstyrke);
  else 
    over(0,0,0,lysstyrke);
    
  if((minute()>=25 && minute()<30) || (minute()>=55 && minute()<=59)) //fem på
    paa_1(1,1,1,lysstyrke);
  else
    paa_1(0,0,0,lysstyrke);
  
  if((minute()>=20 && minute()<25) || (minute()>=50 && minute()<55)) //ti på
    paa_2(1,1,1,lysstyrke);
  else
    paa_2(0,0,0,lysstyrke);
    
  if(minute()>=45 && minute()<50) //kvart på
    paa_3(1,1,1,lysstyrke);
  else
    paa_3(0,0,0,lysstyrke);
    
  if(minute()>=20 && minute()<45)
    halv(1,1,1,lysstyrke);
  else
    halv(0,0,0,lysstyrke);
  
  
  
  //timevisere
    
  if((hour() == 23 && minute()>=20) || (hour() == 0 && minute()<20) || (hour() == 11 && minute()>=20) || (hour() == 12 && minute()<20))
    tolv(1,1,1,lysstyrke);
  else
    tolv(0,0,0,lysstyrke);
    
  if((hour() == 0 && minute()>=20) || (hour() == 1 && minute()<20) || (hour() == 12 && minute()>=20) || (hour() == 13 && minute()<20))
    ett(1,1,1,lysstyrke);
  else
    ett(0,0,0,lysstyrke);
  
  if((hour() == 1 && minute()>=20) || (hour() == 2 && minute()<20) || (hour() == 13 && minute()>=20) || (hour() == 14 && minute()<20))
    to(1,1,1,lysstyrke);
  else
    to(0,0,0,lysstyrke);
  
  if((hour() == 2 && minute()>=20) || (hour() == 3 && minute()<20) || (hour() == 14 && minute()>=20) || (hour() == 15 && minute()<20))
    tre(1,1,1,lysstyrke);
  else
    tre(0,0,0,lysstyrke);
    
  if((hour() == 3 && minute()>=20) || (hour() == 4 && minute()<20) || (hour() == 15 && minute()>=20) || (hour() == 16 && minute()<20))
    fire(1,1,1,lysstyrke);
  else
    fire(0,0,0,lysstyrke);
    
  if((hour() == 4 && minute()>=20) || (hour() == 5 && minute()<20) || (hour() == 16 && minute()>=20) || (hour() == 17 && minute()<20))
    fem(1,1,1,lysstyrke);
  else
    fem(0,0,0,lysstyrke);
    
  if((hour() == 5 && minute()>=20) || (hour() == 6 && minute()<20) || (hour() == 17 && minute()>=20) || (hour() == 18 && minute()<20))
    seks(1,1,1,lysstyrke);
  else
    seks(0,0,0,lysstyrke);
    
  if((hour() == 6 && minute()>=20) || (hour() == 7 && minute()<20) || (hour() == 18 && minute()>=20) || (hour() == 19 && minute()<20))
    syv(1,1,1,lysstyrke);
  else
    syv(0,0,0,lysstyrke);
    
  if((hour() == 7 && minute()>=20) || (hour() == 8 && minute()<20) || (hour() == 19 && minute()>=20) || (hour() == 20 && minute()<20))
    aatte(1,1,1,lysstyrke);
  else
    aatte(0,0,0,lysstyrke);
    
  if((hour() == 8 && minute()>=20) || (hour() == 9 && minute()<20) || (hour() == 20 && minute()>=20) || (hour() == 21 && minute()<20))
    ni(1,1,1,lysstyrke);
  else
    ni(0,0,0,lysstyrke);
    
  if((hour() == 9 && minute()>=20) || (hour() == 10 && minute()<20) || (hour() == 21 && minute()>=20) || (hour() == 22 && minute()<20))
    ti(1,1,1,lysstyrke);
  else
    ti(0,0,0,lysstyrke);
    
  if((hour() == 10 && minute()>=20) || (hour() == 11 && minute()<20) || (hour() == 22 && minute()>=20) || (hour() == 23 && minute()<20))
    elleve(1,1,1,lysstyrke);
  else
    elleve(0,0,0,lysstyrke);
    
}








/*
###################################
########### Binærklokke ###########
###################################
*/


void binaerklokke(int r, int g, int b, int brightness){
  
  klokken(rod,gronn,blaa,lysstyrke);
  er(1,1,1,lysstyrke);
  //skille_binaer(1,1,1,100);
  bg_binaer(1,1,1,lysstyrke*0.25);
  
  
  //Første sekundrad
  
  if(second() == 1 || second() == 11 || second() == 21 || second() == 31 || second() == 41 || second() == 51)
    en_sek1_binaer(r,g,b,brightness);
    
  if(second() == 2 || second() == 12 || second() == 22 || second() == 32 || second() == 42 || second() == 52)
    to_sek1_binaer(r,g,b,brightness);
    
  if(second() == 3 || second() == 13 || second() == 23 || second() == 33 || second() == 43 || second() == 53){
    en_sek1_binaer(r,g,b,brightness);
    to_sek1_binaer(r,g,b,brightness);
    }
    
  if(second() == 4 || second() == 14 || second() == 24 || second() == 34 || second() == 44 || second() == 54)
    fire_sek1_binaer(r,g,b,brightness);
    
  if(second() == 5 || second() == 15 || second() == 25 || second() == 35 || second() == 45 || second() == 55){
    fire_sek1_binaer(r,g,b,brightness);
    en_sek1_binaer(r,g,b,brightness);
    }
  
  if(second() == 6 || second() == 16 || second() == 26 || second() == 36 || second() == 46 || second() == 56){
    fire_sek1_binaer(r,g,b,brightness);
    to_sek1_binaer(r,g,b,brightness);
    }
    
  if(second() == 7 || second() == 17 || second() == 27 || second() == 37 || second() == 47 || second() == 57){
    fire_sek1_binaer(r,g,b,brightness);
    to_sek1_binaer(r,g,b,brightness);
    en_sek1_binaer(r,g,b,brightness);
    }
    
  if(second() == 8 || second() == 18 || second() == 28 || second() == 38 || second() == 48 || second() == 58)
    aatte_sek1_binaer(r,g,b,brightness);
    
  if(second() == 9 || second() == 19 || second() == 29 || second() == 39 || second() == 49 || second() == 59){
    aatte_sek1_binaer(r,g,b,brightness);
    en_sek1_binaer(r,g,b,brightness);
    }
    
  
  
  //Andre sekundrad
  
  if((second() >= 10) && (second() < 20)){ //Ti eller mer
    en_sek2_binaer(r,g,b,brightness);
  }
  
  if((second() >= 20) && (second() < 30)){ //Tjue eller mer
    to_sek2_binaer(r,g,b,brightness);
  }
  
  if((second() >= 30) && (second() < 40)){ //Tretti eller mer
    en_sek2_binaer(r,g,b,brightness);
    to_sek2_binaer(r,g,b,brightness);
  }
  
  if((second() >= 40) && (second() < 50)){ //Førti eller mer
    fire_sek2_binaer(r,g,b,brightness);
  }
  
  if((second() >= 50) && (second() <= 59)){ //Femti eller mer
    en_sek2_binaer(r,g,b,brightness);
    fire_sek2_binaer(r,g,b,brightness);
  }

  
  //Første minuttrad
  
  if(minute() == 1 || minute() == 11 || minute() == 21 || minute() == 31 || minute() == 41 || minute() == 51)
    en_min1_binaer(r,g,b,brightness);
    
  if(minute() == 2 || minute() == 12 || minute() == 22 || minute() == 32 || minute() == 42 || minute() == 52)
    to_min1_binaer(r,g,b,brightness);
    
  if(minute() == 3 || minute() == 13 || minute() == 23 || minute() == 33 || minute() == 43 || minute() == 53){
    en_min1_binaer(r,g,b,brightness);
    to_min1_binaer(r,g,b,brightness);
    }

  if(minute() == 4 || minute() == 14 || minute() == 24 || minute() == 34 || minute() == 44 || minute() == 54)
    fire_min1_binaer(r,g,b,brightness);
  
  if(minute() == 5 || minute() == 15 || minute() == 25 || minute() == 35 || minute() == 45 || minute() == 55){
    en_min1_binaer(r,g,b,brightness);
    fire_min1_binaer(r,g,b,brightness);
    }
    
  if(minute() == 6 || minute() == 16 || minute() == 26 || minute() == 36 || minute() == 46 || minute() == 56){
    fire_min1_binaer(r,g,b,brightness);
    to_min1_binaer(r,g,b,brightness);
    }
    
  if(minute() == 7 || minute() == 17 || minute() == 27 || minute() == 37 || minute() == 47 || minute() == 57){
    fire_min1_binaer(r,g,b,brightness);
    to_min1_binaer(r,g,b,brightness);
    en_min1_binaer(r,g,b,brightness);
    }
    
  if(minute() == 8 || minute() == 18 || minute() == 28 || minute() == 38 || minute() == 48 || minute() == 58)
    aatte_min1_binaer(r,g,b,brightness);
    
  if(minute() == 9 || minute() == 19 || minute() == 29 || minute() == 39 || minute() == 49 || minute() == 59){
    aatte_min1_binaer(r,g,b,brightness);
    en_min1_binaer(r,g,b,brightness);
    }
    
    
  //Andre minuttrad
  
  if((minute() >= 10) && (minute() < 20)){ //Ti eller mer
    en_min2_binaer(r,g,b,brightness);
  }
  
  if((minute() >= 20) && (minute() < 30)){ //Tjue eller mer
    to_min2_binaer(r,g,b,brightness);
  }
  
  if((minute() >= 30) && (minute() < 40)){ //Tretti eller mer
    en_min2_binaer(r,g,b,brightness);
    to_min2_binaer(r,g,b,brightness);
  }
  
  if((minute() >= 40) && (minute() < 50)){ //Førti eller mer
    fire_min2_binaer(r,g,b,brightness);
  }
  
  if((minute() >= 50) && (minute() <= 59)){ //Femti eller mer
    en_min2_binaer(r,g,b,brightness);
    fire_min2_binaer(r,g,b,brightness);
  }
    
    
  //Første timerad
  
  if(hour() == 1 || hour() == 11 || hour() == 21)
    en_time1_binaer(r,g,b,brightness);
    
  if(hour() == 2 || hour() == 12 || hour() == 22)
    to_time1_binaer(r,g,b,brightness);
    
  if(hour() == 3 || hour() == 13 || hour() == 23){
    en_time1_binaer(r,g,b,brightness);
    to_time1_binaer(r,g,b,brightness);
    }
    
  if(hour() == 4 || hour() == 14)
    fire_time1_binaer(r,g,b,brightness);
    
  if(hour() == 5 || hour() == 15){
    en_time1_binaer(r,g,b,brightness);
    fire_time1_binaer(r,g,b,brightness);
    }
    
  if(hour() == 6 || hour() == 16){
    to_time1_binaer(r,g,b,brightness);
    fire_time1_binaer(r,g,b,brightness);
    }
    
  if(hour() == 7 || hour() == 17){
    en_time1_binaer(r,g,b,brightness);
    to_time1_binaer(r,g,b,brightness);
    fire_time1_binaer(r,g,b,brightness);
    }
    
  if(hour() == 8 || hour() == 18)
    aatte_time1_binaer(r,g,b,brightness);
    
  if(hour() == 9 || hour() == 19){
    en_time1_binaer(r,g,b,brightness);
    aatte_time1_binaer(r,g,b,brightness);
    }
    
    
    //Andre timerad
    
    if(hour() > 9 && hour() < 20)
      en_time2_binaer(r,g,b,brightness);
      
    if(hour() >= 20 && hour() <= 23)
      to_time2_binaer(r,g,b,brightness);
    
}

  
//Minuttvisere  
  
void minutter(){
  
  if((minute() > 0 && minute() < 5) || (minute() > 5 && minute() < 10) || (minute() > 10 && minute() < 15) || (minute() > 15 && minute() < 20) || (minute() > 20 && minute() < 25) || (minute() > 25 && minute() < 30) || (minute() > 30 && minute() < 35) || (minute() > 35 && minute() < 40) || (minute() > 40 && minute() < 45) || (minute() > 45 && minute() < 50) || (minute() > 50 && minute() < 55) || (minute() > 55 && minute() <= 59))
    min_en(1,1,1,lysstyrke);
  else
    min_en(0,0,0,lysstyrke);
    
  if((minute() > 1 && minute() < 5) || (minute() > 6 && minute() < 10) || (minute() > 11 && minute() < 15) || (minute() > 16 && minute() < 20) || (minute() > 21 && minute() < 25) || (minute() > 26 && minute() < 30) || (minute() > 31 && minute() < 35) || (minute() > 36 && minute() < 40) || (minute() > 41 && minute() < 45) || (minute() > 46 && minute() < 50) || (minute() > 51 && minute() < 55) || (minute() > 56 && minute() <= 59))
    min_to(1,1,1,lysstyrke);
  else
    min_to(0,0,0,lysstyrke);
    
  if((minute() > 2 && minute() < 5) || (minute() > 7 && minute() < 10) || (minute() > 12 && minute() < 15) || (minute() > 17 && minute() < 20) || (minute() > 22 && minute() < 25) || (minute() > 27 && minute() < 30) || (minute() > 32 && minute() < 35) || (minute() > 37 && minute() < 40) || (minute() > 42 && minute() < 45) || (minute() > 47 && minute() < 50) || (minute() > 52 && minute() < 55) || (minute() > 57 && minute() <= 59))
    min_tre(1,1,1,lysstyrke);
  else
    min_tre(0,0,0,lysstyrke);
    
  if((minute() > 3 && minute() < 5) || (minute() > 8 && minute() < 10) || (minute() > 13 && minute() < 15) || (minute() > 18 && minute() < 20) || (minute() > 23 && minute() < 25) || (minute() > 28 && minute() < 30) || (minute() > 33 && minute() < 35) || (minute() > 38 && minute() < 40) || (minute() > 43 && minute() < 45) || (minute() > 48 && minute() < 50) || (minute() > 53 && minute() < 55) || (minute() > 58 && minute() <= 59))
    min_fire(1,1,1,lysstyrke);
  else
    min_fire(0,0,0,lysstyrke);
    
}





















//############################################
//############### Enkelt-ord #################
//############################################





void temperaturen(int r, int g, int b, int brightness){
  
  for(int i=144;i<156;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void klokken(int r, int g, int b, int brightness){
  
  for(int i=131;i<138;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}
 
void er(int r, int g, int b, int brightness){
  
  for(int i=139;i<141;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void fem_1(int r, int g, int b, int brightness){
  
  for(int i=126;i<129;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void paa_1(int r, int g, int b, int brightness){
  
  for(int i=123;i<125;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void ti_1(int r, int g, int b, int brightness){
  
  for(int i=105;i<107;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void paa_2(int r, int g, int b, int brightness){
  
  for(int i=108;i<110;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void kvart(int r, int g, int b, int brightness){
  
  for(int i=98;i<103;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void paa_3(int r, int g, int b, int brightness){
  
  for(int i=95;i<97;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void over(int r, int g, int b, int brightness){
  
  for(int i=79;i<83;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void halv(int r, int g, int b, int brightness){
  
  for(int i=73;i<77;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void ett(int r, int g, int b, int brightness){
  
  for(int i=53;i<56;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void to(int r, int g, int b, int brightness){
  
  for(int i=57;i<59;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void tre(int r, int g, int b, int brightness){
  
  for(int i=60;i<63;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void fire(int r, int g, int b, int brightness){
  
  for(int i=47;i<51;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void fem(int r, int g, int b, int brightness){
  
  for(int i=44;i<47;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void seks(int r, int g, int b, int brightness){
  
  for(int i=40;i<44;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void syv(int r, int g, int b, int brightness){
  
  for(int i=27;i<30;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void aatte(int r, int g, int b, int brightness){
  
  for(int i=30;i<34;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void ni(int r, int g, int b, int brightness){
  
  for(int i=34;i<36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void ti(int r, int g, int b, int brightness){
  
  for(int i=36;i<38;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void elleve(int r, int g, int b, int brightness){
  
  for(int i=19;i<25;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void tolv(int r, int g, int b, int brightness){
  
  for(int i=14;i<18;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }

}

void grad(int r, int g, int b, int brightness){
  
  for(int i=1;i<5;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  
}

void grader(int r, int g, int b, int brightness){
  
  for(int i=1;i<7;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  
}





void tiende_grad(int r, int g, int b, int brightness){
  
  for(int i=28;i<31;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(48, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(55, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(74, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(100, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(101, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(107, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void tjuende_grad(int r, int g, int b, int brightness){
  
  for(int i=27;i<=31;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(49, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(55, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(73, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(83, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(98, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(108, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(107, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(106, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(102, pixels.Color(r*brightness,g*brightness,b*brightness));

}


void trettiende_grad(int r, int g, int b, int brightness){
  
  for(int i=105;i<=109;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=28;i<=30;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(50, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(46, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(57, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(73, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(99, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void en_grad(int r, int g, int b, int brightness){
  
  for(int i=34;i<37;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(42, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(61, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(68, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(87, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(94, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(95, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void to_grad(int r, int g, int b, int brightness){
  
  for(int i=33;i<38;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(43, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(61, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(67, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(112, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(114, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void tre_grad(int r, int g, int b, int brightness){
  
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(67, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(87, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(93, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(111, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(112, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(114, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(115, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void fire_grad(int r, int g, int b, int brightness){
  
  for(int i=59;i<=63;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(36, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(41, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(70, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(67, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(86, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(88, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(94, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(93, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(114, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void fem_grad(int r, int g, int b, int brightness){
  
  for(int i=111;i<=115;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=85;i<=88;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(66, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void seks_grad(int r, int g, int b, int brightness){
  
  for(int i=67;i<=70;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(95, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(114, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void syv_grad(int r, int g, int b, int brightness){
  
  for(int i=111;i<=115;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(88, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(68, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(60, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(43, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(34, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void aatte_grad(int r, int g, int b, int brightness){
  
  for(int i=112;i<=114;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=67;i<=69;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void ni_grad(int r, int g, int b, int brightness){
  
  for(int i=112;i<=114;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=67;i<=69;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(66, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}


void null_grad(int r, int g, int b, int brightness){
  
  for(int i=112;i<=114;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
  for(int i=34;i<=36;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
    pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(70, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(66, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
    pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
    
}

void min_en(int r, int g, int b, int brightness){
   
    pixels.setPixelColor(156, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void min_to(int r, int g, int b, int brightness){
   
    pixels.setPixelColor(157, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void min_tre(int r, int g, int b, int brightness){
   
    pixels.setPixelColor(158, pixels.Color(r*brightness,g*brightness,b*brightness));

}

void min_fire(int r, int g, int b, int brightness){
   
    pixels.setPixelColor(159, pixels.Color(r*brightness,g*brightness,b*brightness));

}

/*
void digitalClockDisplay(void)
{
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year()); 
    Serial.println(); 
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
*/


void bg_binaer(int r, int g, int b, int brightness){

  pixels.setPixelColor(27, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(29, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(31, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(33, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(35, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(37, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(53, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(55, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(57, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(61, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(79, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(83, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(87, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(105, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(107, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(109, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(111, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(115, pixels.Color(r*brightness,g*brightness,b*brightness));
  
  pixels.setPixelColor(50, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(48, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(76, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(74, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(102, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(100, pixels.Color(r*brightness,g*brightness,b*brightness));
  
  pixels.setPixelColor(46, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(72, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(70, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(98, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
  
  pixels.setPixelColor(42, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(68, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(66, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(94, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(92, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void skille_binaer(int r, int g, int b, int brightness){
  
  pixels.setPixelColor(56, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(60, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(82, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(86, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_sek1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(37, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_sek1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(63, pixels.Color(r*brightness,g*brightness,b*brightness));
  
 }

void fire_sek1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(89, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_sek1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(115, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_sek2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(35, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_sek2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(61, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void fire_sek2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(87, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_sek2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(113, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_min1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(33, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_min1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void fire_min1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_min1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(111, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_min2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(31, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_min2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(57, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void fire_min2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(83, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_min2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(109, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_time1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(29, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_time1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(55, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void fire_time1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_time1_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(107, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}



void en_time2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(27, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void to_time2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(53, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void fire_time2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(79, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}

void aatte_time2_binaer(int r, int g, int b, int brightness){
  pixels.setPixelColor(105, pixels.Color(r*brightness,g*brightness,b*brightness));
  
}


void smileyface(int r, int g, int b, int brightness){
  pixels.setPixelColor(55, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(61, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(83, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(98, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(96, pixels.Color(r*brightness,g*brightness,b*brightness));
  for(int i=43;i<=47;i++){
    pixels.setPixelColor(i, pixels.Color(r*brightness,g*brightness,b*brightness));
  }
}


void hei_splash(int r, int g, int b, int brightness){
  pixels.setPixelColor(131, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(106, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(105, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(80, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(79, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(53, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(52, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(81, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(128, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(109, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(102, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(83, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(76, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(57, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(49, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(126, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(125, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(124, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(123, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(111, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(100, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(85, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(86, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(74, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(59, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(47, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(46, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(45, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(44, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(120, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(95, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(94, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(91, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(68, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(65, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(42, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(41, pixels.Color(r*brightness,g*brightness,b*brightness));
  pixels.setPixelColor(40, pixels.Color(r*brightness,g*brightness,b*brightness));
}
