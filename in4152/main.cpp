//
//  main.cpp
//  in4152
//
//  Created by Arkka Dhiratara on 7/7/16.
//  Copyright © 2016 Dhiratara. All rights reserved.
//

#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "SOIL.h"
#include <glm/glm.hpp>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "trackball.h"
#include "argumentParser.h"

#include <stdio.h>
#include <string.h>
#include <vector>

/**
 * Data Model
 */

struct Material {
    GLfloat Ka [4];
    GLfloat Kd[4];
    GLfloat Ks[4];
    GLfloat e[4];
    GLfloat n;
} Material;

struct Player {
    glm::vec3 pos;
    glm::vec3 move;
    glm::vec3 mouse;
    
    float angle;
    float acceleration;
    
    bool isDead;
    int hp;
    
};

struct Enemy {
    glm::vec3 pos;
    glm::vec3 move;
    
    float angle;
    float acceleration;
    
    int type;
    
    bool isDead;
    int hp;
};

struct Bullet {
    glm::vec3 origin;
    glm::vec3 pos;
    glm::vec3 move;
    float angle;
    float acceleration;
    
    int type; // 0 from player, 1 from enemy
    
    bool isDestroyed;
};



/**
 * Initialize Data Model
 */

Player player;


//////Predefined global variables

//Use the enum values to define different rendering modes
//The mode is used by the function display and the mode is
//chosen during execution with the keys 1-9
enum DisplayModeType {TRIANGLE=1, FACE=2, CUBE=3, ARM=4, MESH=5,};

DisplayModeType DisplayMode = TRIANGLE;

unsigned int screenWidth = 1920;  // screen width
unsigned int screenHeight = 800;  // screen height


// Light pos
//GLfloat lightPosition[] = {5.0, 5.0, 10.0, 0.0};
GLfloat lightPosition[]= { 5.0f, 5.0f, 10.0f, 1.0f };
GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightAmbient[] = {0.1, 0.1, 0.1, 1.0};
GLfloat lightShininess[] = { 50.0 };

GLfloat globalAmbient[] = { 0.2, 0.2, 0.2, 1.0 };

std::vector<float> MeshVertices;
std::vector<unsigned int> MeshTriangles;

// Declare your own global variables here:

// Generated terrain
std::vector<glm::vec3> heightmap;
std::vector<glm::vec3> basemap;

// Game Logic

std::vector<struct Bullet> bullets;
std::vector<struct Enemy> enemies;
int maxEnemies = 5;
bool isBoss = false;

// Options
bool antiAlias = false;
GLfloat	xrot;				// X Rotation
GLfloat	yrot;				// Y Rotation


// Texture
GLuint	texture[5];

// Mesh
float x1l, x2l, y1l, y2l, z1l, z2l;

GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info
GLfloat winX, winY; //variables to hold screen x,y,z coordinates
GLfloat winZ = -5; // Default win Z coord
GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
GLdouble worldLimitX, worldLimitY, worldLimitZ;

// Material
struct Material matCopper {
    {0.19225, 0.0735, 0.0225}, // Ka
    {0.7038, 0.27048, 0.0828}, // Kd
    {0.256777, 0.137622, 0.086014}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.1 // n
};

struct Material matChrome {
    {0.25, 0.25,	0.25}, // Ka
    {0.4,	0.4,	0.4}, // Kd
    {0.774597,	0.774597,	0.774597}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.6 // n
};

struct Material matRuby {
    {0.1745, 0.01175,	0.01175}, // Ka
    {0.61424,	0.04136,	0.04136}, // Kd
    {0.727811,	0.626959,	0.626959}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.6 // n
};

struct Material matGold {
    {0.24725,	0.1995,	0.0745}, // Ka
    {0.75164,	0.60648,	0.22648}, // Kd
    {0.628281,	0.555802,	0.366065}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.4 // n
};

struct Material matGreenBullet {
    {0.0,	0.0,	0.0}, // Ka
    {0.1,	0.35,	0.1}, // Kd
    {0.45,	0.55,	0.45}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.25 // n
};

struct Material matRedBullet {
    {0.0,	0.0,	0.0}, // Ka
    {0.5,	0.0,	0.0}, // Kd
    {0.7,	0.7,	0.7}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.25 // n
};

////////// MATERIAL
void setMaterial(struct Material mat) {
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, mat.Ka);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat.Kd);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat.Ks);
    glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, mat.e);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mat.n * 128);
}

