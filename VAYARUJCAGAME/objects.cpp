#include <GL/gl.h>
#include <GL/glu.h>
#include"objects.h"

    Ball::Ball(){}
    
    Ball::Ball (GLfloat r, GLfloat p[], GLfloat s[]){
        radius=r;
        for(int i=0; i<3; i++){
            position[i]=p[i];
            speed[i]=s[i];
        }
    }

    void Ball::draw(){
        glPushMatrix();
            glColor3f(1,0,0);
            glTranslatef(position[0], position[1], position[2]);
            gluSphere(gluNewQuadric(),radius, 200, 200);
        glPopMatrix();
    }

    Table::Table(){}

    Table::Table (GLfloat l, GLfloat w, GLfloat t, GLfloat h){
        length=l;
        width=w;
        thickness=t;
        height=h;
    }

    void Table::draw(){
        glPushMatrix();
            glColor3f(0.5f, 0.35f, 0.05f);
            glScalef(width, thickness, length);
            drawCube();

            glBegin(GL_POLYGON);
                glNormal3f(0.0f, 1.0f, 0.0f);	// top face
                glVertex3f(1.0f, 1.0f, 1.0f);	
                glVertex3f(1.0f, 1.0f, -1.0f);
                glVertex3f(-1.0f, 1.0f, -1.0f);
                glVertex3f(-1.0f, 1.0f, 1.0f);
            glEnd();

            
        glPopMatrix();
    }

    Glass::Glass(){}

    Glass::Glass (GLfloat br, GLfloat tr, GLfloat h, std::pair<GLfloat,GLfloat> p){
        base_radius=br;
        top_radius=tr;
        height=h;
        position=p;
    }

    void Glass::draw(){
        glPushMatrix();
        
            glColor3f(1.0,1.0,1.0);
            GLfloat r=top_radius;
            GLfloat base_center[3]={0.0f,0.0f,0.0f};


            glPushMatrix();
                glBegin(GL_QUAD_STRIP);

                GLfloat x;
                GLfloat y;
                GLfloat z;

                for(int j=0; j<NUM_SIDES; j++){
                    float theta=2*PI*float(j)/NUM_SIDES;
                    x=base_center[0]+base_radius*cos(theta);
                    z=base_center[2]+base_radius*sin(theta);

                    glVertex3f(x,0.0,z);    //Bottom point
                    glVertex3f(x*top_radius/base_radius, height, z*top_radius/base_radius); //Top point
                }

                x=base_center[0]+base_radius;
                y=0.0;
                z=0.0;
                glVertex3f(x,y,z);
                glVertex3f(x*top_radius/base_radius,height,z);

            glEnd();
            glColor3f(0.8f,0.8f,0.8f);
            glTranslatef(0, height, 0);
            drawTorus(r, TORUS_RADIUS, 100, 100);
            glPopMatrix();

        glPopMatrix();
    }