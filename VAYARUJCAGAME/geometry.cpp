#include "geometry.h"

void crossProduct(GLfloat u[], GLfloat v[], GLfloat result[]){
    result[0]=u[1]*v[2]-u[2]*v[1];
    result[1]=u[2]*v[0]-u[0]*v[2];
    result[2]=u[0]*v[1]-u[1]*v[0];
}

GLfloat scalarProd(GLfloat u[], GLfloat v[]){
    return (u[0]*v[0]+u[1]*v[1]+u[2]*v[2]);
}

GLfloat norm3D(GLfloat v[]){
    return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

GLfloat norm2D(GLfloat v[]){
    return sqrt(v[0]*v[0]+v[1]*v[1]);
}

GLfloat distance2D(GLfloat u[], GLfloat v[]){
    return sqrt((u[0]-v[0])*(u[0]-v[0])+(u[1]-v[1])*(u[1]-v[1]));
}

GLfloat distance3D(GLfloat u[], GLfloat v[]){
    return sqrt((u[0]-v[0])*(u[0]-v[0])+(u[1]-v[1])*(u[1]-v[1])+(u[2]-v[2])*(u[2]-v[2]));
}

GLfloat angle(GLfloat u[], GLfloat v[]){
    return (acos(scalarProd(u,v)/(norm3D(u)*norm3D(v))));
}

void closest_point(GLfloat u[], GLfloat p[], GLfloat v[], GLfloat result[]){
    GLfloat t=(u[0]+u[1]+u[2]-p[0]-p[1]-p[2])/(v[0]+v[1]+v[2]);

    for(int i=0; i<3; i++){
        result[i]=p[i]+t*v[i];
    }
}

void drawCube(){
    glBegin(GL_POLYGON);
        glNormal3f(0.0f, 1.0f, 0.0f);	// top face
        glVertex3f(1.0f, 1.0f, 1.0f);	
        glVertex3f(1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0.0f, 0.0f, 1.0f);	// front face
        glVertex3f(1.0f, 1.0f, 1.0f);	
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(1.0f, 0.0f, 0.0f);	// right face
        glVertex3f(1.0f, 1.0f, 1.0f);	
        glVertex3f(1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(-1.0f, 0.0f, 0.0f);	// left face
        glVertex3f(-1.0f, 1.0f, 1.0f);	
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0.0f, -1.0f, 0.0f);	// bottom face
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);	
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0.0f, 0.0f, -1.0f);	// back face
        glVertex3f(1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);	
    glEnd();

}

void drawCircle(GLfloat r, int num_segments, GLfloat* centre)  //vertical
{
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * PI * float(ii) / float(num_segments);//get the current angle

        float x = r * cos(theta);//calculate the x component
        float z = r * sin(theta);//calculate the y component

        glVertex3f(x+centre[0],centre[1],z+centre[2]);//output vertex

    }
    glEnd();
}

void drawDisk(GLfloat r, GLfloat *centre, int num_triangles) //horizontal
{
    GLfloat x, z, angle;

    glBegin(GL_TRIANGLE_FAN);
        glVertex3fv(centre);

        for (int i=0; i<num_triangles; i++)
        {
            angle=float(i)*2*PI/num_triangles;
            x = centre[0]+cos(angle)*r;
            z = centre[2]+sin(angle)*r;
            glVertex3f(x,0,z);
        }

        glVertex3f(centre[0]+r,0,centre[2]);

    glEnd();
}

void drawTorus(double r1, double r2, int num_circles, int num_segments){
    
    for(int i=0; i<num_circles; i++){
        float theta = 2.0f * PI * float(i) / float(num_circles);//get the current angle

        glBegin(GL_LINE_LOOP);
        for(int j=0; j<num_segments; j++){
            float fi=2.0f * PI * float(j) / float(num_segments);//get the current angle

            float x = (r1+r2*cos(fi))*cos(theta);
            float y = r2*sin(fi);
            float z = (r1+r2*cos(fi))*sin(theta);

            glVertex3f(x,y,z);
        }
        glEnd();
    }
}

