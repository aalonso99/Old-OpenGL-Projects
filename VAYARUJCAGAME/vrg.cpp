#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cmath>
#include <string>
#include <cstdlib>
#include <chrono>  // for high_resolution_clock
#include <iostream>
#include <irrKlang.h>
#include <omp.h>
#include "LTexture.h"
#include "robot.h"
#include "geometry.h"
#include "objects.h"
using namespace std;

/*Screen parameters*/
const double width=1360.0;
const double height=768.0;
const GLfloat camera_depth=-4.0;

/*Sound engine*/
irrklang::ISoundEngine *sound_engine;
bool sound_works=true;

/*Light*/
/*GLfloat Light0Ambient[]={ 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat Light0Diffuse[]={ 0.3f, 1.0f, 0.3f, 1.0f };
GLfloat LightAmbient[]={ 0.5f, 0.5f, 0.5f, 0.8f };
GLfloat LightDiffuse[]={ 0.3f, 0.3f, 0.8f, 0.3f };
GLfloat LightSpecular[]={ 1.0f, 0.8f, 0.8f, 0.8f };
GLfloat LightPosition[]={ 0.0f, 2.0f, 0.0f, 1.0f };
GLfloat SpotDirection[]={0.0f, -1.0f, 0.0f};
GLfloat SpotCutoff=15.0f;*/

/*GL initialization*/
void init(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width/height, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Diffuse);

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, SpotDirection);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, SpotCutoff);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);*/

    glEnable(GL_TEXTURE_2D);

    //Initialize DevIL and DevILU
    ilInit();
    iluInit();
    ilClearColour( 255, 255, 255, 000 );

    //Check for error
    ILenum ilError = ilGetError();
    if( ilError != IL_NO_ERROR )
    {
        printf( "Error initializing DevIL! %s\n", iluErrorString( ilError ) );
        exit(-1);
    }

    sound_engine=irrklang::createIrrKlangDevice();
    if(!sound_engine)   sound_works=false;
}

float DRand(float DMin, float DMax)
{
    float f = (float)rand() / RAND_MAX;
    return DMin + f * (DMax - DMin);
}

//It is necessary to call init() before creating a scene!!!
class Scene{
    Table *table;
    Ball *ball;
    Robot *player1, *player2, *selected_player;
    vector<Glass> glasses;

    chrono::high_resolution_clock::time_point time;
    LTexture *background;
    LTexture *floor;
    const GLint INT_STEPS=5000;
    
    /*Size of the table*/
    const GLfloat TABLE_LENGTH=1.4;
    const GLfloat TABLE_WIDTH=1.1;
    const GLfloat TABLE_THICKNESS=0.1;
    const GLfloat TABLE_HEIGHT=1.0;

    /*Playability preferences*/
    const GLfloat BALL_RADIUS=0.02;
    const GLfloat BALL_BASE_SPEED=1.0;
    const GLfloat SPEED_GROWTH=3.5;     //Specifyies the ratio with which the ball throw speed increases when pressing the button
    const GLfloat MAX_SPEED=12.0;
    const GLfloat GLASS_BASE_RADIUS=0.03;
    const GLfloat GLASS_TOP_RADIUS=0.04;
    const GLfloat GLASS_HEIGHT=0.1;
    const GLfloat GLASS_WALL_ANGLE=atan((GLASS_TOP_RADIUS-GLASS_BASE_RADIUS)/GLASS_HEIGHT);
    const GLfloat GRAVITY=9.0f;
    const GLfloat P_SPEED=1.0f;
    const GLfloat TIME_LIMIT=3.0f;
    const GLfloat MAX_ANGLE_UP=PI/6+PI/2;
    const GLfloat MAX_ANGLE_DOWN=PI/8+PI/2;

    /*Initial values*/
    //const double initial_ball_position[]={0.0,1,0.0};
    const vector<pair<GLfloat, GLfloat> > glasses_positions={make_pair(0.0,-1.025), make_pair(-0.09,-1.125), make_pair(0.09, -1.125), make_pair(-0.14, -1.225), make_pair(0.0, -1.225), make_pair(0.14, -1.225), make_pair(0.0,1.025), make_pair(0.09,1.125), make_pair(-0.09,1.125), make_pair(0.14, 1.225), make_pair(0.0, 1.225), make_pair(-0.14, 1.225)}; 
    vector<bool> active_glasses;
    bool initial_state=true;

    bool touchTable=false;
    bool touchGlass=false;
    bool touchRobot=false;
    bool touchFloor=false;

