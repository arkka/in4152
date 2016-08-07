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
    GLfloat Ke[4];
    GLfloat n;
} Material;

struct Mountain {
    glm::vec3 pos;
    float width;
    float rot;
    GLuint tex;
    struct Material mat;
};

struct Player {
    glm::vec3 pos;
    glm::vec3 move;
    glm::vec3 mouse;
    
    float angle;
    float acceleration;
    
    bool isDead;
    int hp;
    
};

struct Boss {
    std::vector<glm::vec3> pos;
    glm::vec3 move;
    
    float angle;
    float acceleration;
    
    bool isDead;
    int hp;
    int partNum;
    float partDistance;
    
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
Boss boss;


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
GLfloat lightPosition[]= { 5.0f, 20.0f, 40.0f, 1.0f };
GLfloat lightDiffuse[] = {1.0, 1.0, 0.0, 1.0};
GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightAmbient[] = {0.1, 0.1, 0.1, 1.0};
GLfloat lightShininess[] = { 50.0 };

GLfloat globalAmbient[] = { 0.2, 0.2, 0.2, 1.0 };

// Declare your own global variables here:

// Generated terrain
std::vector<struct Mountain> mountains;

// Game Logic

std::vector<struct Bullet> bullets;
std::vector<struct Enemy> enemies;
int maxEnemies = 5;
int curEnemies = maxEnemies;
bool isBoss = false;

// Options
bool antiAlias = false;
GLfloat	xrot;				// X Rotation
GLfloat	yrot;				// Y Rotation
GLfloat mountainX = 0;




// Mesh
std::vector<float> MeshVertices, MeshVerticesDist, MeshVerticesCollapse;
std::vector<unsigned int> MeshTriangles, SimplifiedMeshTriangles;

// Mesh Simplification
std::vector<float> SimplifiedMeshVertices; // old format
std::vector<glm::vec3> SMeshVertices; // new format using vec3
std::vector<std::vector<glm::vec3>> SMeshTriangles, SMeshFaces; // new format using vec3
std::vector<std::vector<glm::vec3>> SMeshVerticesNeighbors;
std::vector<bool> SMeshVerticesCollapse; // new format using vec3
std::vector<float> SMeshVerticesDistance; // new format using vec3


std::vector<float> bossVertices, pokeVertices;
std::vector<unsigned int> bossTriangles, pokeTriangles;

float x1l, x2l, y1l, y2l, z1l, z2l;

GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info
GLfloat winX, winY; //variables to hold screen x,y,z coordinates
GLfloat winZ = -5; // Default win Z coord
GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
GLdouble worldLimitX, worldLimitY, worldLimitZ;

// Texture
GLuint texSky, texWater, texGrass, texStone;
GLuint	texWhite, texArmy, texGreen, texAluminium, texBullet, texSkull;

// Material
struct Material matArmy {
    {1.000000, 1.000000, 1.000000}, // Ka
    {0.640000, 0.640000, 0.640000}, // Kd
    {0.5, 0.5, 0.5}, // Ks
    {0.0, 0.0, 0.0, 1.0}, //Ke
    0.1 // n
};

struct Material matStone {
    {0.05375,	0.05,	0.06625}, // Ka
    {0.18275,	0.17,	0.22525}, // Kd
    {0.332741,	0.328634,	0.346435}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.4 // n
};

struct Material matGrass {
    {0.0,	0.0,	0.0}, // Ka
    {0.1,	0.35,	0.1}, // Kd
    {0.45,	0.55,	0.45}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.25 // n
};

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

struct Material matSkull {
    {0.25, 0.25,	0.25}, // Ka
    {0.4,	0.4,	0.4}, // Kd
    {0.774597,	0.774597,	0.774597}, // Ks
    {0.0, 0.0, 0.0, 1.0},
    0.6 // n
};

float randomRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

////////// MATERIAL
void setMaterial(struct Material mat) {
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, mat.Ka);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat.Kd);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat.Ks);
    glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, mat.Ke);
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
    
    return textureId;
}

