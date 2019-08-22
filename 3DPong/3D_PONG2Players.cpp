#include<SDL/SDL.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<irrKlang.h>
using namespace irrklang;

/*Keys*/
Uint8 *keys = SDL_GetKeyState(NULL); 

/*Screen parameters*/
const double width=1360.0;
const double height=768.0;
const double camera_depth=-35.0;
const double x_translation_constant=0.1;
const double y_translation_constant=0.1; 
double x1_translation=0;
double y1_translation=0; 
double x2_translation=0;
double y2_translation=0; 

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
const GLfloat p1_racket_colour[4]={1,0,0,0.6};
const GLfloat p2_racket_colour[4]={0,0,1,0.6};

/*Initial values*/
float p1_racket_position[]={0.0,0.0,field_depth/2-0.5};
float p2_racket_position[]={0.0,0.0,-field_depth/2+0.5};
float ball_position[]={0.0,0.0,0.0};
float direction[]={0.0,0.0,0.0}; //ball direction
bool initial_state=true;
bool p1_receiving=true;

/*GL initialization*/
void init(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width/(2*height), 1.0, 500.0);
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

void drawBall(){
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslatef(ball_position[0], ball_position[1], ball_position[2]);
        gluSphere(gluNewQuadric(),ball_radius, 200, 200);
    glPopMatrix();
}

void drawField(){
    //Player1 goal line
    glPushMatrix();
        glTranslatef(0,0,field_depth/2-0.5);
        drawWhiteLine();
    glPopMatrix();
    //Mid field line
    glPushMatrix();
        glTranslatef(0,0,0);
        drawWhiteLine();
    glPopMatrix();
    //Player2 goal line
    glPushMatrix();
        glTranslatef(0,0,-field_depth/2+0.5);
        drawWhiteLine();
    glPopMatrix();
}

void drawRacket(const GLfloat * colour, float *position){
    glPushMatrix();
        glColor4fv(colour);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(position[0],position[1],position[2]);
        glScalef(racket_width,racket_height,racket_thickness);
        drawCube();
    glPopMatrix();
}

void drawRacketP1(){
    drawRacket(p1_racket_colour, p1_racket_position);
}

void drawRacketP2(){
    drawRacket(p2_racket_colour, p2_racket_position);
}

bool ballHitsRacketP1(){
    double collision_depth=p1_racket_position[2]+racket_thickness;
    if(ball_position[2]<collision_depth && ball_position[2]+ball_radius>collision_depth){
        if(abs(p1_racket_position[0]-ball_position[0])-ball_radius<racket_width){
            if(abs(p1_racket_position[1]-ball_position[1])-ball_radius<racket_height){
                p1_receiving=false;
                if(sound_works) sound_engine->play2D("clack.wav", false);
                return true;
            }
        }
    }

    return false;
}

bool ballHitsRacketP2(){
    double collision_depth=p2_racket_position[2]-racket_thickness;
    if(ball_position[2]>collision_depth && ball_position[2]-ball_radius<collision_depth){
        if(abs(p2_racket_position[0]-ball_position[0])-ball_radius<racket_width){
            if(abs(p2_racket_position[1]-ball_position[1])-ball_radius<racket_height){
                p1_receiving=true;
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

    else if(p1_receiving){
        if(ballHitsRacketP1()){
            double x=ball_position[0]-p1_racket_position[0];
            double y=ball_position[1]-p1_racket_position[1];
            double z=-rebound_coefficient;
            double norm=abs(x)+abs(y)+abs(z);
            direction[0]=x/norm;
            direction[1]=y/norm;
            direction[2]=z/norm;
        }
    }else{
        if(ballHitsRacketP2()){
            double x=ball_position[0]-p2_racket_position[0];
            double y=ball_position[1]-p2_racket_position[1];
            double z=rebound_coefficient;
            double norm=abs(x)+abs(y)+abs(z);
            direction[0]=x/norm;
            direction[1]=y/norm;
            direction[2]=z/norm;
        }
    }
}

void checkMovementP1(){
    if (keys[SDLK_LEFT] && p1_racket_position[0] > (-field_width+racket_width) ){
            p1_racket_position[0]-=racket_speed;
            x1_translation+=x_translation_constant;
        }

        if (keys[SDLK_RIGHT] && p1_racket_position[0] < (field_width-racket_width) ){
            x1_translation-=x_translation_constant;
            p1_racket_position[0]+=racket_speed;
        } 

        if (keys[SDLK_DOWN] && p1_racket_position[1] > (-field_height+racket_height) ){
            p1_racket_position[1]-=racket_speed;
            y1_translation+=y_translation_constant;
        } 

        if (keys[SDLK_UP] && p1_racket_position[1] < (field_height-racket_height) ){
            p1_racket_position[1]+=racket_speed;
            y1_translation-=y_translation_constant;
        } 
}

void checkMovementP2(){
    if (keys[SDLK_d] && p2_racket_position[0] > (-field_width+racket_width) ){
            p2_racket_position[0]-=racket_speed;
            x2_translation+=x_translation_constant;
        }

        if (keys[SDLK_a] && p2_racket_position[0] < (field_width-racket_width) ){
            x2_translation-=x_translation_constant;
            p2_racket_position[0]+=racket_speed;
        } 

        if (keys[SDLK_s] && p2_racket_position[1] > (-field_height+racket_height) ){
            p2_racket_position[1]-=racket_speed;
            y2_translation+=y_translation_constant;
        } 

        if (keys[SDLK_w] && p2_racket_position[1] < (field_height-racket_height) ){
            p2_racket_position[1]+=racket_speed;
            y2_translation-=y_translation_constant;
        } 
}

void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    moveBall();
    glTranslatef(0,0,-field_depth/2);

    /*Draw the field for Player1*/
    glPushMatrix();
        glTranslatef(x1_translation,y1_translation,camera_depth);
        glViewport(width/2,0,width/2, height);  //I don't know why I need that 64 
        drawField();
        drawBall();
        drawRacketP2();
        drawRacketP1();        
    glPopMatrix();

    /*Draw the field for Player2*/
    glPushMatrix();
        glViewport(0,0,width/2,height);
        glTranslatef(x2_translation,y2_translation,camera_depth);
        glRotatef(180,0,1,0);
        drawField();
        drawBall();
        drawRacketP1();   
        drawRacketP2();     
    glPopMatrix();

}

int main(int argc, char* args[]){

    SDL_Init(SDL_INIT_EVERYTHING);

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
            p1_receiving=true;
            float angle1=DRand(PI/4,3*PI/4);
            //We decide who serves
            if(DRand(0,1)-0.5<0){
                p1_receiving=false;
                angle1+=PI; //This is equal to negate the sin of angle1
            } 
            float angle2=DRand(3*PI/4,5*PI/4);
            //float angle1=DRand(0,2*PI);
            //float angle2=DRand(0,2*PI);
            float x=cos(angle1);
            float y=sin(angle2);
            float z=sin(angle1);
            float norm=abs(x)+abs(y)+abs(z);
            direction[0]=x/norm;
            direction[1]=y/norm;
            direction[2]=z/norm;
            initial_state=false;
        }
        if (initial_state==false && keys[SDLK_r]){
            for(int i=0; i<3; i++)
                ball_position[i]=direction[i]=0;
            
            initial_state=true;
        }

        checkMovementP1();
        checkMovementP2();
        
        
        display();
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}
