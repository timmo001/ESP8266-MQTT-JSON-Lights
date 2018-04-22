#ifndef _NEOPIXEL_EFFECTS
#define _NEOPIXEL_EFFECTS

///**************************** START EFFECTS *****************************************/
// Effects from: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/


bool shouldAbortEffect() {
  yield(); // Watchdog timer
  client.loop(); // Update from MQTT
  return transitionAbort;
}

void showStrip() {
  if (!stateOn) { return; }

  strip.show();
}

void setPixel(int pixel, byte r, byte g, byte b, bool applyBrightness) {
  if (!stateOn) { return; }

  if (applyBrightness) {
    r = map(r, 0, 255, 0, brightness);
    g = map(g, 0, 255, 0, brightness);
    b = map(b, 0, 255, 0, brightness);
  }

   strip.setPixelColor(pixel, strip.Color(r, g, b));
}

void setAll(byte r, byte g, byte b, bool refreshStrip = true) {
  if (!stateOn) { return; }
  
  for (int i = 0; i < LED_COUNT; i++) {
    setPixel(i, r, g, b, false);
  }

  if (refreshStrip) {
    showStrip();

    //Serial.print("Setting LEDs - ");
    //Serial.print("r: ");
    //Serial.print(r);
    //Serial.print(", g: ");
    //Serial.print(g);
    //Serial.print(", b: ");
    //Serial.print(b);
    //Serial.print(", w: ");
    //Serial.println(w);
  }
}

// Twinkle(10, 100, false);
void twinkle(int Count, int speed, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i=0; i<Count; i++) {
    if (shouldAbortEffect()) { return; }
    setPixel(random(LED_COUNT), red, green, blue, false);
    showStrip();
    delay(speed);
    if(OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(speed);
}

// CylonBounce(4, 10, 50);
void cylonBounce(int EyeSize, int speed, int returnDelay){

  for(int i = 0; i < (LED_COUNT-EyeSize-2); i++) {
    if (shouldAbortEffect()) { return; }
    setAll(0, 0, 0, false);
    setPixel(i, red/10, green/10, blue/10, false);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue, false);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10, false);
    showStrip();
    delay(speed);
  }

  delay(returnDelay);

  for(int i = (LED_COUNT-EyeSize-2); i > 0; i--) {
    if (shouldAbortEffect()) { return; }
    setAll(0, 0, 0, false);
    setPixel(i, red/10, green/10, blue/10, false);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue, false);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10, false);
    showStrip();
    delay(speed);
  }

  delay(returnDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp, true);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0, true);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0, true);
  }
}
// Fire(55,120,15);
void fire(int Cooling, int Sparking, int speed) {
  byte heat[LED_COUNT];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < LED_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / LED_COUNT) + 2);

    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k = LED_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < LED_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(speed);
}

// FadeInOut();
void fadeInOut(int speed){
  float r, g, b;

  for(int k = 0; k < 256; k = k + 1) {
    if (shouldAbortEffect()) { return; }
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }

  for(int k = 255; k >= 0; k = k - 2) {
    if (shouldAbortEffect()) { return; }
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
}

// Slower:
// strobe(10, 100);
// Fast:
// strobe(10, 50);
void strobe(int strobeCount, int FlashDelay){
  for(int j = 0; j < strobeCount; j++) {
    if (shouldAbortEffect()) { return; }
    setAll(red, green, blue);
    showStrip();
    delay(FlashDelay);
    setAll(0, 0, 0);
    showStrip();
    delay(FlashDelay);
  }
}

// theaterChase(50);
void theaterChase(int speed) {
  for (int q=0; q < 3; q++) {
    if (shouldAbortEffect()) { return; }
    for (int i=0; i < LED_COUNT; i=i+3) {
      setPixel(i+q, red, green, blue, false);    //turn every third pixel on
    }
    showStrip();

    delay(speed);

    for (int i=0; i < LED_COUNT; i=i+3) {
      setPixel(i + q, 0, 0, 0, false);        //turn every third pixel off
    }
  }
}

byte * wheel(byte wheelPos) {
  static byte c[3];

  if(wheelPos < 85) {
   c[0]=wheelPos * 3;
   c[1]=255 - wheelPos * 3;
   c[2]=0;
  } else if(wheelPos < 170) {
   wheelPos -= 85;
   c[0]=255 - wheelPos * 3;
   c[1]=0;
   c[2]=wheelPos * 3;
  } else {
   wheelPos -= 170;
   c[0]=0;
   c[1]=wheelPos * 3;
   c[2]=255 - wheelPos * 3;
  }

  return c;
}

byte rainbow(byte *c, uint16_t i, uint16_t j) {
  for (i = 0; i < LED_COUNT; i++) {
    c = wheel(((i * 256 / LED_COUNT) + j) & 255);
    setPixel(i, *c, *(c+1), *(c+2), true);
  }
  return *c;
}

//  rainbowCycle(20);
void rainbowCycle(int speed) {
  byte *c;
  uint16_t i, j;
  for(j = 0; j < 256 * 2; j++) { // 2 cycles of all colors on wheel
    if (shouldAbortEffect()) { return; }
    rainbow(c, i, j);
    showStrip();
    delay(speed);
  }
}

//  colorWipe(50);
void colorWipe(int speed) {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    if (shouldAbortEffect()) { return; }
    setPixel(i, red, green, blue, false);
    showStrip();
    delay(speed);
  }
  transitionDone = true;
}