    //Ball throw variables
    chrono::high_resolution_clock::time_point charging_time;
    chrono::high_resolution_clock::time_point throw_start_time;

    GLfloat camera_x;

    public:

    Scene(string name_p1, string name_p2){
        //The table is always created in the center
        table = new Table(TABLE_LENGTH, TABLE_WIDTH, TABLE_THICKNESS, TABLE_HEIGHT);

        GLfloat ball_position[3]={-0.1f,TABLE_HEIGHT+TABLE_THICKNESS+BALL_RADIUS+0.3f,TABLE_LENGTH/2};
        GLfloat ball_speed[3]={0.0f,0.0f, 0.0f};
        ball = new Ball(BALL_RADIUS, ball_position, ball_speed);

        GLfloat position_p1[]={0.1f,-1.0f,-1.8f};
        GLfloat color1[]={0.0f,0.0f,1.0f};
        player1 = new Robot(position_p1, color1, 1, name_p1);

        GLfloat position_p2[]={-0.3f, -1.0f, 1.8f};
        GLfloat color2[]={1.0f, 0.0f, 0.0f};
        player2 = new Robot(position_p2, color2, -1, name_p2);
        selected_player=player2;

        background=new LTexture;
        background->loadTextureFromFile("./background.jpg");
        this->floor=new LTexture;
        background->loadTextureFromFile("./floor.jpg");

        camera_x=-selected_player->xPosition()-selected_player->body->size_torax[0];

        for(int i=0; i<12; i++){
            Glass new_glass(GLASS_BASE_RADIUS, GLASS_TOP_RADIUS, GLASS_HEIGHT, glasses_positions[i]);
            glasses.push_back(new_glass);
            active_glasses.push_back(true);
        }

        setThrow();

    }

    ~Scene(){
        delete table;
        delete ball;
        delete player1;
        delete player2;
    }

    Robot* getPlayer(int num){
        if(num==1){
            return player1;
        }else if(num==2){
            return player2;
        }else return NULL;
    }

    //In case of touching, changes the direction of the ball
    bool ballTouchesTable(GLfloat new_ball_pos[]){
        GLfloat origin[3]={0.0f, 0.0f, 0.0f};
        GLfloat table_size[3]={TABLE_WIDTH, TABLE_THICKNESS, TABLE_LENGTH};

        return ballTouchesPrism(ball, new_ball_pos, origin, table_size, Table::REBOUND_COEFF);
    }

    bool ballTouchesFloor(GLfloat new_ball_pos[]){
        if(new_ball_pos[1]<=-TABLE_HEIGHT+ball->radius){
            ball->speed[1]=-(ball->speed[1])*(Table::REBOUND_COEFF);
            return(true);
        } else return(false);
    }

    //We will consider only two types of collisions with the torus, to simplify the mathematics
    bool ballTouchesTorus(GLfloat new_ball_pos[], GLfloat torPos[], GLfloat r1, GLfloat r2){
        GLfloat ball_projection[]={new_ball_pos[0], new_ball_pos[2]};
        GLfloat torPos_projection[]={torPos[0], torPos[2]};

        if(distance2D(ball_projection, torPos_projection)<=ball->radius+r1){    
            //Possible collision point
            GLfloat collision_point[]={(new_ball_pos[0]-torPos[0])*(r1/(ball->radius+r1)), torPos[1], (new_ball_pos[2]-torPos[2])*(r1/(ball->radius+r1))};

            if(distance3D(new_ball_pos, collision_point)<=ball->radius+r2){ //The ball actually collides with the torus
                GLfloat collision_vector[]={new_ball_pos[0]-collision_point[0], new_ball_pos[1]-collision_point[1], new_ball_pos[2]-collision_point[2]};
                GLfloat collision_vector_projection[]={collision_vector[0], 0.0f, collision_vector[2]};
                GLfloat collision_angle=angle(collision_vector, collision_vector_projection);
                if(collision_angle > PI/2)    collision_angle=PI-collision_angle;
                if(collision_angle >= Glass::CONTACT_ANGLE){
                    ball->speed[1]=-(ball->speed[1])*Glass::REBOUND_COEFF;
                } else {
                    GLfloat speed_module=norm3D(ball->speed)*Glass::REBOUND_COEFF/norm3D(collision_vector);
                    for(int i=0; i<3; i++){
                        ball->speed[i]=collision_vector[i]*speed_module;
                    }
                }
                
                return(true);
            }
        }

        return(false);
   
    }