GLuint loadTexture(const char* texture)                                    // Load Bitmaps And Convert To Textures
{
    /* load an image file directly as a new OpenGL texture */
    GLuint textureId = SOIL_load_OGL_texture
    (
     texture,
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS
     );
    
    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    return textureId;
}

////////// Terrain
//void generateMountain(glm::vec3 base, float width, float height) {
//    
//    glm::vec3 baseTemp = base;
//    float widthTempt = width;
//    float heightTemp = height;
//    
//    glBegin(GL_LINE_STRIP);
//    for(float i=0;i<=360;i+=1)
//    {
//        baseTemp.x += heightTemp;
//        baseTemp.y += heightTemp * sin(i * M_PI / 180.0f);
//        
//        printf("i: %d, x: %f, y: %f\n",i,baseTemp.x, baseTemp.y);
//        
//        glVertex3f(baseTemp.x,baseTemp.y,baseTemp.z);
//    }
//    glEnd();
//    
//    
//    baseTemp = base;
//    widthTempt = width;
//    heightTemp = height - 0.01;
//    
//    glBegin(GL_LINE_STRIP);
//    for(float i=0;i<=360;i+=1)
//    {
//        baseTemp.x += heightTemp;
//        baseTemp.y += heightTemp * sin(i * M_PI / 180.0f);
//        
//        printf("i: %d, x: %f, y: %f\n",i,baseTemp.x, baseTemp.y);
//        
//        glVertex3f(baseTemp.x + 2,baseTemp.y,baseTemp.z);
//    }
//    glEnd();
//
//    
//}
void generateMountain(glm::vec3 peak, float width, float height) {
    glm::vec3 start = glm::vec3(peak.x - width/2,0,0);
    glm::vec3 end = glm::vec3(peak.x + width/2,0,0);
    
    setMaterial(matCopper);
    
    glm::vec3 vert = start;
    float widthTemp = width;
    float heightTemp = height;
    
    glBegin(GL_QUAD_STRIP);
    for(float i=0;i<=360;i+=1)
    {
        vert.x += heightTemp;
        vert.y += heightTemp * sin(i * M_PI / 180.0f);
            
            
        glVertex3f(vert.x, vert.y/10 * 1, vert.z + 2);
        glVertex3f(vert.x, vert.y, vert.z);
          
    }
    glEnd();
     
}
void generateMountainBase(float start, float end) {
    // Mountain consists of three layers: large mountain, medium, and some small/hills
    
    /*
    for(int i=start; i<end; i++)
    {
        glm::vec3 base = glm::vec3((-10+(i+1)*3.0),0.0,(-10+(i+1)*3.0));
        basemap.push_back(base);
        generateMountain(base);
        
    }
    */
    
    glm::vec3 peak = glm::vec3(0.0,2.0,0);
    
    // big
    generateMountain(peak, 2, 0.02);
    
    // medium
    //generateMountain(base, 2, 0.01);
}


////////// Movement
float easingAmount = 0.05;
bool isLeftKeyPressed = false;
bool isRightKeyPressed = false;
bool isDownKeyPressed = false;
bool isUpKeyPressed = false;

std::vector<glm::vec3> computeMovement(glm::vec3 from, glm::vec3 to, bool boundary=false, float acceleration=0.05, int minDistance=0) {
    std::vector<glm::vec3> vec;
    
    bool isOutBoundary = false;
    
    float distanceX = to.x - from.x;
    float distanceY = to.y - from.y;
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
    
    if(boundary){
        if(from.x >= worldLimitX) to.x = from.x - acceleration * 10;
        if(from.x <= -worldLimitX) to.x = from.x + acceleration * 10;
        if(from.y >= worldLimitY) to.y = from.y - acceleration * 10;
        if(from.y <= -worldLimitY) to.y = from.y + acceleration * 10;
    }
    
    if (distance > minDistance) {
        from.x += distanceX * acceleration;
        from.y += distanceY * acceleration;
    }
    
    vec.push_back(from);
    vec.push_back(to);
    
    return vec;
}

float computeAngle(glm::vec2 v1, glm::vec2 v2)
{
    glm::vec2 base = glm::vec2(v2.x, v1.y);
    
    return (0 - atan2(v1.y-v2.y, v2.x-v1.x)) * (180 / M_PI);
    
}