//  colorWipeOnce(50);
void colorWipeOnce(int speed) {
  colorWipe(speed);

  // Reset back to previous color
  red = previousRed;
  green = previousGreen;
  blue = previousBlue;

  colorWipe(speed);
}

//  runningLights(50);
void runningLights(int WaveDelay) {
  int Position=0;

  for(int i=0; i<LED_COUNT; i++)
  {
    if (shouldAbortEffect()) { return; }
    Position++; // = 0; //Position + Rate;
    for(int i=0; i<LED_COUNT; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0,false);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue,
				   false);
    }

    showStrip();
    delay(WaveDelay);
  }
}

//  snowSparkle(20, random(100,1000));
void snowSparkle(int SparkleDelay, int speed) {
  setAll(red, green, blue);

  int Pixel = random(LED_COUNT);
  setPixel(Pixel, 255, 255, 255,false);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel, red, green, blue, false);
  showStrip();
  delay(speed);
}

//  sparkle(0);
void sparkle(int speed) {
  setAll(0, 0, 0);
  int Pixel = random(LED_COUNT);
  setPixel(Pixel, red, green, blue, false);
  showStrip();
  delay(speed);
  setPixel(Pixel, 0, 0, 0, false);
}

//  twinkleRandom(20, 100, false);
void twinkleRandom(int Count, int speed, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i=0; i<Count; i++) {
    if (shouldAbortEffect()) { return; }
    setPixel(random(LED_COUNT), random(0,255), random(0,255), random(0,255), true);
    showStrip();
    delay(speed);
    if(OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(speed);
}


// bouncingBalls(3);
void bouncingBalls(int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    if (shouldAbortEffect()) { return; }
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (LED_COUNT - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], red, green, blue, false);
    }

    showStrip();
    setAll(0, 0, 0);
  }
}


/**************************** START TRANSITION FADER *****************************************/
// From https://www.arduino.cc/en/Tutorial/ColorCrossfader
/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
  The program works like this:
  Imagine a crossfade that moves the red LED from 0-10,
    the green from 0-5, and the blue from 10 to 7, in
    ten steps.
    We'd want to count the 10 steps and increase or
    decrease color values in evenly stepped increments.
    Imagine a + indicates raising a value by 1, and a -
    equals lowering it. Our 10 step fade would look like:
    1 2 3 4 5 6 7 8 9 10
  R + + + + + + + + + +
  G   +   +   +   +   +
  B     -     -     -
  The red rises from 0 to 10 in ten steps, the green from
  0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
  In the real program, the color percentages are converted to
  0-255 values, and there are 1020 steps (255*4).
  To figure out how big a step there should be between one up- or
  down-tick of one of the LED values, we call calculateStep(),
  which calculates the absolute gap between the start and end values,
  and then divides that gap by 1020 to determine the size of the step
  between adjustments in the value.
