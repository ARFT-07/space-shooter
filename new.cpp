#include "iGraphics.h"
#include "iSound.h"

#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define NUM_villains 7

#define mainmenumax 1171
#define MAX_BULLETS 20

// -------------------- Global Variables --------------------
Image bgimage;
Image gamestateimage;
Image difficulty;
Image credits;
Image pause;
int spaceshipX = 200, spaceshipY = 100;
int villainX[NUM_villains], villainY[NUM_villains];
int score_count = 0;
bool gameOver_check = false;
int time_elapsed = 0;
int bg_X1 = 0, bg_X2 = SCREEN_WIDTH;
int bg_mm1 = 0, bg_mm2 = mainmenumax;
int hovernewgame = 0;
int hoverdiff = 0;
int hoverhighscore = 0;
int hovercredits = 0;
int hoverquit = 0;
int bulletX[MAX_BULLETS], bulletY[MAX_BULLETS];
int bulletVisible[MAX_BULLETS];
int bgsound = -1;

int yPos = 0; // Menu scroll position
int fallSpeed = 3;
int imageHeight = 1171;
int bulletSpeed = 20;
char gameState[20] = "menu"; // "menu" or "game"
int pic_checker[NUM_villains];
int newadd = 1;
int exittime = 0;

// -------------------- Function Declarations --------------------
void initialize_Villains();

void initialize_Bullets();
int gameover(int ax, int ay, int sx, int sy);
void rollingBackground();
void show_time();
void fireBullets();
void drawBullets();
void updateBullets();
void checkBulletVillainCollision();
bool level50Unlocked = false;
bool level100Unlocked = false;
bool level150Unlocked = false;

