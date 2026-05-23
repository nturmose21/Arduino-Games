#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "games_bitmaps.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_LEFT 9
#define BTN_SELECT 10
#define BTN_RIGHT 11

bool lastLeft = HIGH, lastSelect = HIGH;
bool leftPressed = false, selectPressed = false;
bool currentLeft = HIGH, currentSelect = HIGH;

const int BIRD_X = 20;
int birdY;
float birdVelocity;
const float gravity = 0.5;
const float jumpStrength = -4.0;

struct Pipe { int x; int gapY; bool passed; };
Pipe pipes[3];
const int pipeWidth = 22;
const int gapHeight = 30;
const int pipeSpeed = 2;
const int pipeSpacing = 70;

const int minGapY = 15;
const int maxGapY = SCREEN_HEIGHT - gapHeight - 15;
const int maxVerticalChange = 25;

int score;
int highScore = 0;
bool gameOver;
unsigned long lastUpdateTime;
bool gameStarted;
int lastGapY;

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  randomSeed(analogRead(A0));
  
  display.clearDisplay();
  display.drawBitmap(0, 0, epd_bitmap_FLAPPY_BIRD_cover, 128, 64, SSD1306_WHITE);
  display.display();
  delay(15000);

  initGame();
}

void loop() {
  readButtons();
  handleInput();
  updateGame();
  drawGame();
}

void readButtons() {
  currentLeft = digitalRead(BTN_LEFT);
  currentSelect = digitalRead(BTN_SELECT);

  leftPressed = (lastLeft == HIGH && currentLeft == LOW);
  selectPressed = (lastSelect == HIGH && currentSelect == LOW);

  lastLeft = currentLeft;
  lastSelect = currentSelect;
}

void initGame() {
  birdY = SCREEN_HEIGHT / 2;
  birdVelocity = 0;
  score = 0;
  gameOver = false;
  gameStarted = false;
  lastUpdateTime = millis();
  lastGapY = (minGapY + maxGapY) / 2;

  for (int i = 0; i < 3; i++) {
    pipes[i].x = SCREEN_WIDTH + 20 + i * pipeSpacing;
    if (i == 0) {
      pipes[i].gapY = lastGapY;
    } else {
      int change = random(-maxVerticalChange, maxVerticalChange + 1);
      pipes[i].gapY = constrain(lastGapY + change, minGapY, maxGapY);
      lastGapY = pipes[i].gapY;
    }
    pipes[i].passed = false;
  }
}

void handleInput() {
  if (gameOver) {
    if (leftPressed) initGame();
    return;
  }
  
  static unsigned long lastJumpTime = 0;
  if (millis() - lastJumpTime < 200) return;

  if (selectPressed) {
    if (!gameStarted) gameStarted = true;
    birdVelocity = jumpStrength;
    lastJumpTime = millis();
  }
}

void updateGame() {
  if (gameOver || !gameStarted) return;
  if (millis() - lastUpdateTime < 30) return;
  lastUpdateTime = millis();

  birdVelocity += gravity;
  birdY += (int)birdVelocity;

  if (birdY < 0 || birdY > SCREEN_HEIGHT - 12) {
    gameOver = true;
    if (score > highScore) highScore = score;
    return;
  }

  for (int i = 0; i < 3; i++) {
    pipes[i].x -= pipeSpeed;
    if (pipes[i].x < -pipeWidth) {
      int maxX = pipes[0].x;
      int rightmostIndex = 0;
      for (int j = 1; j < 3; j++) {
        if (pipes[j].x > maxX) { maxX = pipes[j].x; rightmostIndex = j; }
      }
      pipes[i].x = maxX + pipeSpacing;
      int change = random(-maxVerticalChange, maxVerticalChange + 1);
      pipes[i].gapY = constrain(pipes[rightmostIndex].gapY + change, minGapY, maxGapY);
      pipes[i].passed = false;
    }

    if (pipes[i].x < BIRD_X + 18 && pipes[i].x + pipeWidth > BIRD_X) {
      if (birdY < pipes[i].gapY || birdY + 12 > pipes[i].gapY + gapHeight) {
        gameOver = true;
        if (score > highScore) highScore = score;
        return;
      }
    }

    if (!pipes[i].passed && pipes[i].x + pipeWidth < BIRD_X) {
      pipes[i].passed = true;
      score++;
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
    display.setCursor(2, 9); display.print("Score:"); display.print(score);
    display.setCursor(75, 9); display.print("Hi:"); display.print(highScore);

    if (!gameStarted) {
      display.clearDisplay();
      display.drawBitmap(65, 25, epd_bitmap_start_flappy, 50, 20, SSD1306_WHITE);
    }
    
    display.drawBitmap(BIRD_X, birdY, epd_bitmap_Bird, 18, 12, SSD1306_WHITE);

    if (gameStarted) {
      for (int i = 0; i < 3; i++) {
        int topPipeY = pipes[i].gapY - 64;
        display.drawBitmap(pipes[i].x, topPipeY, epd_bitmap_TOP_PIPE, 22, 64, SSD1306_WHITE);
        int bottomPipeY = pipes[i].gapY + gapHeight;
        display.drawBitmap(pipes[i].x, bottomPipeY, epd_bitmap_BOTTOM_PIPE, 22, 64, SSD1306_WHITE);
      }
    }
  }
  display.display();
}