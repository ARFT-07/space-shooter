#include "iGraphics.h"
#include "iSound.h"
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define NUM_VILLAINS 20
#define MAINMENUMAX 1171
#define MAX_BULLETS 25
#define MAX_ENEMY_BULLETS 15
#define MAX_EXPLOSIONS 12
#define MAX_POWERUPS 6
#define MAX_PARTICLES 80
#define PI 3.14159265359

// -------------------- Enhanced Structures --------------------
typedef struct {
    int x, y;
    int active;
    int timer;
    int type; // 0 = explosion, 1 = hit effect, 2 = power explosion
    int size;
} Explosion;

typedef struct {
    int x, y;
    int active;
    int type; // 0 = health, 1 = rapid fire, 2 = shield, 3 = multi-shot
    int timer;
    int speed;
} PowerUp;

typedef struct {
    float x, y;
    float vx, vy;
    int life;
    int active;
    int color[3]; // RGB
    float size;
} Particle;

typedef struct {
    float x, y;
    int active;
    int speed;
    int damage;
} EnemyBullet;

typedef struct {
    float x, y;
    int health;
    int maxHealth;
    int type; // 0 = asteroid, 1 = fighter, 2 = bomber, 3 = elite
    int active;
    
    // Movement pattern variables
    int movementType; // 0 = straight, 1 = sine wave, 2 = zigzag, 3 = spiral, 4 = circular
    float waveAmplitude;
    float waveFrequency;
    float wavePhase;
    float baseSpeed;
    float horizontalSpeed;
    int direction; 
    
   
    int fireTimer;
    int fireRate;
} Villain;

// -------------------- Global Variables --------------------
Image bgimage;
Image spaceship;
Image gamestateimage;
Image difficulty;
Image bullet_image, asteroid, villain1, villain2, villain3;
Image credits;
Image pause;
Image explosion_img, powerup_img, shield_img;
Image health_img, rapid_img, multishot_img;
Image enemy_bullet_img;
Image title;
Image diffbg;
Image easy;
Image medium;
Image hard;

int spaceshipX = 200, spaceshipY = 100;
Villain villains[NUM_VILLAINS];
int score_count = 0;
int high_score = 0;
bool gameOver_check = false;
int time_elapsed = 0;
int bg_X1 = 0, bg_X2 = SCREEN_WIDTH;
int bg_mm1 = 0, bg_mm2 = MAINMENUMAX;
int hovernewgame = 0;
int hoverdiff = 0;
int hoverhighscore = 0;
int hovercredits = 0;
int hoverquit = 0;
int hovereasy = 0;
int hovermedium = 0;
int hoverhard = 0;

int bulletX[MAX_BULLETS], bulletY[MAX_BULLETS];
int bulletVisible[MAX_BULLETS];
int bgsound = -1;

// Enhanced features
int player_health = 100;
int max_health = 100;
int shield_active = 0;
int shield_timer = 0;
int rapid_fire_active = 0;
int rapid_fire_timer = 0;
int multishot_active = 0;
int multishot_timer = 0;
int difficulty_level = 1;
int lives = 1;
int weapon_level = 1;

Explosion explosions[MAX_EXPLOSIONS];
PowerUp powerups[MAX_POWERUPS];
Particle particles[MAX_PARTICLES];
EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];

int yPos = 0; // Menu scroll position
int fallSpeed = 3;
int imageHeight = 1171;
int bulletSpeed = 25;
char gameState[20] = "menu"; // "menu", "game", "pause", "gameover"
int newadd = 2;
int exittime = 0;

bool level50Unlocked = false;
bool level100Unlocked = false;
bool level150Unlocked = false;
bool level200Unlocked = false;

// Global timers for consolidation
int gameTimer = 0;
int spawnTimer = 180; // Delay between villain spawns

// Names for credits
char playerName[50] = "Space Pilot";
char gameTitle[50] = "Cosmic Defenders";
char developerName[50] = "Created by: Game Dev Team";

// -------------------- Function Declarations --------------------
void initialize_Villains();
void initialize_Bullets();
void initialize_EnemyBullets();
void initialize_Explosions();
void initialize_PowerUps();
void initialize_Particles();
int  gameover(int ax, int ay, int sx, int sy);
void rollingBackground();
void show_time();
void fireBullets();
void drawBullets();
void updateBullets();
void updateEnemyBullets();
void drawEnemyBullets();
void fireEnemyBullet(float x, float y, int type);
void checkBulletVillainCollision();
void checkEnemyBulletPlayerCollision();
void updateExplosions();
void drawExplosions();
void createExplosion(int x, int y, int type);
void updatePowerUps();
void drawPowerUps();
void spawnPowerUp(int x, int y);
void checkPowerUpCollision();
void updateParticles();
void drawParticles();
void createParticles(int x, int y, int count, int type);
void drawHUD();
void drawGameOver();
void resetGame();
void saveHighScore();
void loadHighScore();
void updateVillains();
void drawVillains();
void initializeVillain(int index);
float getWaveOffset(Villain* v, float currentY);
void gameUpdateLoop(); // Consolidated update function
void createCircularCollisionEffect(int x, int y);