void updatePlayerMouseMovement() {
    // Limit motion to screen size
    if((player.mouse.y<=screenHeight && player.mouse.y>=0)&&(player.mouse.x<=screenWidth && player.mouse.x>=0)){
  
        //printf("cord at %f,%f\n",player.mouse.x,player.mouse.y);
        

        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        glGetIntegerv( GL_VIEWPORT, viewport ); //Lokasi dari kamera [x,y,panjang,lebar]
        
        winX = (float) player.mouse.x;
        winY = (float)viewport[3] - (float) player.mouse.y;
        glReadPixels( player.mouse.x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
        
        // get world coord based on mouse
        gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
        //printf("world cord at %f,%f,%f\n",worldX,worldY, worldZ);
        
        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
        glVertex3f(worldX,worldY,worldZ);
        
        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
        glVertex3f(worldX,player.pos.y,worldZ);

        
        
        player.angle = computeAngle(glm::vec2(player.pos.x, player.pos.y), glm::vec2(worldX, worldY));
        
        // Debug mouse angle
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        
        // miring
        glBegin(GL_LINES);
        glColor3f(1,1,0);
        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
        glVertex3f(worldX,worldY,worldZ);
        glEnd();
        
        
        // y
        glBegin(GL_LINES);
        glColor3f(0,1,0);
        glVertex3f(worldX,player.pos.y,player.pos.z);
        glVertex3f(worldX,worldY,worldZ);
        glEnd();
        
        // x
        glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
        glVertex3f(worldX,player.pos.y,worldZ);
        glEnd();
        glPopAttrib();

        
    }
}

float randomRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

bool checkCollide(glm::vec3 a, float aWidth, float aHeight, glm::vec3 b, float bWidth, float bHeight)
{
    float x1Min = a.x;
    float x1Max = a.x+aWidth;
    float y1Max = a.y+aHeight;
    float y1Min = a.y;
    
    float x2Min = b.x;
    float x2Max = b.x+bWidth;
    float y2Max = b.y+bHeight;
    float y2Min = b.y;
    
    // Collision tests
    if( x1Max < x2Min || x1Min > x2Max ) {
        return false;
    }
    if( y1Max < y2Min || y1Min > y2Max ) {
        return false;
    }
    
    return true;
}

void fireBullet(glm::vec3 pos, glm::vec3 move, float angle, int type = 0, float acceleration = 0.10) {
    
    
    struct Bullet bullet {
        pos, //origin
        pos, // pos
        move, // move
        angle, // angle
        acceleration, // acceleration
        type, // type
        false, // isDestroyed
    };

    
    bullets.push_back(bullet);
}


//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(float length=1)
{
    //draw simply colored axes
    
    //remember all states of the GPU
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //deactivate the lighting state
    glDisable(GL_LIGHTING);
    //draw axes
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(length,0,0);
    
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,length,0);
    
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,length);
    glEnd();
    
    //reset to previous state
    glPopAttrib();
}

void drawLight() {
    glPushMatrix();
    glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
    glutSolidSphere(1,10,10);
    glPopMatrix();
}

void drawSky() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR);
    
    glPushMatrix();
    glColor3d(0.52,0.8,0.92);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 5.0f, -10.0f);
    glVertex3f(-10.0f, 5.0f, -10.0f);
    glEnd();

    
    glPopMatrix();
    
    glPopAttrib();
}

void drawTerrain() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR);
    glColor3f(1,1,0);
    
    //setMaterial(matRuby);
    
    glPushMatrix();
    
    
    glBegin(GL_QUADS);
    // front
    glVertex3f(-10.0f, 2.0f, -2.0f);
    glVertex3f(10.0f, 2.0f, -2.0f);
    glVertex3f(10.0f, 0.0f, -2.0f);
    glVertex3f(-10.0f, 0.0f, -2.0f);
    
    glEnd();
    
    
    glPopMatrix();
    glPopAttrib();

}

void drawWater() {
    glPushMatrix();
    //setMaterial(matCopper);
    
    //glEnable(GL_COLOR);
    //glColor3d(1,0,0);
    
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -2.0f, -10.0f);
    glVertex3f( 10.0f, -2.0f, -10.0f);
    glVertex3f( 10.0f, -5.0f, 10.0f);
    glVertex3f(-10.0f, -5.0f, 10.0f);
    glEnd();
    
    glPopMatrix();

}