void drawCylinder(GLfloat base_center[], GLfloat top_center[], float radius, int num_sides){

    GLfloat w[3], uu[3], vv[3];
    for(int i=0; i<3; i++){
        w[i]=top_center[i]-base_center[i];
    }

    if(w[0]!=0 || w[1]!=0 || w[2]!=0){
        if(w[1]!=0 || w[2]!=0){
            GLfloat aux[3]={1.0f, 0.0f, 0.0f};
            crossProduct(w,aux,uu);
        }else{
            GLfloat aux[3]={0.0f, 1.0f, 0.0f};
            crossProduct(w,aux,uu);
        }

        GLfloat n=norm3D(uu);
        GLfloat u[3]={uu[0]/n, uu[1]/n, uu[2]/n};
        crossProduct(w,u,vv);
        n=norm3D(vv);
        GLfloat v[3]={vv[0]/n, vv[1]/n, vv[2]/n};

        //Calcular vectores ortonormales al eje
        glPushMatrix();

        glBegin(GL_QUAD_STRIP);

            GLfloat x;
            GLfloat y;
            GLfloat z;

            for(int j=0; j<num_sides; j++){
                float theta=2*PI*float(j)/num_sides;
                x=base_center[0]+radius*cos(theta)*u[0]+radius*sin(theta)*v[0];
                y=base_center[1]+radius*cos(theta)*u[1]+radius*sin(theta)*v[1];
                z=base_center[2]+radius*cos(theta)*u[2]+radius*sin(theta)*v[2];

                glVertex3f(x,y,z);
                
                glVertex3f(x+w[0],y+w[1],z+w[2]);
            }

            x=base_center[0]+radius*u[0];
            y=base_center[1]+radius*u[1];
            z=base_center[2]+radius*u[2];
            glVertex3f(x,y,z);
            glVertex3f(x+w[0],y+w[1],z+w[2]);

        glEnd();
        
        glPopMatrix();

        GLfloat origin[]={0.0f, 0.0f, 0.0f};
        glPushMatrix();
        glTranslatef(base_center[0], base_center[1], base_center[2]);
        //drawDisk(radius, origin, num_sides);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(top_center[0], top_center[1], top_center[2]);
        //drawDisk(radius, origin, num_sides);
        glPopMatrix();
    }
}

void drawFramedCube(GLfloat *color, GLfloat *frameColor, GLfloat frameRadius){
    GLfloat v[8][3]={{1, 1, 1}, {1, 1, -1}, {-1, 1, -1}, {-1, 1, 1}, {-1, -1, 1}, {-1, -1, -1}, {1, -1, -1}, {1, -1, 1}};

    glColor3fv(color);
    drawCube();    

    glColor3fv(frameColor);
    drawCylinder(v[1], v[2], frameRadius, ACCURACY);
    drawCylinder(v[0], v[1], frameRadius, ACCURACY);
    drawCylinder(v[2], v[3], frameRadius, ACCURACY);
    drawCylinder(v[3], v[0], frameRadius, ACCURACY);

    drawCylinder(v[0], v[3], frameRadius, ACCURACY);
    drawCylinder(v[3], v[4], frameRadius, ACCURACY);
    drawCylinder(v[4], v[7], frameRadius, ACCURACY);
    drawCylinder(v[7], v[0], frameRadius, ACCURACY);

    drawCylinder(v[0], v[7], frameRadius, ACCURACY);
    drawCylinder(v[7], v[6], frameRadius, ACCURACY);
    drawCylinder(v[6], v[1], frameRadius, ACCURACY);
    drawCylinder(v[1], v[0], frameRadius, ACCURACY);

    drawCylinder(v[3], v[2], frameRadius, ACCURACY);
    drawCylinder(v[2], v[5], frameRadius, ACCURACY);
    drawCylinder(v[5], v[4], frameRadius, ACCURACY);
    drawCylinder(v[4], v[3], frameRadius, ACCURACY);

    drawCylinder(v[4], v[5], frameRadius, ACCURACY);
    drawCylinder(v[5], v[6], frameRadius, ACCURACY);
    drawCylinder(v[6], v[7], frameRadius, ACCURACY);
    drawCylinder(v[7], v[4], frameRadius, ACCURACY);

    drawCylinder(v[6], v[5], frameRadius, ACCURACY);
    drawCylinder(v[5], v[2], frameRadius, ACCURACY);
    drawCylinder(v[2], v[1], frameRadius, ACCURACY);
    drawCylinder(v[1], v[6], frameRadius, ACCURACY);

}

void drawSphere(GLfloat radius){
    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        gluSphere(gluNewQuadric(),radius, 100, 100);
    glPopMatrix();
}