// -------------------- iDraw --------------------
void iDraw() {
    iClear();

    if (strcmp(gameState, "menu") == 0) {
        iShowImage(0, 0, "assets/images/mmbg3.jpg");
        iShowLoadedImage(500 , -100 , &title);

        yPos -= fallSpeed;
        if (yPos <= -imageHeight)
            yPos = 0;

        // Menu Texts with enhanced hover effects
        if (hovernewgame) {
            iSetColor(255, 255, 0);
            
            iShowImage(200, (1000 - 370), "assets/images/newgame_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 350), "assets/images/newgame_mm.png");
        }
        if (hoverdiff) {
            iSetColor(255, 255, 0);
            
            iShowImage(200, (1000 - 470), "assets/images/difficulty_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 450), "assets/images/difficulty_mm.png");
        }

        if (hoverhighscore) {
            iSetColor(255, 255, 0);
            
            iShowImage(200, (1000 - 570), "assets/images/highscore_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 550), "assets/images/highscore_mm.png");
        }

        if (hovercredits) {
            iSetColor(255, 255, 0);
            
            iShowImage(200, (1000 - 670), "assets/images/credits_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 650), "assets/images/credits_mm.png");
        }

        if (hoverquit) {
            iSetColor(255, 255, 0);
            
            iShowImage(200, (1000 - 770), "assets/images/quitgame_mm_hover.png");
        } else {
            iShowImage(200, (1000 - 750), "assets/images/quitgame_mm.png");
        }

       
    }

    if (strcmp(gameState, "difficulty") == 0) {

        iShowLoadedImage(0 , 0 , &diffbg);
        if(!hovereasy){
            iShowImage(150, 250 ,  "assets/images/easy.png" );
        }
        else iShowImage(150, 250 ,  "assets/images/easyhover.png" );
        
        if(!hovermedium){
             iShowImage(450, 250 ,  "assets/images/medium.png" );
        }
        else iShowImage(450, 250 ,  "assets/images/mediumhover.png" );
        
        
        if(!hoverhard){
             iShowImage(750, 250 ,  "assets/images/hard.png" );
        }
             
        else 
        iShowImage(750, 250 ,  "assets/images/hardhover.png" );


       
    }

    
    

    else if (strcmp(gameState, "game") == 0) {
        // Background
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        iShowLoadedImage(0, 0, &gamestateimage);
        iWrapImage(&gamestateimage, -5);

        
      

        
        iShowLoadedImage(spaceshipX, spaceshipY, &spaceship);
          if (shield_active) {
            iSetColor(0, 255, 255);
            for (int i = 0; i < 2; i++) {
                iCircle(spaceshipX + 100, spaceshipY + 100, 140 + i*5, 50);
            }
        }

        drawVillains();
        drawBullets();
        drawEnemyBullets();
        drawExplosions();
        drawPowerUps();
        drawParticles();
        drawHUD();
    }

    else if (strcmp(gameState, "pause") == 0) {
        // Draw game state first (frozen)
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &gamestateimage);
        iShowLoadedImage(spaceshipX, spaceshipY, &spaceship);
        drawVillains();

        // Semi-transparent overlay
        iSetColor(0, 0, 0);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Pause text with glow effect
        for (int i = 5; i > 0; i--) {
            iSetColor(255, 255, 255);
            iTextBold(SCREEN_WIDTH/2 - 100 + i, SCREEN_HEIGHT/2 + i, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        iSetColor(255, 255, 0);
        iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, "Press P to Resume", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if (strcmp(gameState, "gameover") == 0) {
        drawGameOver();
    }

    else if (strcmp(gameState, "exit") == 0) {
        iShowImage(0, 0, "assets/images/exit page.png");
        if (exittime >= 4) {
            saveHighScore();
            exit(0);
        }
    }
}

// -------------------- Enhanced Villain System --------------------
void initializeVillain(int index) {
    // Spawn villains from the right edge only
    villains[index].x = SCREEN_WIDTH + 50;
    villains[index].y = 100 + (rand() % (SCREEN_HEIGHT - 300));
    villains[index].active = 1;
    villains[index].direction = -1;
    
    // Determine villain type based on score
    int typeRand = rand() % 100;
    if (score_count >= 200 && typeRand < 15) {
        villains[index].type = 3; // Elite
        villains[index].health = 5;
        villains[index].maxHealth = 5;
        villains[index].fireRate = 90;
    } else if (score_count >= 100 && typeRand < 25) {
        villains[index].type = 2; // Bomber
        villains[index].health = 3;
        villains[index].maxHealth = 3;
        villains[index].fireRate = 120;
    } else if (score_count >= 50 && typeRand < 40) {
        villains[index].type = 1; // Fighter
        villains[index].health = 2;
        villains[index].maxHealth = 2;
        villains[index].fireRate = 150;
    } else {
        villains[index].type = 0; // Asteroid
        villains[index].health = 1;
        villains[index].maxHealth = 1;
        villains[index].fireRate = 0;
    }
    
    // Movement pattern assignment
    villains[index].movementType = rand() % 5;
    villains[index].waveAmplitude = 30 + rand() % 50;
    villains[index].waveFrequency = 0.01f + (rand() % 20) * 0.001f;
    villains[index].wavePhase = (rand() % 360) * PI / 180.0f;
    villains[index].baseSpeed = 2 + (score_count / 200);
    villains[index].horizontalSpeed = 3 + rand() % 10; // CHANGED: Much slower horizontal speed
    villains[index].fireTimer = rand() % (villains[index].fireRate + 1);
    
    if (villains[index].baseSpeed > 4) villains[index].baseSpeed = 4; // CHANGED: Reduced max speed
}

float getWaveOffset(Villain* v, float currentX) {
    float progress = (SCREEN_WIDTH - currentX) / (float)SCREEN_WIDTH;
    
    switch (v->movementType) {
        case 0: // Straight horizontal
            return 0;
        case 1: // Sine wave
            return v->waveAmplitude * sin(progress * v->waveFrequency * 50 + v->wavePhase);
        case 2: // Zigzag
            return v->waveAmplitude * ((int)(progress * 8) % 2 == 0 ? 1 : -1);
        case 3: // Spiral
            return v->waveAmplitude * sin(progress * v->waveFrequency * 100 + v->wavePhase) * progress;
        case 4: // Figure-8
            return v->waveAmplitude * sin(progress * v->waveFrequency * 50 + v->wavePhase) * 
                   cos(progress * v->waveFrequency * 25);
        default:
            return 0;
    }
}

void updateVillains() {
    for (int i = 0; i < NUM_VILLAINS; i++) {
        if (!villains[i].active) {
            if (spawnTimer <= 0 && rand() % 20 == 0) {
                initializeVillain(i);
                spawnTimer = 60 + rand() % 20;
            }
            continue;
        }
        
        float originalY = villains[i].y;
        
        // Update horizontal position
        villains[i].x += villains[i].horizontalSpeed * villains[i].direction;
        
        // Apply wave movement to Y position
        float waveOffset = getWaveOffset(&villains[i], villains[i].x);
        villains[i].y = originalY + waveOffset * 0.05f;
        
        // Boundary checking
        if (villains[i].x < -100) {
            villains[i].active = 0;
            continue;
        }
        
        // Keep villains within screen bounds vertically
        if (villains[i].y < 50) villains[i].y = 50;
        if (villains[i].y > SCREEN_HEIGHT - 100) villains[i].y = SCREEN_HEIGHT - 100;
        
        // AI firing for non-asteroid enemies
        if (villains[i].type > 0 && villains[i].fireRate > 0 && villains[i].fireTimer <= 0) {
            if (abs(villains[i].x - spaceshipX) < 400) {
                fireEnemyBullet(villains[i].x, villains[i].y, villains[i].type);
                villains[i].fireTimer = villains[i].fireRate + rand() % 60;
            }
        } else {
            villains[i].fireTimer--;
        }
        
        // Check collision with player using circular collision
        int dx = villains[i].x + 30 - (spaceshipX + 75);
        int dy = villains[i].y + 30 - (spaceshipY + 50);
        int distance = sqrt(dx*dx + dy*dy);
        
        if (distance < 60) { // Circular collision
            if (shield_active) {
                shield_active = 0;
                createCircularCollisionEffect(villains[i].x + 30, villains[i].y + 30);
                createParticles(villains[i].x + 30, villains[i].y + 30, 15, 1);
                villains[i].active = 0;
            } else {
                player_health -= 25;
                createCircularCollisionEffect(spaceshipX + 75, spaceshipY + 50);
                createParticles(spaceshipX + 75, spaceshipY + 50, 20, 2);
                villains[i].active = 0;
                
                if (player_health <= 0) {
                    lives--;
                    if (lives <= 0) {
                        strcpy(gameState, "gameover");
                        return;
                    } else {
                        player_health = max_health;
                        createExplosion(spaceshipX, spaceshipY, 2);
                    }
                }
            }
        }
    }
    
    if (spawnTimer > 0) spawnTimer--;
}

void drawVillains() {
    for (int i = 0; i < NUM_VILLAINS; i++) {
        if (villains[i].active) {
          
            if (villains[i].type == 0) {
                iShowLoadedImage(villains[i].x, villains[i].y, &asteroid);
            } else if (villains[i].type == 1) {
                iShowLoadedImage(villains[i].x, villains[i].y, &villain1);
            } else if (villains[i].type == 2) {
                iShowLoadedImage(villains[i].x, villains[i].y, &villain2);
            } else if (villains[i].type == 3) {
                iShowLoadedImage(villains[i].x, villains[i].y, &villain3);
            }
            
            // Health bar for multi-health enemies
            if (villains[i].maxHealth > 1) {
                int barWidth = 70;
                int barHeight = 8;
                
                iSetColor(0, 0, 0);
                iFilledRectangle(villains[i].x - 5, villains[i].y + 75, barWidth, barHeight);
                
                iSetColor(0, 255, 0);
                int healthWidth = (barWidth * villains[i].health) / villains[i].maxHealth;
                iFilledRectangle(villains[i].x - 5, villains[i].y + 75, healthWidth, barHeight);
                
                iSetColor(255, 255, 255);
                iRectangle(villains[i].x - 5, villains[i].y + 75, barWidth, barHeight);
            }
            
            // Type indicator glow
            if (villains[i].type == 3) {
                iSetColor(255, 0, 255);
                iCircle(villains[i].x + 35, villains[i].y + 35, 50);
            } else if (villains[i].type == 2) {
                iSetColor(255, 100, 100);
                iCircle(villains[i].x + 32, villains[i].y + 32, 45);
            }
        }
    }
}

// -------------------- Enhanced Bullet Systems --------------------
void fireEnemyBullet(float x, float y, int type) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!enemyBullets[i].active) {
            enemyBullets[i].x = x + 25;
            enemyBullets[i].y = y;
            enemyBullets[i].active = 1;
            enemyBullets[i].speed = 6 + type;
            enemyBullets[i].damage = type + 1;
            break;
        }
    }
}

void updateEnemyBullets() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            enemyBullets[i].x -= enemyBullets[i].speed; // CHANGED: Move horizontally left
            
            if (enemyBullets[i].x < -50) {
                enemyBullets[i].active = 0;
            }
        }
    }
}