// -------------------- iDraw --------------------
void iDraw() {
  iClear();

  if (strcmp(gameState, "menu") == 0) {

    // iShowImage(0, yPos, "assets/images/MM_BG.png");
    // iShowImage(0, yPos + imageHeight, "assets/images/MM_BG.png");
    iShowImage(0, 0, "assets/images/mmbg3.jpg");

    yPos -= fallSpeed;
    if (yPos <= -imageHeight)
      yPos = 0;

    // Menu Texts
    if (hovernewgame)
      iShowImage(200, (1000 - 370), "assets/images/newgame_mm_hover.png");
    else
      iShowImage(200, (1000 - 350), "assets/images/newgame_mm.png");

    if (hoverdiff)
      iShowImage(200, (1000 - 470), "assets/images/difficulty_mm_hover.png");
    else
      iShowImage(200, (1000 - 450), "assets/images/difficulty_mm.png");

    if (hoverhighscore)
      iShowImage(200, (1000 - 570), "assets/images/highscore_mm_hover.png");
    else
      iShowImage(200, (1000 - 550), "assets/images/highscore_mm.png");

    if (hovercredits)
      iShowImage(200, (1000 - 670), "assets/images/credits_mm_hover.png");
    else
      iShowImage(200, (1000 - 650), "assets/images/credits_mm.png");

    if (hoverquit)
      iShowImage(200, (1000 - 770), "assets/images/quitgame_mm_hover.png");
    else
      iShowImage(200, (1000 - 750), "assets/images/quitgame_mm.png");
  }

  else if (strcmp(gameState, "game") == 0) {
    // Background
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    iShowLoadedImage(0, 0, &gamestateimage);
    iWrapImage(&gamestateimage , -5);
    // iShowImage(0, 0, "assets/images/bg_horizontal.png");
    // iShowImage(0, 0, "assets/images/bg_horizontal.png");

    // Spaceship
    iShowImage(spaceshipX, spaceshipY, "assets/images/ss2.png");

    // Villains and Score

    for (int i = 0; i < NUM_villains; i++) {

      for (int i = 0; i < NUM_villains; i++) {

        if (villainY[i] < -50) {
          pic_checker[i] = rand() % newadd;
          villainY[i] = SCREEN_HEIGHT + rand() % 300;
          villainX[i] = rand() % (SCREEN_WIDTH - 50);
          score_count++;
        }

        // Unlock new villain types only once per level
        if (score_count >= 50 && !level50Unlocked) {
          newadd++;
          level50Unlocked = true;
        }
        if (score_count >= 100 && !level100Unlocked) {
          newadd++;
          level100Unlocked = true;
        }
        if (score_count >= 150 && !level150Unlocked) {
          newadd++;
          level150Unlocked = true;
        }

        // Draw villain image
        if (pic_checker[i] == 0)
          iShowImage(villainX[i], villainY[i], "assets/images/asteroids1.png");
        else if (pic_checker[i] == 1)
          iShowImage(villainX[i], villainY[i], "assets/images/villain3.png");
        else if (pic_checker[i] == 2)
          iShowImage(villainX[i], villainY[i], "assets/images/villain2_x.png");

        // Speed control
        int speed = 1;
        if (score_count >= 150)
          speed = 4;
        else if (score_count >= 100)
          speed = 3;
        else if (score_count >= 50)
          speed = 2;

        villainY[i] -= speed;

        // Check for game over
        if (gameover(villainX[i], villainY[i], spaceshipX, spaceshipY)) {
          gameOver_check = true;
          score_count = 0;
          strcpy(gameState, "menu"); // Reset to menu
          return;
        }
      }
    }

    drawBullets();
    // Score Text
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score_count);
    iSetColor(245, 227, 66);
    iTextBold(10, SCREEN_HEIGHT - 30, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Time Text
    char timeStr[30];
    int min = time_elapsed / 60;
    int sec = time_elapsed % 60;
    sprintf(timeStr, "Time Elapsed: %02d:%02d", min, sec);
    iTextBold(10, SCREEN_HEIGHT - 60, timeStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Controls Tip
    iSetColor(255, 255, 255);
    iFilledRectangle(0, 0, 800, 30);
    iSetColor(0, 0, 0);
    iTextBold(10, 10, "Press arrow keys to move the spaceship",
              GLUT_BITMAP_TIMES_ROMAN_24);
  }

  else if (strcmp(gameState, "exit") == 0) {

    iShowImage(0, 0, "assets/images/exit page.png");

    if (exittime == 4) {
      exit(0);
    }
  }
}

// -------------------- Mouse Input --------------------
void iMouse(int button, int state, int mx, int my) {
  if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) {
    if (strcmp(gameState, "menu") == 0) {
      if (((my >= 650 && my <= 720))) {
        // "New Game" clicked
        strcpy(gameState, "game");
        initialize_Bullets();
        score_count = 0;
        time_elapsed = 0;
        initialize_Villains();
      }

      if (((my >= 250 && my <= 320))) {
        // "New Game" clicked
        strcpy(gameState, "exit");

        time_elapsed = 0;
      }
    }
  }
  if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) {
    if (strcmp(gameState, "game")) {
    }
  }
}

// -------------------- Keyboard Input --------------------
void iKeyboard(unsigned char key) {
  if (key == 'q')
    exit(0);
  if (strcmp(gameState, "game") == 0) {
    if (key == ' ') {
      fireBullets();
    }
  }
}
void iMouseMove(int mx, int my) {

  printf("mx = %d  my =%d\n", mx, my);

  if ((my >= 650 && my <= 720)) {
    hovernewgame = 1;
  } else
    hovernewgame = 0;
  if ((my >= 550 && my <= 620)) {
    hoverdiff = 1;
  } else
    hoverdiff = 0;
  if ((my >= 450 && my <= 520)) {
    hoverhighscore = 1;
  } else
    hoverhighscore = 0;
  if ((my >= 350 && my <= 420)) {
    hovercredits = 1;
  } else
    hovercredits = 0;
  if ((my >= 250 && my <= 320)) {
    hoverquit = 1;
  } else
    hoverquit = 0;
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/

void iMouseDrag(int mx, int my) {
  // place your codes here
}
void iMouseWheel(int dir, int mx, int my) {
  // place your code here
}

void iSpecialKeyboard(unsigned char key) {
  if (key == GLUT_KEY_END)
    exit(0);

  if (strcmp(gameState, "game") == 0) {
    if (key == GLUT_KEY_LEFT && spaceshipX > 0)
      spaceshipX -= 12;
    if (key == GLUT_KEY_RIGHT && spaceshipX < SCREEN_WIDTH)
      spaceshipX += 12;
    if (key == GLUT_KEY_UP && spaceshipY < SCREEN_HEIGHT)
      spaceshipY += 12;
    if (key == GLUT_KEY_DOWN && spaceshipY > 10)
      spaceshipY -= 12;
  }
}

// -------------------- Game Logic --------------------

void initialize_Bullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    bulletVisible[i] = 0;
  }
}