void drawBullets() {
    for (int i=0; i < bullets.size(); i++) {
        // printf("Bullet %f,  cos %f, sin %f\n", bullets[i].angle, cos(bullets[i].angle * M_PI / 180.0f), sin(bullets[i].angle * M_PI / 180.0f));
        
        
        // check boundary

        if((bullets[i].pos.x >= worldLimitX)
           ||(bullets[i].pos.x <= -worldLimitX)
           ||(bullets[i].pos.y >= worldLimitY)
           ||(bullets[i].pos.y <= -worldLimitY)) {
            bullets[i].isDestroyed = true;
        }

        if(!bullets[i].isDestroyed) {
            // Check collision
            
            // enemy got hit ?
            if(bullets[i].type == 0) {
                for (int j=0; j < enemies.size(); j++) {
                    if(checkCollide(bullets[i].pos, 0.1, 0.1, enemies[j].pos, 0.1, 0.1)) {
                        enemies[j].hp -= 1;
                        bullets[i].isDestroyed = true;
                        
                        if(enemies[j].hp<=0) {
                            enemies[j].hp = 0;
                            enemies[j].isDead = true;
                        }
                        printf("Enemy HP: %d/%d\n",enemies[j].hp, 5);
                        break;  
                    }
                }
                
            }
            // player got hit ?
            else if(bullets[i].type == 1 && checkCollide(bullets[i].pos, 0.1, 0.1, player.pos, 0.1, 0.1)) {
                player.hp -= 1;
                bullets[i].isDestroyed = true;
                
                if(player.hp<=0) {
                    player.hp = 0;
                    player.isDead = true;
                }
                printf("Player HP: %d/%d\n",player.hp, 20);
            }

            
            
            
            
            bullets[i].pos.x += bullets[i].acceleration * cos(bullets[i].angle * M_PI / 180.0f);
            bullets[i].pos.y += bullets[i].acceleration * sin(bullets[i].angle * M_PI / 180.0f);
            
            glPushMatrix();
            if(bullets[i].type == 0) setMaterial(matGreenBullet);
            else if(bullets[i].type == 1) setMaterial(matRedBullet);
            
            glTranslated(bullets[i].pos.x, bullets[i].pos.y, 0);
            glutSolidCube(0.05);
            
            glPopMatrix();
        }
    }
}

void drawEnemies()
{
    for (int i=0; i < enemies.size(); i++) {
        
        // check collide with player
        if(checkCollide(player.pos, 0.2, 0.2, enemies[i].pos, 0.2, 0.2)) {
            // Suicide bomb!
            player.isDead = true;
            enemies[i].isDead = true;
        }
        
        if(!enemies[i].isDead) {
            // check collide with other
            bool collide = false;
            struct Enemy collideWith;
            
            for (int j=0; j < enemies.size(); j++) {
                if(i!=j && checkCollide(enemies[i].pos, 1, 1, enemies[j].pos, 1, 1)) {
                    collide = true;
                    collideWith = enemies[j];
                    break;
                } else collide = false;
            }
            
            if(collide) {
                glm::vec3 distance = enemies[i].pos - collideWith.pos;
                enemies[i].pos += distance * enemies[i].acceleration;
                
            } else {
                // This enemy follow player position..
                
                enemies[i].move = player.pos;
            }
            
            
            // Movement
            std::vector<glm::vec3> moveVec = computeMovement(enemies[i].pos, enemies[i].move, false, enemies[i].acceleration, enemies[i].type);
            enemies[i].pos = moveVec[0];
            enemies[i].move = moveVec[1];
            
            //printf("enemy pos %d: %f %f\n", i+1, enemies[i].pos.x, enemies[i].pos.y);
            
            // Rotation Angle
            enemies[i].angle = computeAngle(glm::vec2(enemies[i].pos.x, enemies[i].pos.y), glm::vec2(player.pos.x, player.pos.y));
            
            
            // Fire bullets
            if(randomRange(0,1000)>990) fireBullet(enemies[i].pos, player.pos,enemies[i].angle, 1);
            
            // Draw
            glPushMatrix();
            //setMaterial(matChrome);
            
            glTranslated(enemies[i].pos.x, enemies[i].pos.y, 0);
            // Seamless rotation
            glRotatef(enemies[i].angle, 0, 0, 1);
            if(enemies[i].angle > 45 && enemies[i].angle <= 135)  glRotatef((enemies[i].angle - 45) * 2, 1, 0, 0);
            else if (enemies[i].angle > 135) glRotatef(180, 1, 0, 0);
            
            if(enemies[i].angle < -45 && enemies[i].angle >= -135) glRotatef((enemies[i].angle + 45) * -2, 1, 0, 0);
            else if(enemies[i].angle < -135) glRotatef(-180, 1, 0, 0);
            
            glutSolidTeapot(0.1);
            
            glPopMatrix();
        }
        
        
    }
    
    
}