void generateMountains(float start, float end) {
    // big mountain

    
    float width = 10;
    for(int x = start;x<end;x++) {
        if(randomRange(0, 100)>90) {
            struct Mountain mountain {
                glm::vec3(x,randomRange(7,10)/10,randomRange(-80,-60)),
                width,
                0,
                texStone,
                matStone
            };
            mountains.push_back(mountain);
        }
       
    }
    
    // medium mountain
    width = 7;
    for(int x = start;x<end;x++) {
        if(randomRange(0, 100)>80) {
            struct Mountain mountain {
                glm::vec3(x,randomRange(2,6)/10,randomRange(-50,-10)),
                width,
                0,
                texStone,
                matStone
            };
            mountains.push_back(mountain);
        }
        
    }

    

}
void drawMountain(glm::vec3 peak, float width, GLuint texId) {
    glm::vec3 start = glm::vec3(peak.x - width/2,0,peak.z);
    glm::vec3 end = glm::vec3(peak.x + width/2,0,peak.z);
    glm::vec3 distance = end - start;
    
    float sliceX = distance.x / 360;
    
    glm::vec3 vert = start;

    //glRotated(xrot, 1, 0, 0);
    //xrot++;
    //glRotated(45, 0, 1, 0);
    //yrot++;
    
    
    glBindTexture( GL_TEXTURE_2D, texStone );
    setMaterial(matStone);
    
    int slice = 4;

    for(int curSlice = 0; curSlice<slice; curSlice++){
        vert = start;
    
        glBegin(GL_QUAD_STRIP);
        
        for(float i=0;i<=360;i+=1)
        {
            vert.x += sliceX;
            vert.y += (sliceX * sin(i * M_PI / 180.0f)) * peak.y;
            
            float nextX = vert.x + sliceX;
            float nextY = vert.y * (1 - (curSlice+1)/slice);
            float nextZ = vert.z - ((curSlice+1)/slice) * 5;
            
            
            glNormal3f(0, 0, 1);
            
            glTexCoord2f((sliceX*i/distance.x),1); glVertex3f(vert.x, vert.y, vert.z);
            glTexCoord2f((sliceX*i/distance.x),0); glVertex3f(vert.x, nextY, nextZ);
            
        }
        glEnd();
    }

}
void drawMountains() {
    // Mountain consists of three layers: large mountain, medium, and some small/hills
    
    glPushMatrix();
    mountainX-= 0.01;
    glTranslated(mountainX, -1, 0);

    for(int i=0;i<mountains.size();i++) {
        glPushMatrix();
        glRotated(mountains[i].rot, 0, 1, 0);
        drawMountain(mountains[i].pos, mountains[i].width, mountains[i].tex);
        glPopMatrix();
    }
    
    glPopMatrix();
    
}


////////// Movement
float easingAmount = 0.05;
bool isLeftKeyPressed = false;
bool isRightKeyPressed = false;
bool isDownKeyPressed = false;
bool isUpKeyPressed = false;

std::vector<glm::vec3> computeMovement(glm::vec3 from, glm::vec3 to, bool boundary=false, float acceleration=0.05, float minDistance=0) {
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
//        glPushAttrib(GL_ALL_ATTRIB_BITS);
//        glDisable(GL_LIGHTING);
//        
//        // miring
//        glBegin(GL_LINES);
//        glColor3f(1,1,0);
//        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
//        glVertex3f(worldX,worldY,worldZ);
//        glEnd();
//        
//        
//        // y
//        glBegin(GL_LINES);
//        glColor3f(0,1,0);
//        glVertex3f(worldX,player.pos.y,player.pos.z);
//        glVertex3f(worldX,worldY,worldZ);
//        glEnd();
//        
//        // x
//        glBegin(GL_LINES);
//        glColor3f(1,0,0);
//        glVertex3f(player.pos.x,player.pos.y,player.pos.z);
//        glVertex3f(worldX,player.pos.y,worldZ);
//        glEnd();
//        glPopAttrib();

        
    }
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

