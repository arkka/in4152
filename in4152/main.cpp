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

struct Player {
    glm::vec3 pos;
    glm::vec3 move;
    glm::vec3 mouse;
    
    float angle;
    float acceleration;
};

struct Bullet {
    glm::vec3 pos;
    glm::vec3 move;
    
    float angle;
    float acceleration;
    
    int type; // 0 from player, 1 from enemy
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

unsigned int screenWidth = 800;  // screen width
unsigned int screenHeight = 600;  // screen height


float LightPos[4] = {1,1,0.4,1};
std::vector<float> MeshVertices;
std::vector<unsigned int> MeshTriangles;

//Declare your own global variables here:
float a = 30, b = 45, c = 60;
int l = 1;
float pos = 0.1;
float x1l, x2l, y1l, y2l, z1l, z2l;


GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info
GLfloat winX, winY; //variables to hold screen x,y,z coordinates
GLfloat winZ = -5; // Default win Z coord
GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
GLdouble worldLimitX, worldLimitY, worldLimitZ;


float easingAmount = 0.05;
bool isLeftKeyPressed = false;
bool isRightKeyPressed = false;
bool isDownKeyPressed = false;
bool isUpKeyPressed = false;

////////// Movement
std::vector<glm::vec3> animateMovement(glm::vec3 from, glm::vec3 to) {
    std::vector<glm::vec3> vec;
    
    float distanceX = to.x - from.x;
    float distanceY = to.y - from.y;
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
    
    if(from.x >= worldLimitX
       || from.x <= -worldLimitX
       || from.y >= worldLimitY
       || from.y <= -worldLimitY) {
        
        // Make it bounce back
        //printf("LIMIT\n");
        //to.x = 0;
        //to.y = 0;
        
        //vec.push_back(from);
        //vec.push_back(to);
    }
    
    if (distance > 0) {
        from.x += distanceX * easingAmount;
        from.y += distanceY * easingAmount;
    }
    
    vec.push_back(from);
    vec.push_back(to);
    
    return vec;
}

////////// Draw Functions

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

void drawPlayer()
{
    //printf("cord: %f,%f\n",player.x,player.y);
    
    std::vector<glm::vec3> moveVec = animateMovement(player.pos, player.move);
    player.pos = moveVec[0];
    player.move = moveVec[1];

    
    glPushMatrix();
    glTranslated(player.pos.x, player.pos.y, 0);
    drawCoordSystem();
    
    glRotatef(player.angle, 0, 0, 1);

    glutSolidTeapot(.5);
    glPopMatrix();
}

/**
 * Several drawing functions for you to work on
 */

void drawTriangle()
{
    
}


void drawUnitFace()
{
    //1) draw a unit quad in the x,y plane oriented along the z axis
    //2) make sure the orientation of the vertices is positive (counterclock wise)
    //3) What happens if the order is inversed?
    
    glColor3f(0, 1, 1);
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
    /*
     glVertex3f(0, 0, 0);
     glVertex3f(0, 1, 0);
     glVertex3f(1, 1, 0);
     glVertex3f(1, 0, 0);
     */
    
    glVertex3f(1, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    
    
    
    glEnd();
    
}

void drawUnitCube()
{
    //1) draw a cube using your function drawUnitFace
    //rely on glTranslate, glRotate, glPushMatrix, and glPopMatrix
    //the latter two influence the model matrix, as seen during the course.
    //glPushMatrix stores the current matrix and puts a copy on
    //the top of a stack.
    //glPopMatrix pops the top matrix on the stack
    
    
    glPushMatrix();
    glRotated(180, 1, 0, 0);
    glTranslated(0, -1, 0);
    drawUnitFace();
    //drawTriangle();
    
    glPopMatrix();
    
    glPushMatrix();
    glRotated(90,1,0,0);
    drawUnitFace();
    
    glTranslated(.5, 0, -1);
    glRotated(180, 0, 1, 0);
    glTranslated(-.5, 0, 0);
    
    drawUnitFace();
    
    
    glPopMatrix();
    
    glPushMatrix();
    glRotated(-90, 0, 1, 0);
    drawUnitFace();
    
    
    glTranslatef(1, 0, 0);
    glRotated(90, 0, 1, 0);
    drawUnitFace();
    
    
    
    glTranslatef(1, 0, 0);
    glRotated(90, 0, 1, 0);
    drawUnitFace();
    
    
    glPopMatrix();
    
    
    
}

void drawArm()
{
    //produce a three-unit arm (upperarm, forearm, hand) making use of your
    //function drawUnitCube to define each of them
    //1) define 3 global variables that control the angles
    //between the arm parts
    //and add cases to the keyboard function to control these values
    
    //2) use these variables to define your arm
    //use glScalef to achieve different arm length
    //use glRotate/glTranslate to correctly place the elements
    
    //3 optional) make an animated snake out of these boxes
    //(an arm with 10 joints that moves using the animate function)
    
    glPushMatrix();
    
    glRotatef(a,0,0,1);
    
    glPushMatrix();
    glScalef(.5,1.5,.5);
    drawUnitCube();
    glPopMatrix();
    
    glTranslatef(0, 1.5, 0);
    glRotatef(b, 0, 0, 1);
    
    glPushMatrix();
    glScalef(.5, 1, .5);
    drawUnitCube();
    glPopMatrix();
    
    glTranslatef(0, 1, 0);
    glRotatef(c, 0, 0, 1);
    glScalef(.5, .5, .5);
    drawUnitCube();
    
    glPopMatrix();
}

void drawLight()
{
    //1) use glutSolidSphere to draw a sphere at the light's position LightPos
    //use glTranslatef to move it to the right location
    //to make the light source bright, follow the drawCoordSystem function
    //to deactivate the lighting temporarily and draw it in yellow
    
    //2) make the light position controllable via the keyboard function
    
    //3) add normal information to all your faces of the previous functions
    //and observe the shading after pressing 'L' to activate the lighting
    //you can use 'l' to turn it off again
    
    glPushMatrix();
    
    glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
    
    glColor3f(1, 1, l);
    glNormal3f(0, 0, 1);
    glutSolidSphere(.05,50,50);
    
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
    glColor3f(1, 0, 0);
    
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

/**
 * Scene
 */
void computeWorldLimit(){
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport ); //Lokasi dari kamera [x,y,panjang,lebar]
    
    // get world limit for object limit
    gluUnProject( 0, 0, 1, modelview, projection, viewport, &worldLimitX, &worldLimitY, &worldZ);
    
    // convert world limit to abs
    worldLimitX = abs(worldLimitX)/2;
    worldLimitY = abs(worldLimitY)/2;
    //printf("world cord Limit at %f,%f,%f\n",worldLimitX,worldLimitY, worldLimitZ);
}

void display( )
{
    // Compute viewpoint limit on world cord
    computeWorldLimit();
    
    //set the light to the right position
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
    drawLight();
    
    drawCoordSystem();
    drawPlayer();
    
    /*
    switch( DisplayMode )
    {
        case TRIANGLE:
            drawCoordSystem();
            drawTriangle();
            break;
        case FACE:
            drawCoordSystem();
            drawUnitFace();
            break;
        case CUBE:
            drawCoordSystem();
            drawUnitCube();
            break;
        case ARM:
            drawCoordSystem();
            drawArm();
        case MESH:
            drawMesh();
            drawCoordSystem();
            
            //...
            
        default:
            
            break;
    }
     */
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
        //printf("mouse click pressed at %d,%d\n",x,y);
        
        
        
    }
    
    
}

void mouseMotion( int x, int y )
{
    // Limit motion to screen size
    
    
    if((y<=screenHeight && y>=0)&&(x<=screenWidth && x>=0)){
        
        //printf("cord at %d,%d\n",x,y);
        
        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        glGetIntegerv( GL_VIEWPORT, viewport ); //Lokasi dari kamera [x,y,panjang,lebar]

        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
        
        // get world coord based on mouse
        gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
        printf("world cord at %f,%f,%f\n",worldX,worldY, worldZ);
        
        // Hitung angle relatif dari player
        //printf("range %f,%f\n", worldY - player.y,worldX - player.x);
        float playerAngle = (atan2(worldY - player.pos.y,worldX - player.pos.x) * 180 / M_PI);
        if(playerAngle) player.angle = playerAngle;
        
        computeWorldLimit();
        
    }
    
    
    
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
        case 'L':
            //turn lighting on
            glEnable(GL_LIGHTING);
            l = 0;
            break;
        case 'l':
            //turn lighting off
            glDisable(GL_LIGHTING);
            l = 1;
            break;
        case 'a':
            //first arm angle increase
            a = a + 5;
            break;
        case 'z':
            //first arm angle decrease
            a = a - 5;
            break;
        case 's':
            //second arm angle increase
            b = b + 5;
            break;
        case 'x':
            //second arm angle decrease
            b = b - 5;
            break;
        case 'd':
            //third arm angle increase
            c = c + 5;
            break;
        case 'c':
            //third arm angle decrease
            c = c - 5;
            break;
        case 'q':
            LightPos[0] = LightPos[0] + pos;
            break;
        case 'Q':
            LightPos[0] = LightPos[0] - pos;
            break;
        case 'w':
            LightPos[1] = LightPos[1] + pos;
            break;
        case 'W':
            LightPos[1] = LightPos[1] - pos;
            break;
        case 'e':
            LightPos[2] = LightPos[2] + pos;
            break;
        case 'E':
            LightPos[2] = LightPos[2] - pos;
            break;
    }
}

void keyboardSpecial(int key, int x, int y) {
    //printf("key: %d\n",key);
    
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
    // SCENE
    
    glDisable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
//    int MatSpec [4] = {1,1,1,1};
//    glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
//    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);
    
    
    // Enable Depth test
    glEnable( GL_DEPTH_TEST );
    
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //Draw frontfacing polygons as filled
    glPolygonMode(GL_FRONT,GL_FILL);
    //draw backfacing polygons as outlined
    glPolygonMode(GL_BACK, GL_FILL);
    //glPolygonMode(GL_BACK, GL_LINE);
    glShadeModel(GL_SMOOTH);
    
    
    
    // MESHES
    loadMesh("/Users/arkkadhiratara/Desktop/3DCG/in4152/in4152/David.obj");
    
    // Initialize player
    player.pos = glm::vec3(-2,0,0);
    player.move = player.pos;
    player.acceleration = 0.5;
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
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    
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
    glClearColor (0.0, 0.0, 0.0, 0.0);
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
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective (50, (float)w/h, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}

