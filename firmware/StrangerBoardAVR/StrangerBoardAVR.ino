/*******************************************************************************
 *    ___ _                                ___                   _ 
 *   / __| |_ _ _ __ _ _ _  __ _ ___ _ _  | _ ) ___  __ _ _ _ __| |
 *   \__ \  _| '_/ _` | ' \/ _` / -_) '_| | _ \/ _ \/ _` | '_/ _` |
 *   |___/\__|_| \__,_|_||_\__, \___|_|   |___/\___/\__,_|_| \__,_|
 *                         |___/                                   
 *      ___   _____    __ _                              
 *     /_\ \ / / _ \  / _(_)_ _ _ ____ __ ____ _ _ _ ___ 
 *    / _ \ V /|   / |  _| | '_| '  \ V  V / _` | '_/ -_)
 *   /_/ \_\_/ |_|_\ |_| |_|_| |_|_|_\_/\_/\__,_|_| \___|
 *                                                       
 * 
 *******************************************************************************
 *  Dajovic 2019 - danjovic@hotmail.com
 *   
 *  This code is released under GPL 
 *******************************************************************************
 *
*/

////////////////////////////////////////////////////////////////////////////////
// Definitions


#define DEBUG 1           // Uncomment this line to enable debug on serial console

// #define TIMEOFDAY1       // Uncomment this line to show time of day (noon, etc) 
#define WIRESOFT 1        // Uncomment this line to use hardware WIRE library

#define SOFT_SDA 10      // SCL and SDA pins for soft wire
#define SOFT_SCL 11 


#define _DelayPalavra 2000  // delay on the end of a received string
#define  _DelayLetra 1000 // delay between characters, in milliseconds
                          // TODO: modulate the delay according to the distance
                          // between subsequent characters.  


#define PIN        9    // 
#define NUMPIXELS  26   // Amount of neopixels in the board


////////////////////////////////////////////////////////////////////////////////
// Libraries
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "Wire.h"
#include "uRTCLib.h"

////////////////////////////////////////////////////////////////////////////////
//Constants
const PROGMEM uint8_t lampColor[26*3]  =
{
    253 , 244 , 237  ,  //  A    Pre defined lamp colors picked from a snapshot 
     26 ,  54 , 158  ,  //  B    of the TV series
    179 ,  10 , 144  ,  //  C
    174 , 230 , 221  ,  //  D
     21 , 191 , 219  ,  //  E
    255 , 198 , 61   ,  //  F
    214 ,  86 , 137  ,  //  G
      4 , 172 , 200  ,  //  H
     61 , 209 , 219  ,  //  I
    212 ,  77 , 131  ,  //  J
     75 , 193 , 219  ,  //  K
    200 , 239 , 239  ,  //  L
    240 , 195 , 31   ,  //  M
    219 ,  89 , 149  ,  //  N
    179 ,   1 , 160  ,  //  O
    214 , 232 , 235  ,  //  P
    235 , 109 , 209  ,  //  Q
    242 , 244 , 244  ,  //  R
    255 , 239 , 212  ,  //  S
    251 , 202 , 26   ,  //  T
      4 , 144 , 191  ,  //  U
    214 , 126 , 174  ,  //  V
     31 , 135 , 177  ,  //  W
    247 , 195 , 59   ,  //  X
    237 , 175 , 212  ,  //  Y
    239 , 186 , 214     //  Z
};

// adapted from https://github.com/wouterdevinck/wordclock/blob/master/firmware/wordclock.ino

static const char * wMinutes[]  = { " one", " two", " three", " four", " five", " six", " seven", " eight", " nine", " ten", " eleven", " twelve", " thirteen", " fourteen", " quarter", " sixteen", " seventeen", " eighteen", " nineteen", " twenty" };
static const char * wHours[] = { " one", " two", " three", " four", " five", " six", " seven", " eight", " nine", " ten", " eleven", " twelve" };



// Brazilian Portuguese spell time
static const char * uma_duas[]  = { "", " Uma", " Duas"};
static const char * unidades[]  = { "", " Um", " Dois", " Tres", " Quatro", " Cinco", " Seis", " Sete", " Oito", " Nove", " Dez", " Onze" };
static const char * dez_vinte[] = { "", " Onze", " Doze", " Treze", " Quatorze", " Quinze", " Dezesseis", " Dezessete", " Dezoito", " Dezenove" };
static const char * dezenas[]   = { "", " Dez", " Vinte", " Trinta", " Quarenta", " Cinquenta", " Sessenta", " Setenta", " Oitenta", " Noventa" };


                                 // JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC    