void spawnBoss() {
    isBoss = true;
    boss.isDead = false;
    boss.hp = 20;
    boss.pos.clear();
    glm::vec3 part = glm::vec3(randomRange(0, 10),-10,0);
    
    for(int i=0;i<boss.partNum;i++) {
        part.x += boss.partDistance;
        boss.pos.push_back(part);
    }
    
    boss.move = player.pos;
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

bool simplifyMesh() {
    printf("\n Original Vertices: %lu", MeshVertices.size()/3);
    printf("\n Original Triangles: %lu \n", MeshTriangles.size()/3);
    
//    for (unsigned int i = 0; i < MeshTriangles.size()/3; i ++) {
//        printf("TRIANGLES: %i\n", MeshTriangles[i]);
//    }
    
    // 1. register to new format
    for (unsigned int i = 0; i < MeshTriangles.size(); i += 3) {
        glm::vec3 vertex = glm::vec3(MeshVertices[MeshTriangles[i]],MeshVertices[MeshTriangles[i+1]],MeshVertices[MeshTriangles[i+2]]);
        
        // push
        SMeshVertices.push_back(vertex);
        
        
        std::vector<glm::vec3> smvn;
        // push empty neighbor placeholder
        SMeshVerticesNeighbors.push_back(smvn);
        
        // push faces
        SMeshFaces.push_back(smvn);
        
        // push default colapse state
        SMeshVerticesCollapse.push_back(false);
        
        // push default dist
        SMeshVerticesDistance.push_back(1000000);
    }
    
    printf("\n Simplified Vertices-1: %lu \n", SMeshVertices.size());
    
    // 2. build triangles
    for (unsigned int i = 0; i < SMeshVertices.size(); i+= 3) {
        std::vector<glm::vec3> vertex;
        
        // Triangle
        vertex.push_back(SMeshVertices[i]);
        vertex.push_back(SMeshVertices[i+1]);
        vertex.push_back(SMeshVertices[i+2]);
        
        SMeshTriangles.push_back(vertex);
        
        for(int j=0;j<3;j++) {
            SMeshFaces[i] = vertex;
            
            for(int k=0;k<3;k++) if(j!=k) {
                long neighborsCount = std::count(SMeshVerticesNeighbors[i].begin(), SMeshVerticesNeighbors[i].end(), vertex[j]);
                if(neighborsCount > 0) {
//                    printf(" (%f,%f,%f \n) already registered\n", vertex[j].x, vertex[j].y, vertex[j].z);
//                    
//                    for(int l=0; l<SMeshVerticesNeighbors[i].size(); l++) {
//                        printf("      -->  (%f,%f,%f)\n", SMeshVerticesNeighbors[i][l].x, SMeshVerticesNeighbors[i][l].y, SMeshVerticesNeighbors[i][l].z );
//                    }
                    
                } else {
//                    printf(" (%f,%f,%f) == neighbors == (%f,%f,%f) \n", SMeshVertices[i].x, SMeshVertices[i].y, SMeshVertices[i].z, vertex[j].x, vertex[j].y, vertex[j].z );
                    SMeshVerticesNeighbors[i].push_back(vertex[j]);
                }
            }
        }
    }
    
    // 3. Determine cost on each vertex
    
    int debugTrue = 0;
    int debugFalse = 0;
    
    for (unsigned int i = 0; i < SMeshVertices.size(); i++) {
        if(SMeshVerticesNeighbors[i].size() == 0) {
            // no neighbors
            SMeshVerticesDistance[i] = -0.01f;
            SMeshVerticesCollapse[i] = true;
            
            debugTrue++;
            
        } else {
            SMeshVerticesDistance[i] = 1000000; //arbitrary max length
            SMeshVerticesCollapse[i] = false;
            
            debugFalse++;
            
            for(int j=0;j< SMeshVerticesNeighbors[i].size();j++){
                float dist;
                
                glm::vec3 diff = SMeshVerticesNeighbors[i][j] - SMeshVertices[i];
                float edgelength = sqrtf(dot(diff, diff));
                float curvature=0;
                
                
                
                
                std::vector<glm::vec3> sides;
                for (unsigned int k = 0; k<SMeshFaces[i].size(); k++) {
                    if(SMeshFaces[i][k] == SMeshVerticesNeighbors[i][j]){
                        sides.push_back(SMeshFaces[i][k]);
                    }
                }
                
                
                // STILL LOT OF BUG USING THIS APPROACH.. too many looping.. :(
                for (unsigned int k = 0; k<SMeshFaces[i].size(); k++) {
                    float mincurv=1; // curve for face i and closer side to it
                    for (unsigned int l = 0; l<sides.size(); l++) {
                        float dotprod = dot(SMeshFaces[i][k] , sides[l]);	  // use dot product of face normals.
                        mincurv = std::min(mincurv,(1-dotprod)/2.0f);
                    }
                    curvature = std::max(curvature, mincurv);
                }
                
                dist = edgelength * curvature;
                
                printf(" (%f,%f,%f) == %f  == (%f,%f,%f) \n", SMeshVerticesNeighbors[i][j].x, SMeshVerticesNeighbors[i][j].y, SMeshVerticesNeighbors[i][j].z, dist, SMeshVertices[i].x, SMeshVertices[i].y, SMeshVertices[i].z);
                
                if(dist<SMeshVerticesDistance[i]) {
                    
                    v->collapse=v->neighbor[i];  // candidate for edge collapse
                    v->objdist=dist;             // cost of the collapse
                }
                
            }
    
            
        }
    
    }
    
    printf("TRUE: %i, FALSE: %i\n", debugTrue, debugFalse);
    
    // Export back to old format/variable
    
    //for(
    
    
//
//    int numReductions = 0;
//    // Last, ported ti back to old format :D
//    for (unsigned int i = 0; i < SMeshVertices.size(); i++) {
//        //printf("Distance %f\n",SMeshVerticesDistance[i]);
//        if(SMeshVerticesCollapse[i]) numReductions++;
//    }
//    
//    printf("Simplified Vertices: %lu \n", SMeshVertices.size());
//    printf("Simplified Vertices reduction: %i \n", numReductions);
    
   
    
    return true;
}

void drawMesh(std::vector<float> MeshVertices, std::vector<unsigned int> MeshTriangles)
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
    
    glPushMatrix();
    
    
    
    for (unsigned int i = 0; i < MeshTriangles.size(); i += 3) {
        
        
        x1l = MeshVertices[MeshTriangles[i + 1] * 3] - MeshVertices[MeshTriangles[i] * 3];
        y1l= MeshVertices[MeshTriangles[i + 1] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
        z1l = MeshVertices[MeshTriangles[i + 1] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
        
        x2l = MeshVertices[MeshTriangles[i + 2] * 3] - MeshVertices[MeshTriangles[i] * 3];
        y2l = MeshVertices[MeshTriangles[i + 2] * 3 + 1] - MeshVertices[MeshTriangles[i] * 3 + 1];
        z2l = MeshVertices[MeshTriangles[i + 2] * 3 + 2] - MeshVertices[MeshTriangles[i] * 3 + 2];
        
        glNormal3f(y1l * z2l - y2l * z1l, z1l * x2l - z2l * x1l, x1l * y2l - x2l * y1l);
        
        
        
        glBegin(GL_TRIANGLES);
        //glTexCoord2f(0.0f, 0.0f);
        glVertex3f(MeshVertices[MeshTriangles[i]*3],
                   MeshVertices[MeshTriangles[i]*3+1],
                   MeshVertices[MeshTriangles[i]*3+2]
                   );
        //glTexCoord2f(0.0f, 1.0f);
        glVertex3f(MeshVertices[MeshTriangles[i+1]*3],
                   MeshVertices[MeshTriangles[i+1]*3 + 1],
                   MeshVertices[MeshTriangles[i+1]*3 + 2]
                   );
        //glTexCoord2f(1.0f, 1.0f);
        glVertex3f(MeshVertices[MeshTriangles[i+2]*3],
                   MeshVertices[MeshTriangles[i+2]*3 + 1],
                   MeshVertices[MeshTriangles[i+2]*3 + 2]
                   );
        glEnd();
        
        
    }
    
    glPopMatrix();
}


void drawSky() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    
    glPushMatrix();
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texSky );
    //setMaterial(matChrome);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, -1.0f, -100.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 10.0f, -1.0f, -100.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 10.0f, 5.0f, -100.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 5.0f, -100.0f);
    glEnd();
    
    
    //glutSolidTeapot(2);
    
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glPopAttrib();
}

