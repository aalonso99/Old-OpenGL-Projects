#include<SDL/SDL.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<cmath>
#include<string>
#include<cstdlib>
#include<iostream>
#include<irrKlang.h>
using namespace irrklang;

/*Screen parameters*/
const double width=1360.0;
const double height=768.0;
const double camera_depth=-30.0;
const double x_translation_constant=0.1;
const double y_translation_constant=0.1; 
double x_translation=0;
double y_translation=0; 

/*Sound engine*/
ISoundEngine *sound_engine;
bool sound_works=true;

/*Mathematical constants*/
const double PI=3.14159;
const double rebound_coefficient=1;

/*Size of the field*/
const double field_width=10.0;
const double field_height=8.0;
const double field_depth=40.0;

/*Playability preferences*/
const double racket_width=2.0;
const double racket_height=1.6;
const double racket_thickness=0.5;
const double ball_radius=0.5;
const double racket_speed=0.4;
const double speed=0.4;

/*Initial values*/
double racket_position[]={0.0,0.0,-0.5};
double ball_position[]={0.0,0.0,-field_depth/2};
double direction[]={0.0,0.0,0.0}; //ball direction
bool initial_state=true;

/*GL initialization*/
void init(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width/height, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    sound_engine=createIrrKlangDevice();
    if(!sound_engine)   sound_works=false;
}

float DRand(float DMin, float DMax)
{
    float f = (float)rand() / RAND_MAX;
    return DMin + f * (DMax - DMin);
}

void drawWhiteLine(){
    glColor3f(1,1,1);
    glBegin(GL_QUADS);          // top line
        glVertex3f(field_width, field_height, 0.5f);   
        glVertex3f(field_width, field_height, -0.5f);
        glVertex3f(-field_width, field_height, -0.5f);
        glVertex3f(-field_width, field_height, 0.5f);
    glEnd();
    glBegin(GL_QUADS);          // right line
        glVertex3f(field_width, field_height, 0.5f);   
        glVertex3f(field_width, -field_height, 0.5f);
        glVertex3f(field_width, -field_height, -0.5f);
        glVertex3f(field_width, field_height, -0.5f);
    glEnd();
    glBegin(GL_QUADS);          // left line
        glVertex3f(-field_width, field_height, 0.5f);  
        glVertex3f(-field_width, field_height, -0.5f);
        glVertex3f(-field_width, -field_height, -0.5f);
        glVertex3f(-field_width, -field_height, 0.5f);
    glEnd();
    glBegin(GL_QUADS);          // bottom line
        glVertex3f(-field_width, -field_height, 0.5f);
        glVertex3f(-field_width, -field_height, -0.5f);
        glVertex3f(field_width, -field_height, -0.5f);
        glVertex3f(field_width, -field_height, 0.5f);  
    glEnd();
}

void drawCube(){
    glBegin(GL_QUADS);          // top face
        glVertex3f(1, 1, 1);   
        glVertex3f(1, 1, -1);
        glVertex3f(-1, 1, -1);
        glVertex3f(-1, 1, 1);
    glEnd();
    glBegin(GL_QUADS);          // front face
        glVertex3f(1, 1, 1);   
        glVertex3f(-1, 1, 1);
        glVertex3f(-1, -1, 1);
        glVertex3f(1, -1, 1);
    glEnd();
    glBegin(GL_QUADS);          // right face
        glVertex3f(1, 1, 1);   
        glVertex3f(1, -1, 1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, 1, -1);
    glEnd();
    glBegin(GL_QUADS);          // left face
        glVertex3f(-1, 1, 1);  
        glVertex3f(-1, 1, -1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1, 1);
    glEnd();
    glBegin(GL_QUADS);          // bottom face
        glVertex3f(-1, -1, 1);
        glVertex3f(-1, -1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, -1, 1);  
    glEnd();
    glBegin(GL_QUADS);          // back face
        glVertex3f(1, -1, -1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, 1, -1);
        glVertex3f(1, 1, -1);  
    glEnd();

}

void drawRacket(){
    glPushMatrix();
        glColor4f(1,1,1,0.6);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(racket_position[0],racket_position[1],racket_position[2]);
        glScalef(racket_width,racket_height,racket_thickness);
        drawCube();
    glPopMatrix();
}

void drawBall(){
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslatef(ball_position[0], ball_position[1], ball_position[2]);
        gluSphere(gluNewQuadric(),ball_radius, 200, 200);
    glPopMatrix();
}

void drawField(){
    //Goal line
    glPushMatrix();
        glTranslatef(0,0,-0.5);
        drawWhiteLine();
    glPopMatrix();
    //Mid field line
    glPushMatrix();
        glTranslatef(0,0,-field_depth/2);
        drawWhiteLine();
    glPopMatrix();
    //Wall
    glPushMatrix();
        glColor3f(0.5,0.5,0.5);
        glTranslatef(0,0,-field_depth);
        glBegin(GL_QUADS);
            glVertex3f(-field_width, field_height, 0);
            glVertex3f(field_width, field_height, 0);
            glVertex3f(field_width, -field_height, 0);
            glVertex3f(-field_width, -field_height, 0);  
        glEnd();
    glPopMatrix();
}

