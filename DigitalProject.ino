#include "MaxMatrix.h"
#include <avr/pgmspace.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////
// Display State Assignments (0 = LED off, 1 = LED on) ////////////////
///////////////////////////////////////////////////////////////////////

PROGMEM const unsigned char CH[] = {
8, 8, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, // 0 (blank)
8, 8, B00111100, B01000010, B10100101, B10100001, B10100001, B10100101, B01000010, B00111100, // 1 (neutral) :|
8, 8, B00111100, B01000010, B10010101, B10100001, B10100001, B10010101, B01000010, B00111100, // 2 (happy)   :)
8, 8, B00111100, B01000010, B10100101, B10010001, B10010001, B10100101, B01000010, B00111100, // 3 (sad)     :(
8, 8, B00001100, B00011110, B00111110, B01111100, B01111100, B00111110, B00011110, B00001100, // 4 (heart)   <3
8, 8, B11000011, B11100111, B01111110, B00111100, B00111100, B01111110, B11100111, B11000011, // 5 (wrong)   X
};

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Variable Declarations / Pin Assignments ////////////////////////////
///////////////////////////////////////////////////////////////////////

int data = 8;    // DIN pin of MAX7219 module 8x8 LED
int load = 9;    // CS pin of MAX7219 module 8x8 LED
int _clock = 10; // CLK pin of MAX7219 module 8x8 LED

int eat = 3;     // Eat button
int drink = 4;   // Drink button
int sleep = 5;   // Sleep button

int maxInUse = 1;    //change this variable to set how many MAX7219's you'll use

MaxMatrix m(data, load, _clock, maxInUse); // define MAX7219 module (data pin, clock pin, max # of displays)
byte buffer[10]; // buffer size

int state = 0;   // State of the pal, and what should be displayed on the screen (Default: 0 blank)

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Main Functions /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void setup(){
  pinMode(eat, INPUT_PULLUP);   // eat button
  pinMode(drink, INPUT_PULLUP); // drink button
  pinMode(sleep, INPUT_PULLUP); // sleep button
  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not
  m.init(); // module initialize
  m.setIntensity(15); // dot matix intensity 0-15
  Serial.begin(9600); // serial communication initialize
  SetState(2); // happy for 10 secs before LifeSim starts
  delay(10000);
}

void loop(){
  LifeSim();
  SetState(1);  // neutral for 4 secs until life sim again
  delay(4000);
  //Serial.println(digitalRead(eat) + digitalRead(drink) +  digitalRead(sleep)); // For testing buttons
}

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Helper Functions ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void LifeSim() // Causes random "events" to be solved
{
  int randNum = random(1,100); // random num from 1 to 100
  Serial.println(randNum); // print randNum to serial monitor
  Serial.println("Needs: ");

  if(randNum < 40) // all good, bro (#useful-comments)
  {
    Serial.println("Just chillin");
    delay(2000); // just chill for 2 secs
  }
  else if(randNum >= 40 && randNum < 60) // needs food 40-59
  {
    Serial.println("Needs food");
    NeedsToEat(); // Wait for food
  }
  else if(randNum >= 60 && randNum < 80) // needs drink 60-79
  {
    Serial.println("Needs water");
    NeedsToDrink(); // Wait for drink
  }
  else if(randNum >= 80) // needs sleep 80-100
  {
    Serial.println("Needs sleep");
    NeedsToSleep(); // Wait for sleep
  }
}

void NeedsToEat()
{
  while(digitalRead(eat) == HIGH) // HIGH means button not pressed
  {
    SetState(3); // sad

    if(digitalRead(drink) == LOW || digitalRead(sleep) == LOW) // If incorrect button pressed (LOW means button is pressed)
    {
      SetState(5); // wrong
      delay(2000); // wait two seconds to try again
    }
  }
  SetState(4); // heart for 2 secs
  delay(2000);
  SetState(2); // happy for 5 secs
  delay(5000);
}

void NeedsToDrink()
{
  while(digitalRead(drink) == HIGH) // HIGH means button not pressed
  {
    SetState(3); // sad

    if(digitalRead(eat) == LOW || digitalRead(sleep) == LOW) // If incorrect button pressed (LOW means button is pressed)
    {
      SetState(5); // wrong
      delay(2000); // wait two seconds to try again
    }
  }
  SetState(4); // heart for 2 secs
  delay(2000);
  SetState(2); // happy for 5 secs
  delay(5000);
}

void NeedsToSleep()
{
  while(digitalRead(sleep) == HIGH) // HIGH means button not pressed
  {
    SetState(3); // sad

    if(digitalRead(eat) == LOW || digitalRead(drink) == LOW) // If incorrect button pressed (LOW means button is pressed)
    {
      SetState(5); // wrong
      delay(2000); // wait two seconds to try again
    }
  }
  SetState(4); // heart for 2 secs
  delay(2000);
  SetState(2); // happy for 5 secs
  delay(5000);
}

void SetState(int s)
{
  state = s;

  if(state == 0)
  {
    printString("0");
  }
  else if(state == 1)
  {
    printString("1");
  }
  else if(state == 2)
  {
    printString("2");
  }
  else if(state == 3)
  {
    printString("3");
  }
  else if(state == 4)
  {
    printString("4");
  }
  else if(state == 5)
  {
    printString("5");
  }
}

void printString(char* s)
{
  int col = 0;      // set current column to 0
  while (*s != 0)   // check that there is a char to print
  {
    if (*s < 48 || *s > 53) continue; // make sure the char is not outside of the hardcoded range of display state values (0-5) in ASCII
    char c = *s - 48;                 // convert the char value to a useable index
    memcpy_P(buffer, CH + 10*c, 10);  // copies the array to memory
    m.writeSprite(col, 0, buffer);    // writes sprite to the display
    m.setColumn(col + buffer[0], 0);  // defines the column
    col += buffer[0] + 1;             // readies the next column
    s++;                              // moves to next char in the input string
  }
}

///////////////////////////////////////////////////////////////////////