void fireBullets() {
  for (int i = 0; i < MAX_BULLETS - 1; i++) {
    if (bulletVisible[i] == 0 && bulletVisible[i + 1] == 0) {
      bulletX[i] = spaceshipX + 39;
      bulletY[i] = spaceshipY + 85;
      bulletVisible[i] = 1;

      bulletX[i + 1] = spaceshipX + 108;
      bulletY[i + 1] = spaceshipY + 85;
      bulletVisible[i + 1] = 1;
      break;
    }
  }
}
void drawBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bulletVisible[i] == 1) {
      iShowImage(bulletX[i], bulletY[i], "assets/images/bullets1.png");
    }
  }
}

void updateBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bulletVisible[i] == 1) {
      bulletY[i] += bulletSpeed;

      if (bulletY[i] > SCREEN_HEIGHT)
        bulletVisible[i] = 0; // Disappear offscreen
    }
  }
}

void checkBulletVillainCollision() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bulletVisible[i] == 1) {
      for (int j = 0; j < NUM_villains; j++) {
        // Check overlap cords;
        if (abs(bulletX[i] - villainX[j]) < 100 &&
            abs(bulletY[i] - villainY[j]) < 100) {
          // overlap done;
          bulletVisible[i] = 0; // Hide bullet
          villainY[j] =
              SCREEN_HEIGHT + rand() % 300; // Reset Villain Y position
          villainX[j] = rand() % (SCREEN_WIDTH - 50); // Random X position

          score_count++; // Increase score
          pic_checker[i] = rand() % newadd;
        }
      }
    }
  }
}
void initialize_Villains() {
  for (int i = 0; i < NUM_villains; i++) {
    villainX[i] = rand() % (SCREEN_WIDTH - 50);
    villainY[i] = SCREEN_HEIGHT + rand() % 500;
  }
}

int gameover(int ax, int ay, int sx, int sy) {
  return (abs(ax - sx) < 50 && abs(ay - sy) < 50);
}

void rollingBackground() {
  if (strcmp(gameState, "menu") == 0) {

    // bg_mm1 -= 3;
    // bg_mm2 -= 3;
    // if ( <= -mainmenumax)
    //    = 0 + mainmenumax;
    // if (0 <= -mainmenumax)
    //   0 =  + mainmenumax;
  }
  if (strcmp(gameState, "game") == 0) {

    bg_X1 -= 3;
    bg_X2 -= 3;
    if (bg_X1 <= SCREEN_WIDTH)
      bg_X1 = 0 + SCREEN_WIDTH;
    if (bg_X2 <= -SCREEN_WIDTH)
      bg_X2 = +SCREEN_WIDTH;
  }
}

void show_time() {
  if (strcmp(gameState, "game") == 0)
    time_elapsed++;

  else if (strcmp(gameState, "exit") == 0)
    exittime++;
}
void loadResources() {
  iLoadImage(&gamestateimage, "assets/images/bg_horizontal.png");
}

// -------------------- Menu Resource Load --------------------

// -------------------- Main --------------------
int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  loadResources();

  initialize_Villains();
  initialize_Bullets();
  iSetTimer(10, rollingBackground);
  iSetTimer(1000, show_time);
  iSetTimer(15, updateBullets);
  iSetTimer(15, checkBulletVillainCollision);
  iInitializeSound();
  if (strcmp(gameState, "menu") == 0) {
    bgsound = iPlaySound("assets/sounds/mm.wav", true);
  }

  iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter Game");

  return 0;
}