void drawTerrain() {
    glPushMatrix();
    
    
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texGrass );
    setMaterial(matStone);
    
    
    glBegin(GL_QUADS);
    // front
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 2.0f, -2.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, 2.0f, -2.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 0.0f, -2.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 0.0f, -2.0f);
    glEnd();
    
    glPopMatrix();
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
            
           
            if(bullets[i].type == 0) {
                
                if(!boss.isDead && checkCollide(bullets[i].pos, 0.1, 0.1, boss.pos[0], 0.5, 0.5)) {
                    boss.hp -= 1;
                    bullets[i].isDestroyed = true;
                    
                    if(boss.hp<=0) {
                        boss.hp = 0;
                        boss.isDead = true;
                    }
                    printf("Boss HP: %d/%d\n",boss.hp, 20);
                }
                
                // enemy got hit?
                for (int j=0; j < enemies.size(); j++) {
                    if(!enemies[j].isDead && checkCollide(bullets[i].pos, 0.1, 0.1, enemies[j].pos, 0.3, 0.3)) {
                        enemies[j].hp -= 1;
                        bullets[i].isDestroyed = true;
                        
                        if(enemies[j].hp<=0) {
                            enemies[j].hp = 0;
                            enemies[j].isDead = true;
                            curEnemies--;
                        }
                        printf("Enemy HP: %d/%d\n",enemies[j].hp, 5);
                        break;  
                    }
                }
                
            }
            // player got hit ?
            else if(bullets[i].type == 1 && checkCollide(bullets[i].pos, 0.1, 0.1, player.pos, 0.4, 0.4)) {
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

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texBullet );
            

            if(bullets[i].type == 0) setMaterial(matGold);
            else if(bullets[i].type == 1) setMaterial(matRuby);
            
            glTranslated(bullets[i].pos.x, bullets[i].pos.y, 0);
            glRotatef(bullets[i].angle,0,0,1);
            
            glBegin(GL_QUADS);
            glNormal3f(0,0,1);
            glTexCoord2f(0.0f, 0.8f); glVertex3f(-0.1f, -0.05f, 0.0f);
            glTexCoord2f(0.6f, 0.8f); glVertex3f( 0.1f, -0.05f, 0.0f);
            glTexCoord2f(0.6f, 0.2f); glVertex3f( 0.1f, 0.05f, 0.0f);
            glTexCoord2f(0.0f, 0.2f); glVertex3f(-0.1f, 0.05f, 0.0f);
            
            glTexCoord2f(0.6f, 0.8f); glVertex3f(0.1f, -0.05f, 0.0f);
            glTexCoord2f(0.8f, 0.8f); glVertex3f(0.25f, -0.02f, 0.0f);
            glTexCoord2f(0.8f, 0.2f); glVertex3f(0.25f, 0.02f, 0.0f);
            glTexCoord2f(0.6f, 0.2f); glVertex3f(0.1f, 0.05f, 0.0f);
            
            glEnd();
            


            glPopMatrix();
        }
    }
}

