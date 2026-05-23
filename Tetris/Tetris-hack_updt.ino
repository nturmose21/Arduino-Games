#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include "games_bitmaps.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define BTN_LEFT 9
#define BTN_SELECT 10
#define BTN_RIGHT 11
#define BUZZER_PIN 12


#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define REST     0

// --- MELODY DATA ---

const uint16_t intro_notes[] PROGMEM = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6 };
const uint8_t intro_durations[] PROGMEM = { 16, 16, 16, 16, 16, 16, 4 }; 
const int intro_length = 7;

// 2. Full Classic Tetris Theme (Korobeiniki)
// Durations: 4 = quarter note, 8 = eighth note
const uint16_t theme_notes[] PROGMEM = {
  // Part A
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, REST,
  
  // Part B
  NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, REST
};
const uint8_t theme_durations[] PROGMEM = {
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 4,
  4, 4, 4, 4,
  
  4, 8, 4, 8, 8,
  4, 8, 4, 8, 8,
  4, 8, 8, 4, 4,
  4, 4, 4, 4
};
const int theme_length = 39;


const uint16_t over_notes[] PROGMEM = { NOTE_C5, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4, NOTE_G4 };
const uint8_t over_durations[] PROGMEM = { 4, 4, 4, 8, 8, 8, 8, 8, 8, 2 };
const int over_length = 10;


int tempo = 144;
int wholenote = (60000 * 4) / tempo; 

unsigned long previousNoteTime = 0;
int currentNoteIndex = 0;
int noteDuration = 0;
int pauseBetweenNotes = 0;
bool gameOverMusicPlayed = false;

// --- GAME VARIABLES ---
bool lastLeft = HIGH, lastRight = HIGH, lastSelect = HIGH;
bool leftPressed = false, rightPressed = false;
bool currentLeft = HIGH, currentRight = HIGH, currentSelect = HIGH;

const int GRID_WIDTH = 14;
const int GRID_HEIGHT = 22;
const int BLOCK_SIZE = 3;
const int OFFSET_X = 11;
const int OFFSET_Y = 10;

uint8_t grid[GRID_HEIGHT][GRID_WIDTH];
struct Tetromino { int x, y; int type; int rotation; };

const uint8_t shapes[7][4][4][4] PROGMEM = {
  {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}, {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}}, // I
  {{{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}}, // O
  {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}, {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}}, {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}}, // T
  {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}, {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}}, // S
  {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}, {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}}, // Z
  {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}, {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}}, {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}}, // J
  {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}}, {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}}, {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}}  // L
};

Tetromino current;
int score;
bool gameOver;
unsigned long lastFallTime;
int fallDelay = 500;
unsigned long selectPressStart = 0;
bool rotatedThisPress = false;

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  randomSeed(analogRead(A0));
  
  display.clearDisplay();
  display.drawBitmap(0, 0, epd_bitmap_TETRIS_cover, 128, 64, SSD1306_WHITE);
  display.display();

  // Play Intro Arpeggio
  playMelodyBlocking(intro_notes, intro_durations, intro_length);
  delay(500); 

  initGame();
}

void loop() {
  readButtons();
  handleInput();
  updateGame();
  drawGame();
  
  if (gameOver) {
    if (!gameOverMusicPlayed) {
      noTone(BUZZER_PIN);
      playMelodyBlocking(over_notes, over_durations, over_length);
      gameOverMusicPlayed = true;
    }
  } else {
    updateThemeNonBlocking();
  }
}

// --- MUSIC FUNCTIONS ---


void playMelodyBlocking(const uint16_t* notes, const uint8_t* durations, int length) {
  for (int i = 0; i < length; i++) {
    int noteDivider = pgm_read_byte(&durations[i]);
    int noteDuration = wholenote / noteDivider;
    int note = pgm_read_word(&notes[i]);
    
    if (note != REST) {
      tone(BUZZER_PIN, note, noteDuration * 0.9);
    }
    delay(noteDuration);
    noTone(BUZZER_PIN);
  }
}

void updateThemeNonBlocking() {
  unsigned long currentTime = millis();
  
  if (currentTime - previousNoteTime >= (noteDuration + pauseBetweenNotes)) {
    
    currentNoteIndex++;
    if (currentNoteIndex >= theme_length) {
      currentNoteIndex = 0; 
    }

    int noteDivider = pgm_read_byte(&theme_durations[currentNoteIndex]);
    noteDuration = wholenote / noteDivider;
    

    pauseBetweenNotes = noteDuration * 0.10; 
    
    int currentFrequency = pgm_read_word(&theme_notes[currentNoteIndex]);
    
    if (currentFrequency != REST) {

      tone(BUZZER_PIN, currentFrequency, noteDuration * 0.9);
    } else {
      noTone(BUZZER_PIN);
    }
    
    previousNoteTime = currentTime;
  }
}