    //Since the ball needs to reflect against the glass wall, to compute the direction we need not only the 
    //new position of the ball, but also the previous one
    bool ballTouchesGlassWall(GLfloat new_ball_pos[], const int &i){

        GLfloat ball_proj[2]={new_ball_pos[0], new_ball_pos[2]};
        GLfloat glass_centre[2]={glasses[i].position.first, glasses[i].position.second};

        GLfloat min_distance = BALL_RADIUS*cos(GLASS_WALL_ANGLE)+GLASS_BASE_RADIUS+(new_ball_pos[1]-TABLE_THICKNESS)*tan(GLASS_WALL_ANGLE);

        //Check if the ball is colliding the glass
        if(distance2D(ball_proj, glass_centre) <= min_distance){
            //cout<<"V1: "<<ball->speed[0]<<", "<<ball->speed[1]<<", "<<ball->speed[2]<<endl;

            GLfloat normal_vector_proj[2]={ball_proj[0]-glass_centre[0], ball_proj[1]-glass_centre[1]};
            GLfloat v_length=norm2D(normal_vector_proj);
            
            GLfloat normal_vector[3]={normal_vector_proj[0]/v_length*cos(GLASS_WALL_ANGLE), -sin(GLASS_WALL_ANGLE), normal_vector_proj[1]/v_length*cos(GLASS_WALL_ANGLE)};
            //The normal vector and the new position of the ball let us calculate a normal line to the glass 
            //this line is of the form (new_ball_pos[0]+t*normal_vector[0], ...) where t is a parameter

            //This is the value of the parameter for the closest point in the line to ball_position 
            GLfloat t=(ball->position[0]+ball->position[1]+ball->position[2]-new_ball_pos[0]-new_ball_pos[1]-new_ball_pos[2])
                        /(normal_vector[0]+normal_vector[1]+normal_vector[2]);

            GLfloat closest_p[3]={new_ball_pos[0]+t*normal_vector[0], new_ball_pos[1]+t*normal_vector[1], new_ball_pos[2]+t*normal_vector[2]};
            GLfloat next_pos[3]={2*closest_p[0]-ball->position[0], 2*closest_p[1]-ball->position[1], 2*closest_p[2]-ball->position[2]};

            GLfloat speed_module=norm3D(ball->speed);
            GLfloat new_speed[3]={next_pos[0]-new_ball_pos[0], next_pos[1]-new_ball_pos[1], next_pos[2]-new_ball_pos[2]};
            GLfloat new_speed_module=norm3D(new_speed);

            for(int j=0; j<3; j++){
                //cout<<ball->position[j]<<endl<<next_pos[j]<<endl;
                ball->position[j]=next_pos[j];
                ball->speed[j] = (new_speed[j]/new_speed_module)*speed_module*Glass::REBOUND_COEFF;
            }

            //cout<<"V2: "<<ball->speed[0]<<", "<<ball->speed[1]<<", "<<ball->speed[2]<<endl;

            return (true);
        } else return (false);
        
    }

    //Parametre i must be the index of the glass that we are checking
    bool ballTouchesGlass(GLfloat new_ball_pos[], const int &i){
        if(new_ball_pos[1]<=TABLE_THICKNESS+GLASS_HEIGHT+ball->radius){
            //GLfloat glass_top_centre[3]={glasses[i].position.first, TABLE_THICKNESS+GLASS_HEIGHT, glasses[i].position.second};
            if(ballTouchesGlassWall(new_ball_pos, i)){
                return(true);
            } /*else if(ballTouchesTorus(new_ball_pos, glass_top_centre, GLASS_TOP_RADIUS, Glass::TORUS_RADIUS)){
                return(true);
            } */else return(false);
        }else return(false);

        /*for(int i=0; i<INT_STEPS; i++){
            GLfloat baricentric_weight=(float)i/INT_STEPS;
            GLfloat intermediate_pos[3];
            for(int j=0; j<3; j++){
                intermediate_pos[j]={baricentric_weight*new_ball_pos[j]+(1-baricentric_weight)*ball->position[j]};
            }

            if(intermediate_pos[1]<=TABLE_THICKNESS+GLASS_HEIGHT+ball->radius){
                GLfloat glass_top_centre[3]={glasses[i].position.first, TABLE_THICKNESS+GLASS_HEIGHT, glasses[i].position.second};
                if(ballTouchesGlassWall(intermediate_pos, i)){
                    cout<<"Toca pared"<<endl;
                    return(true);
                } else if(ballTouchesTorus(intermediate_pos, glass_top_centre, GLASS_TOP_RADIUS, Glass::TORUS_RADIUS)){
                    cout<<"Toca borde"<<endl;
                    return(true);
                } else return(false);
            }
        }

        return false;*/
    }

