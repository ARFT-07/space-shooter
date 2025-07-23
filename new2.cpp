#include "iGraphics.h"
#include "iSound.h"

#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define DIFF_EASY 1
#define DIFF_MEDIUM 2
#define DIFF_HARD 3

#define NUM_villains 7
#define mainmenumax 1171
#define MAX_BULLETS 20
#define MAX_EXPLOSIONS 10
#define MAX_POWERUPS 5
#define MAX_PARTICLES 5
#define MAX_HIGHSCORES 5



// -------------------- Structures --------------------
typedef struct {
    int x, y;
    int active;
    int timer;
    int type; // 0 = explosion, 1 = hit effect
} Explosion;

typedef struct {
    int x, y;
    int active;
    int type; // 0 = health, 1 = rapid fire, 2 = shield
    int timer;
} PowerUp;

typedef struct {
    float x, y;
    float vx, vy;
    int life;
    int active;
    int color[3]; // RGB
} Particle;

// -------------------- Global Variables --------------------
Image bgimage;
Image spaceship;
Image gamestateimage;
Image difficulty;
Image bullet_image, asteroid, villain1, villain2;
Image credits;
Image pause;
Image explosion_img, powerup_img, shield_img;
Image health_img ,rapid_img;

int spaceshipX = 200, spaceshipY = 100;
int villainX[NUM_villains], villainY[NUM_villains];
int villainHealth[NUM_villains];
int score_count = 0;
int high_score = 0;
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
int highScores[MAX_HIGHSCORES] = {0, 0, 0, 0, 0};
int numHighScores = 0;  

// Enhanced features
int player_health = 100;
int max_health = 100;
int shield_active = 0;
int shield_timer = 0;
int rapid_fire_active = 0;
int rapid_fire_timer = 0;
int difficulty_level = 1;
int lives = 3;
int combo_count = 0;
int combo_timer = 0;
int difficulty_level = DIFF_EASY; 

Explosion explosions[MAX_EXPLOSIONS];
PowerUp powerups[MAX_POWERUPS];
Particle particles[MAX_PARTICLES];

int yPos = 0; // Menu scroll position
int fallSpeed = 3;
int imageHeight = 1171;
int bulletSpeed = 20;
char gameState[20] = "menu"; // "menu", "game", "pause", "gameover"
int pic_checker[NUM_villains];
int newadd = 1;
int exittime = 0;

bool level50Unlocked = false;
bool level100Unlocked = false;
bool level150Unlocked = false;
static bool scoreInserted = false;

// -------------------- Function Declarations --------------------
void initialize_Villains();
void initialize_Bullets();
void initialize_Explosions();
void initialize_PowerUps();
void initialize_Particles();
int gameover(int ax, int ay, int sx, int sy);
void rollingBackground();
void show_time();
void fireBullets();
void drawBullets();
void updateBullets();
void checkBulletVillainCollision();
void updateExplosions();
void drawExplosions();
void createExplosion(int x, int y, int type);
void updatePowerUps();
void drawPowerUps();
void spawnPowerUp(int x, int y);
void checkPowerUpCollision();
void updateParticles();
void drawParticles();
void createParticles(int x, int y, int count);
void drawHUD();
void drawGameOver();
void resetGame();
void saveHighScore();
void loadHighScore();
void insertHighScore();
void drawHighScores();

