#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>
#include "LTexture.h"
#include "robot.h"
#include "geometry.h"
using namespace std;

GLfloat WHITE[]={1.0f, 1.0f, 1.0f};
GLfloat BLACK[]={0.0f, 0.0f, 0.0f};
GLint INT_STEPS=800;    //number of steps checked in the collision
GLfloat RADIUS_RATIO=0.8f;  //proportion of the radius of the ball considered in the collision

//Functions for collision detection 
//Explanation for the parameters in robot.h
bool ballTouchesPrism(Ball *ball, GLfloat new_ball_pos[], GLfloat prism_centre[], GLfloat prism_size[], GLfloat rebound_coeff){
    if(abs(new_ball_pos[0]-prism_centre[0])<=prism_size[0] && abs(new_ball_pos[2]-prism_centre[2])<=prism_size[2]){

        if(abs(new_ball_pos[1]-prism_centre[1])<=prism_size[1]+ball->radius){
            ball->speed[1]=-(ball->speed[1])*rebound_coeff;
            return true;
        }else return false;

    } else if(abs(new_ball_pos[1]-prism_centre[1])<=prism_size[1] && abs(new_ball_pos[2]-prism_centre[2])<=prism_size[2]){

        if(abs(new_ball_pos[0]-prism_centre[0])<=prism_size[0]+ball->radius){
            ball->speed[0]=-(ball->speed[0])*(1-(1-rebound_coeff)/2);
            return true;
        }else return false;

    } else if(abs(new_ball_pos[1]-prism_centre[1])<=prism_size[1] && abs(new_ball_pos[0]-prism_centre[0])<=prism_size[0]){

        if(abs(new_ball_pos[2]-prism_centre[2])<=prism_size[2]+ball->radius){
            ball->speed[2]=-(ball->speed[2])*(1-(1-rebound_coeff)/2);
            return true;
        }else return false;

    } else return false;

    /*for(int i=0; i<=INT_STEPS; i++){
        GLfloat baricentric_weight=(float)i/INT_STEPS;
        GLfloat intermediate_pos[3];
        for(int j=0; j<3; j++){
            intermediate_pos[j]=baricentric_weight*new_ball_pos[j]+(1-baricentric_weight)*ball->position[j];
        }

        if(abs(intermediate_pos[0]-prism_centre[0])<=prism_size[0] && abs(intermediate_pos[2]-prism_centre[2])<=prism_size[2]){

            if(abs(intermediate_pos[1]-prism_centre[1])<=prism_size[1]+ball->radius*RADIUS_RATIO){
                for(int k=0; k<3; k++){
                    ball->position[k]=intermediate_pos[k];
                }
                ball->speed[1]=-(ball->speed[1])*rebound_coeff;
                return true;
            }

        } else if(abs(intermediate_pos[1]-prism_centre[1])<=prism_size[1] && abs(intermediate_pos[2]-prism_centre[2])<=prism_size[2]){

            if(abs(intermediate_pos[0]-prism_centre[0])<=prism_size[0]+ball->radius*RADIUS_RATIO){
                for(int k=0; k<3; k++){
                    ball->position[k]=intermediate_pos[k];
                }
                ball->speed[0]=-(ball->speed[0])*(1-(1-rebound_coeff)/2);
                return true;
            }

        } else if(abs(intermediate_pos[1]-prism_centre[1])<=prism_size[1] && abs(intermediate_pos[0]-prism_centre[0])<=prism_size[0]){

            if(abs(intermediate_pos[2]-prism_centre[2])<=prism_size[2]+ball->radius*RADIUS_RATIO){
                for(int k=0; k<3; k++){
                    ball->position[k]=intermediate_pos[k];
                }
                ball->speed[2]=-(ball->speed[2])*(1-(1-rebound_coeff)/2);
                return true;
            }

        }
    }

    return false;*/
    
}

