#include "iGraphics.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 1000
#define BG_maxY 1067
#define NUM_ASTEROIDS 5
#define mainmenumax 1171

// -------------------- Global Variables --------------------
int spaceshipX = 250, spaceshipY = 100;
int asteroidX[NUM_ASTEROIDS], asteroidY[NUM_ASTEROIDS];
int score_count = 0;
bool gameOver_check = false;
int time_elapsed = 0;
int bg_y1 = 0, bg_y2 = BG_maxY;
int bg_mm1 = 0, bg_mm2 =mainmenumax;
int hovernewgame= 0;
int hoverdiff= 0;
int hoverhighscore= 0;
int hovercredits= 0;
int hoverquit= 0;




int yPos = 0;              // Menu scroll position
int fallSpeed = 3;
int imageHeight = 1171;

char gameState[20] = "menu";  // "menu" or "game"

// -------------------- Function Declarations --------------------
void initialize_Asteroids();
int gameover(int ax, int ay, int sx, int sy);
void rollingBackground();
void show_time();


// -------------------- iDraw --------------------
void iDraw() {
    iClear();

    if (strcmp(gameState, "menu") == 0) {
        iShowImage(0, yPos, "assets/images/MM_BG.png");
        iShowImage(0, yPos + imageHeight, "assets/images/MM_BG.png");

        yPos -= fallSpeed;
        if (yPos <= -imageHeight) yPos = 0;

        // Menu Texts
        if(hovernewgame) iShowImage(40 , (1000-370), "assets/images/newgame_mm_hover.png" );
        else iShowImage(40 , (1000-350), "assets/images/newgame_mm.png" );

        if(hoverdiff)  iShowImage(40 , (1000-470), "assets/images/difficulty_mm_hover.png" );
        else iShowImage(40 , (1000-450), "assets/images/difficulty_mm.png" );

        if(hoverhighscore) iShowImage(40 , (1000-570), "assets/images/highscore_mm_hover.png" );
        else iShowImage(40 , (1000-550), "assets/images/highscore_mm.png" );

        if(hovercredits)   iShowImage(40 , (1000-670), "assets/images/credits_mm_hover.png" );
        else iShowImage(40 , (1000-650), "assets/images/credits_mm.png" );

        if(hoverquit)  iShowImage(40 , (1000-770), "assets/images/quitgame_mm_hover.png" );
         else iShowImage(40 , (1000-750), "assets/images/quitgame_mm.png" );
    }

    else if (strcmp(gameState, "game") == 0) {
        // Background
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowImage(0, bg_y1, "assets/images/another_Bg.png");
        iShowImage(0, bg_y2, "assets/images/another_Bg.png");

        // Spaceship
        iShowImage(spaceshipX, spaceshipY, "assets/images/new spaceship.png");

        // Asteroids and Score
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            iShowImage(asteroidX[i], asteroidY[i], "assets/images/asteroids1.png");
            asteroidY[i] -= 8;

            if (asteroidY[i] < -50) {
                asteroidY[i] = SCREEN_HEIGHT + rand() % 300;
                asteroidX[i] = rand() % (SCREEN_WIDTH - 50);
                score_count++;
            }

            if (gameover(asteroidX[i], asteroidY[i], spaceshipX, spaceshipY)) {
                gameOver_check = true;
                strcpy(gameState, "menu"); // Reset to menu
                return;
            }
        }  

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
        iTextBold(10, 10, "Press arrow keys to move the spaceship", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

// -------------------- Mouse Input --------------------
void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (strcmp(gameState, "menu") == 0) {
            if (mx >= 0 && mx <= 600 && my >= 0 && my <= 800) {
                // "New Game" clicked
                strcpy(gameState, "game");
                score_count = 0;
                time_elapsed = 0;
                initialize_Asteroids();
            }
        }
    }
}

// -------------------- Keyboard Input --------------------
void iKeyboard(unsigned char key) {
    if (key == 'q') exit(0);
}
void iMouseMove(int mx, int my) {
  if(my>(1000-324)){
    hovernewgame = 1;
  }
  else hovernewgame = 0;
  if(my<(1000-324) && my > (1000-424)){
    hoverdiff = 1;
  }
  else hoverdiff = 0;
  if(my<(1000-424) && my> (1000-524)){
    hoverhighscore = 1;
  }
   else hoverhighscore = 0;
  if(my<(1000-524) && my> (1000-624)){
    hovercredits = 1;
  } else hovercredits = 0;
  if(my<(1000-624) && my> (1000-724)){
    hoverquit = 1;
  } else hoverquit = 0;
 
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
    if (key == GLUT_KEY_END) exit(0);

    if (strcmp(gameState, "game") == 0) {
        if (key == GLUT_KEY_LEFT && spaceshipX > 0) spaceshipX -= 12;
        if (key == GLUT_KEY_RIGHT && spaceshipX < 530) spaceshipX += 12;
        if (key == GLUT_KEY_UP && spaceshipY < 730) spaceshipY += 12;
        if (key == GLUT_KEY_DOWN && spaceshipY > 10) spaceshipY -= 12;
    }
}

// -------------------- Game Logic --------------------
void initialize_Asteroids() {
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        asteroidX[i] = rand() % (SCREEN_WIDTH - 50);
        asteroidY[i] = SCREEN_HEIGHT + rand() % 500;
    }
}

int gameover(int ax, int ay, int sx, int sy) {
    return (abs(ax - sx) < 50 && abs(ay - sy) < 50);
}

void rollingBackground() {
    if (strcmp(gameState, "menu") == 0) {

    bg_y1 -= 3;
    bg_y2 -= 3;
    if (bg_y1 <= -BG_maxY) bg_y1 = bg_y2 + BG_maxY;
    if (bg_y2 <= -BG_maxY) bg_y2 = bg_y1 + BG_maxY;

    }
    if (strcmp(gameState, "game") == 0) {

    bg_mm1 -= 3;
    bg_mm2 -= 3;
    if (bg_y1 <= -mainmenumax) bg_y1 = bg_y2 + mainmenumax;
    if (bg_y2 <= -mainmenumax) bg_y2 = bg_y1 + mainmenumax;
        
    }
}

void show_time() {
    if (strcmp(gameState, "game") == 0)
        time_elapsed++;
}

// -------------------- Menu Resource Load --------------------

// -------------------- Main --------------------
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    initialize_Asteroids();
    iSetTimer(10, rollingBackground);
    iSetTimer(1000, show_time);
    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter Game");
    return 0;
}