bool ballHitsRacket(){
    double collision_depth=racket_position[2]+racket_thickness;
    if(ball_position[2]<collision_depth && ball_position[2]+ball_radius>collision_depth){
        if(abs(racket_position[0]-ball_position[0])-ball_radius<racket_width){
            if(abs(racket_position[1]-ball_position[1])-ball_radius<racket_height){
                if(sound_works) sound_engine->play2D("clack.wav", false);
                return true;
            }
        }
    }

    return false;
}

void moveBall(){
    //for(int i=0; i<3; i++)  ball_position[i] += speed*direction[i];
    ball_position[0] += speed*direction[0];
    ball_position[1] += speed*direction[1];
    ball_position[2] += 1.5*speed*direction[2];

    if(ball_position[0]-ball_radius < -field_width){
        ball_position[0]=-field_width+ball_radius;
        direction[0]=-direction[0];
        if(sound_works) sound_engine->play2D("blop.wav", false);
    }
    else if(ball_position[0]+ball_radius > field_width){
        ball_position[0]=field_width-ball_radius;
        direction[0]=-direction[0];
        if(sound_works) sound_engine->play2D("blop.wav", false);
    }

    if(ball_position[1]-ball_radius < -field_height){
        ball_position[1]=-field_height+ball_radius;
        direction[1]=-direction[1];
        if(sound_works) sound_engine->play2D("blop.wav", false);
    }

    if(ball_position[1]+ball_radius > field_height){
        ball_position[1]=field_height-ball_radius;
        direction[1]=-direction[1];
        if(sound_works) sound_engine->play2D("blop.wav", false);
    }

    if(ball_position[2]-ball_radius < -field_depth ){
        direction[2]=-direction[2];
        if(sound_works) sound_engine->play2D("blop.wav", false);
    }

    if(ballHitsRacket()){
        double x=ball_position[0]-racket_position[0];
        double y=ball_position[1]-racket_position[1];
        double z=-rebound_coefficient;
        double norm=abs(x)+abs(y)+abs(z);
        direction[0]=x/norm;
        direction[1]=y/norm;
        direction[2]=z/norm;
    }

}

void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(x_translation,y_translation,camera_depth);
    //glRotatef(90,0,1,0);

    drawField();
    moveBall();
    drawBall();
    drawRacket();

}

int main(int argc, char* args[]){

    SDL_Init(SDL_INIT_EVERYTHING);

    Uint8 *keys = SDL_GetKeyState(NULL); 

    SDL_SetVideoMode(width,height,0,SDL_SWSURFACE|SDL_OPENGL|SDL_FULLSCREEN);

    int loop=1;
    SDL_Event myevent;
    init();
    while (loop==1)
    {
        while (SDL_PollEvent(&myevent))
        {
            switch(myevent.type)
            {
                case SDL_QUIT:
                    loop=0;
                break;
            }
        }

        if ((keys[SDLK_LALT]||keys[SDLK_RALT])&&keys[SDLK_F4]) loop=0;
        if (initial_state==true && keys[SDLK_SPACE]){
            float angle1=DRand(PI/4,3*PI/4);
            float angle2=DRand(3*PI/4,5*PI/4);
            //float angle1=DRand(0,2*PI);
            //float angle2=DRand(0,2*PI);
            float x=cos(angle1);
            float y=sin(angle2);
            float z=-sin(angle1);
            float norm=abs(x)+abs(y)+abs(z);
            direction[0]=x/norm;
            direction[1]=y/norm;
            direction[2]=z/norm;
            initial_state=false;
        }
        if (initial_state==false && keys[SDLK_r]){
            ball_position[0]=0.0;
            ball_position[1]=0.0;
            ball_position[2]=-field_depth/2;
            direction[0]=0.0;
            direction[1]=0.0;
            direction[2]=0.0;
            initial_state=true;
        }

        if (keys[SDLK_LEFT] && racket_position[0] > (-field_width+racket_width) ){
            racket_position[0]-=racket_speed;
            x_translation+=x_translation_constant;
        }

        if (keys[SDLK_RIGHT] && racket_position[0] < (field_width-racket_width) ){
            x_translation-=x_translation_constant;
            racket_position[0]+=racket_speed;
        } 

        if (keys[SDLK_DOWN] && racket_position[1] > (-field_height+racket_height) ){
            racket_position[1]-=racket_speed;
            y_translation+=y_translation_constant;
        } 

        if (keys[SDLK_UP] && racket_position[1] < (field_height-racket_height) ){
            racket_position[1]+=racket_speed;
            y_translation-=y_translation_constant;
        } 
        
        display();
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}