// -------------------- iDraw --------------------
void iDraw() {
    iClear();

    if (strcmp(gameState, "menu") == 0) {
        iShowImage(0, 0, "assets/images/mmbg3.jpg");

        yPos -= fallSpeed;
        if (yPos <= -imageHeight)
            yPos = 0;

        // Menu Texts with enhanced hover effects
        if (hovernewgame) {
            iSetColor(255, 255, 0);
            iFilledRectangle(195, 625, 410, 50);
            iShowImage(200, (1000 - 370), "assets/images/newgame_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 350), "assets/images/newgame_mm.png");
        }

        if (hoverdiff) {
            iSetColor(255, 255, 0);
            iFilledRectangle(195, 525, 410, 50);
            iShowImage(200, (1000 - 470), "assets/images/difficulty_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 450), "assets/images/difficulty_mm.png");
        }

        if (hoverhighscore) {
            iSetColor(255, 255, 0);
            iFilledRectangle(195, 425, 410, 50);
            iShowImage(200, (1000 - 570), "assets/images/highscore_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 550), "assets/images/highscore_mm.png");
        }

        if (hovercredits) {
            iSetColor(255, 255, 0);
            iFilledRectangle(195, 325, 410, 50);
            iShowImage(200, (1000 - 670), "assets/images/credits_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 650), "assets/images/credits_mm.png");
        }

        if (hoverquit) {
            iSetColor(255, 255, 0);
            iFilledRectangle(195, 225, 410, 50);
            iShowImage(200, (1000 - 770), "assets/images/quitgame_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 750), "assets/images/quitgame_mm.png");
        }

        // Display high score on menu
        char highScoreStr[50];
        sprintf(highScoreStr, "High Score: %d", high_score);
        iSetColor(255, 255, 255);
        iTextBold(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 50, highScoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
        drawHighScores();
    }

    else if (strcmp(gameState, "game") == 0) {
        // Background
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        iShowLoadedImage(0, 0, &gamestateimage);
        iWrapImage(&gamestateimage, -5);

        // Draw shield effect around spaceship
        if (shield_active) {
            iSetColor(0, 255, 255);
            iCircle(spaceshipX + 75, spaceshipY + 50, 80, 100);
        }

        // Spaceship
        iShowLoadedImage(spaceshipX, spaceshipY, &spaceship);

        // Villains
        for (int i = 0; i < NUM_villains; i++) {
            if (villainY[i] < -50) {
                pic_checker[i] = rand() % newadd;
                villainY[i] = SCREEN_HEIGHT + rand() % 300;
                villainX[i] = rand() % (SCREEN_WIDTH - 50);
                villainHealth[i] = (pic_checker[i] == 0) ? 1 : 2; // Asteroids have 1 HP, others have 2
                score_count++;
            }

            // Unlock new villain types
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

            // Draw villain with health indicator
            if (pic_checker[i] == 0)
                iShowLoadedImage(villainX[i], villainY[i], &asteroid);
            else if (pic_checker[i] == 1)
                iShowLoadedImage(villainX[i], villainY[i], &villain1);
            else if (pic_checker[i] == 2)
                iShowLoadedImage(villainX[i], villainY[i], &villain2);

            // Health bar for villains with > 1 HP
            if (villainHealth[i] > 1) {
                iSetColor(255, 0, 0);
                iFilledRectangle(villainX[i], villainY[i] + 60, 50, 5);
                iSetColor(0, 255, 0);
                iFilledRectangle(villainX[i], villainY[i] + 60, 25 * villainHealth[i], 5);
            }

            // Enhanced movement with different patterns
            int speed = 1 + (score_count / 50);
            if (speed > 5) speed = 5;

            villainY[i] -= speed;

            // Different movement patterns for different villains
            if (pic_checker[i] == 0) {
                // Asteroids move straight down
            } else if (pic_checker[i] == 1) {
                // Villain1 moves in sine wave
                villainX[i] += (int)(5 * sin(villainY[i] * 0.02));
            } else if (pic_checker[i] == 2) {
                // Villain2 moves left-right
                villainX[i] += (i % 2 == 0) ? 2 : -2;
            }

            // Boundary checking
            if (villainX[i] < 0) villainX[i] = 0;
            if (villainX[i] > SCREEN_WIDTH - 50) villainX[i] = SCREEN_WIDTH - 50;

            // Check for collision with player
            if (gameover(villainX[i], villainY[i], spaceshipX, spaceshipY)) {
                if (shield_active) {
                    shield_active = 0;
                    createExplosion(villainX[i], villainY[i], 0);
                    villainY[i] = SCREEN_HEIGHT + rand() % 300;
                    villainX[i] = rand() % (SCREEN_WIDTH - 50);
                } else {
                    player_health -= 20;
                    createExplosion(spaceshipX, spaceshipY, 1);
                    villainY[i] = SCREEN_HEIGHT + rand() % 300;
                    villainX[i] = rand() % (SCREEN_WIDTH - 50);
                    
                    if (player_health <= 0) {
                        lives--;
                        if (lives <= 0) {
                            strcpy(gameState, "gameover");
                            return;
                        } else {
                            player_health = max_health;
                            createExplosion(spaceshipX, spaceshipY, 0);
                        }
                    }
                }
            }
        }

        drawBullets();
        drawExplosions();
        drawPowerUps();
        drawParticles();
        drawHUD();

        // Combo display
        if (combo_count > 1) {
            char comboStr[20];
            sprintf(comboStr, "COMBO x%d", combo_count);
            iSetColor(255, 255, 0);
            iTextBold(SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT - 100, comboStr, GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }

    else if (strcmp(gameState, "pause") == 0) {
        // Draw game state first (frozen)
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &gamestateimage);
        iShowLoadedImage(spaceshipX, spaceshipY, &spaceship);

        // Semi-transparent overlay
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Pause text
        iSetColor(255, 255, 255);
        iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, "Press P to Resume", GLUT_BITMAP_TIMES_ROMAN_24);
    }

   else if (strcmp(gameState, "gameover") == 0) {
    if (!scoreInserted) {
        insertHighScore(score_count);
        saveHighScore();
        scoreInserted = true;
    }
    drawGameOver();
}

    else if (strcmp(gameState, "exit") == 0) {
        iShowImage(0, 0, "assets/images/exit page.png");
        if (exittime == 4) {
            saveHighScore();
            exit(0);
        }
    }
}

// -------------------- Enhanced Functions --------------------
void drawHUD() {
    // Health bar
    iSetColor(255, 0, 0);
    iFilledRectangle(10, SCREEN_HEIGHT - 100, 200, 20);
    iSetColor(0, 255, 0);
    iFilledRectangle(10, SCREEN_HEIGHT - 100, (200 * player_health) / max_health, 20);
    
    iSetColor(255, 255, 255);
    iTextBold(10, SCREEN_HEIGHT - 125, "Health", GLUT_BITMAP_TIMES_ROMAN_24);

    // Lives
    char livesStr[20];
    sprintf(livesStr, "Lives: %d", lives);
    iSetColor(255, 255, 255);
    iTextBold(10, SCREEN_HEIGHT - 150, livesStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Score
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score_count);
    iSetColor(245, 227, 66);
    iTextBold(10, SCREEN_HEIGHT - 30, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Time
    char timeStr[30];
    int min = time_elapsed / 60;
    int sec = time_elapsed % 60;
    sprintf(timeStr, "Time: %02d:%02d", min, sec);
    iTextBold(10, SCREEN_HEIGHT - 60, timeStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Power-up indicators
    if (shield_active) {
        iSetColor(0, 255, 255);
        iTextBold(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 30, "SHIELD", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    if (rapid_fire_active) {
        iSetColor(255, 255, 0);
        iTextBold(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 60, "RAPID FIRE", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    // Controls
    iSetColor(255, 255, 255);
    iFilledRectangle(0, 0, 600, 30);
    iSetColor(0, 0, 0);
    iTextBold(10, 10, "Arrow Keys: Move | Space: Shoot | P: Pause", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawGameOver() {
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    iSetColor(255, 0, 0);
    iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 100, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    
    char finalScoreStr[50];
    sprintf(finalScoreStr, "Final Score: %d", score_count);
    iSetColor(255, 255, 255);
    iTextBold(SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 50, finalScoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
    
    if (score_count > high_score) {
        iSetColor(255, 255, 0);
        iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, "NEW HIGH SCORE!", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    
    iSetColor(255, 255, 255);
    iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, "Press R to Restart", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 100, "Press Q to Quit", GLUT_BITMAP_TIMES_ROMAN_24);
}

void createExplosion(int x, int y, int type) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].type = type;
            explosions[i].active = 1;
            explosions[i].timer = 30;
            createParticles(x+50, y+50, 10);
            break;
        }
    }
}

void updateExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            explosions[i].timer--;
            if (explosions[i].timer <= 0) {
                explosions[i].active = 0;
            }
        }
    }
}

void drawExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            int size = 30 - explosions[i].timer;
            if (explosions[i].type == 0) {
                iSetColor(255, 100, 0);
            } else {
                iSetColor(255, 255, 0);
            }
            iFilledCircle(explosions[i].x, explosions[i].y, size);
        }
    }
}

void createParticles(int x, int y, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (rand() % 21 - 10) * 0.5;
            particles[i].vy = (rand() % 21 - 10) * 0.5;
            particles[i].life = 30 + rand() % 30;
            particles[i].active = 1;
            particles[i].color[0] = 255;
            particles[i].color[1] = rand() % 256;
            particles[i].color[2] = rand() % 256;
            count--;
        }
    }
}

void updateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].life--;
            if (particles[i].life <= 0) {
                particles[i].active = 0;
            }
        }
    }
}

void drawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            iSetColor(particles[i].color[0], particles[i].color[1], particles[i].color[2]);
            iFilledCircle((int)particles[i].x, (int)particles[i].y, 3);
        }
    }
}

void spawnPowerUp(int x, int y) {
    if (rand() % 100 < 15) { // 15% chance
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!powerups[i].active) {
                powerups[i].x = x;
                powerups[i].y = y;
                powerups[i].type = rand() % 3;
                powerups[i].active = 1;
                powerups[i].timer = 600; // 10 seconds at 60 FPS
                break;
            }
        }
    }
}

void updatePowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            powerups[i].y -= 2;
            powerups[i].timer--;
            if (powerups[i].y < 0 || powerups[i].timer <= 0) {
                powerups[i].active = 0;
            }
        }
    }
}
void drawHighScores() {
    iSetColor(255, 255, 255);
    iTextBold(50, SCREEN_HEIGHT - 100, "Top 5 High Scores:", GLUT_BITMAP_TIMES_ROMAN_24);
    for (int i = 0; i < numHighScores; i++) {
        char scoreStr[30];
        sprintf(scoreStr, "%d. %d", i + 1, highScores[i]);
        iTextBold(50, SCREEN_HEIGHT - 130 - i * 30, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void drawPowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            if (powerups[i].type == 0) {
                 iShowLoadedImage(powerups[i].x, powerups[i].y, &health_img);

            } else if (powerups[i].type == 1) {
                 iShowLoadedImage(powerups[i].x, powerups[i].y, &rapid_img);
                
            } else {
                iShowLoadedImage(powerups[i].x, powerups[i].y, &shield_img);
            }
            //iFilledCircle(powerups[i].x, powerups[i].y, 15);
            iSetColor(255, 255, 255);

        }
    }
}