void drawEnemyBullets() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            iSetColor(255, 100, 100);
            iFilledCircle(enemyBullets[i].x, enemyBullets[i].y, 5);
            iSetColor(255, 200, 200);
            iFilledCircle(enemyBullets[i].x, enemyBullets[i].y, 3);
        }
    }
}

void checkEnemyBulletPlayerCollision() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            // Circular collision detection
            int dx = enemyBullets[i].x - (spaceshipX + 75);
            int dy = enemyBullets[i].y - (spaceshipY + 50);
            int distance = sqrt(dx*dx + dy*dy);
            
            if (distance < 40) {
                enemyBullets[i].active = 0;
                
                if (shield_active) {
                    createCircularCollisionEffect(enemyBullets[i].x, enemyBullets[i].y);
                } else {
                    player_health -= enemyBullets[i].damage * 8;
                    createCircularCollisionEffect(spaceshipX + 75, spaceshipY + 50);
                    createParticles(spaceshipX + 75, spaceshipY + 50, 8, 2);
                    
                    if (player_health <= 0) {
                        lives--;
                        if (lives <= 0) {
                            strcpy(gameState, "gameover");
                            return;
                        } else {
                            player_health = max_health;
                        }
                    }
                }
            }
        }
    }
}

void fireBullets() {
    int shots = 1;
    if (multishot_active) shots = 3;
    else if (weapon_level >= 2) shots = 2;
    if (weapon_level >= 3) shots += 1;
    
    int spread = 20;
    int startOffset = -(shots - 1) * spread / 2;
    
    for (int shot = 0; shot < shots; shot++) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bulletVisible[i] == 0) {
                bulletX[i] = spaceshipX + 75 + startOffset + (shot * spread);
                bulletY[i] = spaceshipY + 100;
                bulletVisible[i] = 1;
                break;
            }
        }
    }
}

