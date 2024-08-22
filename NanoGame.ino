#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define RINGPIN 12
#define BUTTONPIN2 4
#define NUMPIXELS 24

// This is based off the "strandtest" example code.

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino ring pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, RINGPIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

int toggleState;
int lastButtonState = 1;
long unsigned int lastPress;
volatile int buttonFlag;
int debounceTime = 20;
int colorChangeCount = 0;
int r=255;
int g=255;
int b=255;
float speed;

void setup() {
  //pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(BUTTONPIN2, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(BUTTONPIN), ISR_Button, CHANGE);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {

// Button debounce
    if ((millis() - lastPress) > debounceTime )
  {
    lastPress = millis();
    if(digitalRead(BUTTONPIN2) == 0 && lastButtonState == 1)
    {
      toggleState =! toggleState;
      int winner;

      // Instead of a random number we will just take the current 
      // millisecods since start up
      winner = millis()%24;

      // Start fast and slow to a stop
      for(uint8_t i=10; i<60; i=i+5) {
          rouletteWipe(i,0,24);
        }
      rouletteWipe(60,0,winner);

      // Illuminate the final value
      strip.setPixelColor(winner,strip.Color(255, 255, 255));
      strip.show();
      delay(500); // Pause to let them see where they landed

      // Check for the winner.
      // Change what values are considered winners to balance the odds
      // If you want more winners, check for more values.
      // This example is coded this way in case you wanted to spread out the
      // winning values to be more like 1,7,15,22
      if(winner == 0 || winner == 1 || winner == 2 || winner == 3 || winner == 4){
        for(uint8_t i=0; i<12; i++) {
          ringFlash(255, 255, 255);
          delay(100);
          ringFlash(0, 0, 0);
          delay(20);
        }
        ringFlash(255, 255, 255);
        delay(3000);
      }
      else{
        // Nope, show red for loss
        ringFlash(255, 0, 0);
        delay(3000);
      }
      lastButtonState = 0;
    }
    else if(digitalRead(BUTTONPIN2) == 1 && lastButtonState == 0)
    {
      lastButtonState = 1;
    }
  }


// This is the "draw" idle state animation
if (lastButtonState == 1)
  {
    speed = 1.0f;
    getRainbowColor(speed, &r, &g, &b);
    theaterChase2(strip.Color(r, g, b), 50);
  }

}

void HSVtoRGB(float h, float s, float v, int *r, int *g, int *b) {
    // Routine to allow an easy path through the spectrum of colors

    float p, q, t, ff;
    long i;
    float hue = h / 60.0f;
    
    i = (long)hue;
    ff = hue - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * ff));
    t = v * (1.0f - (s * (1.0f - ff)));

    switch(i) {
    case 0:
        *r = v * 255;
        *g = t * 255;
        *b = p * 255;
        break;
    case 1:
        *r = q * 255;
        *g = v * 255;
        *b = p * 255;
        break;
    case 2:
        *r = p * 255;
        *g = v * 255;
        *b = t * 255;
        break;
    case 3:
        *r = p * 255;
        *g = q * 255;
        *b = v * 255;
        break;
    case 4:
        *r = t * 255;
        *g = p * 255;
        *b = v * 255;
        break;
    case 5:
    default:
        *r = v * 255;
        *g = p * 255;
        *b = q * 255;
        break;
    }
}

// Function to generate RGB values cycling through the rainbow
void getRainbowColor(float speed, int *r, int *g, int *b) {
    static float hue = 0.0f;
    
    // Convert hue to RGB
    HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
    
    // Increment hue
    hue += speed;
    if (hue >= 360.0f) {
        hue -= 360.0f;
    }
}


//void ISR_Button()
//{
//  buttonFlag = 1;
//}


void rouletteWipe(uint8_t wait, uint8_t start, uint8_t end){
  uint8_t pixel;
  for(pixel = start; pixel < end; pixel++){
    strip.setPixelColor(pixel,strip.Color(255, 255, 255));
    strip.show();
    delay(wait);
    strip.setPixelColor(pixel,strip.Color(0, 0, 0));
    strip.show();
  }
}

void ringFlash(uint8_t r, uint8_t g, uint8_t b){
  uint8_t i;
  for (i = 0; i<NUMPIXELS; i++ ){
    strip.setPixelColor(i,strip.Color(r, g, b));
  }
  strip.show();
}

//Theatre-style crawling lights.
void theaterChase2(uint32_t c, uint8_t wait) {
  for (int j=0; j<1; j++) {  //do 1 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
