#ifndef OBJECTS_H
#define OBJECTS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <utility>
#include <vector>
#include "geometry.h"

struct Ball
{
    GLfloat radius;
    GLfloat position[3];
    //speed must represent the change of position per time unit (not per cycle)
    GLfloat speed[3];

    Ball();
    
    Ball (GLfloat r, GLfloat p[], GLfloat s[]);

    void draw();

};

struct Table{
    GLfloat length;
    GLfloat width;
    GLfloat thickness;
    GLfloat height;

    static constexpr GLfloat REBOUND_COEFF=0.75f;

    Table();

    Table (GLfloat l, GLfloat w, GLfloat t, GLfloat h);
    
    void draw();
};

struct Glass{
    GLfloat base_radius; //Radius in the base of the glass
    GLfloat top_radius;
    GLfloat height;
    std::pair<GLfloat,GLfloat> position;

    static const int NUM_SIDES=50;
    static constexpr GLfloat TORUS_RADIUS=0.001F;
    static constexpr GLfloat CONTACT_ANGLE=PI/4;    //Angle for which ball bounds up instead of back
    static constexpr GLfloat REBOUND_COEFF=0.5f;

    Glass();

    Glass (GLfloat br, GLfloat tr, GLfloat h, std::pair<GLfloat,GLfloat> p);

    void draw();
};

#endif