// -------------------- Enhanced Effects System --------------------
void createCircularCollisionEffect(int x, int y) {
    // Create expanding circular rings for collision effect
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].type = 1; // Collision effect type
            explosions[i].active = 1;
            explosions[i].timer = 20;
            explosions[i].size = 10;
            break;
        }
    }
}

void createParticles(int x, int y, int count, int type) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (rand() % 21 - 10) * 0.6f;
            particles[i].vy = (rand() % 21 - 10) * 0.6f;
            particles[i].life = 30 + rand() % 30;
            particles[i].active = 1;
            particles[i].size = 2 + rand() % 3;
            
            switch (type) {
                case 0: // Explosion
                    particles[i].color[0] = 255;
                    particles[i].color[1] = 100 + rand() % 156;
                    particles[i].color[2] = rand() % 100;
                    break;
                case 1: // Enemy destruction
                    particles[i].color[0] = 255;
                    particles[i].color[1] = 255;
                    particles[i].color[2] = 100 + rand() % 156;
                    break;
                case 2: // Player hit
                    particles[i].color[0] = 255;
                    particles[i].color[1] = rand() % 100;
                    particles[i].color[2] = rand() % 100;
                    break;
                default:
                    particles[i].color[0] = 255;
                    particles[i].color[1] = 255;
                    particles[i].color[2] = 255;
            }
            count--;
        }
    }
}

void updateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].vy -= 0.05f;
            particles[i].life--;
            particles[i].size *= 0.99f;
            
            if (particles[i].life <= 0 || particles[i].size < 0.5f) {
                particles[i].active = 0;
            }
        }
    }
}

void drawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            iSetColor(particles[i].color[0], particles[i].color[1], particles[i].color[2]);
            iFilledCircle((int)particles[i].x, (int)particles[i].y, (int)particles[i].size);
        }
    }
}


void checkBulletVillainCollision() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            for (int j = 0; j < NUM_VILLAINS; j++) {
                if (villains[j].active) {
                    // Circular collision detection
                    int dx = bulletX[i] - (villains[j].x + 30);
                    int dy = bulletY[i] - (villains[j].y + 30);
                    int distance = sqrt(dx*dx + dy*dy);
                    
                    if (distance < 50) {
                        bulletVisible[i] = 0;
                        villains[j].health--;
                        
                        createCircularCollisionEffect(bulletX[i], bulletY[i]);
                        createParticles(villains[j].x + 30, villains[j].y + 30, 5, 1);
                        
                        if (villains[j].health <= 0) {
                            int points = 0;
                            switch (villains[j].type) {
                                case 0: points = 10; break;
                                case 1: points = 25; break;
                                case 2: points = 50; break;
                                case 3: points = 100; break;
                            }
                            
                            createExplosion(villains[j].x + 30, villains[j].y + 30, 0);
                            createParticles(villains[j].x + 30, villains[j].y + 30, 15, 1);
                            spawnPowerUp(villains[j].x + 30, villains[j].y + 30);
                            
                            score_count += points;
                            
                            if (score_count >= 500 && weapon_level < 2) weapon_level = 2;
                            if (score_count >= 1000 && weapon_level < 3) weapon_level = 3;
                            
                            villains[j].active = 0;
                        }
                        break;
                    }
                }
            }
        }
    }
}

// -------------------- Enhanced Power-up System --------------------
void spawnPowerUp(int x, int y) {
    if (rand() % 100 < 15) {
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!powerups[i].active) {
                powerups[i].x = x;
                powerups[i].y = y;
                powerups[i].type = rand() % 4;
                powerups[i].active = 1;
                powerups[i].timer = 600;
                powerups[i].speed = 2 + rand() % 3; // CHANGED: Horizontal movement speed
                break;
            }
        }
    }
}

void updatePowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            powerups[i].x -= powerups[i].speed; // CHANGED: Move horizontally left
            powerups[i].timer--;
            
            if (powerups[i].x < -50 || powerups[i].timer <= 0) {
                powerups[i].active = 0;
            }
        }
    }
}

void drawPowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            // Pulsing glow effect
            int pulse = (int)(30 + 20 * sin(powerups[i].timer * 0.05));
            
            if (powerups[i].type == 0) { // Health
                iSetColor(0, 255, 0);
                iCircle(powerups[i].x, powerups[i].y, 30, pulse);
                iFilledRectangle(powerups[i].x - 15, powerups[i].y - 15, 30, 30);
                // Draw cross symbol
                iSetColor(255, 255, 255);
                iFilledRectangle(powerups[i].x - 2, powerups[i].y - 10, 4, 20);
                iFilledRectangle(powerups[i].x - 10, powerups[i].y - 2, 20, 4);
            } else if (powerups[i].type == 1) { // Rapid fire
                iSetColor(255, 255, 0);
                iCircle(powerups[i].x, powerups[i].y, 30, pulse);
                iFilledRectangle(powerups[i].x - 15, powerups[i].y - 15, 30, 30);
                iSetColor(0, 0, 0);
                iTextBold(powerups[i].x - 8, powerups[i].y - 5, "R", GLUT_BITMAP_TIMES_ROMAN_24);
            } else if (powerups[i].type == 2) { // Shield
                iShowLoadedImage(powerups[i].x , powerups[i].y ,&shield_img);
            } else { // Multishot
                iSetColor(255, 0, 255);
                iCircle(powerups[i].x, powerups[i].y, 30, pulse);
                iFilledRectangle(powerups[i].x - 15, powerups[i].y - 15, 30, 30);
                iSetColor(255, 255, 255);
                iTextBold(powerups[i].x - 8, powerups[i].y - 5, "M", GLUT_BITMAP_TIMES_ROMAN_24);
            }
        }
    }
}

void checkPowerUpCollision() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            // Circular collision detection
            int dx = powerups[i].x - (spaceshipX + 75);
            int dy = powerups[i].y - (spaceshipY + 50);
            int distance = sqrt(dx*dx + dy*dy);
            
            if (distance < 60) {
                powerups[i].active = 0;
                createParticles(powerups[i].x, powerups[i].y, 10, 1);
                
                switch (powerups[i].type) {
                    case 0: // Health
                        player_health = max_health;
                        break;
                    case 1: // Rapid fire
                        rapid_fire_active = 1;
                        rapid_fire_timer = 600;
                        break;
                    case 2: // Shield
                        shield_active = 1;
                        shield_timer = 450;
                        break;
                    case 3: // Multishot
                        multishot_active = 1;
                        multishot_timer = 400;
                        break;
                }
            }
        }
    }
}