void drawPlayer()
{
    // Mouse movement relative to player
    updatePlayerMouseMovement();
    // printf("%f\n", player.angle);
    
    // Update player object movement
    std::vector<glm::vec3> moveVec = computeMovement(player.pos, player.move, true);
    player.pos = moveVec[0];
    player.move = moveVec[1];

    glPushMatrix();
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture[0] );
    setMaterial(matChrome);
    
    glTranslated(player.pos.x, player.pos.y, 0);
    
    // Seamless player rotation
    glRotatef(player.angle, 0, 0, 1);
    if(player.angle > 45 && player.angle <= 135)  glRotatef((player.angle - 45) * 2, 1, 0, 0);
    else if (player.angle > 135) glRotatef(180, 1, 0, 0);
    
    if(player.angle < -45 && player.angle >= -135) glRotatef((player.angle + 45) * -2, 1, 0, 0);
    else if(player.angle < -135) glRotatef(-180, 1, 0, 0);
    
    // Draw object
    glutSolidTeapot(1);
    
//    
//    
//    for (unsigned int i = 0; i < MeshTriangles.size(); i += 3) {
//        
//        
//        x1l = MeshVertices[MeshTriangles[i + 1] * 3] - MeshVertices[MeshTriangles[i] * 3];
//        y1l= MeshVertices[MeshTriangles[i + 1] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
//        z1l = MeshVertices[MeshTriangles[i + 1] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
//        
//        x2l = MeshVertices[MeshTriangles[i + 2] * 3] - MeshVertices[MeshTriangles[i] * 3];
//        y2l = MeshVertices[MeshTriangles[i + 2] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
//        z2l = MeshVertices[MeshTriangles[i + 2] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
//        
//        glNormal3f(y1l * z2l - y2l * z1l, z1l * x2l - z2l * x1l, x1l * y2l - x2l * y1l);
//        
//        glBegin(GL_TRIANGLES);
//        glVertex3f(MeshVertices[MeshTriangles[i]*3],
//                   MeshVertices[MeshTriangles[i]*3+1],
//                   MeshVertices[MeshTriangles[i]*3+2]
//                   );
//        glVertex3f(MeshVertices[MeshTriangles[i+1]*3],
//                   MeshVertices[MeshTriangles[i+1]*3 + 1],
//                   MeshVertices[MeshTriangles[i+1]*3 + 2]
//                   );
//        glVertex3f(MeshVertices[MeshTriangles[i+2]*3],
//                   MeshVertices[MeshTriangles[i+2]*3 + 1],
//                   MeshVertices[MeshTriangles[i+2]*3 + 2]
//                   );
//        glEnd();
//    }
    
    glPopMatrix();
}