void drawEnemies()
{
    for (int i=0; i < enemies.size(); i++) {
        
        // check collide with player
        if(!enemies[i].isDead && checkCollide(player.pos, 0.2, 0.2, enemies[i].pos, 0.2, 0.2)) {
            // Suicide bomb!
            player.isDead = true;
            enemies[i].isDead = true;
            curEnemies--;
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
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texWhite );
            setMaterial(matChrome);
            
            glTranslated(enemies[i].pos.x, enemies[i].pos.y, 0);
            // Seamless rotation
            glRotatef(enemies[i].angle, 0, 0, 1);
            if(enemies[i].angle > 45 && enemies[i].angle <= 135)  glRotatef((enemies[i].angle - 45) * 2, 1, 0, 0);
            else if (enemies[i].angle > 135) glRotatef(180, 1, 0, 0);
            
            if(enemies[i].angle < -45 && enemies[i].angle >= -135) glRotatef((enemies[i].angle + 45) * -2, 1, 0, 0);
            else if(enemies[i].angle < -135) glRotatef(-180, 1, 0, 0);
            
            glutSolidTeapot(0.3);
            
            glPopMatrix();
        }
        
        
    }
    
    
}

void drawPlayer()
{
    if(!player.isDead) {
        // Movement
        updatePlayerMouseMovement();
        std::vector<glm::vec3> moveVec = computeMovement(player.pos, player.move, true);
        player.pos = moveVec[0];
        player.move = moveVec[1];
        
        // TEXTURE AND MATERIAL
        glPushMatrix();
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texArmy );
        setMaterial(matChrome);
        
        // Apply movement
        glTranslated(player.pos.x, player.pos.y, 0);
        
        // Apply seamless rotation
        glRotatef(player.angle, 0, 0, 1);
        if(player.angle > 45 && player.angle <= 135)  glRotatef((player.angle - 45) * 2, 1, 0, 0);
        else if (player.angle > 135) glRotatef(180, 1, 0, 0);
        
        if(player.angle < -45 && player.angle >= -135) glRotatef((player.angle + 45) * -2, 1, 0, 0);
        else if(player.angle < -135) glRotatef(-180, 1, 0, 0);
        
        glutSolidTeapot(0.4);
        
        glPopMatrix();
    }
    
}

