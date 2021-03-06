#include <TFT.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define TFT_CS 10
#define TFT_RST 0
#define TFT_DC 8

#define LED_PIN 4
#define BUTTON_PIN 9
#define POT_PIN A0
#define LED_COUNT 31

TFT tft = TFT(TFT_CS, TFT_DC, TFT_RST);
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// global selection variable
int select = 0;

// preview color
int red = 0;
int green = 0;
int blue = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("Program starting...");

  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  Serial.println("Initialized screen");

  leds.begin();
  clearLEDs();
  Serial.println("Initialized leds");

  initMenu();
}

void loop() {
  checkJoystick(); // TODO make interrupt driven
  menu();
}

void initMenu() {
  // title
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(50, 0);
  tft.setTextColor(ST7735_RED);
  tft.print("R");
  tft.setTextColor(ST7735_GREEN);
  tft.print("G");
  tft.setTextColor(ST7735_BLUE);
  tft.print("B");
  tft.setTextColor(ST7735_WHITE);
  tft.print(" Manager");
  tft.stroke(255, 255, 255);
  tft.line(0, 10, 160, 10);
  
  // options
  tft.setCursor(10, 22);
  tft.print("Option 1");
  tft.setCursor(10, 37);
  tft.print("Option 2");
  tft.setCursor(10, 52);
  tft.print("Option 3");
  tft.setCursor(10, 67);
  tft.print("Option 4");
  tft.setCursor(10, 82);
  tft.print("Option 5");
  tft.setCursor(10, 97);
  tft.print("Option 6");

  // preview
  tft.rect(70, 24, 80, 80);

  // credits
  tft.setCursor(0, 120);
  tft.print("v1.0");
  tft.setCursor(115, 120);
  tft.print("tlee753");  
  tft.line(0, 117, 160, 117);
}

void menu() { // TODO make changes interrupt driven and maintain otherwise
  // clear all circles
  tft.stroke(0, 0, 0);
  tft.circle(5, 25, 1);
  tft.circle(5, 40, 1);
  tft.circle(5, 55, 1);
  tft.circle(5, 70, 1);
  tft.circle(5, 85, 1);
  tft.circle(5, 100, 1);
  
  // menu cursor
  tft.stroke(255, 255, 255);
  switch(select) {
    case 0:
      tft.circle(5, 25, 1);
      clearLEDs();
      break;
    case 1:
      tft.circle(5, 40, 1);
      displayColor(255, 255, 255);
      break;
    case 2:
      tft.circle(5, 55, 1);
      displayColor(0, 255, 0);
      break;
    case 3:
      tft.circle(5, 70, 1);
      randomLEDs();
      break;
    case 4:
      tft.circle(5, 85, 1);
      for (int i=0; i<LED_COUNT*10; i++) {
        rainbowLEDs(i);
        delay(100);
      }
      break;
    case 5:
      tft.circle(5, 100, 1);
      cycloneLEDs(255, 255, 255, 10);
      break;
  }

  // preview
  tft.fill(red, green, blue);
  tft.rect(70, 24, 80, 80);
}

void checkJoystick()
{
  int joystickState = analogRead(3);
  
  if(joystickState < 50) { // left
    select--;
    delay(100);
  } else if(joystickState < 150) { // down
    select++;
    delay(1000);
  } else if (joystickState < 250) { // press
    select++;
    delay(1000);
  } else if (joystickState < 500) { // right
    select++;
    delay(1000);
  } else if(joystickState < 650) { // up
    select--;
    delay(1000);
  }
  
  if (select < 0) {
    select = 5;
  }
  select = select % 6;
}

void displayColor(int r, int g, int b) {
  red = r;
  green = g;
  blue = b;
  for (int i=0; i < LED_COUNT; i++) {
    leds.setPixelColor(i, r, g, b);
  }
  leds.show();
}

void clearLEDs() {
  red = green = blue = 0;
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();
  Serial.println("Clearing");
}

void brightLEDs(int input, int maxRange) {
  input = input/(maxRange/256);
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, input, input, input);
  }
  leds.show();
}

void randomLEDs() {
  for (int i=0; i<LED_COUNT; i++) {
    int r = random(0, 256);
    int g = random(0, 256);
    int b = random(0, 256);
    leds.setPixelColor(i, r, g, b);
    if (i == 0) {
      red = r;
      green = g;
      blue = b;
    }
  }
  leds.show();
  delay(50);
}