void drawMesh()
{
    //1) use the mesh data structure;
    //each triangle is defined with 3 consecutive indices in the MeshTriangles table
    //these indices correspond to vertices stored in the MeshVertices table.
    //provide a function that draws these triangles.
    
    //2) compute the normals of these triangles
    
    //3) try computing a normal per vertex as the average of the adjacent face normals
    // call glNormal3f with the corresponding values before each vertex
    // What do you observe with respect to the lighting?
    
    //4) try loading your own model (export it from Blender as a Wavefront obj) and replace the provided mesh file.
    
    //cout << MeshTriangles[1] << "\n";
    
    for (unsigned int i = 0; i < MeshTriangles.size(); i += 3) {
        
        
        x1l = MeshVertices[MeshTriangles[i + 1] * 3] - MeshVertices[MeshTriangles[i] * 3];
        y1l= MeshVertices[MeshTriangles[i + 1] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
        z1l = MeshVertices[MeshTriangles[i + 1] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
        
        x2l = MeshVertices[MeshTriangles[i + 2] * 3] - MeshVertices[MeshTriangles[i] * 3];
        y2l = MeshVertices[MeshTriangles[i + 2] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
        z2l = MeshVertices[MeshTriangles[i + 2] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
        
        glNormal3f(y1l * z2l - y2l * z1l, z1l * x2l - z2l * x1l, x1l * y2l - x2l * y1l);
        
        glBegin(GL_TRIANGLES);
        glVertex3f(MeshVertices[MeshTriangles[i]*3],
                   MeshVertices[MeshTriangles[i]*3+1],
                   MeshVertices[MeshTriangles[i]*3+2]
                   );
        glVertex3f(MeshVertices[MeshTriangles[i+1]*3],
                   MeshVertices[MeshTriangles[i+1]*3 + 1],
                   MeshVertices[MeshTriangles[i+1]*3 + 2]
                   );
        glVertex3f(MeshVertices[MeshTriangles[i+2]*3],
                   MeshVertices[MeshTriangles[i+2]*3 + 1],
                   MeshVertices[MeshTriangles[i+2]*3 + 2]
                   );
        glEnd();
    }
}

void drawCube(){
    glPushMatrix();
    
    glRotated(xrot, 1, 0, 0);
    glRotated(yrot, 0, 1, 0);
    xrot+=1;
    yrot+=1;
    
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    
    glBegin(GL_QUADS);
    // Front Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, -0.5f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 5.0f, -0.5f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 5.0f,  0.5f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f,  0.5f,  1.0f);  // Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glEnd();
     
    glPopMatrix();
}


void spawnEnemy() {
    struct Enemy enemy {
        glm::vec3(randomRange(0, 15),randomRange(-15, 15),0), // pos
        glm::vec3(0,0,0), // move
        0, // angle
        0.01, // acceleration
        (int) randomRange(0, 5), // type
        false, // is dead
        5 // hp
        
    };
    
    enemies.push_back(enemy);
}




void display( )
{
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);		// Setup The Ambient Light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);		// Setup The Diffuse Light
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);   // Specular
    glLightfv(GL_LIGHT0, GL_SHININESS, lightShininess);   // Specular
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	// Position The Light
    
    

    
    // Light
    //drawLight();
    
    if(!player.isDead) drawPlayer();
    //drawCube();
    
    
    /*
    
    

    // Game Logic
    
    // normal mobs
    if(enemies.size()< maxEnemies && !isBoss) {
        spawnEnemy();
    }
    
    
    
    
    
    // End of Game Logic
    
    // Draw Environments
    drawSky();
    generateMountainBase(-5,5);
    //drawTerrain();
    //drawWater();
    
    // Draw Units
    //drawCoordSystem();
    
    
    
    drawEnemies();
    drawBullets();
     */
    
    glFlush ();
    
}


/**
 * Animation
 */
void animate( )
{
    //tri_x = tri_x + inc;
}


/**
 * Mouse
 */

void mouseClick( int button, int state, int x, int y )
{
    if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN )
    {
        fireBullet(player.pos, glm::vec3(worldX,worldY,0),player.angle);
        
        
    }
    
    
}

void mouseMotion( int x, int y )
{
    player.mouse.x = x;
    player.mouse.y = y;
    
}

/**
 * Keyboard
 */

void keyboard(unsigned char key, int x, int y)
{
    //printf("key %d pressed at %d,%d\n",key,x,y);
    fflush(stdout);
    
    if ((key>='1')&&(key<='9'))
    {
        DisplayMode= (DisplayModeType) (key-'0');
        return;
    }
    
    switch (key)
    {
        case 27:     // touche ESC
            exit(0);
        
        case 32:
            fireBullet(player.pos, glm::vec3(worldX,worldY,0),player.angle);
            break;
            
        case 'a':
            player.move.x -= player.acceleration;
            break;
            
        case 'd':
            player.move.x += player.acceleration;
            break;
            
        case 's':
            player.move.y -= player.acceleration;
            break;
            
        case 'w':
            player.move.y += player.acceleration;
            break;
            
        case 'i':
            //turn AA on
            lightPosition[0] += 1.00;
            break;
        case 'I':
            //turn AA off
            lightPosition[0] -= 1.00;
            break;
        case 'o':
            //turn AA on
            lightPosition[1] += 1.00;
            break;
        case 'O':
            //turn AA off
            lightPosition[1] -= 1.00;
            break;
        case 'p':
            //turn AA on
            lightPosition[2] += 1.00;
            break;
        case 'P':
            //turn AA off
            lightPosition[2] -= 1.00;
            break;
    }
    
    printf("light pos %f,%f,%f\n", lightPosition[0], lightPosition[1], lightPosition[2]);
}

void keyboardSpecial(int key, int x, int y) {
    printf("key: %d\n",key);
    
    switch (key) {
        case GLUT_KEY_LEFT:
            isLeftKeyPressed = true;
            if (!isRightKeyPressed) {
                player.move.x -= player.acceleration;
            }
            break;
        case GLUT_KEY_RIGHT:
            isRightKeyPressed = true;
            if (!isLeftKeyPressed) {
                player.move.x += player.acceleration;
            }
            break;
            
        case GLUT_KEY_DOWN:
            isDownKeyPressed = true;
            if (!isUpKeyPressed) {
                player.move.y -= player.acceleration;            }
            break;
            
        case GLUT_KEY_UP:
            isUpKeyPressed = true;
            if (!isDownKeyPressed) {
                player.move.y += player.acceleration;
            }
            break;
            
    }
}

void keyboardSpecialUp(int key, int x, int y) {
    printf("key: %d\n",key);
    switch (key) {
        case GLUT_KEY_LEFT:
            isLeftKeyPressed = false;
            break;
        case GLUT_KEY_RIGHT:
            isRightKeyPressed = false;
            break;
        case GLUT_KEY_UP:
            isUpKeyPressed = false;
            break;
        case GLUT_KEY_DOWN:
            isDownKeyPressed = false;
            break;
    }
}



//Nothing needed below this point
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING





void displayInternal(void);
void reshape(int w, int h);
bool loadMesh(const char * filename);
void init()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Texture
    texture[0] = loadTexture("textures/yellow.bmp");
    //if(!loadTexture(0, "textures/crate.bmp")) printf("Load  texture failed!!!!\n");
    //if(!loadTexture(1, "textures/test.bmp")) printf("Load  texture failed!!!!\n");
    
    glEnable(GL_NORMALIZE);
    
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST );
    //glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    
    // MESHES
    loadMesh("/Users/arkkadhiratara/Desktop/3DCG/in4152/in4152/David.obj");
    
    // Initialize scene
    worldLimitX = 8;
    worldLimitY = 3;
    
    // Initialize player
    player.pos = glm::vec3(-2,0,0);
    player.move = player.pos;
    player.acceleration = 0.5;
    player.hp = 20;
    player.isDead = false;
}


