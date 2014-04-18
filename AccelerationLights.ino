#include <Wire.h>              // for the accelerometer
#include <Adafruit_LSM303.h>   // for the 
#include <Adafruit_NeoPixel.h> // for the Neo Pixel strip

Adafruit_LSM303 lsm;     // something for the LSM
Adafruit_LSM303 compass; // something for the LSM

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
#define PIN 6
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// GLOBAL VARIABLES

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// ACCEL DATA NORMALIZATIONS
// Maximum and minimum accelrations recorded by the LSM303. 
// These values may be specific to your device but can be read off from the serial monitor using the LSM303 Test sketch. 
float AXMin = -1136;
float AXMax = 950;
float AYMin = -1032;
float AYMax = 1032;
float AZMin = -775;
float AZMax = 1210;

// These are parameters that will be used to normalize meaured 7accelerations
float AXMean = (AXMax + AXMin)/2;
float AYMean = (AYMax + AYMin)/2;
float AZMean = (AZMax + AZMin)/2;
float AXLeng = (AXMax - AXMin)/2;
float AYLeng = (AYMax - AYMin)/2;
float AZLeng = (AZMax - AZMin)/2;

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// MAG DATA NORMALIZATIONS

// Maximum and minimum magnometer data recorded by the LSM303. 
// These values can be read off from the serial monitor using the LSM303 Test sketch. 
float XMMin = -150;
float XMMax = 350;
float YMMin = -175;
float YMMax = 280;

// These are parameters that will be used to normalize meaured mag data
float XMMean = (XMMax + XMMin)/2;
float YMMean = (YMMax + YMMin)/2;
float XMLeng = (XMMax - XMMin)/2;
float YMLeng = (YMMax - YMMin)/2;