    bool ballTouchesRobot(Robot *player, GLfloat new_ball_pos[]){
        return player->ballTouchesRobot(ball, new_ball_pos);
    }

    /*bool ballEntersGlass(int i){
        bool in_scope = i<glasses.size() && i>=0;
        if(in_scope){
            bool in_height_range=ball->position[1]<TABLE_THICKNESS+GLASS_HEIGHT && ball->position[1]<TABLE_THICKNESS+ball->radius;
            GLfloat ball_proj[2]={ball->position[0], ball->position[2]};
            GLfloat glass_proj[2]={glasses[i].position.first, glasses[i].position.second};
            //base radius plus the ball height with respect to the table times the ratio between the height and the increment in the radius of the glass
            //glass_radius depends on the height of the ball
            GLfloat glass_radius=GLASS_BASE_RADIUS+(ball->position[1]-TABLE_THICKNESS)*tan(GLASS_WALL_ANGLE);
            bool close_to_centre=distance2D(ball_proj,glass_proj)<glass_radius;
            if(in_height_range && close_to_centre){
                return true;
            }else return false;
        }else return false;
    }*/

    bool ballEntersGlass(int i){
        bool in_scope = i<glasses.size() && i>=0;
        if(in_scope){
            bool in_height_range=ball->position[1]<TABLE_THICKNESS+GLASS_HEIGHT && ball->position[1]>TABLE_THICKNESS+ball->radius;
            GLfloat ball_proj[2]={ball->position[0], ball->position[2]};
            GLfloat glass_proj[2]={glasses[i].position.first, glasses[i].position.second};
            //base radius plus the ball height with respect to the table times the ratio between the height and the increment in the radius of the glass
            //glass_radius depends on the height of the ball
            GLfloat glass_radius=GLASS_BASE_RADIUS+(ball->position[1]-TABLE_THICKNESS)*tan(GLASS_WALL_ANGLE);
            bool close_to_centre=distance2D(ball_proj,glass_proj)<glass_radius;
            if(in_height_range && close_to_centre){
                return true;
            }else return false;
        }else return false;
    }