const uint8_t daysPerMonth[12] = {  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

enum {  _BR=1,
        _EN
  } Language;



////////////////////////////////////////////////////////////////////////////////
// Variables/Objects
uint8_t language = _BR;
//uint8_t pcf8563regiters[15];
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
uRTCLib rtc;


////////////////////////////////////////////////////////////////////////////////
//    ___      _             
//   / __| ___| |_ _  _ _ __ 
//   \__ \/ -_)  _| || | '_ \
//   |___/\___|\__|\_,_| .__/
//                     |_|   

void setup() {
  // Initialize the board
  pixels.begin();
  pixels.clear();
  
  // Initialize serial port
  Serial.begin(9600);
#ifdef DEBUG  
  Serial.println(F("Stranger Board AVR"));
#endif

  // Initialize I2C bus
  Wire.begin();


}


////////////////////////////////////////////////////////////////////////////////
//    __  __      _        _                  
//   |  \/  |__ _(_)_ _   | |   ___  ___ _ __ 
//   | |\/| / _` | | ' \  | |__/ _ \/ _ \ '_ \
//   |_|  |_\__,_|_|_||_| |____\___/\___/ .__/
//                                      |_|  

void loop() {

  if (Serial.available())
     receiveMessage();
  else if ( aMinuteHasPassed() )
     showTime();
  else
     animateBoard();  
}       






////////////////////////////////////////////////////////////////////////////////
//     __              _   _             
//    / _|_  _ _ _  __| |_(_)___ _ _  ___
//   |  _| || | ' \/ _|  _| / _ \ ' \(_-<
//   |_|  \_,_|_||_\__|\__|_\___/_||_/__/
//                                       
//

////////////////////////////////////////////////////////////////////////////////
// Get a message from serial port and parse it 
// If message begins with the character '+' the message is used to set the clock
// Otherwise the message is sent to the panel
//
void receiveMessage() {  
  while(Serial.available() > 0 ){
    String str = Serial.readString();

    switch (str[0]) {
      case '+': setupClock( str );
                break;

      case '@': changeLanguage( str);
                break;          

      default:  strangePrint ( str );
                delay (_DelayPalavra);

    }
  }

}

////////////////////////////////////////////////////////////////////////////////
// Sent current time as a string to the board
void showTime() {
  rtc.refresh();
#ifdef DEBUG  
  printTime(rtc.hour(),rtc.minute());
#endif  
  strangeTime(rtc.hour(),rtc.minute());
}


////////////////////////////////////////////////////////////////////////////////
// Perform some special effect on the lights
void animateBoard() {
#ifdef DEBUG  
  Serial.println(F( "doing animation"));
  delay(500);
#endif
 
  
  }  


////////////////////////////////////////////////////////////////////////////////
// Check if a minute has passed since last time 
// 
bool aMinuteHasPassed() {
  static uint8_t lastMinute=0;
  uint8_t thisMinute;
#ifdef DEBUG
  Serial.println("?");  
#endif
  rtc.refresh();
  thisMinute = rtc.minute();

  if (thisMinute != lastMinute) {
     lastMinute=thisMinute;
     return true;    
  } else 
     return false;
}





////////////////////////////////////////////////////////////////////////////////
//        _      ___             _   _             
//    _ _| |_ __| __|  _ _ _  __| |_(_)___ _ _  ___
//   | '_|  _/ _| _| || | ' \/ _|  _| / _ \ ' \(_-<
//   |_|  \__\__|_| \_,_|_||_\__|\__|_\___/_||_/__/
//                                                 

////////////////////////////////////////////////////////////////////////////////
// Setup the clock 
// 
void setupClock( String s) {
  
#ifdef DEBUG
  Serial.print("set clock:");
  Serial.println (s);
  rtc.refresh();
  Serial.print("Time now: ");
  displayTime();
  Serial.println();
  
#endif
  bool timeChanged = false; 
  uint8_t _second, _minute, _hour, _dayOfWeek, _dayOfMonth, _month, _year;

  rtc.refresh();  // update 
  _second = rtc.second();
  _minute = rtc.minute();
  _hour   = rtc.hour();
  _dayOfWeek = rtc.dayOfWeek();
  _dayOfMonth = rtc.day();
  _month = rtc.month();
  _year  = rtc.year();      
  
  // Iterate through message. Each command correspond to a command
  char *pStr = &s[0];
  if (*pStr != '\0')
    while (*pStr != '\0') {
      switch (*pStr++) {
        case  'R': // Reset time -> 00:00:00 sunday 01/january/xxxx 
               //        second  minute  hour  dayOfWeek  dayOfMonth  month  year
               rtc.set( 0    ,  0    ,  0  ,  1       ,  1        ,  1   ,  00  ); 
               break;

        // TIME Settings       

        case  'H': // Advance 10 units of hour, round at 24:00
               _hour += 10;
               _hour = _hour %24;
               timeChanged = true;
               break;
               
        case  'h': // Advance  1 unit  of hour, round at 24:00
               _hour += 1;
               _hour = _hour %24;
               timeChanged = true;
               break;        
               break;

        case  'M': // Advance 10 units of minute, round at :60
               _minute += 10;
               _minute = _minute %60;
               timeChanged = true;        
               break;
               
        case  'm': // Advance  1 unit  of minute, round at :60
               _minute += 1;
               _minute = _minute %60;
               timeChanged = true;                
               break;

        case  'S': // reset seconds
        case  's': 
               _second = 0;
               timeChanged = true;
               break;

        // DATE Settings 
        
        case  'D': // Advance 10 days, round at end of month
               _dayOfMonth += 10;
               if (_dayOfMonth > daysPerMonth[_month-1]) _dayOfMonth=1;
               timeChanged = true;        
               break;
               
        case  'd': // Advance  1 unit  of minute, round at :60
               _dayOfMonth += 1;
               if (_dayOfMonth > daysPerMonth[_month-1]) _dayOfMonth=1;
               timeChanged = true;                
               break;
               // NOTE: Don't consider leap years. if you want to  
               // set the clock at february 29, wait for the next day
               
        
        
        case  'O': // Advance 10 units of month, round at :12
               _month += 10;
               if (_month>12) _month = 1;
               timeChanged = true;        
               break;
               
        case  'o': // Advance  1 unit  of month, round at :12
               _month += 1;
               if (_month>12) _month = 1;
               timeChanged = true;        
               break;


        case  'W': // Advance day of week, round at 7 
        case  'w': 
               _dayOfWeek += 1;
               if ( _dayOfWeek > 7) _dayOfWeek = 1;
               timeChanged = true;                               
               break;

//        case  'C': // Change century setup
//        case  'c':
//               break;

        case  'Y': // Advance 10 units of year, round at 99
               _year += 10;
               _year = _year % 100;
               timeChanged = true;                
               break;
               
        case  'y': // Advance  1 unit  of century, round at 99
               _year += 1;
               _year = _year % 100;
               timeChanged = true;                
               break;      

                 

  
      } // switch
    } // while

    if (timeChanged){
      rtc.set(_second, _minute, _hour, _dayOfWeek, _dayOfMonth, _month, _year);
      showTime();                              
    }
         
} // 



////////////////////////////////////////////////////////////////////////////////
// Setup the Language 
// 
void changeLanguage( String s) {

uint8_t lang;
  
#ifdef DEBUG
  Serial.print(F("Change Language"));
  Serial.println();
  
#endif
  bool languageChanged = false; 

  language = EEPROM.read(0x02);   
  
  // Iterate through message. Each command correspond to a command
  char *pStr = &s[0];
  if (*pStr != '\0')
    while (*pStr != '\0') {
      switch (*pStr++) {
      
        case  'E':
        case  'e': 
        default:   language = _EN;  // English
                   languageChanged = true;
                   break;
      
        case  'B':
        case  'b': language = _BR;  // Brazilian portuguese
                   languageChanged = true;
                   break;
      } // switch
    } // while

    if (languageChanged){
      EEPROM.write(0x02,language);
      showTime();         
    }        
} //




////////////////////////////////////////////////////////////////////////////////
//       _                          ___     _     _   
//    __| |_ _ _ __ _ _ _  __ _ ___| _ \_ _(_)_ _| |_ 
//   (_-<  _| '_/ _` | ' \/ _` / -_)  _/ '_| | ' \  _|
//   /__/\__|_| \__,_|_||_\__, \___|_| |_| |_|_||_\__|
//                        |___/                       
//
////////////////////////////////////////////////////////////////////////////////
// Lit the message 
// 
void strangePrint ( String s) {
  char *pStr = &s[0];
  if (*pStr != '\0')
    while (*pStr != '\0')
      putpixel (*pStr++);
  pixels.clear();
  pixels.show();
//  delay (1000);
#ifdef DEBUG  
  Serial.println(F("Finis"));
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Lit one character
//
void putpixel (char letra) {
  uint8_t pixelPosition;
  uint8_t r, g, b;


  // blank the board  
  pixels.clear(); // clear board
  pixels.show();  // 
  delay(100);     // wait some time before lit another character
                  // to blink on repeated characters 
  
  // select a position to lit
  if ( (letra >= 'a') && (letra <= 'z'))  // very crude uppercase conversion
    letra = letra - 32;
  
  if ( (letra >= 'A') && (letra <= 'Z')) {
    pixelPosition = letra - 'A';

    r = pgm_read_byte(lampColor + pixelPosition * 3 + 0);
    g = pgm_read_byte(lampColor + pixelPosition * 3 + 1);
    b = pgm_read_byte(lampColor + pixelPosition * 3 + 2);

#ifdef DEBUG  
    Serial.print(letra);
    Serial.print('(');
    Serial.print(r);
    Serial.print('/');   
    Serial.print(g);
    Serial.print('/');
    Serial.print(b);
    Serial.println(')');    
#endif
    pixels.setPixelColor(pixelPosition, pixels.Color(r, g, b));
  }
#ifdef DEBUG
    else Serial.println(letra);
#endif 

  // Update the board 
  pixels.show();
  delay (_DelayLetra);
}




////////////////////////////////////////////////////////////////////////////////
//       _                         _____ _           
//    __| |_ _ _ __ _ _ _  __ _ __|_   _(_)_ __  ___ 
//   (_-<  _| '_/ _` | ' \/ _` / -_)| | | | '  \/ -_)
//   /__/\__|_| \__,_|_||_\__, \___||_| |_|_|_|_\___|
//                        |___/                      
//
////////////////////////////////////////////////////////////////////////////////
void strangeTime(uint8_t h, uint8_t m) {

  switch(language) {
    case _EN: strangeTimeEN( h, m);
              break;          

    case _BR: 
    default:  strangeTimeBR( h, m);
              break;
    }
}





// Spell time on board in BRAZILIAN PORTUGUESE
void strangeTimeBR(uint8_t h, uint8_t m) {

  uint8_t d = m / 10 ;
  uint8_t u = m - (m / 10) * 10;
  uint8_t dv = d * 10 + u;


  // Horas
  if (h == 0) {
    strangePrint(" meia noite");
  } else if (h == 12) {
    strangePrint(" meio dia");
  } else { //

    if (h > 12) h -= 12; // relogio de 12 horas

    if (h < 3) { // uma, duas
      strangePrint(uma_duas[h] );
    } else {
      strangePrint(unidades[h]);
    }
  } //

  if ((m == 0) && (h != 12) && (h != 0)   ) { // horas inteiras
    strangePrint (" hora");
    if (h > 1) strangePrint ("s");

  } else { // horas quebradas

    if (d > 0)
      strangePrint (" e");

    if (m == 30) { // meia hora
      strangePrint (" meia");
    } else { // 01-29, 31-59 minutos



      if (u == 0)
        strangePrint(dezenas[d]); // dezenas inteiras
      else {
        if ( (dv > 10) && (dv < 20) ) {
          strangePrint(dez_vinte[ dv - 10 ] );
        } else {
          strangePrint (dezenas[d]);
          strangePrint (" e");
          strangePrint (unidades[u]);
        }
      }
    }
  }
}



// Spell time on board in ENGLISH
void strangeTimeEN(uint8_t h, uint8_t m) {
  
  uint8_t h2 = h;
  bool oclock=false;
  
  strangePrint("it is") ;
  
  // Minutes
  if (m == 0) {
    
    if (h == 0) {
      strangePrint(" midnight");
    } else if (h == 12) {
      strangePrint(" noon");
    } else {
      oclock=true;
    }

  } else {
  
    if (m <= 20) {
      strangePrint(wMinutes[m - 1]);
    } else if (m < 30) {
      strangePrint(wMinutes[19]); // twenty
      strangePrint(wMinutes[m - 21]);
    } else if (m == 30) {
      strangePrint(" half");
    } else if (m < 40) {
      strangePrint(wMinutes[19]); // twenty
      strangePrint(wMinutes[60 - m - 21]);
    } else {
      strangePrint(wMinutes[60 - m - 1]);
    }
 
    if(m <= 30) {
      strangePrint (" past");
    } else {
      strangePrint(" to");
      ++h2;
    }
    
  } 
  
  if(!(m ==0 && (h == 0 || h == 12))) {
  
    // Hours
    if(h2 == 0) {
      strangePrint(wHours[11]);
    } else if (h2 <= 12) {
      strangePrint(wHours[h2-1]);
    } else {
      strangePrint(wHours[h2-13]); 
    }
    

    if (oclock)
       strangePrint(" o clock");

#ifdef TIMEOFDAY  
    // Time of day
    if(h < 6) {
      strangePrint(" at night");
    } else if(h < 12) {
      strangePrint(" in the morning");
    } else if(h < 18) {
      strangePrint(" in the afternoon");
    } else {
      strangePrint(" at night");
    }
#endif    
  }
}







////////////////////////////////////////////////////////////////////////////////
//       _     _             _____         _    
//    __| |___| |__ _  _ __ |_   _|__  ___| |___
//   / _` / -_) '_ \ || / _` || |/ _ \/ _ \ (_-<
//   \__,_\___|_.__/\_,_\__, ||_|\___/\___/_/__/
//                      |___/                   
//

#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
// Print time - for debug purposes
//

void displayTime(){
  Serial.print("\nRTC DateTime: ");

  Serial.print(rtc.day());
  Serial.print('/');
  Serial.print(rtc.month());
  Serial.print('/');
  Serial.print(rtc.year());

  Serial.print(' ');

  Serial.print(rtc.hour());
  Serial.print(':');
  Serial.print(rtc.minute());
  Serial.print(':');
  Serial.print(rtc.second());

  Serial.print(" DOW: ");
  Serial.print(rtc.dayOfWeek());

  Serial.println();
  
  
  }

////////////////////////////////////////////////////////////////////////////////
// Spell Time
//
void printTime(uint8_t h, uint8_t m) {
  
  uint8_t h2 = h;
  bool oclock=false;
  
  Serial.print("it is") ;
  
  // Minutes
  if (m == 0) {
    
    if (h == 0) {
      Serial.print(" midnight");
    } else if (h == 12) {
      Serial.print(" noon");
    } else {
      oclock=true;
    }

  } else {
  
    if (m <= 20) {
      Serial.print(wMinutes[m - 1]);
    } else if (m < 30) {
      Serial.print(wMinutes[19]); // twenty
      Serial.print(wMinutes[m - 21]);
    } else if (m == 30) {
      Serial.print(" half");
    } else if (m < 40) {
      Serial.print(wMinutes[19]); // twenty
      Serial.print(wMinutes[60 - m - 21]);
    } else {
      Serial.print(wMinutes[60 - m - 1]);
    }
 
    if(m <= 30) {
      Serial.print (" past");
    } else {
      Serial.print(" to");
      ++h2;
    }
    
  } 
  
  if(!(m ==0 && (h == 0 || h == 12))) {
  
    // Hours
    if(h2 == 0) {
      Serial.print(wHours[11]);
    } else if (h2 <= 12) {
      Serial.print(wHours[h2-1]);
    } else {
      Serial.print(wHours[h2-13]); 
    }

    if (oclock)
       Serial.print(" o clock");

  
    // Time of day
    if(h < 6) {
      Serial.print(" at night");
    } else if(h < 12) {
      Serial.print(" in the morning");
    } else if(h < 18) {
      Serial.print(" in the afternoon");
    } else {
      Serial.print(" at night");
    }  
  }
}
 
 
// show time in portuguese
 
void showTimeBR(uint8_t h, uint8_t m) {

  uint8_t d = m / 10 ;
  uint8_t u = m - (m / 10) * 10;
  uint8_t dv = d * 10 + u;

  // Horas
  if (h == 0) {
    Serial.print(" meia noite");
  } else if (h == 12) {
    Serial.print(" meio dia");
  } else { //

    if (h > 12) h -= 12; // relogio de 12 horas

    if (h < 3) { // uma, duas
      Serial.print(uma_duas[h] );
    } else {
      Serial.print(unidades[h]);
    }
  } //

  if ((m == 0) && (h != 12) && (h != 0)   ) { // horas inteiras
    Serial.print (" hora");
    if (h > 1) Serial.print ("s");

  } else { // horas quebradas

    if (d > 0)
      Serial.print (" e");

    if (m == 30) { // meia hora
      Serial.print (" meia");
    } else { // 01-29, 31-59 minutos



      if (u == 0)
        Serial.print(dezenas[d]); // dezenas inteiras
      else {
        if ( (dv > 10) && (dv < 20) ) {
          Serial.print(dez_vinte[ dv - 10 ] );
        } else {
          Serial.print (dezenas[d]);
          Serial.print (" e");
          Serial.print (unidades[u]);
        }
      }
    }
  }
} 

  
#endif