//For this function I will asume the cylinder is in a vertical position
bool ballTouchesCylinder(Ball *ball, GLfloat new_ball_pos[], GLfloat cyl_origin[], GLfloat cyl_end[], GLfloat cyl_radius, GLfloat rebound_coeff){

    /*GLfloat cyl_centre[3];
    for(int i=0; i<3; i++){
        cyl_centre[i]=(cyl_origin[i]+cyl_end[i])/2;
    }
    GLfloat min_distance=sqrt( pow(distance3D(cyl_centre, cyl_end), 2) + cyl_radius*cyl_radius );

    //If the ball will be close enough to the cylinder
    if(distance3D( cyl_centre, new_ball_pos )<=min_distance){*/
        GLfloat closest_p[3];
        GLfloat cyl_axis[3];
        for(int i=0; i<3; i++)  cyl_axis[i]=cyl_end[i]-cyl_origin[i];

        //Store the closest point coordinates in closest_p
        closest_point( new_ball_pos, cyl_origin, cyl_axis, closest_p );

        if(distance3D(new_ball_pos, closest_p)<cyl_radius+ball->radius){
            GLfloat normal_vector[3];
            for(int i=0; i<3; i++)  normal_vector[i]=new_ball_pos[i]-closest_p[i];
            closest_point(ball->position, new_ball_pos, normal_vector, closest_p);
            GLfloat speed_module=norm3D(ball->speed);

            GLfloat next_pos[3];
            GLfloat new_speed[3];

            for(int i=0; i<3; i++){
                next_pos[i]=2*closest_p[i]-ball->position[i];
                new_speed[i]=next_pos[i]-new_ball_pos[i];
            }
            
            GLfloat new_speed_module=norm3D(new_speed);
            for(int i=0; i<3; i++){
                ball->position[i] = next_pos[i];
                ball->speed[i] = (new_speed[i]/new_speed_module)*rebound_coeff;
            }

            return true;
        }
    //}else return false;
    
}

bool ballTouchesSphere(Ball *ball, GLfloat new_ball_pos[], GLfloat sph_centre[], GLfloat sph_radius, GLfloat rebound_coeff){
    if(distance3D(new_ball_pos, sph_centre)<=ball->radius+sph_radius){
        GLfloat normal_vector[3];
        for(int i=0; i<3; i++){
            normal_vector[i]=new_ball_pos[i]-sph_centre[i];
        }

        GLfloat closest_p[3];
        closest_point(ball->position, new_ball_pos, normal_vector, closest_p);
        GLfloat speed_module=norm3D(ball->speed);

        GLfloat next_pos[3];
        GLfloat new_speed[3];

        for(int i=0; i<3; i++){
            next_pos[i]=2*closest_p[i]-ball->position[i];
            new_speed[i]=next_pos[i]-new_ball_pos[i];
        }
        
        GLfloat new_speed_module=norm3D(new_speed);
        for(int j=0; j<3; j++){
            ball->position[j] = next_pos[j];
            ball->speed[j] = (new_speed[j]/new_speed_module)*rebound_coeff;
        }

        return true;
    }else return false;
}

bool ballTouchesEllipsoid(Ball *ball, GLfloat new_ball_pos[], GLfloat ellip_centre[], GLfloat ellip_size[], GLfloat rebound_coeff){

    //Next condition comes from the equation for an ellipsoid
    if(pow(new_ball_pos[0]-ellip_centre[0], 2)/pow(ellip_size[0]+ball->radius, 2) + pow(new_ball_pos[1]-ellip_centre[1], 2)/pow(ellip_size[1]+ball->radius, 2) + pow(new_ball_pos[2]-ellip_centre[2], 2)/pow(ellip_size[2]+ball->radius, 2)<=1){
        for(int i=0; i<3; i++){
            ball->speed[i]=-ball->speed[i]*rebound_coeff;
        }
        return true;
    }else return false;
};

RobotHead::RobotHead(GLfloat pos[], GLfloat s[], GLfloat* cl, LTexture *f, GLint orient){
    for (int i=0; i<3; i++){
        position[i]=pos[i];
        size[i]=s[i];
    }
    color=cl;
    if(orient==-1)  orientation=orient;
    face=f;
    
}

RobotHead::~RobotHead(){
    delete face;
}