void checkPowerUpCollision() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            if (abs(powerups[i].x - (spaceshipX + 75)) < 50 && 
                abs(powerups[i].y - (spaceshipY + 50)) < 50) {
                
                powerups[i].active = 0;
                
                switch (powerups[i].type) {
                    case 0: // Health
                        player_health = max_health;
                        break;
                    case 1: // Rapid fire
                        rapid_fire_active = 1;
                        rapid_fire_timer = 300; // 5 seconds
                        break;
                    case 2: // Shield
                        shield_active = 1;
                        shield_timer = 300; // 5 seconds
                        break;
                }
            }
        }
    }
}

// -------------------- Mouse Input --------------------
void iMouse(int button, int state, int mx, int my) {
    if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) {
        if (strcmp(gameState, "menu") == 0) {
            if (((my >= 650 && my <= 720))) {
                // "New Game" clicked
                resetGame();
                strcpy(gameState, "game");
            }
            if (((my >= 250 && my <= 320))) {
                // "Quit" clicked
                strcpy(gameState, "exit");
                time_elapsed = 0;
            }
        }
        else if (strcmp(gameState, "gameover") == 0) {
            // Click anywhere to restart
            resetGame();
            strcpy(gameState, "game");
        }
    }
}

// -------------------- Keyboard Input --------------------
void iKeyboard(unsigned char key) {
    if (key == 'q' || key == 'Q') {
        saveHighScore();
        exit(0);
    }
    
    if (strcmp(gameState, "game") == 0) {
        if (key == ' ') {
            fireBullets();
        }
        if (key == 'p' || key == 'P') {
            strcpy(gameState, "pause");
        }
    }
    else if (strcmp(gameState, "pause") == 0) {
        if (key == 'p' || key == 'P') {
            strcpy(gameState, "game");
        }
    }
    else if (strcmp(gameState, "gameover") == 0) {
        if (key == 'r' || key == 'R') {
            resetGame();
            strcpy(gameState, "game");
        }
    }
}

