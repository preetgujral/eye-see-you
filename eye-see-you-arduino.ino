#include <OctoWS2811.h>

// The actual arrangement of the LEDs connected to this Teensy 3.0 board.
// LED_HEIGHT *must* be a multiple of 8.  When 16, 24, 32 are used, each
// strip spans 2, 3, 4 rows.  LED_LAYOUT indicates the direction the strips
// are arranged.  If 0, each strip begins on the left for its first row,
// then goes right to left for its second row, then left to right,
// zig-zagging for each successive row.
#define LED_WIDTH      90   // number of LEDs horizontally
#define LED_HEIGHT     8   // number of LEDs vertically (must be multiple of 8)
#define LED_LAYOUT     0    // 1 = even rows left->right, 1 = even rows right->left

const int ledsPerStrip = LED_WIDTH * LED_HEIGHT / 8;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];
elapsedMicros elapsedUsecSinceLastFrameSync = 0;

const int config = WS2811_GRB | WS2811_800kHz; // color config is on the PC side
//TODO add color config here, since it is not being handled on the PC side

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  pinMode(13, OUTPUT);
  leds.begin();
  leds.show();
}
//brighter colors
#define RED    0xFF0000
#define GREEN  0x00FF00
//#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
//#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

//dimmer colors
//#define RED    0x160000
//#define GREEN  0x001600
#define BLUE   0x000026
//#define YELLOW 0x101400
#define PINK   0x120009
//#define ORANGE 0x100400
//#define WHITE  0x101010

//bool do_animation = true;
bool data[90];
elapsedMillis timeout;
elapsedMillis ledOnTime;

void loop() {
  int c = Serial.read();
  if (c == 100) {
    // 100 means new data arriving
    for (int i = 0; i < 90; i++) {
      data[i] = false;
    }
  } else if (c >= 1 && c <= 90) {
    // 1 to 90 are position of motion detected
    data[c - 1] = true;
    timeout = 0;
    digitalWrite(13, HIGH);
    ledOnTime = 0;
  } else if (c == 102) {
    // 102 means end of data
    //do_animation = false;
    for (int i = 0; i < 90; i++) {
      if (data[i]) {
        leds.setPixel(i + 0, WHITE);
        leds.setPixel(i + 90, WHITE);
        leds.setPixel(i + 180, WHITE);
        leds.setPixel(i + 270, WHITE);
      } else {
        leds.setPixel(i + 0, BLUE);
        leds.setPixel(i + 90, BLUE);
        leds.setPixel(i + 180, BLUE);
        leds.setPixel(i + 270, BLUE);
      }
    }
    leds.show();
  } else if (c == 101) {
    //101 means no new motion data
    //do_animation = true;
  }

  if (timeout > 5000) {
    //colorWipe(3000);
    twinkleRandom(5, YELLOW, PINK, 320000);
  }
  if (ledOnTime > 25) {
    digitalWrite(13, LOW);
  }
}

uint32_t colorlist[] = {RED, GREEN, BLUE, YELLOW, PINK, ORANGE, WHITE};

void twinkleRandom(int num, uint32_t fg_color, uint32_t bg_color, unsigned int wait)
{
  static elapsedMicros usec;

  if (usec < wait) return;
  usec = 0;
  for (int i=0; i < 360; i++) {
    leds.setPixel(i, bg_color);
  }
  for (int i=0; i < num; i++) {
    leds.setPixel(random(90)+0, fg_color);
    leds.setPixel(random(90)+90, fg_color);
    leds.setPixel(random(90)+180, fg_color);
    leds.setPixel(random(90)+270, fg_color);
  }
  leds.show();
}

void colorWipe(unsigned int wait)
{
  static elapsedMicros usec;
  static int color = 0;
  static int position = 0;

  if (usec < wait) return;
  //Serial.println("test");
  leds.setPixel(position, colorlist[color]);
  leds.show();
  position++;
  if (position >= leds.numPixels()) {
    position = 0;
    color++;
    if (color >= 7) {
      color = 0;
    }
  }
}
