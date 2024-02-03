#include <string>
#include <vector>

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#include "scrolltext.h"

#define PIN 16

const char *predefinedEntries[] = {
  "S8 faellt aus. Grund: Zu kalt.",
  "RB75 heute 2900 Minuten spaeter",
  "Entry 3",
  "Entry 4",
  "Entry 5",
  "Entry 6",
  "Entry 7",
  "Entry 8",
  "Entry 9",
  "Entry 10"
};

#define NUM_ENTRIES (sizeof(predefinedEntries) / sizeof(predefinedEntries[0]))

#define tile_w (32)
#define tile_h  (8)

#define tiles_num_x (1)
#define tiles_num_y (1)

#define mw (tile_w * tiles_num_x)
#define mh (tile_h * tiles_num_y)
#define NUMMATRIX (mw * mh)

void scrollText(const char *text, int delayTime);

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(
  matrixleds,   // reference to 1-dim array of type CRGB, each element is a LED.
  mw,           // matrix width
  mh,           // matrix height,
  tiles_num_x,  // number of tiles in x direction
  tiles_num_y,  // number of tiles in y direction
  0
    // layout of a single tile
    + NEO_MATRIX_TOP
    + NEO_MATRIX_LEFT
    + NEO_MATRIX_COLUMNS
    + NEO_MATRIX_ZIGZAG

    // layout of the tiles
    + NEO_TILE_TOP
    + NEO_TILE_LEFT
    + NEO_TILE_ROWS
  );

static std::vector<std::string> userInputList;   ///< Vector to store user input
const char *scrollText_text = "";
int16_t scrollText_x = mw;

unsigned long scrollText_lastUpdateTime = 0;
const unsigned long scrollText_updateInterval = 50;
int scrollText_times_to_show = 0;
void scrollText_set(const char *text, int times);
void scrollText_setup(void);
void scrollText_loop(void);

void scrollText_set(const char *text, int times) {
  scrollText_text = text;
  scrollText_times_to_show = times;
}

bool scrollText_finished() {
  return (scrollText_times_to_show == 0);
}

void scrollText_update() {
  if (userInputList.size() > 1)
    userInputList.erase(userInputList.begin());

  const char *text = userInputList.front().c_str();
  scrollText_set(text, 1);
}

void scrollText_setup(void){
  FastLED.addLeds<NEOPIXEL, PIN>(matrixleds, NUMMATRIX);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);

#ifdef CFG_TEST
  matrix->clear();
  matrix->drawPixel(0,       0, CRGB( 255, 0,   0));    // red    links oben
  matrix->drawPixel(mw-1,    0, CRGB( 0,   0,   255));  // blue   rechts oben
  matrix->drawPixel(0,    mh-1, CRGB( 0,   255, 0));    // green  links unten
  matrix->drawPixel(mw-1, mh-1, CRGB( 255, 255, 0));    // yellow rechts unten
  matrix->show();
#endif // CFG_TEST
} // scrollText_setup

void scrollText_loop() {
  if (millis() - scrollText_lastUpdateTime >= scrollText_updateInterval) {

    if (scrollText_finished()) {
      scrollText_update();
    }

    // matrix->setTextColor(matrix->Color(255, 0, 0));      // red
    matrix->setTextColor(matrix->Color(255, 255, 255));  // white

    // Calculate text width to scroll the entire text
    uint16_t textWidth, textHeight;
    int16_t x1, y1;
    matrix->getTextBounds(scrollText_text, (int16_t)0, (int16_t)0, &x1, &y1, &textWidth, &textHeight);

    if (scrollText_x >= -textWidth) {
    } else {
      scrollText_x = mw;
      scrollText_times_to_show -= (scrollText_times_to_show > 0 ? 1 : 0);
    }
    matrix->fillScreen(matrix->Color(0, 0, 0));  // Clear the matrix
    matrix->setCursor(scrollText_x, 0);          // Set text starting position
    matrix->print(scrollText_text);              // Print the text at the current position
    matrix->show();                              // Show the updated matrix

    scrollText_x--;

    scrollText_lastUpdateTime = millis();
  }
} // scrollText_loop