void RobotHead::translate(GLfloat v[]){
    for(int i=0; i<3; i++){
        position[i]+=v[i];
    }
}

void RobotHead::draw(){

    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    glScalef(size[0], size[1], size[2]);
    drawFramedCube(WHITE, color, 0.05);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]+orientation*(size[2]+0.001f));
    glScalef(size[0], size[1], 1.0f);
    if(orientation==-1) glRotatef(180, 0, 1, 0);
    face->render(1.0f, 1.0f);
    glPopMatrix();
}

void RobotHead::loadRandomFace(std::string path){

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        /*Counting the number of faces*/
        vector<string> photos_names;
        int number_of_faces=0;
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0]!='.'){
                photos_names.push_back(ent->d_name);
                number_of_faces++;
            }
        }

        //cout<<number_of_faces<<endl;
        if(number_of_faces>0){
            int random_face_number=rand()%(number_of_faces);
            string photo_path=path+"/"+photos_names[random_face_number];
            cout<<photo_path<<endl;
            face->loadTextureFromFile(photo_path); 
        }
        
        closedir (dir);
    } 

}

bool RobotHead::ballTouchesHead(Ball *ball, GLfloat new_ball_pos[]){
    return ballTouchesPrism(ball, new_ball_pos, position, size, Robot::REBOUND_COEFF);
}

RobotBody::RobotBody(GLfloat rn, GLfloat s_torax[], GLfloat s_hip[], GLfloat pos[], GLfloat* cl){
    r_neck=rn;
    for(int i=0; i<3; i++){
        size_torax[i]=s_torax[i];
        size_hip[i]=s_hip[i];
        position[i]=pos[i];
    }
    color=cl;
}

void RobotBody::translate(GLfloat v[]){
    for(int i=0; i<3; i++){
        position[i]+=v[i];
    }
}

void RobotBody::draw(){
    GLfloat pos[3]={position[0], position[1]+size_torax[1]+r_neck, position[2]};
    glPushMatrix();
    glColor3fv(color);
    glTranslatef(pos[0], pos[1], pos[2]);
    drawSphere(r_neck);
    glPopMatrix();

    glPushMatrix();
    glColor3fv(WHITE);
    glTranslatef(position[0], position[1], position[2]);
    glScalef(size_torax[0], size_torax[1], size_torax[2]);
    drawFramedCube(WHITE, color, 0.05);
    glPopMatrix();

    glPushMatrix();
    glColor3fv(color);
    glTranslatef(position[0], position[1]-size_torax[1]-size_hip[1], position[2]);
    glScalef(size_hip[0], size_hip[1], size_hip[2]);
    drawSphere(1.0);
    glPopMatrix();
}

bool RobotBody::ballTouchesBody(Ball *ball, GLfloat new_ball_pos[]){
    GLfloat neck_pos[3]={position[0], position[1]+size_torax[1]+r_neck, position[2]};
    GLfloat hip_pos[3]={position[0], position[1]-size_torax[1]-size_hip[1], position[2]};

    //ballTouchesSphere and Ellipsoid do not work properly for now
    /*if(ballTouchesPrism(ball, new_ball_pos, position, size_torax, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesSphere(ball, new_ball_pos, neck_pos, r_neck, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesEllipsoid(ball, new_ball_pos, hip_pos, size_hip, Robot::REBOUND_COEFF)) return true;
    else return false;*/

    if(ballTouchesPrism(ball, new_ball_pos, position, size_torax, Robot::REBOUND_COEFF)) return true;
    else{
        GLfloat cube_r[]={r_neck, r_neck, r_neck};
        if(ballTouchesPrism(ball, new_ball_pos, neck_pos, cube_r, Robot::REBOUND_COEFF)) return true;
        else if(ballTouchesPrism(ball, new_ball_pos, hip_pos, size_hip, Robot::REBOUND_COEFF)) return true;
        else return false;
    }

}


RobotArm::RobotArm(GLfloat position[], GLfloat *cl, GLint orient){
    
    for(int i=0; i<3; i++){
        shoulder_p[i]=position[i];
    } 
    arm_angle=0;    //*orientation
    forearm_angle=0;    //*orientation
    finger_angle=PI/4.5f;
    joint_angle=PI/3.f;
    computePositions();

    color=cl;
    orientation=orient;
}