//load mesh
//this code is NOT how you should load a mesh... it is a bit hacky...
void centerAndScaleToUnit (std::vector<float> & vertices)
{
    if (vertices.size()%3!=0)
    {
        cout<<"ERROR while loading!!!\n";
        return;
    }
    
    float x=0;
    float y=0;
    float z=0;
    for  (unsigned int i = 0; i < vertices.size (); i+=3)
    {
        x += vertices[i];
        y += vertices[i+1];
        z += vertices[i+2];
    }
    
    
    x /= vertices.size ()/3;
    y /= vertices.size ()/3;
    z /= vertices.size ()/3;
    
    float maxD = 0;
    for (unsigned int i = 0; i < vertices.size (); i+=3){
        float dX= (vertices[i]-x);
        float dY= (vertices[i+1]-y);
        float dZ= (vertices[i+2]-z);
        
        float m = sqrt(dX*dX+dY*dY+dZ*dZ);
        if (m > maxD)
            maxD = m;
    }
    float center[]={x,y,z};
    for  (unsigned int i = 0; i < vertices.size (); i+=3)
    {
        vertices[i] = (vertices[i] - x) / maxD;
        vertices[i+1] = (vertices[i+1] - y) / maxD;
        vertices[i+2] = (vertices[i+2] - z) / maxD;
    }
}