*/
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue;  // What's the overall gap?
  if (step) {                       // If its non-zero,
    step = 1020/step;              //   divide by 1020
  }

  return step;
}
/* The next function is calculateVal. When the loop value, i,
   reaches the step size appropriate for one of the
   colors, it increases or decreases the value of that color by 1.
   (R, G, and B are each calculated separately.)
*/
int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    }
  }

  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  }
  else if (val < 0) {
    val = 0;
  }

  return val;
}
// fade(50);
void fade(int speed){
  int redVal = previousRed;
  int grnVal = previousGreen;
  int bluVal = previousBlue;
  int stepR = calculateStep(redVal, red);
  int stepG = calculateStep(grnVal, green);
  int stepB = calculateStep(bluVal, blue);

  // If no change then exit
  if (stepR == 0 && stepG == 0 && stepB == 0){ return; }

  for (int i=0; i<1020; i++) {
    if (shouldAbortEffect()) { return; }

    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    if (i % 50 == 0) {
      setAll(redVal, grnVal, bluVal); // Write current values to LED pins
      delay(speed);
    }
  }

  setAll(redVal, grnVal, bluVal); // Write current values to LED pins
  transitionDone = true;
}

void lightning(int speed){
  setAll(0, 0, 0);
  int ledstart = random(LED_COUNT);           // Determine starting location of flash
  int ledlen = random(LED_COUNT - ledstart);  // Determine length of flash (not to go beyond LED_COUNT-1)
  for (int flashCounter = 0; flashCounter < random(1, 4); flashCounter++) {
    int dimmer = random(10, brightness);          // return strokes are brighter than the leader
    int rr = map(red, 0, 255, 0, dimmer);
    int gg = map(green, 0, 255, 0, dimmer);
    int bb = map(blue, 0, 255, 0, dimmer);

    for (int i = ledstart ; i < (ledstart + ledlen) ; i++) {
      setPixel(i, rr, gg, bb, false);
    }
    showStrip();    // Show a section of LED's
    delay(random(4, 15));                // each flash only lasts 4-10 milliseconds
    for (int i = ledstart ; i < (ledstart + ledlen) ; i++) {
      setPixel(i, 0, 0, 0, false);
    }
    showStrip();
    //if (flashCounter == 0) delay (130);   // longer delay until next flash after the leader
    delay(50 + random(100));             // shorter delay between strokes
  }
  delay(random(speed) * 50);        // delay between strikes
}

#endif
///**************************** END EFFECTS *****************************************/


//
///**************************** START STRIPLED PALETTE *****************************************/
//void setupStripedPalette( CRGB A, CRGB AB, CRGB B, CRGB BA) {
//  currentPalettestriped = CRGBPalette16(
//                            A, A, A, A, A, A, A, A, B, B, B, B, B, B, B, B
//                            //    A, A, A, A, A, A, A, A, B, B, B, B, B, B, B, B
//                          );
//}
//
//
//
///********************************** START FADE************************************************/
//void fadeall() {
//  for (int i = 0; i < LED_COUNT; i++) {
//    leds[i].nscale8(250);  //for CYCLon
//  }
//}
//
//
//
///********************************** START FIRE **********************************************/
//void Fire2012WithPalette()
//{
//  // Array of temperature readings at each simulation cell
//  static byte heat[LED_COUNT];
//
//  // Step 1.  Cool down every cell a little
//  for ( int i = 0; i < LED_COUNT; i++) {
//    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / LED_COUNT) + 2));
//  }
//
//  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
//  for ( int k = LED_COUNT - 1; k >= 2; k--) {
//    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
//  }
//
//  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
//  if ( random8() < SPARKING ) {
//    int y = random8(7);
//    heat[y] = qadd8( heat[y], random8(160, 255) );
//  }
//
//  // Step 4.  Map from heat cells to LED colors
//  for ( int j = 0; j < LED_COUNT; j++) {
//    // Scale the heat value from 0-255 down to 0-240
//    // for best results with color palettes.
//    byte colorindex = scale8( heat[j], 240);
//    CRGB color = ColorFromPalette( gPal, colorindex);
//    int pixelnumber;
//    if ( gReverseDirection ) {
//      pixelnumber = (LED_COUNT - 1) - j;
//    } else {
//      pixelnumber = j;
//    }
//    leds[pixelnumber] = color;
//  }
//}
//
//
//
///********************************** START ADD GLITTER *********************************************/
//void addGlitter( fract8 chanceOfGlitter)
//{
//  if ( random8() < chanceOfGlitter) {
//    leds[ random16(LED_COUNT) ] += CRGB::White;
//  }
//}
//
//
//
///********************************** START ADD GLITTER COLOR ****************************************/
//void addGlitterColor( fract8 chanceOfGlitter, int red, int green, int blue)
//{
//  if ( random8() < chanceOfGlitter) {
//    leds[ random16(LED_COUNT) ] += CRGB(red, green, blue);
//  }
//}
//


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