void RobotArm::computePositions(){
    arm_p[0][0]=arm_p[1][0]=forearm_p[0][0]=forearm_p[1][0]=elbow_p[0]=hand_p[0]=shoulder_p[0];
    arm_p[0][1]=shoulder_p[1]-cos(arm_angle)*shoulder_radius;
    arm_p[0][2]=shoulder_p[2]+sin(arm_angle)*shoulder_radius;
    arm_p[1][1]=shoulder_p[1]-cos(arm_angle)*(shoulder_radius+2*arm_length);
    arm_p[1][2]=shoulder_p[2]+sin(arm_angle)*(shoulder_radius+2*arm_length);

    elbow_p[1]=shoulder_p[1]-cos(arm_angle)*(shoulder_radius+2*arm_length+elbow_radius);
    elbow_p[2]=shoulder_p[2]+sin(arm_angle)*(shoulder_radius+2*arm_length+elbow_radius);

    forearm_p[0][1]=elbow_p[1]-cos(arm_angle+forearm_angle)*elbow_radius;
    forearm_p[0][2]=elbow_p[2]+sin(arm_angle+forearm_angle)*elbow_radius;
    forearm_p[1][1]=elbow_p[1]-cos(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length);
    forearm_p[1][2]=elbow_p[2]+sin(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length);

    hand_p[1]=elbow_p[1]-cos(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length+hand_radius);
    hand_p[2]=elbow_p[2]+sin(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length+hand_radius);

    phalanx_p[0][0][0]=shoulder_p[0]-hand_radius*sin(finger_angle);
    phalanx_p[1][0][0]=shoulder_p[0]+hand_radius*sin(finger_angle);
    phalanx_p[0][1][0]=shoulder_p[0]-(hand_radius+phalanx_length)*sin(finger_angle);
    phalanx_p[1][1][0]=shoulder_p[0]+(hand_radius+phalanx_length)*sin(finger_angle);
    phalanx_p[0][0][1]=phalanx_p[1][0][1]=hand_p[1]-hand_radius*cos(arm_angle+forearm_angle)*cos(finger_angle);
    phalanx_p[0][1][1]=phalanx_p[1][1][1]=hand_p[1]-(hand_radius+phalanx_length)*cos(arm_angle+forearm_angle)*cos(finger_angle);
    phalanx_p[0][0][2]=phalanx_p[1][0][2]=hand_p[2]+hand_radius*sin(arm_angle+forearm_angle)*cos(finger_angle);
    phalanx_p[0][1][2]=phalanx_p[1][1][2]=hand_p[2]+(hand_radius+phalanx_length)*sin(arm_angle+forearm_angle)*cos(finger_angle);

    joint_p[0][0]=hand_p[0]-(hand_radius+phalanx_length+joint_radius)*sin(finger_angle);
    joint_p[1][0]=hand_p[0]+(hand_radius+phalanx_length+joint_radius)*sin(finger_angle);
    joint_p[0][1]=joint_p[1][1]=phalanx_p[0][1][1]-joint_radius*cos(arm_angle+forearm_angle);
    joint_p[0][2]=joint_p[1][2]=phalanx_p[0][1][2]+joint_radius*sin(arm_angle+forearm_angle);
    
    phalanx_p[2][0][0]=joint_p[0][0]+joint_radius*sin(joint_angle);
    phalanx_p[3][0][0]=joint_p[1][0]-joint_radius*sin(joint_angle);
    phalanx_p[2][1][0]=joint_p[0][0]+(joint_radius+phalanx_radius)*sin(joint_angle);
    phalanx_p[3][1][0]=joint_p[1][0]-(joint_radius+phalanx_radius)*sin(joint_angle);
    phalanx_p[2][0][1]=phalanx_p[3][0][1]=joint_p[0][1]-joint_radius*cos(arm_angle+forearm_angle)*cos(joint_angle);
    phalanx_p[2][1][1]=phalanx_p[3][1][1]=joint_p[0][1]-(joint_radius+phalanx_length)*cos(arm_angle+forearm_angle)*cos(joint_angle);
    phalanx_p[2][0][2]=phalanx_p[3][0][2]=joint_p[0][2]+joint_radius*sin(arm_angle+forearm_angle)*cos(joint_angle);
    phalanx_p[2][1][2]=phalanx_p[3][1][2]=joint_p[0][2]+(joint_radius+phalanx_length)*sin(arm_angle+forearm_angle)*cos(finger_angle);

}

