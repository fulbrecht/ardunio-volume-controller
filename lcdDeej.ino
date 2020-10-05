#include <LiquidCrystal.h>
#include <Encoder.h>

Encoder knob(2,3);


const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//you can tweak following values for you needs
const int encoder0PinA = 2; //encoder pins
const int encoder0PinB = 3;
const int encoderSteps = 4;
const int buttonpin = 4;//pushbutton from encoder
const int amountsliders = 6; //amount of sliders you want, also name them in the array below
const String slidernames[amountsliders] = {"Master",
                                           "Music",
                                           "Browser",
                                           "Games",
                                           "Discord",
                                           "Mic",
                                          };
const int increment[amountsliders] = {5, 5, 1, 2, 5, 5}; //choose you're increment for each slider 1,2,4,5,10,20,25,50,100

int displayValue[amountsliders] = {50, 50, 50, 50, 50, 50}; //start values for every slider


//leave following values at their default
bool encoder0PinALast = HIGH;//start values for the encoder
bool firstEncoderRead = LOW;
bool encoderupdate = 0;
bool lcdupdate = 1; //when this variable is 1 the lcd will update and the variable will turn back to 0
bool sliderupdate = 1; //when this variable is 1 the slidervalues get sended to deej and the variable will turn back to 0
int slidernumber = 0; //variable which numbers all the sliders
bool singlebuttonpress = 0; //variable to let the pushbutton from the encoder toggle 1

int knobNew;
int knobOld;
int state = 0;    //state 0 is the menu screen to select what you want to change
//state 1 is the screen where you change the value itself

byte arrow[] = {  //byte for creating an arrow on the lcd screen
  B11000,
  B11100,
  B11110,
  B11111,
  B11110,
  B11100,
  B11000,
  B00000
};

void setup() {
//  pinMode (encoder0PinA, INPUT);
//  pinMode (encoder0PinB, INPUT);
  knob.write(51);
  knobNew = knob.read();
  knobOld = knob.read();
  pinMode (buttonpin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.createChar(0, arrow);
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  while (! Serial) {
    //wait for Serial connection
  }
}

void loop() {
  if ((digitalRead(buttonpin) == LOW) && singlebuttonpress == 0) { //this reads the button of the encoder and will only react to the first press
    singlebuttonpress = 1;
    lcdupdate = 1;
    if (state == 0) {
      state = 1;
    }
    else if (state == 1) {
      state = 0;
      slidernumber = 0;
    }
    delay(10);
  }
  if (digitalRead(buttonpin) == HIGH) {
    singlebuttonpress = 0;
  }

  knobNew = knob.read();
  if (knobNew <= knobOld - encoderSteps || knobNew >= knobOld + encoderSteps){
    encoderupdate = 1;
  }
//  firstEncoderRead = digitalRead(encoder0PinA);//determing if encoder is turning, and if so, what side it is turning
  if (encoderupdate == 1) {
    lcdupdate = 1;
    if (knobNew > knobOld) {
      if (state == 0) { //scrolling between all the slides
        slidernumber++;
        if (slidernumber > (amountsliders - 1)) {
          slidernumber = 0;
        }
      }
      else if (state == 1) {
        if ((100 - displayValue[slidernumber]) >= increment[slidernumber]) { //increasing slider
          displayValue[slidernumber] = displayValue[slidernumber] + increment[slidernumber];
          sliderupdate = 1;
        }
      }
    }
    else {
      if (state == 0) {
        slidernumber--;
        if (slidernumber < 0) {
          slidernumber = amountsliders - 1;
          sliderupdate = 1;
        }
      }
      else if (state == 1) {
        if (displayValue[slidernumber] >= increment[slidernumber]) { //decreasing slider
          displayValue[slidernumber] = displayValue[slidernumber] - increment[slidernumber];
        }
      }
    }
    knobOld = knobNew;
  }
  encoderupdate = 0;

  if (lcdupdate == 1) { //update the lcd with new values
    lcdupdate = 0;
    lcd.clear();
    if (state == 0) {
      lcd.setCursor(0, 0); //put slider names on screen
      lcd.write(byte(0));
      lcd.print(String(slidernumber + 1) + ". " + slidernames[slidernumber]);
      lcd.setCursor(13,0);
      lcd.print(displayValue[slidernumber]);
      lcd.print("%");
      lcd.setCursor(1, 1);
      if (slidernumber >= (amountsliders - 1)) {
        lcd.print(String(1) + ". " +slidernames[0]);
        lcd.setCursor(13,1);
        lcd.print(displayValue[0]);
        lcd.print("%");
      }
      else {
        lcd.print(String(slidernumber + 2) + ". " +slidernames[slidernumber + 1]);
        lcd.setCursor(13,1);
        lcd.print(displayValue[slidernumber + 1]);
        lcd.print("%");
      }
    }
    else if (state == 1) { //put slider on screen
      lcd.setCursor(0, 0);
      lcd.print(slidernames[slidernumber]);
      lcd.setCursor(0, 1);
      lcd.print(displayValue[slidernumber]);
      lcd.print("%");
    }
  }
  if (sliderupdate == 1) { //update the slider values
    sliderupdate == 0;
    String builtString = String("");
    for (int index = 0; index < amountsliders; index++) {
      builtString += String(map(displayValue[index],0,100,0,1023));
      if (index < amountsliders - 1) {
        builtString += String("|");
      }
    }
    Serial.println(builtString);//combining every slider values seperated by | and sending it through the serial console
  }
}