//
///********************************** GLOBALS for EFFECTS ******************************/
////RAINBOW
//uint8_t thishue = 0;                                          // Starting hue value.
//uint8_t deltahue = 10;
//
////CANDYCANE
//CRGBPalette16 currentPalettestriped; //for Candy Cane
//CRGBPalette16 gPal; //for fire
//
////NOISE
//static uint16_t dist;         // A random number for our noise generator.
//uint16_t scale = 30;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
//uint8_t maxChanges = 48;      // Value for blending between palettes.
//CRGBPalette16 targetPalette(OceanColors_p);
//CRGBPalette16 currentPalette(CRGB::Black);
//
////TWINKLE
//#define DENSITY     80
//int twinklecounter = 0;
//
////RIPPLE
//uint8_t colour;                                               // Ripple colour is randomized.
//int center = 0;                                               // Center of the current ripple.
//int step = -1;                                                // -1 is the initializing step.
//uint8_t myfade = 255;                                         // Starting brightness.
//#define maxsteps 16                                           // Case statement wouldn't allow a variable.
//uint8_t bgcol = 0;                                            // Background colour rotates.
//int thisdelay = 20;                                           // Standard delay value.
//
////DOTS
//uint8_t   count =   0;                                        // Count up to 255 and then reverts to 0
//uint8_t fadeval = 224;                                        // Trail behind the LED's. Lower => faster fade.
//uint8_t bpm = 30;
//
////LIGHTNING
//uint8_t frequency = 50;                                       // controls the interval between strikes
//uint8_t flashes = 8;                                          //the upper limit of flashes per strike
//unsigned int dimmer = 1;
//uint8_t ledstart;                                             // Starting location of a flash
//uint8_t ledlen;
//int lightningcounter = 0;
//
////FUNKBOX
//int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
//int TOP_INDEX = int(LED_COUNT / 2);
//int thissat = 255;           //-FX LOOPS DELAY VAR
//uint8_t thishuepolice = 0;
//int antipodal_index(int i) {
//  int iN = i + TOP_INDEX;
//  if (i >= TOP_INDEX) {
//    iN = ( i + TOP_INDEX ) % LED_COUNT;
//  }
//  return iN;
//}
//
////FIRE
//#define COOLING  55
//#define SPARKING 120
//bool gReverseDirection = false;
//
////BPM
//uint8_t gHue = 0;