// --- GAME FUNCTIONS ---

void readButtons() {
  currentLeft = digitalRead(BTN_LEFT);
  currentRight = digitalRead(BTN_RIGHT);
  currentSelect = digitalRead(BTN_SELECT);

  leftPressed = (lastLeft == HIGH && currentLeft == LOW);
  rightPressed = (lastRight == HIGH && currentRight == LOW);

  lastLeft = currentLeft;
  lastRight = currentRight;
  lastSelect = currentSelect;
}

void initGame() {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = 0;
    }
  }
  score = 0;
  gameOver = false;
  gameOverMusicPlayed = false;
  lastFallTime = millis();
  
  // Reset Theme engine
  currentNoteIndex = -1;
  previousNoteTime = millis();
  noteDuration = 0;
  pauseBetweenNotes = 0;
  
  spawnNew();
}

bool checkCollision(int newX, int newY, int rotation) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (pgm_read_byte(&shapes[current.type][rotation][y][x])) {
        int gridX = newX + x;
        int gridY = newY + y;
        if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT) return true;
        if (gridY >= 0 && grid[gridY][gridX]) return true;
      }
    }
  }
  return false;
}

void lockPiece() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (pgm_read_byte(&shapes[current.type][current.rotation][y][x])) {
        int gridY = current.y + y;
        int gridX = current.x + x;
        if (gridY >= 0 && gridY < GRID_HEIGHT) grid[gridY][gridX] = 1;
      }
    }
  }
}

void clearLines() {
  for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
    bool fullLine = true;
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (!grid[y][x]) { fullLine = false; break; }
    }
    if (fullLine) {
      score += 10;
      for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < GRID_WIDTH; x++) grid[yy][x] = grid[yy - 1][x];
      }
      y++; 
    }
  }
}

void spawnNew() {
  current.x = GRID_WIDTH / 2 - 2;
  current.y = 0;
  current.type = random(7);
  current.rotation = 0;
  if (checkCollision(current.x, current.y, current.rotation)) gameOver = true;
}

void handleInput() {
  if (gameOver) {
    if (leftPressed) initGame();
    return;
  }


  if (leftPressed && !checkCollision(current.x - 1, current.y, current.rotation)) current.x--;
  if (rightPressed && !checkCollision(current.x + 1, current.y, current.rotation)) current.x++;

  static unsigned long lastFastDropTime = 0;


  if (currentSelect == LOW) {
    if (selectPressStart == 0) {
      selectPressStart = millis();
      rotatedThisPress = false;
    }
    unsigned long holdDuration = millis() - selectPressStart;
    
    if (!rotatedThisPress && holdDuration < 300) {
      int newRot = (current.rotation + 1) % 4;
      if (!checkCollision(current.x, current.y, newRot)) {
        current.rotation = newRot;
        rotatedThisPress = true;
      }
    }
    
    if (holdDuration >= 300 && millis() - lastFastDropTime >= 50) {
      if (!checkCollision(current.x, current.y + 1, current.rotation)) {
        current.y++;
      }
      lastFastDropTime = millis();
    }
  } else {
    selectPressStart = 0;
    rotatedThisPress = false;
  }
}

void updateGame() {
  if (gameOver) return;
  if (millis() - lastFallTime > fallDelay) {
    lastFallTime = millis();
    if (!checkCollision(current.x, current.y + 1, current.rotation)) {
      current.y++;
    } else {
      lockPiece();
      clearLines();
      spawnNew();
    }
  }
}

void drawGame() {
  display.clearDisplay();
  if (gameOver) {
    display.drawBitmap(0, 0, epd_bitmap_Game_over_cover, 128, 64, SSD1306_WHITE);
  } else {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(4, 1);
    display.print("SC:");
    display.print(score);
    
    display.drawBitmap(0, 0, epd_bitmap_tetris_playfield, 128, 64, SSD1306_WHITE);
    display.drawRect(OFFSET_X - 1, OFFSET_Y - 1, GRID_WIDTH * BLOCK_SIZE + 2, (GRID_HEIGHT - 4) * BLOCK_SIZE + 2, SSD1306_WHITE);

    for (int y = 4; y < GRID_HEIGHT; y++) {
      for (int x = 0; x < GRID_WIDTH; x++) {
        if (grid[y][x]) {
          display.fillRect(OFFSET_X + x * BLOCK_SIZE, OFFSET_Y + (y - 4) * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, SSD1306_WHITE);
        }
      }
    }

    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (pgm_read_byte(&shapes[current.type][current.rotation][y][x])) {
          int drawY = current.y + y - 4;
          if (drawY >= 0) {
            display.fillRect(OFFSET_X + (current.x + x) * BLOCK_SIZE, OFFSET_Y + drawY * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, SSD1306_WHITE);
          }
        }
      }
    }
  }
  display.display();
}