void spectrumLEDs(int input, int maxRange) {
  // local variables
  int thirds = round(maxRange/3);
  int temp;
  int red;
  int green;
  int blue;

  // color control flow
  if (input < thirds ) {                  
    temp = (input * 3) / 4;
    red = 256 - temp;
    green = temp;
    blue = 0;
  }
  else if (input < 2*thirds ) {
    temp = ( (input-thirds) * 3) / 4;
    red = 0;
    green = 256 - temp;
    blue = temp;
  }
  else {
    temp = ( (input-(2*thirds)) * 3) / 4;
    red = temp;
    green = 0;
    blue = 256 - temp;
  }

  for (int i=0; i < LED_COUNT; i++) {
    leds.setPixelColor(i, red, green, blue);
  }
  leds.show();
}

void rainbowLEDs(byte startPosition) 
{
  // scaling for the rainbow
  int rainbowScale = 192 / LED_COUNT;
  
  // Next we setup each pixel with the right color
  for (int i=0; i<LED_COUNT; i++) {
    // It'll return a color between red->orange->green->...->violet for 0-191.
    uint32_t color = rainbowOrder((rainbowScale * (i + startPosition)) % 192);
    leds.setPixelColor(i, color);
    if (i == 0) {
//      red = (int) color % 16;
//      green = (int) color % 8;
//      blue = (int) color %
    }
  }
  leds.show();
}

// Input a value 0 to 191 to get a color value.
// The colors are a transition red->yellow->green->aqua->blue->fuchsia->red...
uint32_t rainbowOrder(byte position) 
{
  // 6 total zones of color change:
  if (position < 31)  // Red -> Yellow (Red = FF, blue = 0, green goes 00-FF)
  {
    return leds.Color(0xFF, position * 8, 0);
  }
  else if (position < 63)  // Yellow -> Green (Green = FF, blue = 0, red goes FF->00)
  {
    position -= 31;
    return leds.Color(0xFF - position * 8, 0xFF, 0);
  }
  else if (position < 95)  // Green->Aqua (Green = FF, red = 0, blue goes 00->FF)
  {
    position -= 63;
    return leds.Color(0, 0xFF, position * 8);
  }
  else if (position < 127)  // Aqua->Blue (Blue = FF, red = 0, green goes FF->00)
  {
    position -= 95;
    return leds.Color(0, 0xFF - position * 8, 0xFF);
  }
  else if (position < 159)  // Blue->Fuchsia (Blue = FF, green = 0, red goes 00->FF)
  {
    position -= 127;
    return leds.Color(position * 8, 0, 0xFF);
  }
  else  //160 <position< 191   Fuchsia->Red (Red = FF, green = 0, blue goes FF->00)
  {
    position -= 159;
    return leds.Color(0xFF, 0x00, 0xFF - position * 8);
  }
}

void cycloneLEDs(int r, int g, int b, byte wait)
{
  red = r;
  green = g;
  blue = b;
  // weight determines how much lighter the outer "eye" colors are
  const byte weight = 4;

  // Start at closest LED, and move to the outside
  for (int i=0; i<=LED_COUNT-1; i++)
  {
    clearLEDs();
    leds.setPixelColor(i, r, g, b);  // Set the bright middle eye
    
    // Now set two eyes to each side to get progressively dimmer
    for (int j=1; j<3; j++)
    {
      if (i-j >= 0)
        leds.setPixelColor(i-j, r/(weight*j), g/(weight*j), b/(weight*j));
      if (i-j <= LED_COUNT)
        leds.setPixelColor(i+j, r/(weight*j), g/(weight*j), b/(weight*j));
    }
    leds.show();  // Turn the LEDs on
    delay(wait);  // Delay for visibility
  }
  
  // Now we go back to where we came. Do the same thing.
  for (int i=LED_COUNT-2; i>=1; i--)
  {
    clearLEDs();
    leds.setPixelColor(i, r, g, b);
    for (int j=1; j<3; j++)
    {
      if (i-j >= 0)
        leds.setPixelColor(i-j, r/(weight*j), g/(weight*j), b/(weight*j));
      if (i-j <= LED_COUNT)
        leds.setPixelColor(i+j, r/(weight*j), g/(weight*j), b/(weight*j));
      }
    
    leds.show();
    delay(wait);
  } 
}