// -------------------- Enhanced HUD --------------------
void drawHUD() {
    // Health bar
    iSetColor(100, 0, 0);
    iFilledRectangle(10, SCREEN_HEIGHT - 100, 250, 25);
    
    iSetColor(0, 255, 0);
    int healthWidth = (250 * player_health) / max_health;
    iFilledRectangle(10, SCREEN_HEIGHT - 100, healthWidth, 25);
    
    iSetColor(255, 255, 255);
    iRectangle(10, SCREEN_HEIGHT - 100, 250, 25);
    iTextBold(10, SCREEN_HEIGHT - 130, "Health", GLUT_BITMAP_TIMES_ROMAN_24);

    // Lives
    char livesStr[20];
    sprintf(livesStr, "Lives: %d", lives);
    iSetColor(255, 255, 255);
    iTextBold(10, SCREEN_HEIGHT - 160, livesStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Score
    char scoreStr[30];
    sprintf(scoreStr, "Score: %d", score_count);
    iSetColor(255, 215, 0);
    iTextBold(10, SCREEN_HEIGHT - 40, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // Time
    char timeStr[30];
    int min = time_elapsed / 60;
    int sec = time_elapsed % 60;
    sprintf(timeStr, "Time: %02d:%02d", min, sec);
    iSetColor(200, 200, 255);
    iTextBold(10, SCREEN_HEIGHT - 70, timeStr, GLUT_BITMAP_TIMES_ROMAN_24);

    // CHANGED: Simple power-up status indicators
    int indicatorY = SCREEN_HEIGHT - 40;
    if (shield_active) {
        iSetColor(0, 255, 255);
        iFilledCircle(SCREEN_WIDTH - 50, indicatorY, 15);
        iSetColor(0, 0, 0);
        iTextBold(SCREEN_WIDTH - 55, indicatorY - 5, "S", GLUT_BITMAP_TIMES_ROMAN_24);
        indicatorY -= 40;
    }
    if (rapid_fire_active) {
        iSetColor(255, 255, 0);
        iFilledCircle(SCREEN_WIDTH - 50, indicatorY, 15);
        iSetColor(0, 0, 0);
        iTextBold(SCREEN_WIDTH - 55, indicatorY - 5, "R", GLUT_BITMAP_TIMES_ROMAN_24);
        indicatorY -= 40;
    }
    if (multishot_active) {
        iSetColor(255, 0, 255);
        iFilledCircle(SCREEN_WIDTH - 50, indicatorY, 15);
        iSetColor(255, 255, 255);
        iTextBold(SCREEN_WIDTH - 55, indicatorY - 5, "M", GLUT_BITMAP_TIMES_ROMAN_24);
        indicatorY -= 40;
    }

    // Controls
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, 800, 35);
    iSetColor(255, 255, 255);
    iRectangle(0, 0, 800, 35);
    iSetColor(255, 255, 255);
    iTextBold(10, 15, "Arrow Keys: Move | Space: Shoot | P: Pause | Q: Quit", GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Player name
    iSetColor(255, 255, 255);
    iTextBold(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 160, playerName, GLUT_BITMAP_TIMES_ROMAN_24);
}

// -------------------- Enhanced Game Over Screen --------------------
void drawGameOver() {
    // Dark background with stars effect
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Random stars
    for (int i = 0; i < 100; i++) {
        iSetColor(255, 255, 255);
        int x = (i * 137) % SCREEN_WIDTH;
        int y = (i * 211) % SCREEN_HEIGHT;
        iFilledCircle(x, y, 1);
    }
    
    // Game Over text with glow effect
    for (int i = 8; i > 0; i--) {
        iSetColor(255 - i*20, 0, 0);
        iTextBold(SCREEN_WIDTH/2 - 120 + i, SCREEN_HEIGHT/2 + 150 + i, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    iSetColor(255, 255, 255);
    iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 150, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Player name
    iSetColor(255, 255, 255);
    char playerStr[60];
    sprintf(playerStr, "Pilot: %s", playerName);
    iTextBold(SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 110, playerStr, GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Statistics
    char finalScoreStr[50];
    sprintf(finalScoreStr, "Final Score: %d", score_count);
    iSetColor(255, 215, 0);
    iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 80, finalScoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
    
    char timeStr[50];
    int min = time_elapsed / 60;
    int sec = time_elapsed % 60;
    sprintf(timeStr, "Survival Time: %02d:%02d", min, sec);
    iSetColor(200, 200, 255);
    iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 40, timeStr, GLUT_BITMAP_TIMES_ROMAN_24);
    
    if (score_count > high_score) {
        // New high score celebration
        for (int i = 3; i > 0; i--) {
            iSetColor(255, 255, 0);
            iTextBold(SCREEN_WIDTH/2 - 140 + i, SCREEN_HEIGHT/2 - 40 + i, "NEW HIGH SCORE!", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        iSetColor(255, 255, 255);
        iTextBold(SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 40, "NEW HIGH SCORE!", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    
    // CHANGED: Added option to go to homepage
    iSetColor(255, 255, 255);
    iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 100, "Press R to Restart", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 130, "Press H for Homepage", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 160, "Press Q to Quit", GLUT_BITMAP_TIMES_ROMAN_24);
}

// -------------------- Initialization Functions --------------------
void initialize_Villains() {
    for (int i = 0; i < NUM_VILLAINS; i++) {
        villains[i].active = 0;
    }
}

void initialize_Bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bulletVisible[i] = 0;
    }
}

void initialize_EnemyBullets() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        enemyBullets[i].active = 0;
    }
}

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

// -------------------- Enhanced Explosion System --------------------
void createExplosion(int x, int y, int type) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].type = type;
            explosions[i].active = 1;
            explosions[i].timer = 30 + type * 10;
            explosions[i].size = 15 + type * 8;
            break;
        }
    }
}

void updateExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            explosions[i].timer--;
            explosions[i].size += 1;
            if (explosions[i].timer <= 0) {
                explosions[i].active = 0;
            }
        }
    }
}

void drawExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            // Circular explosion rings
            for (int ring = 0; ring < 3; ring++) {
                if (explosions[i].type == 0) { // Enemy explosion
                    iSetColor(255, 200 - ring*50, 0);
                } else if (explosions[i].type == 1) { // Hit effect - circular rings
                    iSetColor(255, 255, 100 + ring*50);
                } else { // Power explosion
                    iSetColor(255, 0, 255 - ring*80);
                }
                iCircle(explosions[i].x, explosions[i].y, explosions[i].size - ring*3, 20 - ring*5);
            }
        }
    }
}

// -------------------- Consolidated Game Update Loop --------------------
void gameUpdateLoop() {
    if (strcmp(gameState, "game") == 0) {
        gameTimer++;
        
        // Update all game systems
        updateBullets();
        updateEnemyBullets();
        updateVillains();
        updateExplosions();
        updatePowerUps();
        updateParticles();
        
        // Check all collisions
        checkBulletVillainCollision();
        checkEnemyBulletPlayerCollision();
        checkPowerUpCollision();
        
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
        
        if (multishot_active) {
            multishot_timer--;
            if (multishot_timer <= 0) {
                multishot_active = 0;
            }
        }
    }
}

// -------------------- Game Management --------------------
void resetGame() {
    score_count = 0;
    time_elapsed = 0;
    player_health = max_health;
    lives = 3;
    shield_active = 0;
    rapid_fire_active = 0;
    multishot_active = 0;
    weapon_level = 1;
    spaceshipX = 200;
    spaceshipY = 100;
    gameTimer = 0;
    spawnTimer = 120;
    
    initialize_Villains();
    initialize_Bullets();
    initialize_EnemyBullets();
    initialize_Explosions();
    initialize_PowerUps();
    initialize_Particles();
    
    // Reset unlock flags
    level50Unlocked = false;
    level100Unlocked = false;
    level150Unlocked = false;
    level200Unlocked = false;
    newadd = 2;
}

void saveHighScore() {
    if (score_count > high_score) {
        high_score = score_count;
        FILE* file = fopen("highscore.txt", "w");
        if (file) {
            fprintf(file, "%d", high_score);
            fclose(file);
        }
    }
}

void loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        if (fscanf(file, "%d", &high_score) != 1) {
            high_score = 0;
        }
        fclose(file);
    } else {
        high_score = 0;
    }
}

// -------------------- Utility Functions --------------------
int gameover(int ax, int ay, int sx, int sy) {
    int dx = ax - (sx + 75);
    int dy = ay - (sy + 50);
    return sqrt(dx*dx + dy*dy) < 60; // Circular collision
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

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            bulletX[i] += bulletSpeed; // CHANGED: Move horizontally right
            if (bulletX[i] > SCREEN_WIDTH + 50) {
                bulletVisible[i] = 0;
            }
        }
    }
}

void drawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletVisible[i] == 1) {
            // Enhanced bullet graphics
            iSetColor(0, 255, 255);
            iFilledCircle(bulletX[i], bulletY[i], 4);
            iSetColor(255, 255, 255);
            iFilledCircle(bulletX[i], bulletY[i], 2);
            
            // Bullet trail
            iSetColor(0, 200, 255);
            iFilledCircle(bulletX[i] - 8, bulletY[i], 2);
            iSetColor(0, 150, 255);
            iFilledCircle(bulletX[i] - 16, bulletY[i], 1);
        }
    }
}

// -------------------- Timer Functions --------------------
void show_time() {
    if (strcmp(gameState, "game") == 0) {
        time_elapsed++;
    }
    else if (strcmp(gameState, "exit") == 0) {
        exittime++;
    }
}