    void moveBall(){
        if(!initial_state){

            //Checking the turn does not take too long
            chrono::duration<double> turn_time=time-throw_start_time;
            if(turn_time.count()<TIME_LIMIT){
                int i=0;
                bool goal=false;
                while(i<=INT_STEPS && !goal){
                    
                    auto current = chrono::high_resolution_clock::now();
                    chrono::duration<double> elapsed = current - time;
                    GLfloat dt=elapsed.count();
                    time = current;

                    ball->speed[1]-=GRAVITY*dt;
                    GLfloat new_ball_pos[3];
                    for(int i=0; i<3; i++){
                        new_ball_pos[i]=ball->position[i]+ball->speed[i]*dt;
                    }

                    bool on_table=abs(new_ball_pos[0])<TABLE_WIDTH && abs(new_ball_pos[2])<TABLE_LENGTH;

                    if(on_table){
                        if(new_ball_pos[2]<0){
                            int j=0;
                            while(!goal && j<glasses.size()/2){
                                if(active_glasses[j] && ballEntersGlass(j)){
                                    active_glasses[j]=false;
                                    goal=true;
                                }

                                j++;
                            }
                        }else{
                            int j=6;
                            while(!goal && j<glasses.size()){
                                if(active_glasses[j] && ballEntersGlass(j)){
                                    active_glasses[j]=false;
                                    goal=true;
                                }

                                j++;
                            }
                        }
                    }

                    #pragma omp parallel sections num_threads(4)
                    {

                        #pragma omp section
                        {
                            if(goal){
                                //make score sound
                                //restart original state
                                restArm(selected_player);
                                changeSelectedPlayer();
                                resetState();
                                setThrow();
                            }
                        }

                        #pragma omp section
                        {
                            if(!touchFloor){   
                                touchFloor=ballTouchesFloor(new_ball_pos);
                                if(touchFloor && norm3D(ball->speed)>1.0f){
                                    if(sound_works) sound_engine->play2D("blop.wav", false);
                                }
                            }else touchFloor=false;
                        }

                        
                        #pragma omp section
                        {
                            if(on_table && !touchGlass){
                                if(new_ball_pos[2]<0){
                                    int i=0;
                                    while(i<glasses.size()/2 && !touchGlass){
                                        if(active_glasses[i])   touchGlass=ballTouchesGlass(new_ball_pos, i);
                                        i++;
                                    }
                                }else{
                                    int i=6;
                                    while(i<glasses.size() && !touchGlass){
                                        if(active_glasses[i])   touchGlass=ballTouchesGlass(new_ball_pos, i);
                                        i++;
                                    }
                                }
                                

                                if(touchGlass && norm3D(ball->speed)>1.0f){ 
                                    if(sound_works) sound_engine->play2D("clack.wav", false);
                                    //cout<<ball->speed[0]<<", "<<ball->speed[1]<<", "<<ball->speed[2]<<endl;
                                }

                            }else   touchGlass=false;
                        }
                        
                        #pragma omp section
                        {
                            if(!touchTable){
                                touchTable=ballTouchesTable(new_ball_pos);
                                if(touchTable && norm3D(ball->speed)>1.0f){
                                    if(sound_works) sound_engine->play2D("blop.wav", false);
                                }

                            }else touchTable=false;
                        }
                        
                        #pragma omp section
                        {
                            if(!touchRobot){
                                if(!on_table){
                                    if(new_ball_pos[2]>0) touchRobot=ballTouchesRobot(player2, new_ball_pos);
                                    else    touchRobot=ballTouchesRobot(player1, new_ball_pos);

                                    if(touchRobot && norm3D(ball->speed)>1.0f){
                                        if(sound_works) sound_engine->play2D("clack.wav", false);
                                    }
                                }
                                
                            }else touchRobot=false;
                        }

                    }

                    //I only keep new_ball_pos as the next position if it does not collide
                    //Otherwise, I manage the change of speed and position inside each ballTouches* function
                    if(!goal && !touchFloor && !touchGlass && !touchRobot && !touchTable){
                        for(int i=0; i<3; i++){
                            ball->position[i]=new_ball_pos[i];
                        }
                    //In the next ifs I patch poorly the problems with the movement of the ball
                    }else if(touchTable && touchGlass){
                        ball->speed[1]=abs(ball->speed[1]);
                    }

                    if(on_table && abs(ball->position[1])<TABLE_THICKNESS)   ball->position[1]=TABLE_THICKNESS+BALL_RADIUS;

                    i++;
                        
                } 
        
            }else{
                restArm(selected_player);
                changeSelectedPlayer();
                resetState();
                setThrow();
            }
            
        }    

    }

    bool initialState(){
        return initial_state;
    }

    void resetState(){
        initial_state=true;
        touchFloor=touchGlass=touchRobot=touchTable=false;
        selected_player->loadHappyFace();
    }

    void setThrow(){
        if(initial_state){
            RobotArm *arm;
            if(selected_player->orientation==1){
                arm=selected_player->rarm;
            }else arm=selected_player->larm;

            arm->lift(PI/2-arm->orientation*arm->arm_angle);
            arm->flex(PI/2-arm->orientation*arm->forearm_angle);

            arm->setBallPosition(ball->position);
        }
    }

    void restArm(Robot* player){
        RobotArm *arm;
        if(player->orientation==1){
            arm=player->rarm;
        }else arm=player->larm;

        arm->lift(-arm->orientation*arm->arm_angle);
        arm->flex(-arm->orientation*arm->forearm_angle);
    }

    void liftArm(GLfloat angle){
        RobotArm *arm;
        if(selected_player==player1)    arm=player1->rarm;
        else if(selected_player==player2)   arm=player2->larm;

        if(angle>0 && abs(arm->arm_angle)<MAX_ANGLE_UP){
            arm->lift(angle);
            if(initial_state)   arm->setBallPosition(ball->position);
        } 
        else if(angle<0 && abs(arm->arm_angle)<MAX_ANGLE_DOWN){
            arm->lift(angle);
            if(initial_state)   arm->setBallPosition(ball->position);
        }

    }

    void startChargingThrow(){
        charging_time=chrono::high_resolution_clock::now();
    }

