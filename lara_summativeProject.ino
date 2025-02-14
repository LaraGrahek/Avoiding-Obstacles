/*
 *  Author: Lara Grahek
 *  Date: January 23rd, 2023
 *  Description: This program uses a button and potentiometer as input, 
 *  and uses a buzzer and 16x2 LCD as output. The LCD displays a cat
 *  that the user can move from on row to the other by pushing the button,
 *  while obstacles come towards it. The buzzer makes a sound whenever the
 *  cat passes an obstacle, and when it collides with an obstacle.
 */

#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// row that the cat, rock, and bird are on
int currentRow=0;
int rowRock = 0;  
int rowBird = 0;  
// columns that the rock and bird and spike are on
int currentCol_r = 16;
int currentCol_b = 16;
int currentCol_s = 16;

int button = 9; // button
int score = 0;  // score
int hi = 0; // high score
int times = 1;  // amount of times the score has hit 4
int speed = 110;  // speed of game
int var = 0;  // variable used for placement of obstacles
const int BUZZER_PIN = 13;  // buzzer to make sounds

// defining notes to use
#define NOTE_A4  440
#define NOTE_A5  880

// creating the cat, block, bird, and spike characters
byte cat[8] = 
{
  0b00000,
  0b00000,
  0b00101,
  0b00111,
  0b10111,
  0b11110,
  0b11110,
  0b10010
};

byte block[8] = 
{
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte bird[8] = 
{
  0b00000,
  0b00010,
  0b00110,
  0b10100,
  0b11111,
  0b00110,
  0b00000,
  0b00000
};

byte spike[8] =
{
  0b00100,
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b11111
};

void setup() 
{
  randomSeed(analogRead(A1)); // for random
  // creating characters
  lcd.createChar(0, cat);
  lcd.createChar(1,block);
  lcd.createChar(2,bird);
  lcd.createChar(3,spike);

  pinMode(button, INPUT_PULLUP);  // setting the button as input
  lcd.begin(16,2);  // start the LCD
  lcd.setCursor(0, 1);  // setting cursor bottom left
  pinMode(BUZZER_PIN, OUTPUT);  // setting buzzer as output
}

void loop() 
{ 
  if ((score/times) == 5) // score has 5 more points
  {
    times += 1; // the amount of times the game has been sped up
    if (speed!=0)
    {
      speed -= 10;  // taking 10 away from speed
    }
  }
  
  lcd.clear();
  lcd.setCursor(currentCol_r,rowRock);  // putting the rock where it should be
  lcd.write(byte(1)); // displaying the rock
  currentCol_r--; // moving it one to the left
  if ((score>5)&&(var<8))  // score is more than 5
  {
  	var++;  // add one to this variable each time the rock moves left by one
  }
  if(currentCol_r ==-1) // the rock is no longer of the screen
  {
   currentCol_r =16;  // reset the rock back to the right
   rowRock = random(0,2); // randomly choose which row the rock will be on
  }
  
  if ((score>5)&&(var>7))  // score is more than 5 and the rock has moved to the left 6 times
  {
    lcd.setCursor(currentCol_b,rowBird);  // putting bird in it's place
    lcd.write(byte(2)); // displaying bird
    currentCol_b--; // moving to the left by 1
    if ((score>14)&&(var<20)) // score is more than 14
    {
      var++;  // add one to the variable each time the bird moves left by one
    }
    if(currentCol_b ==-1) // the bird is out of the LCD
    {
     currentCol_b = 16;  // reset it to the right
     rowBird = random(0,2); // random row 
    }
  }
  Serial.println(var);
  if ((score>14)&&(var>19)) // score is more than 14 and the bird has moved to the left a few times
  {
    lcd.setCursor(currentCol_s,1);  // stays on the bottom row
    lcd.write(byte(3)); // displays spike
    currentCol_s--; // moves it to the left by one
    if(currentCol_s ==-1) // spike is out of frame
    {
     currentCol_s =16;  // resetting it all the way to the left
    }
  }

  if (digitalRead(button) == LOW)  // button is pushed
  {
    currentRow++; // add one to the row
    if (currentRow>1){  // the row is more than 1
      currentRow = 0; // row is set to the upper one
    }
    // putting the cat on the row that it should be on
    lcd.setCursor(0,currentRow);
    lcd.write(byte(0)); 
    delay(500);
  }
  else  // button is not being pushed
  {
    lcd.setCursor(0,currentRow);  // cat while stay on it's row
    lcd.write(byte(0)); // displaying cat
  }
  if (currentCol_r==0 || currentCol_b==0 || currentCol_s==0) // the cat passes the rock, bird, or spike
  {
    score++;  // add one to the score
    tone(BUZZER_PIN, NOTE_A4, 180); // make a sound
  }

  // the cat is in the same place as the rock or bird or spike (they collide with one another)
  if((currentRow == rowRock && currentCol_r==0)||(currentRow == rowBird && currentCol_b==0)||(currentRow == 1 && currentCol_s==0))
  {
    lcd.clear();  // clear screen
    delay(100);
    tone(BUZZER_PIN, NOTE_A5, 250); // sound
    lcd.write("Game Over"); // displays game over
    delay(250);
    tone(BUZZER_PIN, NOTE_A4, 250); // sound
    delay(900);
    lcd.clear();

    while(digitalRead(button) == HIGH)
    {
     delay(10);
     lcd.setCursor(0,0);  // cursor is top left
     lcd.write("press button to"); 
     lcd.setCursor(0,1);
     lcd.write("restart");
     if (score>hi)  // score is more than the previous high score
     {
       hi = score-1;  // high score is the score, minus 1 because cat died
     }
     // displaying the high score in bottom right
     lcd.setCursor(11,2);
     lcd.write("HI:");
     lcd.setCursor(14,2);
     lcd.print(hi);
    }
    // resetting all variables
    currentRow= 0;
    rowRock = 0;
    rowBird = 0;
    currentCol_b = 16;
    currentCol_r = 16;
    currentCol_s = 16;
    score = 0;
    speed = 100;
    timer = 0;
    var = 0;
  }
  else
  {
    lcd.setCursor(14,0);  // top right
    lcd.print(score); // showing score
  }
  

  delay(speed); // the time between each movement of the obstacles
}