void RobotArm::lift(GLfloat lift_angle){
    arm_angle+=lift_angle*orientation;
    computePositions();
}

void RobotArm::flex(GLfloat flex_angle){
    forearm_angle+=flex_angle*orientation;
    computePositions();
}

void RobotArm::translate(GLfloat v[]){
    for(int i=0; i<4; i++){
        for(int j=0; j<2; j++){
            for(int k=0; k<3; k++){
                phalanx_p[i][j][k]+=v[k];
            }
        }
    }

    for(int j=0; j<2; j++){
        for(int k=0; k<3; k++){
            joint_p[j][k]+=v[k];
            arm_p[j][k]+=v[k];
            forearm_p[j][k]+=v[k];
        }
    }

    for(int k=0; k<3; k++){
        shoulder_p[k]+=v[k];
        elbow_p[k]+=v[k];
        hand_p[k]+=v[k];
    }
}

void RobotArm::draw(){
    glPushMatrix();
    glColor3fv(color);
    glTranslatef(shoulder_p[0], shoulder_p[1], shoulder_p[2]);
    drawSphere(shoulder_radius);
    glPopMatrix();

    glColor3fv(WHITE);
    drawCylinder(arm_p[0], arm_p[1], arm_radius, ACCURACY);

    glPushMatrix();
    glColor3fv(color);
    glTranslatef(elbow_p[0], elbow_p[1], elbow_p[2]);
    drawSphere(elbow_radius);
    glPopMatrix();

    glColor3fv(WHITE);
    drawCylinder(forearm_p[0], forearm_p[1], forearm_radius, ACCURACY);

    glPushMatrix();
    glColor3fv(color);
    glTranslatef(hand_p[0], hand_p[1], hand_p[2]);
    drawSphere(hand_radius);
    glPopMatrix();

    glColor3fv(WHITE);
    drawCylinder(phalanx_p[0][0], phalanx_p[0][1], phalanx_radius, ACCURACY);
    drawCylinder(phalanx_p[1][0], phalanx_p[1][1], phalanx_radius, ACCURACY);

    glPushMatrix();
    glColor3fv(color);
    glTranslatef(joint_p[0][0], joint_p[0][1], joint_p[0][2]);
    drawSphere(joint_radius);
    glPopMatrix();

    glPushMatrix();
    glColor3fv(color);
    glTranslatef(joint_p[1][0], joint_p[1][1], joint_p[1][2]);
    drawSphere(joint_radius);
    glPopMatrix();

    glColor3fv(WHITE);
    drawCylinder(phalanx_p[2][0], phalanx_p[2][1], phalanx_radius, ACCURACY);
    drawCylinder(phalanx_p[3][0], phalanx_p[3][1], phalanx_radius, ACCURACY);

}