//  //EFFECT BPM
//  if (effect == "bpm") {
//    uint8_t BeatsPerMinute = 62;
//    CRGBPalette16 palette = PartyColors_p;
//    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//    for ( int i = 0; i < LED_COUNT; i++) { //9948
//      leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
//    }
//    if (transition == 0 or transition == NULL) {
//      transition = 30;
//    }
//    showleds();
//  }
//
//
//  //EFFECT Candy Cane
//  if (effect == "candy cane") {
//    static uint8_t startIndex = 0;
//    startIndex = startIndex + 1; /* higher = faster motion */
//    fill_palette( ledsRGB, LED_COUNT,
//                  startIndex, 16, /* higher = narrower stripes */
//                  currentPalettestriped, 255, LINEARBLEND);
//    if (transition == 0 or transition == NULL) {
//      transition = 0;
//    }
//    showleds();
//  }
//
//
////  //EFFECT CONFETTI
////  if (effect == "confetti" ) {
////    fadeToBlackBy( ledsRGB, LED_COUNT, 25);
////    int pos = random16(LED_COUNT);
////    leds[pos] += CRGBW(red + random8(64), green, blue, 0);
////    if (transition == 0 or transition == NULL) {
////      transition = 30;
////    }
////    showleds();
////  }
//
//
//  //EFFECT CYCLON RAINBOW
//  if (effect == "cyclon rainbow") {                    //Single Dot Down
//    static uint8_t hue = 0;
//    // First slide the led in one direction
//    for (int i = 0; i < LED_COUNT; i++) {
//      // Set the i'th led to red
//      leds[i] = CHSV(hue++, 255, 255);
//      // Show the leds
//      showleds();
//      // now that we've shown the leds, reset the i'th led to black
//      // leds[i] = CRGB::Black;
//      fadeall();
//      // Wait a little bit before we loop around and do it again
//      delay(10);
//    }
//    for (int i = (LED_COUNT) - 1; i >= 0; i--) {
//      // Set the i'th led to red
//      leds[i] = CHSV(hue++, 255, 255);
//      // Show the leds
//      showleds();
//      // now that we've shown the leds, reset the i'th led to black
//      // leds[i] = CRGB::Black;
//      fadeall();
//      // Wait a little bit before we loop around and do it again
//      delay(10);
//    }
//  }
//
//
//  //EFFECT DOTS
//  if (effect == "dots") {
//    uint8_t inner = beatsin8(bpm, LED_COUNT / 4, LED_COUNT / 4 * 3);
//    uint8_t outer = beatsin8(bpm, 0, LED_COUNT - 1);
//    uint8_t middle = beatsin8(bpm, LED_COUNT / 3, LED_COUNT / 3 * 2);
//    leds[middle] = CRGB::Purple;
//    leds[inner] = CRGB::Blue;
//    leds[outer] = CRGB::Aqua;
//    nscale8(ledsRGB, LED_COUNT, fadeval);
//
//    if (transition == 0 or transition == NULL) {
//      transition = 30;
//    }
//    showleds();
//  }
//
//
//  //EFFECT FIRE
//  if (effect == "fire") {
//    Fire2012WithPalette();
//    if (transition == 0 or transition == NULL) {
//      transition = 150;
//    }
//    showleds();
//  }
//
//  random16_add_entropy( random8());
//
//
//  //EFFECT Glitter
//  if (effect == "glitter") {
//    fadeToBlackBy( ledsRGB, LED_COUNT, 20);
//    addGlitterColor(80, red, green, blue);
//    if (transition == 0 or transition == NULL) {
//      transition = 30;
//    }
//    showleds();
//  }
//
//
//  //EFFECT JUGGLE
//  if (effect == "juggle" ) {                           // eight colored dots, weaving in and out of sync with each other
//    fadeToBlackBy(ledsRGB, LED_COUNT, 20);
//    for (int i = 0; i < 8; i++) {
//      ledsRGB[beatsin16(i + 7, 0, LED_COUNT - 1  )] |= CRGB(red, green, blue);
//    }
//    if (transition == 0 or transition == NULL) {
//      transition = 130;
//    }
//    showleds();
//  }
//
//
//  //EFFECT LIGHTNING
//  if (effect == "lightning") {
//    twinklecounter = twinklecounter + 1;                     //Resets strip if previous animation was running
//    if (twinklecounter < 2) {
//      FastLED.clear();
//      FastLED.show();
//    }
//    ledstart = random8(LED_COUNT);           // Determine starting location of flash
//    ledlen = random8(LED_COUNT - ledstart);  // Determine length of flash (not to go beyond LED_COUNT-1)
//    for (int flashCounter = 0; flashCounter < random8(3, flashes); flashCounter++) {
//      if (flashCounter == 0) dimmer = 5;    // the brightness of the leader is scaled down by a factor of 5
//      else dimmer = random8(1, 3);          // return strokes are brighter than the leader
//      fill_solid(ledsRGB + ledstart, ledlen, CHSV(255, 0, 255 / dimmer));
//      showleds();    // Show a section of LED's
//      delay(random8(4, 10));                // each flash only lasts 4-10 milliseconds
//      fill_solid(ledsRGB + ledstart, ledlen, CHSV(255, 0, 0)); // Clear the section of LED's
//      showleds();
//      if (flashCounter == 0) delay (130);   // longer delay until next flash after the leader
//      delay(50 + random8(100));             // shorter delay between strokes
//    }
//    delay(random8(frequency) * 100);        // delay between strikes
//    if (transition == 0 or transition == NULL) {
//      transition = 0;
//    }
//    showleds();
//  }
//
//
//  //EFFECT POLICE ALL
//  if (effect == "police all") {                 //POLICE LIGHTS (TWO COLOR SOLID)
//    idex++;
//    if (idex >= LED_COUNT) {
//      idex = 0;
//    }
//    int idexR = idex;
//    int idexB = antipodal_index(idexR);
//    int thathue = (thishuepolice + 160) % 255;
//    leds[idexR] = CHSV(thishuepolice, thissat, 255);
//    leds[idexB] = CHSV(thathue, thissat, 255);
//    if (transition == 0 or transition == NULL) {
//      transition = 30;
//    }
//    showleds();
//  }
//
//  //EFFECT POLICE ONE
//  if (effect == "police one") {
//    idex++;
//    if (idex >= LED_COUNT) {
//      idex = 0;
//    }
//    int idexR = idex;
//    int idexB = antipodal_index(idexR);
//    int thathue = (thishuepolice + 160) % 255;
//    for (int i = 0; i < LED_COUNT; i++ ) {
//      if (i == idexR) {
//        leds[i] = CHSV(thishuepolice, thissat, 255);
//      }
//      else if (i == idexB) {
//        leds[i] = CHSV(thathue, thissat, 255);
//      }
//      else {
//        leds[i] = CHSV(0, 0, 0);
//      }
//    }
//    if (transition == 0 or transition == NULL) {
//      transition = 30;
//    }
//    showleds();
//  }
//
//
//  //EFFECT RAINBOW
//  if (effect == "rainbow") {
//    // FastLED's built-in rainbow generator
//    static uint8_t starthue = 0;    thishue++;
//    fill_rainbow(ledsRGB, LED_COUNT, thishue, deltahue);
//    if (transition == 0 or transition == NULL) {
//      transition = 130;
//    }
//    showleds();
//  }
//
//
//  //EFFECT RAINBOW WITH GLITTER
//  if (effect == "rainbow with glitter") {               // FastLED's built-in rainbow generator with Glitter
//    static uint8_t starthue = 0;
//    thishue++;
//    fill_rainbow(ledsRGB, LED_COUNT, thishue, deltahue);
//    addGlitter(80);
//    if (transition == 0 or transition == NULL) {
//      transition = 130;
//    }
//    showleds();
//  }
//
//
//  //EFFECT SIENLON
//  if (effect == "sinelon") {
//    fadeToBlackBy( ledsRGB, LED_COUNT, 20);
//    int pos = beatsin16(13, 0, LED_COUNT - 1);
//    leds[pos] += CRGB(red, green, blue);
//    if (transition == 0 or transition == NULL) {
//      transition = 150;
//    }
//    showleds();
//  }
//
//

