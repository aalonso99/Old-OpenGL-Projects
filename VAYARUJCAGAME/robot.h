#ifndef ROBOT_H
#define ROBOT_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <string>
#include <dirent.h>
#include "LTexture.h"
#include "objects.h"

/*For next funtions:
    ball->pointer to the ball that bounds
    new_ball_pos->position of the ball whose collision we are checking
    prism_centre->coordinates of the centre of the prism
    prism_size->vector with the middle-length of each dimension of the prism
    rebound_coeff->reduction of the module of the ball speed after colliding
    cyl_origin->centre of one base of the cylinder
    cyl_end->centre of the other base of the cylinder
    cyl_radius->self explainatory (duh)
    sph_centre-> "       "          "
    sph_radius-> "       "          "   
*/

bool ballTouchesPrism(Ball *ball, GLfloat new_ball_pos[], GLfloat prism_centre[], GLfloat prism_size[], GLfloat rebound_coeff);
bool ballTouchesCylinder(Ball *ball, GLfloat new_ball_pos[], GLfloat cyl_origin[], GLfloat cyl_end[], GLfloat cyl_radius, GLfloat rebound_coeff);
bool ballTouchesSphere(Ball *ball, GLfloat new_ball_pos[], GLfloat sph_centre[], GLfloat sph_radius, GLfloat rebound_coeff);
bool ballTouchesEllipsoid(Ball *ball, GLfloat new_ball_pos[], GLfloat ellip_centre[], GLfloat size[], GLfloat rebound_coeff);

struct RobotHead{

    GLfloat position[3];
    GLfloat size[3]; //Three real numbers for the half sizes of the head in three dimensions: {x,y,z}
    GLfloat *color;
    GLint orientation=1;
    LTexture *face;

    RobotHead(GLfloat pos[], GLfloat s[], GLfloat* cl, LTexture *face, GLint orient);
    ~RobotHead();
    void draw();
    void translate(GLfloat v[]);
    bool ballTouchesHead(Ball *ball, GLfloat new_ball_pos[]);
    void loadRandomFace(std::string path);
};

struct RobotBody{
    //Formed by a spherical neck, a cubical torax and an elipsoidal hip
    GLfloat r_neck;
    GLfloat size_torax[3];
    GLfloat size_hip[3];
    GLfloat position[3];
    GLfloat *color;

    RobotBody(GLfloat rn, GLfloat s_torax[], GLfloat s_hip[], GLfloat pos[], GLfloat* cl);
    void draw();
    void translate(GLfloat v[]);
    bool ballTouchesBody(Ball *ball, GLfloat new_ball_pos[]);
};

struct RobotArm{
    //Shoulder, elbow, hand and joints of the hand are formed by spheres
    //Arm, forearm and phalanges are formed by Cylinders
    static constexpr GLfloat shoulder_radius=0.08;
    static constexpr GLfloat arm_radius=0.06;
    static constexpr GLfloat arm_length=0.1;
    static constexpr GLfloat elbow_radius=0.06;
    static constexpr GLfloat forearm_radius=0.06;
    static constexpr GLfloat forearm_length=0.12;
    static constexpr GLfloat hand_radius=0.05;
    static constexpr GLfloat phalanx_radius=0.015;
    static constexpr GLfloat phalanx_length=0.025;
    static constexpr GLfloat joint_radius=0.018;

    GLfloat *color;
    GLint orientation=1;

    //For Cylinders, 2 coordinates are given: the point where it starts and where it ends
    //The three first variables will be necessary to store the position of the arm
    //The rest are additional, but only will be changed when it moves and will help avoid extra
    //computations every display
    GLfloat shoulder_p[3];
    GLfloat arm_angle;
    GLfloat forearm_angle;
    GLfloat finger_angle;   //I want both fingers to be symmetric so the finger
    GLfloat joint_angle;    //and joint angles will be the same for both fingers
    GLfloat arm_p[2][3];
    GLfloat elbow_p[3];
    GLfloat forearm_p[2][3];
    GLfloat hand_p[3];
    GLfloat joint_p[2][3];
    GLfloat phalanx_p[4][2][3]; //There are 4 phalanges

    RobotArm(GLfloat position[], GLfloat *cl, GLint orient);
    void computePositions();
    void lift(GLfloat lift_angle);
    void flex(GLfloat flex_angle);
    void draw();
    void translate(GLfloat v[]);
    bool ballTouchesArm(Ball *ball, GLfloat new_ball_pos[]);
    void setBallPosition(GLfloat pos[]);
};

struct RobotPlatform{
    static constexpr GLfloat pillar_length=0.5f;
    static constexpr GLfloat pillar_radius=0.08f;
    static constexpr GLfloat base_height=0.1f;
    static constexpr GLfloat base_radius=0.2f;

    GLfloat position[3];

    GLfloat *color;

    RobotPlatform(const GLfloat pos[], GLfloat *cl);
    ~RobotPlatform();
    void draw();
    void translate(GLfloat v[]);
    bool ballTouchesPlatform(Ball *ball, GLfloat new_ball_pos[]);
};

enum Mood{happy, sad};

struct Robot{
    RobotHead *head; //Cubic-shaped head
    RobotBody *body;
    RobotArm *larm, *rarm;
    RobotPlatform *plat;
    GLfloat color[3];
    //orientation can be 1 or -1. Specifyies if the robot is looking in the positive or negative direction of the z axis
    GLint orientation=1;
    std::string name;
    Mood mood;
    
    GLfloat static constexpr REBOUND_COEFF=0.6f;
    GLfloat static constexpr SENSITIVITY=3.0f;  //minimum speed module that makes you sad

    Robot(GLfloat pos[], GLfloat cl[], GLint orient, std::string name);
    ~Robot();
    void loadHappyFace();
    void loadSadFace();
    //void loadAngryFace();
    void draw();
    void translate(GLfloat v[]);
    bool ballTouchesRobot(Ball *ball, GLfloat new_ball_pos[]);
    //Sets pos[] to the position of the ball grabbed by the right hands
    void setBallPosition(GLfloat pos[]);
    GLfloat xPosition();
};


#endif