//I do not check the fingers
bool RobotArm::ballTouchesArm(Ball *ball, GLfloat new_ball_pos[]){
    /*if(ballTouchesSphere(ball, new_ball_pos, shoulder_p, shoulder_radius, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesSphere(ball, new_ball_pos, elbow_p, elbow_radius, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesSphere(ball, new_ball_pos, hand_p, hand_radius, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesCylinder(ball, new_ball_pos, arm_p[0], arm_p[1], arm_radius, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesCylinder(ball, new_ball_pos, forearm_p[0], forearm_p[1], forearm_radius, Robot::REBOUND_COEFF)) return true;
    else return false;*/

    GLfloat shoulder_size[]={shoulder_radius,shoulder_radius,shoulder_radius};
    if(ballTouchesPrism(ball, new_ball_pos, shoulder_p, shoulder_size, Robot::REBOUND_COEFF)) return true;
    else{
        GLfloat elbow_size[]={elbow_radius, elbow_radius, elbow_radius};
        if(ballTouchesPrism(ball, new_ball_pos, elbow_p, elbow_size, Robot::REBOUND_COEFF)) return true;
        else{
            GLfloat hand_size[]={hand_radius, hand_radius, hand_radius};
            if(ballTouchesPrism(ball, new_ball_pos, hand_p, hand_size, Robot::REBOUND_COEFF)) return true;
            else{

                GLfloat arm_size[]={arm_radius, (arm_p[1][1]-arm_p[0][1])/2, arm_radius};
                GLfloat arm_centre[3];
                for(int i=0; i<3; i++){
                    arm_centre[i]=arm_p[0][i]/2+arm_p[1][i]/2;
                }
                if(ballTouchesPrism(ball, new_ball_pos, arm_centre, arm_size, Robot::REBOUND_COEFF)) return true;
                else{
                    GLfloat forearm_size[]={forearm_radius, (forearm_p[1][1]-forearm_p[0][1])/2, forearm_radius};
                    GLfloat forearm_centre[3];
                    for(int i=0; i<3; i++){
                        forearm_centre[i]=forearm_p[0][i]/2+forearm_p[1][i]/2;
                    }
                    if(ballTouchesPrism(ball, new_ball_pos, forearm_centre, forearm_size, Robot::REBOUND_COEFF)) return true;
                    else return false;
                }
            }
        }
    }
}

void RobotArm::setBallPosition(GLfloat pos[]){
    pos[0]=hand_p[0];
    //Using the cosine law
    GLfloat distance_from_hand=sqrt(2*(phalanx_length+joint_radius)*(phalanx_length+joint_radius)*( 1.0f-cos(joint_angle) ) );
    pos[1]=elbow_p[1]-cos(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length+2*hand_radius+distance_from_hand);
    pos[2]=elbow_p[2]+sin(arm_angle+forearm_angle)*(elbow_radius+2*forearm_length+2*hand_radius+distance_from_hand);
}

RobotPlatform::RobotPlatform(const GLfloat pos[], GLfloat *cl){
    for(int i=0; i<3; i++){
        position[i]=pos[i];
    }

    color=cl;
}

void RobotPlatform::translate(GLfloat v[]){
    for(int i=0; i<3; i++){
        position[i]+=v[i];
    }
}

void RobotPlatform::draw(){
    GLfloat top_base[3]={position[0], position[1]+base_height, position[2]};
    GLfloat top_pillar[3]={position[0], top_base[1]+pillar_length, position[2]};

    glColor3fv(color);
    drawCylinder(position, top_base, base_radius, ACCURACY);
    drawCylinder(top_base, top_pillar, pillar_radius, ACCURACY);

    GLfloat origin[3]={0.0f, 0.0f, 0.0f};
    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    drawDisk(base_radius, origin, ACCURACY);
    glTranslatef(0.0f, base_height, 0.0f);
    drawDisk(base_radius, origin, ACCURACY);
    glPopMatrix();

    glPushMatrix();
    glColor3fv(WHITE);
    glTranslatef(position[0], position[1], position[2]);
    drawTorus(base_radius, 0.005, ACCURACY*9, 40);
    glTranslated(0, base_height, 0);
    drawTorus(base_radius, 0.005, ACCURACY*9, 40);
    glPopMatrix();
}

bool RobotPlatform::ballTouchesPlatform(Ball *ball, GLfloat new_ball_pos[]){
    /*GLfloat top_base[3]={position[0], position[1]+base_height, position[2]};
    GLfloat top_pillar[]={position[0], top_base[1]+pillar_length, position[2]};

    if(ballTouchesCylinder(ball, new_ball_pos, position, top_base, base_radius, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesCylinder(ball, new_ball_pos, top_base, top_pillar, pillar_radius, Robot::REBOUND_COEFF)) return true;
    else return false;*/

    GLfloat centre_base[3]={position[0], position[1]+base_height/2, position[2]};
    GLfloat base_size[3]={base_radius, base_height/2, base_radius};
    GLfloat centre_pillar[]={position[0], position[1]+base_height+pillar_length/2, position[2]};
    GLfloat pillar_size[3]={pillar_radius, pillar_length/2, pillar_radius};

    if(ballTouchesPrism(ball, new_ball_pos, centre_base, base_size, Robot::REBOUND_COEFF)) return true;
    else if(ballTouchesPrism(ball, new_ball_pos, centre_pillar, pillar_size, Robot::REBOUND_COEFF)) return true;
    else return false;
}