void resetGame() {
    score_count = 0;
    time_elapsed = 0;
    player_health = max_health;
    lives = 3;
    shield_active = 0;
    rapid_fire_active = 0;
    combo_count = 0;
    spaceshipX = 200;
    spaceshipY = 100;
    
    initialize_Villains();
    initialize_Bullets();
    initialize_Explosions();
    initialize_PowerUps();
    initialize_Particles();
    
    // Reset unlock flags
    level50Unlocked = false;
    level100Unlocked = false;
    level150Unlocked = false;
    newadd = 1;
    scoreInserted = false;
}

void saveHighScore() {
    FILE *file = fopen("highscores.txt", "w");
    if (!file) return;

    for (int i = 0; i < numHighScores; i++) {
        fprintf(file, "%d\n", highScores[i]);
    }
    fclose(file);
}
void insertHighScore(int newScore) {
   
    if (numHighScores == 0) {
        highScores[0] = newScore;
        numHighScores = 1;
        return;
    }

    
    int pos = numHighScores;
    for (int i = 0; i < numHighScores; i++) {
        if (newScore > highScores[i]) {
            pos = i;
            break;
        }
    }

   
    if (pos == numHighScores && numHighScores == MAX_HIGHSCORES) {
        return;
    }

    
    if (numHighScores < MAX_HIGHSCORES) {
        numHighScores++;
    }
    for (int i = numHighScores - 1; i > pos; i--) {
        highScores[i] = highScores[i - 1];
    }

   
    highScores[pos] = newScore;
}



void loadHighScore() {
    FILE *file = fopen("highscores.txt", "r");
    if (!file) {
        numHighScores = 0;
        return;
    }

    numHighScores = 0;
    while (numHighScores < MAX_HIGHSCORES && fscanf(file, "%d", &highScores[numHighScores]) == 1) {
        numHighScores++;
    }
    fclose(file);
}

// -------------------- Enhanced Collision Detection --------------------
void checkBulletVillainCollision() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            for (int j = 0; j < NUM_villains; j++) {
                if (abs(bulletX[i] - villainX[j]) < 50 && abs(bulletY[i] - villainY[j]) < 50) {
                    bulletVisible[i] = 0;
                    villainHealth[j]--;
                    
                    if (villainHealth[j] <= 0) {
                        createExplosion(villainX[j], villainY[j], 0);
                        spawnPowerUp(villainX[j], villainY[j]);
                        
                        villainY[j] = SCREEN_HEIGHT + rand() % 300;
                        villainX[j] = rand() % (SCREEN_WIDTH - 50);
                        villainHealth[j] = (pic_checker[j] == 0) ? 1 : 2;
                        
                        score_count += (pic_checker[j] == 0) ? 10 : 20;
                        combo_count++;
                        combo_timer = 180; // 3 seconds
                        
                        if (combo_count > 1) {
                            score_count += combo_count * 5; // Bonus points
                        }
                    }
                    break;
                }
            }
        }
    }
}

// -------------------- Enhanced Bullet System --------------------
void fireBullets() {
    int bullet_count = rapid_fire_active ? 4 : 2;
    int found = 0;
    
    for (int i = 0; i < MAX_BULLETS - bullet_count + 1 && found < bullet_count; i++) {
        if (bulletVisible[i] == 0) {
            bulletX[i] = spaceshipX + 39 + (found * 35);
            bulletY[i] = spaceshipY + 85;
            bulletVisible[i] = 1;
            found++;
        }
    }
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            bulletY[i] += bulletSpeed;
            if (bulletY[i] > SCREEN_HEIGHT) {
                bulletVisible[i] = 0;
            }
        }
    }
}