bool loadMesh(const char * filename)
{
    const unsigned int LINE_LEN=256;
    char s[LINE_LEN];
    FILE * in;
#ifdef WIN32
    errno_t error=fopen_s(&in, filename,"r");
    if (error!=0)
#else
        in = fopen(filename,"r");
    if (!(in))
#endif
        return false;
    
    //temp stuff
    float x, y, z;
    std::vector<int> vhandles;
    
    
    
    while(in && !feof(in) && fgets(s, LINE_LEN, in))
    {
        // vertex
        if (strncmp(s, "v ", 2) == 0)
        {
            if (sscanf(s, "v %f %f %f", &x, &y, &z))
                MeshVertices.push_back(x);
            MeshVertices.push_back(y);
            MeshVertices.push_back(z);
        }
        // face
        else if (strncmp(s, "f ", 2) == 0)
        {
            int component(0), nV(0);
            bool endOfVertex(false);
            char *p0, *p1(s+2); //place behind the "f "
            
            vhandles.clear();
            
            while (*p1 == ' ') ++p1; // skip white-spaces
            
            while (p1)
            {
                p0 = p1;
                
                // overwrite next separator
                
                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' &&
                       *p1 != ' ' && *p1 != '\0')
                    ++p1;
                
                // detect end of vertex
                if (*p1 != '/') endOfVertex = true;
                
                // replace separator by '\0'
                if (*p1 != '\0')
                {
                    *p1 = '\0';
                    p1++; // point to next token
                }
                
                // detect end of line and break
                if (*p1 == '\0' || *p1 == '\n')
                    p1 = 0;
                
                
                // read next vertex component
                if (*p0 != '\0')
                {
                    switch (component)
                    {
                        case 0: // vertex
                            vhandles.push_back(atoi(p0)-1);
                            break;
                            
                        case 1: // texture coord
                            //assert(!vhandles.empty());
                            //assert((unsigned int)(atoi(p0)-1) < texcoords.size());
                            //_bi.set_texcoord(vhandles.back(), texcoords[atoi(p0)-1]);
                            break;
                            
                        case 2: // normal
                            //assert(!vhandles.empty());
                            //assert((unsigned int)(atoi(p0)-1) < normals.size());
                            //_bi.set_normal(vhandles.back(), normals[atoi(p0)-1]);
                            break;
                    }
                }
                
                ++component;
                
                if (endOfVertex)
                {
                    component = 0;
                    nV++;
                    endOfVertex = false;
                }
            }
            
            
            if (vhandles.size()>3)
            {
                //model is not triangulated, so let us do this on the fly...
                //to have a more uniform mesh, we add randomization
                unsigned int k=(false)?(rand()%vhandles.size()):0;
                for (unsigned int i=0;i<vhandles.size()-2;++i)
                {
                    MeshTriangles.push_back(vhandles[(k+0)%vhandles.size()]);
                    MeshTriangles.push_back(vhandles[(k+i+1)%vhandles.size()]);
                    MeshTriangles.push_back(vhandles[(k+i+2)%vhandles.size()]);
                }
            }
            else if (vhandles.size()==3)
            {
                MeshTriangles.push_back(vhandles[0]);
                MeshTriangles.push_back(vhandles[1]);
                MeshTriangles.push_back(vhandles[2]);
            }
            else
            {
                std::cout<<"TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face \n";
            }
        }
        memset(&s, 0, LINE_LEN);
    }
    cout << MeshTriangles.size();
    fclose(in);
    centerAndScaleToUnit (MeshVertices);
    return true;
}



/**
 * Programme principal
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH );
    
    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(screenWidth,screenHeight);
    glutCreateWindow(argv[0]);
    
    init( );
    
    // Initialize viewpoint
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,winZ);
    tbInitTransform();     
    tbHelp();

    // cablage des callback
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    glutSpecialUpFunc(keyboardSpecialUp);
    glutDisplayFunc(displayInternal);
    glutMouseFunc(mouseClick);    // traqueboule utilise la souris
    glutPassiveMotionFunc(mouseMotion);  // traqueboule utilise la souris
    glutIdleFunc(animate);
    
    // lancement de la boucle principale
    glutMainLoop();
    
    return 0;  // instruction jamais exécutée
}

/**
 * Fonctions de gestion opengl à ne pas toucher
 */
// Actions d'affichage
// Ne pas changer
void displayInternal(void)
{
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT); // la couleur et le z
    
    glLoadIdentity();  // repere camera
    
    tbVisuTransform(); // origine et orientation de la scene
    
    display( );    
    
    glutSwapBuffers();
    glutPostRedisplay();
}
// pour changement de taille ou desiconification
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-worldLimitX, worldLimitX, -worldLimitY, worldLimitY, -1000.0, 1000.0);
    //gluPerspective (50, (float)w/h, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}