void drawBossTail(float r, float divisions) {
    float x, y, z, dTheta=180/divisions, dLon=360/divisions, degToRad=3.14/180 ;
    
    for(float lat =0 ; lat <=180 ; lat+=dTheta)
    {
        glBegin( GL_QUAD_STRIP ) ;
        for(float lon = 0 ; lon <=360; lon+=dLon)
        {
            
            x = r*cosf(lat * degToRad) * sinf(lon * degToRad) ;
            y = r*sinf(lat * degToRad) * sinf(lon * degToRad) ;
            z = r*cosf(lon * degToRad) ;
            
            glNormal3f( x, y, z) ;
            glVertex3f( x, y, z ) ;
            
            x = r*cosf((lat + dTheta) * degToRad) * sinf(lon * degToRad) ;
            y = r*sinf((lat + dTheta) * degToRad) * sinf(lon * degToRad) ;
            z = r*cosf( lon * degToRad ) ;
            
            glNormal3f( x, y, z ) ;
            glVertex3f( x, y, z ) ;
        }
        glEnd() ;
        
    }
}

void drawBoss()
{
    std::vector<glm::vec3> moveVec;
    
    if(!boss.isDead) {
        
        // check collide with player
        if(checkCollide(player.pos, 0.2, 0.2, boss.pos[0], 0.5, 0.5)) {
            // Suicide bomb!
            player.isDead = true;
        }

        
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texArmy );
        setMaterial(matChrome);
        
        
        boss.move = player.pos;
        
        for(int i=0;i<boss.partNum;i++) {
            //printf("x: %f, y: %f\n",boss.pos[i].x, boss.pos[i].y);
            glPushMatrix();
            
            // gravity
            
        
            // head
            if(i==0) {
                
                moveVec = computeMovement(boss.pos[i], boss.move, false, boss.acceleration, 0);
                boss.pos[i] = moveVec[0];
                
                boss.angle = computeAngle(glm::vec2(boss.pos[i].x, boss.pos[i].y), glm::vec2(player.pos.x, player.pos.y));
                
                
                glTranslatef(boss.pos[i].x, boss.pos[i].y, boss.pos[i].z);
                
                glRotatef(boss.angle, 0, 0, 1);
                
                
                if(boss.angle > 45 && boss.angle <= 135)  glRotatef((boss.angle - 45) * 2, 1, 0, 0);
                else if (boss.angle > 135) glRotatef(180, 1, 0, 0);
                
                if(boss.angle < -45 && boss.angle >= -135) glRotatef((boss.angle + 45) * -2, 1, 0, 0);
                else if(boss.angle < -135) glRotatef(-180, 1, 0, 0);
                
                glRotatef(90, 0, 1, 0);
                
                glBindTexture( GL_TEXTURE_2D, texSkull );
                setMaterial(matSkull);
                
                drawMesh(bossVertices, bossTriangles);

                
            } else {
                // gravity
                if(boss.pos[i].y >= worldLimitY) boss.pos[i].y -= 0.05;
                
                // tail near head move way more faster then the real 'tail'
                moveVec = computeMovement(boss.pos[i], boss.pos[i-1], false, 0.01 + 0.005 * (boss.partNum-i/boss.partNum), boss.partDistance);
                boss.pos[i] = moveVec[0];
                
                
                glTranslatef(boss.pos[i].x, boss.pos[i].y, 0);
                
                
                drawBossTail(0.5 - 0.2 * i/boss.partNum ,10);
                //glutSolidTeapot(0.5);
            }
            
            
            glPopMatrix();
        }
    

    }
    
}