// -------------------- Timer Functions --------------------
void show_time() {
    if (strcmp(gameState, "game") == 0) {
        time_elapsed++;
        
        // Update power-up timers
        if (shield_active) {
            shield_timer--;
            if (shield_timer <= 0) {
                shield_active = 0;
            }
        }
        
        if (rapid_fire_active) {
            rapid_fire_timer--;
            if (rapid_fire_timer <= 0) {
                rapid_fire_active = 0;
            }
        }
        
        // Update combo timer
        if (combo_timer > 0) {
            combo_timer--;
            if (combo_timer <= 0) {
                combo_count = 0;
            }
        }
    }
    else if (strcmp(gameState, "exit") == 0) {
        exittime++;
    }
}

// -------------------- Initialization Functions --------------------
void initialize_Explosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        explosions[i].active = 0;
    }
}

void initialize_PowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = 0;
    }
}

void initialize_Particles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = 0;
    }
}

void initialize_Villains() {
    for (int i = 0; i < NUM_villains; i++) {
        villainX[i] = rand() % (SCREEN_WIDTH - 50);
        villainY[i] = SCREEN_HEIGHT + rand() % 500;
        villainHealth[i] = 1;
        pic_checker[i] = 0;
    }
}

void initialize_Bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bulletVisible[i] = 0;
    }
}

int gameover(int ax, int ay, int sx, int sy) {
    return (abs(ax - (sx+80)) < 80 && abs(ay - (sy+80)) < 80);
}

void rollingBackground() {
    if (strcmp(gameState, "game") == 0) {
        bg_X1 -= 3;
        bg_X2 -= 3;
        if (bg_X1 <= -SCREEN_WIDTH)
            bg_X1 = SCREEN_WIDTH;
        if (bg_X2 <= -SCREEN_WIDTH)
            bg_X2 = SCREEN_WIDTH;
    }
}

void drawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            iShowLoadedImage(bulletX[i], bulletY[i], &bullet_image);
        }
    }
}

void iMouseMove(int mx, int my) {
    if (strcmp(gameState, "menu") == 0) {
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
}

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
            spaceshipX -= 15;
        if (key == GLUT_KEY_RIGHT && spaceshipX < SCREEN_WIDTH - 150)
            spaceshipX += 15;
        if (key == GLUT_KEY_UP && spaceshipY < SCREEN_HEIGHT - 100)
            spaceshipY += 15;
        if (key == GLUT_KEY_DOWN && spaceshipY > 10)
            spaceshipY -= 15;
    }
}

void loadResources() {
    iLoadImage(&gamestateimage, "assets/images/bg_horizontal.png");
    iLoadImage(&spaceship, "assets/images/ss5.png");
    iLoadImage(&bullet_image, "assets/images/bullets1.png");
    iLoadImage(&villain1, "assets/images/villain3.png");
    iLoadImage(&villain2, "assets/images/villain2_x.png");
    iLoadImage(&asteroid, "assets/images/asteroids1.png");
    iLoadImage(&shield_img,"assets/images/shield.png" );
    iLoadImage(&health_img,"assets/images/health.png" );
    iLoadImage(&rapid_img,"assets/images/rapid.png" );


}

// -------------------- Main --------------------
int main(int argc, char *argv[]) {
    srand(time(NULL)); // Initialize random seed
    glutInit(&argc, argv);
    
    loadResources();
    loadHighScore();
    
    initialize_Villains();
    initialize_Bullets();
    initialize_Explosions();
    initialize_PowerUps();
    initialize_Particles();
    
    // Set up timers
    iSetTimer(10, rollingBackground);
    iSetTimer(1000, show_time);
    iSetTimer(15, updateBullets);
    iSetTimer(15, checkBulletVillainCollision);
    iSetTimer(20, updateExplosions);
    iSetTimer(25, updatePowerUps);
    iSetTimer(30, checkPowerUpCollision);
    iSetTimer(16, updateParticles);
    
    iInitializeSound();
    if (strcmp(gameState, "menu") == 0) {
        bgsound = iPlaySound("assets/sounds/mm.wav", true);
    }

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Enhanced Space Shooter Game");
    
    return 0;
}