    //ONLY USE AFTER startChargingThrow() BECAUSE charging_time MUST BE INITIALIZED AND/OR UPDATED
    void throwBall(){
        if(initial_state){
            //Updating de time variable to move the ball according to time
            throw_start_time=time=chrono::high_resolution_clock::now();
            //Calculating the speed of the ball throw using the time we have pressed the button
            chrono::duration<double> elapsed = time-charging_time;
            GLfloat dt=elapsed.count();
            GLfloat initial_speed=BALL_BASE_SPEED+dt*SPEED_GROWTH;
            initial_speed=min(initial_speed, MAX_SPEED);

            //Setting the new speed. This method assumes we only throw the ball in the z axis
            GLfloat new_ball_speed[]={0.0f, 0.0f, selected_player->orientation*initial_speed};
            for(int i=0; i<3; i++){
                ball->speed[i]=new_ball_speed[i];
            }

            initial_state=false;
        }
    }

    void changeSelectedPlayer(){
        if(selected_player==player1)    selected_player=player2;
        else if(selected_player==player2)    selected_player=player1;

        camera_x=selected_player->orientation*selected_player->xPosition()-selected_player->body->size_torax[0];
    }

    //Player can only be moved in the x axis
    void movePlayer(GLfloat q){
        GLfloat v[]={-selected_player->orientation*P_SPEED*q, 0.0, 0.0};
        selected_player->translate(v);
        camera_x=selected_player->orientation*selected_player->xPosition()-selected_player->body->size_torax[0];
    }

    void displayFloor(){
        glPushMatrix();
        glTranslatef(0.0f,-TABLE_HEIGHT, 0.0f);
        glRotatef(-90, 1, 0, 0);
        glScalef(3*TABLE_WIDTH, 2.5f*TABLE_LENGTH, 1.0f);
        floor->render(1.0f, 1.0f);
        glPopMatrix();
    }

    void display(){

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        //Test settings
        /*
        //glRotatef(x_rotation,1,0,0);
        //glRotatef(y_rotation,0,1,0);
        //glRotatef(90, 1, 0, 0);
        glTranslatef(0, 0, -5);
        glRotatef(10, 1, 0, 0);
        glRotatef(90, 0, 1, 0);*/
        
        glTranslatef(camera_x, -0.6f, -TABLE_LENGTH-1.3f);
        glRotatef(10, 1, 0, 0);
        glRotatef(90+selected_player->orientation*90, 0, 1, 0);

        table->draw();
        ball->draw();
        player1->draw();
        player2->draw();
        displayFloor();

        glTranslatef(0.0,TABLE_THICKNESS, 0.0);
        for(int i=0; i<glasses.size(); i++){
            if(active_glasses[i]){
                glPushMatrix();
                glTranslatef(glasses[i].position.first, 0.0, glasses[i].position.second);
                glasses[i].draw();
                glPopMatrix();
            }
        }
    
    }
};


int main(int argc, char* args[]){

    SDL_Init(SDL_INIT_EVERYTHING);

    Uint8 *keys = SDL_GetKeyState(NULL); 

    SDL_SetVideoMode(width,height,0,SDL_SWSURFACE|SDL_OPENGL);

    int loop=1;
    //Uint32 start;
    SDL_Event myevent;
    init();

    string name_p1="";
    string name_p2="";
    if(argc==3){
        name_p1=args[1];
        name_p2=args[2];
    }

    Scene scene(name_p1, name_p2);
    scene.setThrow();
    scene.resetState();
    bool button_pressed=false;

    while (loop==1)
    {
        //start=SDL_GetTicks();
        while (SDL_PollEvent(&myevent))
        {
            switch(myevent.type)
            {
                case SDL_QUIT:
                    loop=0;
                break;
            }
        }

        if((keys[SDLK_LALT]||keys[SDLK_RALT])&&keys[SDLK_F4]) loop=0;
        
        if(keys[SDLK_LEFT]){
            scene.movePlayer(-0.1f);
            if(scene.initialState())    scene.setThrow();
        }
        if(keys[SDLK_RIGHT]){
            scene.movePlayer(0.1f);
            if(scene.initialState())    scene.setThrow();
        }

        if(keys[SDLK_SPACE]){
            if(!button_pressed){
                scene.startChargingThrow();
                button_pressed=true;
            }
        }else if(button_pressed){
            button_pressed=false;
            scene.throwBall();
        }

        if(keys[SDLK_UP]){
            scene.liftArm(0.05f);
        }

        if(keys[SDLK_DOWN]){
            scene.liftArm(-0.05f);
        }
        
        scene.moveBall();
        scene.display();
        SDL_GL_SwapBuffers();
        //if (1000/30>(SDL_GetTicks()-start)) SDL_Delay(SDL_GetTicks()-start);
    }
    SDL_Quit();
    return 0;
}