Robot::Robot(GLfloat pos[], GLfloat cl[], GLint orient, std::string name){

    plat = new RobotPlatform (pos, color);

    GLfloat torax_size[3]={0.2, 0.25, 0.2};
    GLfloat hip_size[3]={0.2, 0.1, 0.2};
    GLfloat body_pos[3]={pos[0], 
                        pos[1]+RobotPlatform::base_height+RobotPlatform::pillar_length+2*hip_size[1]+torax_size[1], 
                        pos[2]};
    body = new RobotBody (0.05, torax_size, hip_size, body_pos, color);

    GLfloat head_size[3]={0.2, 0.15, 0.15};
    GLfloat head_pos[3]={body_pos[0], body_pos[1]+torax_size[1]+2*body->r_neck+head_size[1], body_pos[2]};
    LTexture* face=new LTexture;
    head = new RobotHead (head_pos, head_size, color, face, orient);

    GLfloat larm_pos[3]={body_pos[0]+torax_size[0]+RobotArm::shoulder_radius,
                        body_pos[1]+torax_size[1]-RobotArm::shoulder_radius, 
                        body_pos[2]};
    larm = new RobotArm (larm_pos, color, orient);

    GLfloat rarm_pos[3]={body_pos[0]-torax_size[0]-RobotArm::shoulder_radius,
                        body_pos[1]+torax_size[1]-RobotArm::shoulder_radius, 
                        body_pos[2]};
    rarm = new RobotArm (rarm_pos, color, orient);

    for(int i=0; i<3; i++) color[i]=cl[i];
    
    if(orient==-1)  orientation=orient;

    this->name=name;
    loadHappyFace();

}

Robot::~Robot(){
    delete head;
    delete body;
    delete larm;
    delete rarm;
}

void Robot::loadHappyFace(){
    if(mood!=happy){
        string path="./Characters/"+name+"/HappyFaces";
        head->loadRandomFace(path);
        mood=happy;
    }
    
}

void Robot::loadSadFace(){
    if(mood!=sad){
        string path="./Characters/"+name+"/SadFaces";
        head->loadRandomFace(path);
        mood=sad;
    }
    
}

/*void Robot::loadAngryFace(){
    string path="./Characters/"+name+"/AngryFaces";
    head->loadRandomFace(path);
}*/

void Robot::draw(){
    head->draw();
    body->draw();
    larm->draw();
    rarm->draw();
    plat->draw();
} //This will draw every part

void Robot::translate(GLfloat v[]){
    head->translate(v);
    body->translate(v);
    larm->translate(v);
    rarm->translate(v);
    plat->translate(v);
}

bool Robot::ballTouchesRobot(Ball *ball, GLfloat new_ball_pos[]){
    bool touching=false;
    if(head->ballTouchesHead(ball, new_ball_pos))   touching=true;
    else if(body->ballTouchesBody(ball, new_ball_pos))  touching=true;
    else if(larm->ballTouchesArm(ball, new_ball_pos))   touching=true;
    else if(rarm->ballTouchesArm(ball, new_ball_pos))   touching=true;
    else if(plat->ballTouchesPlatform(ball, new_ball_pos))  touching=true;
    
    if(touching){
        if(norm3D(ball->speed)>SENSITIVITY){
            loadSadFace();
        }

        return true;
    }else return false;
}

void Robot::setBallPosition(GLfloat pos[]){
    if(orientation==1) return rarm->setBallPosition(pos);
    else return larm->setBallPosition(pos);
}

GLfloat Robot::xPosition(){
    return plat->position[0];
}