void drawCube(){
    glPushMatrix();
    
    glRotated(xrot, 1, 0, 0);
    glRotated(yrot, 0, 1, 0);
    xrot+=1;
    yrot+=1;
    
    glBindTexture(GL_TEXTURE_2D, texArmy);
    
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
    
    // Game Logic
    
    curEnemies = 0;
    for(int i=0;i<enemies.size();i++){
        if(enemies[i].isDead==false) curEnemies++;
    }
    
    if(curEnemies == 0 && enemies.size() == maxEnemies && boss.isDead) {
        spawnBoss();
    }
    
    if(enemies.size() < maxEnemies && boss.isDead) {
        spawnEnemy();
    }
    
    
    
    // End of Game logic
    
    // Light
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);		// Setup The Ambient Light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);		// Setup The Diffuse Light
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);   // Specular
    glLightfv(GL_LIGHT0, GL_SHININESS, lightShininess);   // Specular
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	// Position The Light
    
    //drawCoordSystem();
  
    
    // Environments
    drawSky();
    drawMountains();
    //drawTerrain();
    //drawWater();
    //drawCube();
    
    
    // Units
    
    
    drawPlayer();
    drawEnemies();
    drawBoss();
    drawBullets();
    
    
    
    
    //glFlush ();
    
    
    
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
        case 'r':
            //player respawn
            player.isDead = false;
            player.hp = 20;
            break;
        case 'b':
            spawnBoss();
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
bool simplifyMesh();

void init()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Texture
    texWhite = loadTexture("textures/white.bmp");
    texGreen = loadTexture("textures/green.bmp");
    
    texSky = loadTexture("textures/sky.jpg");
    texArmy = loadTexture("textures/army.bmp");
    texStone = loadTexture("textures/stone.bmp");
    texGrass = loadTexture("textures/grass.bmp");
    texAluminium = loadTexture("textures/aluminium.bmp");
    texBullet = loadTexture("textures/bullet.png");
    texSkull = loadTexture("textures/skull.jpg");
    
    glEnable(GL_NORMALIZE);
    
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST );
    //glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    
    
//    // MESHES
    loadMesh("meshes/boss.obj");
    simplifyMesh();
    bossVertices =  MeshVertices;
    bossTriangles = MeshTriangles;
    
    //loadMesh("meshes/poke.obj");
    //pokeVertices =  MeshVertices;
    //pokeTriangles = MeshTriangles;
    
    
    // Initialize scene
    worldLimitX = 8;
    worldLimitY = 3;
    
    // Iniialize Mountains
    generateMountains(-worldLimitX, worldLimitX*20);
    
    // Initialize player
    player.pos = glm::vec3(-2,0,0);
    player.move = player.pos;
    player.acceleration = 0.5;
    player.hp = 20;
    player.isDead = false;
    
    //boss.move = player.pos;
    boss.angle = 0;
    boss.acceleration = 0.02;
    boss.isDead = true;
    boss.hp = 20;
    boss.partNum = 20;
    boss.partDistance = 0.5;
   
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