//
//void Twinkle2() {
//    twinklecounter = twinklecounter + 1;
//    if (twinklecounter < 2) {                               //Resets strip if previous animation was running
//      FastLED.clear();
//      FastLED.show();
//    }
//    const CRGBW lightcolor(8, 7, 1, 0);
//    for ( int i = 0; i < LED_COUNT; i++) {
//      if ( !leds[i]) continue; // skip black pixels
//      if ( leds[i].r & 1) { // is red odd?
//        leds[i] -= lightcolor; // darken if red is odd
//      } else {
//        leds[i] += lightcolor; // brighten if red is even
//      }
//    }
//    if ( random8() < DENSITY) {
//      int j = random16(LED_COUNT);
//      if ( !leds[j] ) leds[j] = lightcolor;
//    }
//
//    if (transition == 0 or transition == NULL) {
//      transition = 0;
//    }
//    showleds();
//  }
//
//
//  EVERY_N_MILLISECONDS(10) {
//
//    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // FOR NOISE ANIMATIon
//    {
//      gHue++;
//    }
//
//    //EFFECT NOISE
//    if (effect == "noise") {
//      for (int i = 0; i < LED_COUNT; i++) {                                     // Just onE loop to fill up the LED array as all of the pixels change.
//        uint8_t index = inoise8(i * scale, dist + i * scale) % 255;            // Get a value from the noise function. I'm using both x and y axis.
//        leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
//      }
//      dist += beatsin8(10, 1, 4);                                              // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
//      // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
//      if (transition == 0 or transition == NULL) {
//        transition = 0;
//      }
//      showleds();
//    }
//
//    //EFFECT RIPPLE
//    if (effect == "ripple") {
//      for (int i = 0; i < LED_COUNT; i++) leds[i] = CHSV(bgcol++, 255, 15);  // Rotate background colour.
//      switch (step) {
//        case -1:                                                          // Initialize ripple variables.
//          center = random(LED_COUNT);
//          colour = random8();
//          step = 0;
//          break;
//        case 0:
//          leds[center] = CHSV(colour, 255, 255);                          // Display the first pixel of the ripple.
//          step ++;
//          break;
//        case maxsteps:                                                    // At the end of the ripples.
//          step = -1;
//          break;
//        default:                                                             // Middle of the ripples.
//          leds[(center + step + LED_COUNT) % LED_COUNT] += CHSV(colour, 255, myfade / step * 2);   // Simple wrap from Marc Miller
//          leds[(center - step + LED_COUNT) % LED_COUNT] += CHSV(colour, 255, myfade / step * 2);
//          step ++;                                                         // Next step.
//          break;
//      }
//      if (transition == 0 or transition == NULL) {
//        transition = 30;
//      }
//      showleds();
//    }
//