// Compass directions (specific to orientation of Flora in my project)
float NEast = -0.70;
float NWest = -2.30;
float SEast = +0.60;
float SWest = +2.50;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Acceleration Thresholds
float AHigh = 1.3;  // Acceleration threshold for activating LSpeed lights
// Number of pixels in neo pixel strip (some have more than 30)
int NPixels = 30;
// Heading
int HN;
// Accel
float AN;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPixels, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  // STRIP SETUP
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // LSM SETUP
  Serial.begin(9600);
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  } 
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void loop() {

  // Get Heading and Normalized Acceleration
  HN = Heading();    // Heading Number
  AN = Acceleration(); 
  Serial.print("Heading number: "); 
  Serial.print(HN);
  Serial.print('\n');

  // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
  if (AN > AHigh) {
    // Ludicrous Speed mode
    Serial.print("LSpeed: ");
    LudicrousSpeed(8000);  
    delay(5000);    
  }
  else {
    // NORTH
    if (HN == 1)  {
      Serial.print("n");  
      Tron(300); // Tron mode      
    }
    // EAST
    else if (HN == 2) {
      Serial.print("e");  
      BTTF(60);       
    }    
    // SOUTH
    else if (HN == 3) {
      Serial.print("s");  
      Ecto(100, 80);
    }     
    // WEST
    else if (HN == 4) {
      Serial.print("w");  
      NR(80, NPixels); // NRider mode
    }        
    else {    
      Serial.print("Oh no, no HN. Check Heading call.");  
    }
  }
  Serial.print('\n');
  lsm.read();
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HEADING AND ACCELERATION FUNCTIONS
float Acceleration() {   // Measure acceleration
  lsm.read();
  float XN = (lsm.accelData.x - AXMean)/AXLeng; // normalized x-component of acceleration
  float YN = (lsm.accelData.y - AYMean)/AYLeng; // normalized y-component of acceleration
  float ZN = (lsm.accelData.z - AZMean)/AZLeng; // normalized z-component of acceleration
  float AN = sqrt(pow(XN,2)+pow(YN,2)+pow(ZN,2));  // normalized magnitude of acceleration vector, should be close to 1 if chip is stationary
  return AN;
}
float Heading() {
  lsm.read();
  float XMN = (lsm.magData.x - XMMean)/XMLeng; // normalized x-component of mag
  float YMN = (lsm.magData.y - YMMean)/YMLeng; // normalized z-component of mag
  float H = atan2(YMN,XMN);                    // compass heading    
  int M;
  if ((H > NWest) && (H <= NEast))  { // NORTH
    Serial.print("N");  
    M=1;
  }
  else if ((H > NEast) && (H <= SEast)) { // EAST
    Serial.print("E");  
    M=2;
  }
  else if ((H > SEast) && (H <= SWest)) {  // SOUTH
    Serial.print("S"); 
    M=3; 
  }
  else if ((H > SWest) || (H <= NWest)) { // WEST
    Serial.print("W");  
    M=4;
  }
  return M;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// COLOUR MODE FUNCTIONS
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void LudicrousSpeed(uint8_t wait) {
  uint32_t L,P;
  uint32_t R = strip.Color(255,   0, 0);
  uint32_t O = strip.Color(255, 64, 0);
  uint32_t B = strip.Color(  0,   0, 0);
  byte M = NPixels;
  for(L=0; L<=NPixels/6; L++) {
    P = (5*L + 0) % M; 
    strip.setPixelColor(P, B); 
    strip.show(); 
    delay(80);
    P = (5*L + 1) % M; 
    strip.setPixelColor(P, O); 
    strip.show(); 
    delay(80);
    P = (5*L + 2) % M; 
    strip.setPixelColor(P, B); 
    strip.show(); 
    delay(80);
    P = (5*L + 3) % M; 
    strip.setPixelColor(P, R); 
    strip.show(); 
    delay(80);
    P = (5*L + 4) % M; 
    strip.setPixelColor(P, R); 
    strip.show(); 
    delay(80);     
  }
  delay(wait);
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void BTTF(uint8_t wait) {
  uint32_t L,P;
  uint32_t Y = strip.Color(255,255, 0);
  uint32_t O = strip.Color(255, 64, 0);
  uint32_t G = strip.Color(  0,255, 0);
  byte M = NPixels;
  for(L=0; L<=NPixels/6; L++) {
    P = (5*L + 0) % M; 
    strip.setPixelColor(P, Y); 
    strip.show(); 
    delay(wait);
    P = (5*L + 1) % M; 
    strip.setPixelColor(P, O); 
    strip.show(); 
    delay(wait);
    P = (5*L + 2) % M; 
    strip.setPixelColor(P, G); 
    strip.show(); 
    delay(wait);
    P = (5*L + 3) % M; 
    strip.setPixelColor(P, Y); 
    strip.show(); 
    delay(wait);
    P = (5*L + 4) % M; 
    strip.setPixelColor(P, O); 
    strip.show(); 
    delay(wait);     
  }
  delay(wait);
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void Ecto(uint8_t wait, uint8_t Time) {
  uint32_t l,t,c;
  uint32_t RedFull = strip.Color(255, 0, 0);
  // Loop over time
  for(t=1; t<Time; t++) {
    // Loop over pixels
    for(l=0; l<NPixels-1; l++) {
      strip.setPixelColor(l, (20*t + strip.Color(0, 0, 20*l)) % 255);
    }
    // Some lights constant red
    strip.setPixelColor(8, RedFull);
    strip.setPixelColor(14, RedFull);
    strip.setPixelColor(15, RedFull);    
    strip.setPixelColor(29, RedFull);    
    // Show current light
    strip.show();
    delay(20);  
  }
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void NR(uint8_t wait, int NPixels) {

  uint32_t RedFull = strip.Color(255, 0, 0);
  uint32_t RedSoft = strip.Color(128, 0, 0);
  uint32_t RedDark = strip.Color(16, 0, 0);
  //  uint32_t Green = strip.Color(0, 128, 0); // only for testing
  uint32_t Black = strip.Color(0, 0, 0);

  // Beginning of strip step 1
  strip.setPixelColor(0, RedFull);  
  strip.show();
  delay(wait);

  // Beginning of strip step 2
  strip.setPixelColor(0,RedSoft);    
  strip.setPixelColor(1,RedFull);     
  strip.show();  
  delay(wait);

  for(uint16_t i=0; i<(NPixels-3); i++) {
    strip.setPixelColor(i+0, Black);
    strip.setPixelColor(i+1, RedDark);
    strip.setPixelColor(i+2, RedSoft);    
    strip.setPixelColor(i+3, RedFull);     
    strip.show();    
    delay(wait);
  }

  // End of strip two on
  strip.setPixelColor(NPixels-1,Black);      
  strip.setPixelColor(NPixels-2,RedFull);    
  strip.setPixelColor(NPixels,RedSoft);     
  strip.show();    
  delay(wait);  

  for(uint16_t j=NPixels-4; j>0; j--) {
    strip.setPixelColor(j+3, Black);
    strip.setPixelColor(j+2, RedDark);
    strip.setPixelColor(j+1, RedSoft);    
    strip.setPixelColor(j+0, RedFull);     
    strip.show();      
    delay(wait);
  }  

  // Beginning of strip 
  strip.setPixelColor(0,RedFull);    
  strip.setPixelColor(1,RedSoft);     
  strip.setPixelColor(2,RedDark);     
  strip.setPixelColor(3,Black);     
  strip.show();    
  delay(wait);

  // Beginning of strip 
  strip.setPixelColor(2,Black);  
  strip.setPixelColor(1,RedSoft);    
  strip.setPixelColor(0,RedSoft);     
  strip.show();  

}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Fill the dots one after the other with a bright turquoise color
void Tron(uint8_t wait) {
  uint32_t B = strip.Color(0, 128, 255);
  uint32_t Y = strip.Color(255, 255, 0);
  uint32_t P = strip.Color(100,30,140);
  for(uint16_t i=0; i<=NPixels/2; i++) {
    strip.setPixelColor(i, B);
    strip.show();
    delay(wait);
  }
  strip.setPixelColor(NPixels/2+1, P);  
  strip.setPixelColor(NPixels/2+2, P); 
  strip.show();  
  for(uint16_t i=NPixels/2+3; i<NPixels-1; i++) {
    strip.setPixelColor(i, Y);
    strip.show();
    delay(wait);
  }  
  strip.setPixelColor(NPixels-1, P);
  strip.show();
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}