// -------------------- Input Handlers --------------------
void iMouse(int button, int state, int mx, int my) {
    if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) {
        if (strcmp(gameState, "menu") == 0) {
            if (((my >= 650 && my <= 720))) {
                resetGame();
                strcpy(gameState, "game");
            }

            if (((my >= 250 && my <= 320))) {
                strcpy(gameState, "exit");
                time_elapsed = 0;
            }



            if (((my >= 550 && my <= 620))) {
                strcpy(gameState, "difficulty");
                time_elapsed = 0;
            }
        }
        
        else if (strcmp(gameState, "difficulty") == 0) {

      if (mx >= 145 && mx <= 400) {
        difficulty_level = 1;
        strcpy(gameState, "menu");
      }
      if (mx >= 445 && mx <= 700) {
        difficulty_level = 2;
        strcpy(gameState, "menu");
      }

      if (mx >= 745 && mx <= 1000) {
        difficulty_level = 3;
        strcpy(gameState, "menu");
      }

    }



        else if (strcmp(gameState, "gameover") == 0) {
            resetGame();
            strcpy(gameState, "game");
        }
    }
}

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
        // CHANGED: Added homepage option
        if (key == 'h' || key == 'H') {
            strcpy(gameState, "menu");
        }
    }
}

void iMouseMove(int mx, int my) {

    printf("%d %d", mx , my);
    if (strcmp(gameState, "menu") == 0) {
        hovernewgame = (my >= 650 && my <= 720) ? 1 : 0;
        hoverdiff = (my >= 550 && my <= 620) ? 1 : 0;
        hoverhighscore = (my >= 450 && my <= 520) ? 1 : 0;
        hovercredits = (my >= 350 && my <= 420) ? 1 : 0;
        hoverquit = (my >= 250 && my <= 320) ? 1 : 0;
    }

    if (strcmp(gameState, "difficulty") == 0) {
        hovereasy = (mx >= 145 && mx <= 400) ? 1 : 0;
        hovermedium = (mx >= 445 && mx <= 700) ? 1 : 0;
        hoverhard = (mx >= 745 && mx <= 1000) ? 1 : 0;
        
    }
    
}

void iMouseDrag(int mx, int my) {
    // Not used
}

void iMouseWheel(int dir, int mx, int my) {
    // Not used
}

void iSpecialKeyboard(unsigned char key) {
    if (key == GLUT_KEY_END)
        exit(0);

    if (strcmp(gameState, "game") == 0) {
        int moveSpeed = 15;
        if (key == GLUT_KEY_LEFT && spaceshipX > 0)
            spaceshipX -= moveSpeed;
        if (key == GLUT_KEY_RIGHT && spaceshipX < SCREEN_WIDTH - 150)
            spaceshipX += moveSpeed;
        if (key == GLUT_KEY_UP && spaceshipY < SCREEN_HEIGHT - 100)
            spaceshipY += moveSpeed;
        if (key == GLUT_KEY_DOWN && spaceshipY > 10)
            spaceshipY -= moveSpeed;
    }
}

// -------------------- Resource Loading --------------------
void loadResources() {
    iLoadImage(&gamestateimage, "assets/images/bg_horizontal.png");
    iLoadImage(&spaceship, "assets/images/ss5.png");
    iLoadImage(&bullet_image, "assets/images/bullets1.png");
    iLoadImage(&villain1, "assets/images/villain3.png");
    iLoadImage(&villain2, "assets/images/villain2_x.png");
    iLoadImage(&villain3, "assets/images/villain1.png");
    iLoadImage(&asteroid, "assets/images/asteroids1.png");
    iLoadImage(&shield_img, "assets/images/shield.png");
    iLoadImage(&health_img, "assets/images/health.png");
    iLoadImage(&rapid_img, "assets/images/rapid.png");
    iLoadImage(&multishot_img, "assets/images/multishot.png");
    iLoadImage(&enemy_bullet_img, "assets/images/enemy_bullet.png");
    iLoadImage(&title, "assets/images/title.png" );
    iLoadImage(&diffbg, "assets/images/diffbg.png" );
    iLoadImage(&easy, "assets/images/easy.png" );
    iLoadImage(&medium, "assets/images/medium.png" );
    iLoadImage(&hard, "assets/images/hard.png" );


}

// -------------------- Main Function --------------------
int main(int argc, char *argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    
    loadResources();
    loadHighScore();
    
    initialize_Villains();
    initialize_Bullets();
    initialize_EnemyBullets();
    initialize_Explosions();
    initialize_PowerUps();
    initialize_Particles();
    
    // Maximum 8 timers as requested
    iSetTimer(8, rollingBackground);        // 1. Background scrolling
    iSetTimer(1000, show_time);             // 2. Time counter  
    iSetTimer(16, gameUpdateLoop);          // 3. Main game update loop (consolidated)
    
    // Additional timers (total = 8 max)
    iSetTimer(100, NULL);                   // 4. Reserved
    iSetTimer(200, NULL);                   // 5. Reserved  
    iSetTimer(300, NULL);                   // 6. Reserved
    iSetTimer(400, NULL);                   // 7. Reserved
    iSetTimer(500, NULL);                   // 8. Reserved
    
    iInitializeSound();
    if (strcmp(gameState, "menu") == 0) {
        bgsound = iPlaySound("assets/sounds/mm.wav", true);
    }

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, gameTitle);
    
    return 0;
}