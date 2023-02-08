#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#if defined(__linux__)
#include <GL/glut.h>
#else

#include <GLUT/glut.h>

#endif

#include "model/SocialForce.h"
#include "utility/Utility.h"
#include "constant/Constant.h"

using namespace std;
using namespace Constant;
using namespace Utility;

// Global Variables
GLsizei winWidth = 1080; // Window width (16:10 ratio)
GLsizei winHeight = 660; // Window height (16:10 ratio)
SocialForce *socialForce;
float fps = 0; // Frames per second
int currTime = 0;
int startTime = 0;
bool animate = false; // Animate scene flag

std::vector<double> inputData;
std::map<std::string, std::vector<float>> mapData;
std::vector<float> juncData;
std::string juncName;
float walkwayWidth;

int typeGetVelocity = 0;

// int numOfPeople[] = {3, 5, 7, 4, 5, 9, 2, 4, 5, 3, 6, 4};
std::vector<int> numOfPeople;
float minSpeed = -1;
float maxSpeed = -1;
int threshold = 0;

// Function Prototypes
void init();

void createWalls();

void createAgents();

void createAGVs();

void display();

void drawAgents();

void drawAGVs();

void drawCylinder(float x, float y, float radius = 0.2, int slices = 15,
                  float height = 0.5);

void drawWalls();

void showInformation();

void drawText(float x, float y, const char text[]);

void reshape(int width, int height);

void normalKey(unsigned char key, int xMousePos, int yMousePos);

void update();

void computeFPS();

int main(int argc, char **argv)
{
    inputData = Utility::readInput("data/input.txt");
    mapData = Utility::readMapData("data/map.txt");
    std::string input1;

    do
    {
        cout << "Select the type of traffic you want to simulate" << endl;
        cout << "1. Hallway" << endl;
        cout << "2. Junction" << endl;
        cout << "Your choice: ";
        getline(cin, input1);
        if (input1 == "1")
        {
            walkwayWidth = inputData[2];
            float length1Side = (inputData[18] - walkwayWidth) / 2;
            juncData = {length1Side, length1Side};
        }
        else if (input1 == "2")
        {
            do
            {
                cout << "Please enter the junction you want to emulate" << endl;
                cout << "(Press enter to randomly select a junction in the map)" << endl;
                cout << "Your choice: ";
                getline(cin, juncName);
                if (juncName == "")
                {
                    auto it = mapData.begin();
                    std::advance(it, Utility::randomInt(1, mapData.size() - 3));
                    std::string random_key = it->first;
                    juncName.assign(random_key);
                }

            } while (mapData[juncName].size() < 3);
            juncData = mapData[juncName];
            walkwayWidth = mapData["walkwayWidth"][0];
        }
    } while (input1 != "1" && input1 != "2");

    // Threshold people stopping at the corridor
    threshold = int(inputData[0]) * (float)(inputData[11]) / 100;

    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |
                        GLUT_DEPTH);         // Set display mode  Default mode used
    glutInitWindowSize(winWidth, winHeight); // Set window width and height
    glutInitWindowPosition(90, 90);          // Set window position
    glutCreateWindow(
        "Crowd Simulation using Social Force"); // Set window title and create
    // display window

    animate = true;
    startTime = currTime;
    if ((int)inputData[6] == 0)
    {
        glutHideWindow();
    }

    init();                   // Initialization
    glutDisplayFunc(display); // Send graphics to display window
    glutReshapeFunc(reshape); // Maintain aspect ratio when window first created,
    // resized and moved

    glutKeyboardFunc(normalKey);
    glutIdleFunc(update); // Continuously execute 'update()'
    glutMainLoop();       // Enter GLUT's main loop

    return 0;
}

void init()
{
    // General Light Intensity
    GLfloat gnrlAmbient[] = {
        0.8F, 0.8F, 0.8F,
        1.0}; // Ambient (R, G, B, A) light intensity of entire scene

    // Object Light Intensity
    GLfloat lghtDiffuse[] = {0.7F, 0.7F, 0.7F,
                             1.0}; // Diffuse (R, G, B, A) light intensity

    // Light Position
    GLfloat lghtPosition[] = {4.0, -4.0, 4.0, 0.0};

    glClearColor(1.0, 1.0, 1.0,
                 0.0);       // Set color used when color buffer cleared
    glShadeModel(GL_SMOOTH); // Set shading option

    // General Lighting
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gnrlAmbient);

    // Object Lighting
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lghtDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lghtPosition);

    glEnable(GL_DEPTH_TEST); // Enable hidden surface removal
    glEnable(GL_NORMALIZE);  // Normalize normal vector
    glEnable(GL_LIGHTING);   // Prepare OpenGL to perform lighting calculations
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHT0);

    glCullFace(GL_BACK);    // Specify face to be culled
    glEnable(GL_CULL_FACE); // Enable culling of specified face

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);

    srand(1604010629); // Seed to generate random numbers

    socialForce = new SocialForce;
    createWalls();
    createAgents();
    createAGVs();
}

void createWalls()
{
    Wall *wall;

    vector<float> coors = Utility::getWallCoordinates(walkwayWidth, juncData);

    if (juncData.size() == 2)
    {
        // Upper Wall
        wall = new Wall(coors[0], coors[1], coors[2], coors[3]);
        socialForce->addWall(wall);
        // Lower Wall
        wall = new Wall(coors[4], coors[5], coors[6], coors[7]);
        socialForce->addWall(wall);
    }
    else
    {
        // Upper Wall
        if (juncData.size() == 4)
        {
            wall = new Wall(coors[0], coors[1], coors[2], coors[3]);
            socialForce->addWall(wall);

            wall = new Wall(coors[4], coors[5], coors[6], coors[7]);
            socialForce->addWall(wall);
        }
        else if (juncData.size() == 3)
        {
            wall = new Wall(coors[0], coors[1], coors[6], coors[7]);
            socialForce->addWall(wall);
        }

        // Lower Wall
        wall = new Wall(coors[8], coors[9], coors[10], coors[11]);
        socialForce->addWall(wall);

        wall = new Wall(coors[12], coors[13], coors[14], coors[15]);
        socialForce->addWall(wall);

        // Left Wall
        if (juncData.size() == 4)
        {
            wall = new Wall(coors[16], coors[17], coors[18], coors[19]);
            socialForce->addWall(wall);
        }

        wall = new Wall(coors[20], coors[21], coors[22], coors[23]);
        socialForce->addWall(wall);

        // Right Wall
        if (juncData.size() == 4)
        {
            wall = new Wall(coors[24], coors[25], coors[26], coors[27]);
            socialForce->addWall(wall);
        }

        wall = new Wall(coors[28], coors[29], coors[30], coors[31]);
        socialForce->addWall(wall);
    }
}

void setAgentsFlow(Agent *agent, float desiredSpeed, float maxSpeed, float minSpeed, int caseJump, int juncType)
{
    if (socialForce->getCrowdSize() < threshold)
    {
        agent->setStopAtCorridor(true);
    }

    int codeSrc = 0;
    int codeDes = 0;

    if (juncType == 4)
    {
        if (caseJump < 3)
        {
            codeSrc = 0; // Go from Left to Right
        }
        else if (caseJump < 6)
        {
            codeSrc = 1; // Go from Right to Left
        }
        else if (caseJump < 9)
        {
            codeSrc = 2; // Go from Top to Bottom
        }
        else
        {
            codeSrc = 3; // Go from Bottom to Top
        }
    }
    else if (juncType == 3)
    {
        if (caseJump < 6)
        {
            codeSrc = 0;
            if (caseJump % 2 == 0)
            {
                codeDes = 0;
            }
            else
            {
                codeDes = 2;
            }
        }
        else if (caseJump < 12)
        {
            codeSrc = 1;
            if (caseJump % 2 == 0)
            {
                codeDes = 1;
            }
            else
            {
                codeDes = 2;
            }
        }
        else if (caseJump < 18)
        {
            codeSrc = 3;
            if (caseJump % 2 == 0)
            {
                codeDes = 0;
            }
            else
            {
                codeDes = 1;
            }
        }
    }
    else if (juncType == 2)
    {
        if (caseJump < 3)
        {
            codeSrc = 0;
        }
        else
        {
            codeSrc = 1;
        }
    }

    vector<float> position = Utility::getPedesSource(codeSrc, (float)inputData[3], (float)inputData[4],
                                                     (float)inputData[5], walkwayWidth, juncData);
    vector<float> desList;
    if (juncType == 4 || juncType == 2)
    {
        desList = Utility::getPedesDestination(codeSrc, caseJump % 3, walkwayWidth, juncData, agent->getStopAtCorridor());
    }
    else if (juncType == 3)
    {
        desList = Utility::getPedesDestination(codeDes, caseJump % 3, walkwayWidth, juncData, agent->getStopAtCorridor());
    }

    agent->setPosition(position[0], position[1]);
    if (juncType == 3 && codeSrc != codeDes)
    {
        agent->setPath(randomFloat(-walkwayWidth / 2, walkwayWidth / 2), randomFloat(-walkwayWidth / 2, walkwayWidth / 2), 2.0);
    }
    agent->setPath(desList[0], desList[1], desList[2]);
    agent->setDestination(desList[0], desList[1]);
    agent->setDesiredSpeed(desiredSpeed);
    std::vector<float> color = getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed(), typeGetVelocity);
    agent->setColor(color[0], color[1], color[2]);
    socialForce->addAgent(agent);
}

void createAgents()
{
    Agent *agent;

    numOfPeople = Utility::getNumPedesInFlow(juncData.size(), int(inputData[0]));
    vector<double> velocityList = Utility::getPedesVelocity(typeGetVelocity, inputData);
    if (typeGetVelocity == 0)
    {
        minSpeed = 0.52;
        maxSpeed = 2.28;
    }
    else
    {
        minSpeed = velocityList[0];
        maxSpeed = velocityList[velocityList.size() - 1];
    }

    auto rng = std::default_random_engine{};
    std::shuffle(velocityList.begin(), velocityList.end(), rng);

    int pedesCount = 0;

    // test

    // for (int temp = 0; temp < 3; temp++)
    // {
    //     agent = new Agent;
    //     // setAgentsFlow(agent, 1, maxSpeed, minSpeed, Point3f(randomFloat(-3.0, -2.0), randomFloat(9.0, 10.0), 0.0), Point3f(randomFloat(-3.2, -2.8), randomFloat(-2.2, -1.8), 0.0));
    //     agent->setPosition(randomFloat(-3.0, -2.0), randomFloat(9.0, 10.0));
    //     // // float x = randomFloat(-13.3F, -6.0);
    //     // // float y = randomFloat(-2.0, 2.0);
    //     float x = randomFloat(-3.2, -2.8);
    //     float y = randomFloat(-2.2, -1.8);
    //     agent->setPath(x, y, 2);
    //     agent->setDestination(x, y);
    //     // // agent->setPath(randomFloat(22.0, 25.0), randomFloat(-3.0, -2.0), 1.0);
    //     agent->setDesiredSpeed(1);
    //     agent->setStopAtCorridor(true);
    //     std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed(), typeGetVelocity);
    //     agent->setColor(color[0], color[1], color[2]);
    //     socialForce->addAgent(agent);
    // }

    // test

    if (juncData.size() == 2)
    {
        for (int idx = 0; idx < 6; idx++)
        {
            for (int temp = 0; temp < numOfPeople[idx]; temp++)
            {
                agent = new Agent;
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx, juncData.size());
                pedesCount = pedesCount + 1;
            }
        }
    }
    else if (juncData.size() == 3)
    {
        for (int idx = 0; idx < 18; idx++)
        {
            for (int temp = 0; temp < numOfPeople[idx]; temp++)
            {
                agent = new Agent;
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx, juncData.size());
                pedesCount = pedesCount + 1;
            }
        }
    }
    else if (juncData.size() == 4)
    {
        for (int idx = 0; idx < 12; idx++)
        {
            for (int temp = 0; temp < numOfPeople[idx]; temp++)
            {
                agent = new Agent;
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx, juncData.size());
                pedesCount = pedesCount + 1;
            }
        }
    }
}

void createAGVs()
{
    AGV *agv = NULL;
    vector<int> array;

    // test
    // agv = new AGV();
    // vector<Point3f> route = Utility::getRouteAGV(juncData.size(), 0, 2, walkwayWidth, juncData);
    // agv->setDirection(0, 2);
    // agv->setPosition(route[0].x, route[0].y);

    // for (Agent *agent : socialForce->getCrowd())
    // {
    //     if (agent->getPosition().distance(agv->getPosition()) < 0.5F)
    //     {
    //         do
    //         {
    //             agent->setPosition(agent->getPosition().x - 0.1F, agent->getPosition().y - 0.1F);
    //         } while (agent->getPosition().distance(agv->getPosition()) < 0.5F);
    //     }
    // }

    // agv->setDestination(route[route.size() - 1].x, route[route.size() - 1].y);
    // agv->setDesiredSpeed(0.6F);
    // agv->setAcceleration(inputData[9]);
    // agv->setDistance((float)inputData[10]);
    // for (int i = 1; i < route.size(); i++)
    // {
    //     agv->setPath(route[i].x, route[i].y, 1.0);
    //     std::cout << route[i] << endl;
    // }
    // socialForce->addAGV(agv);

    // test

    for (int i = 0; i < juncData.size(); i++)
    {
        if (juncData.size() == 4)
        {
            array = {0, 1, 2};
        }
        else if (juncData.size() == 3)
        {
            if (i == 0)
            {
                array = {1, 2};
            }
            else if (i == 1)
            {
                array = {0, 2};
            }
            else
            {
                array = {0, 1};
            }
        }
        else if (juncData.size() == 2)
        {
            array = {1};
        }

        for (int j : array)
        {
            agv = new AGV();
            vector<Point3f> route = Utility::getRouteAGV(juncData.size(), i, j, walkwayWidth, juncData);
            agv->setDirection(i, j);
            agv->setPosition(route[0].x, route[0].y);

            for (Agent *agent : socialForce->getCrowd())
            {
                if (agent->getPosition().distance(agv->getPosition()) < 0.5F)
                {
                    do
                    {
                        agent->setPosition(agent->getPosition().x - 0.1F, agent->getPosition().y - 0.1F);
                    } while (agent->getPosition().distance(agv->getPosition()) < 0.5F);
                }
            }

            agv->setDestination(route[route.size() - 1].x, route[route.size() - 1].y);
            agv->setDesiredSpeed(0.6F);
            agv->setAcceleration(inputData[9]);
            agv->setThresholdDisToPedes((float)inputData[10]);
            for (int i = 1; i < route.size(); i++)
            {
                agv->setPath(route[i].x, route[i].y, 1.0);
            }
            socialForce->addAGV(agv);
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffer
    glLoadIdentity();             // Initialize modelview matrix to identity matrix

    // Camera
    gluLookAt(0.0, 0.0, 18.0, // Position
              0.0, 0.0, 0.0,  // Look-at point
              0.0, 1.0, 0.0); // Up-vector

    glPushMatrix();
    glScalef(1.0, 1.0, 1.0);

    drawAgents();
    drawAGVs();
    drawWalls();
    glPopMatrix();

    showInformation();

    glutSwapBuffers();
}

void drawAgents()
{
    vector<Agent *> agents = socialForce->getCrowd();

    for (Agent *agent : agents)
    {
        // Draw Agents
        glColor3f(agent->getColor().x, agent->getColor().y, agent->getColor().z);
        drawCylinder(agent->getPosition().x, agent->getPosition().y,
                     agent->getRadius(), 15, 0.0);
    }
}

void drawAGVs()
{
    vector<AGV *> agvs = socialForce->getAGVs();
    Vector3f e_ij;
    if (juncData.size() == 2 && (int)inputData[19] == 1)
    {
        for (AGV *agv : agvs)
        {
            agv->setIsMoving(true);
            if (agv->getTravelingTime() == 0)
            {
                agv->setTravelingTime(glutGet(GLUT_ELAPSED_TIME));
            }
            // Draw AGVs
            glColor3f(agv->getColor().x, agv->getColor().y, agv->getColor().z);
            float w, l;
            Vector3f a, b;
            Point3f top, bottom, pointA, pointB, pointC, pointD;
            w = agv->getWidth();
            l = agv->getLength();
            e_ij = agv->getPath() - agv->getPosition();
            e_ij.normalize();
            top = agv->getPosition() + e_ij * l * 0.5F;
            bottom = agv->getPosition() - e_ij * l * 0.5F;

            a = Vector3f(e_ij.y, -e_ij.x, 0.0F);
            a.normalize();
            b = Vector3f(-e_ij.y, e_ij.x, 0.0F);
            b.normalize();

            pointA = top + a * w * 0.5F;
            pointB = top + b * w * 0.5F;
            pointC = bottom + b * w * 0.5F;
            pointD = bottom + a * w * 0.5F;

            agv->setPoints(pointA, pointB, pointC, pointD);

            glBegin(GL_QUADS);
            glVertex3f(pointA.x, pointA.y, 0);
            glVertex3f(pointB.x, pointB.y, 0);
            glVertex3f(pointC.x, pointC.y, 0);
            glVertex3f(pointD.x, pointD.y, 0);
            glEnd();
        }
    }
    else
    {
        AGV *agv = NULL;
        int i;
        vector<int> j;

        for (i = 0; i < agvs.size(); i++)
        {
            if (agvs[i]->getIsMoving() && agvs[i]->getTravelingTime() != 0)
            {
                agv = agvs[i];
                break;
            }
            else if (!agvs[i]->getIsMoving() && agvs[i]->getTravelingTime() == 0)
            {
                j.push_back(i);
            }
        }

        if (i == agvs.size())
        {
            agv = agvs[j.front()];
            float distance = (agv->getWidth() > agv->getLength()) ? agv->getWidth() : agv->getLength();
            for (Agent *agent : socialForce->getCrowd())
            {
                if (agent->getPosition().distance(agv->getPosition()) < distance / 2)
                {
                    do
                    {
                        agent->setPosition(agent->getPosition().x - 0.1F, agent->getPosition().y - 0.1F);
                    } while (agent->getPosition().distance(agv->getPosition()) < distance / 2);
                }
            }
        }

        if (agv)
        {
            agv->setIsMoving(true);
            if (agv->getTravelingTime() == 0)
            {
                agv->setTravelingTime(glutGet(GLUT_ELAPSED_TIME));
            }
            // Draw AGVs
            glColor3f(agv->getColor().x, agv->getColor().y, agv->getColor().z);
            float w, l;
            Vector3f a, b;
            Point3f top, bottom, pointA, pointB, pointC, pointD;
            w = agv->getWidth();
            l = agv->getLength();
            e_ij = agv->getPath() - agv->getPosition();
            e_ij.normalize();
            top = agv->getPosition() + e_ij * l * 0.5F;
            bottom = agv->getPosition() - e_ij * l * 0.5F;

            a = Vector3f(e_ij.y, -e_ij.x, 0.0F);
            a.normalize();
            b = Vector3f(-e_ij.y, e_ij.x, 0.0F);
            b.normalize();

            pointA = top + a * w * 0.5F;
            pointB = top + b * w * 0.5F;
            pointC = bottom + b * w * 0.5F;
            pointD = bottom + a * w * 0.5F;

            agv->setPoints(pointA, pointB, pointC, pointD);

            glBegin(GL_QUADS);
            glVertex3f(pointA.x, pointA.y, 0);
            glVertex3f(pointB.x, pointB.y, 0);
            glVertex3f(pointC.x, pointC.y, 0);
            glVertex3f(pointD.x, pointD.y, 0);
            glEnd();
        }
    }
}

void drawCylinder(float x, float y, float radius, int slices, float height)
{
    float sliceAngle;
    Point3f current, next;

    glPushMatrix();
    glTranslatef(x, y, 0.0);

    sliceAngle = static_cast<float>(360.0 / slices);
    current.x = radius; // Set initial point
    current.y = 0;      // Set initial point

    for (float angle = sliceAngle; angle <= 360; angle += sliceAngle)
    {
        next.x = radius * cos(angle * PI / 180); // Compute next point
        next.y = radius * sin(angle * PI / 180); // Compute next point

        // Top Cover
        glBegin(GL_TRIANGLES);
        glVertex3f(0.0, 0.0, height);             // Centre of triangle
        glVertex3f(current.x, current.y, height); // Left of triangle
        glVertex3f(next.x, next.y, height);       // Right of triangle
        glEnd();

        // Body
        glBegin(GL_QUADS);
        glVertex3f(current.x, current.y, height); // Top-left of quadrilateral
        glVertex3f(current.x, current.y, 0.0);    // Bottom-left of quadrilateral
        glVertex3f(next.x, next.y, 0.0);          // Bottom-right of quadrilateral
        glVertex3f(next.x, next.y, height);       // Top-right of quadrilateral
        glEnd();

        // Bottom Cover
        glBegin(GL_TRIANGLES);
        glVertex3f(0.0, 0.0, 0.0);             // Centre of triangle
        glVertex3f(current.x, current.y, 0.0); // Left of triangle
        glVertex3f(next.x, next.y, 0.0);       // Right of triangle
        glEnd();

        current = next; // New point becomes initial point
    }
    glPopMatrix();
}

void drawWalls()
{
    vector<Wall *> walls = socialForce->getWalls();

    glColor3f(0.2F, 0.2F, 0.2F);
    glPushMatrix();
    for (Wall *wall : walls)
    {
        glBegin(GL_LINES);
        glVertex2f(wall->getStartPoint().x, wall->getStartPoint().y);
        glVertex2f(wall->getEndPoint().x, wall->getEndPoint().y);
        glEnd();
    }
    glPopMatrix();
}

void drawSquare(double x1, double y1, double sidelength, Color3f color)
{
    double halfside = sidelength / 2;

    glColor3f(color.x / 255, color.y / 255, color.z / 255);
    glBegin(GL_POLYGON);

    glVertex3f(x1 - halfside, y1 - halfside, 0.0);
    glVertex3f(x1 + halfside, y1 - halfside, 0.0);
    glVertex3f(x1 + halfside, y1 + halfside, 0.0);
    glVertex3f(x1 - halfside, y1 + halfside, 0.0);

    glEnd();
}

void showInformation()
{
    Point3f margin;

    margin.x = static_cast<float>(-winWidth) / 60;
    margin.y = static_cast<float>(winHeight) / 60 - 0.75F;

    glColor3f(0.0, 0.0, 0.0);

    // Total Agents
    drawText(margin.x, margin.y - 0.9F, "Total agents:");
    std::string s = std::to_string(socialForce->getCrowdSize());
    drawText(margin.x + 4.0F, margin.y - 0.9F, // totalAgentsStr
             s.c_str());

    // FPS
    drawText(margin.x, margin.y, "FPS:");
    s = std::to_string(static_cast<int>(fps));
    drawText(margin.x + 1.7F, margin.y, s.c_str() /*fpsStr*/);

    // Simulation Time
    drawText(margin.x, margin.y - 1.8F, "Simulation time:");
    if (animate)
    {
        s = convertTime(currTime - startTime);
    }
    else
    {
        s = convertTime(0);
    }

    drawText(margin.x + 5.0F, margin.y - 1.8F, // totalAgentsStr
             s.c_str());

    // Appendix
    if (typeGetVelocity == 0)
    {
        drawSquare(margin.x, -margin.y + 5.2, 0.3, GREEN);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 5, "No disability, without overtaking behavior");

        drawSquare(margin.x, -margin.y + 4.2, 0.3, PURPLE);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 4, "No disability, with overtaking behavior");

        drawSquare(margin.x, -margin.y + 3.2, 0.3, RED);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 3, "Walking with crutches");

        drawSquare(margin.x, -margin.y + 2.2, 0.3, WOOD);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 2, "Walking with sticks");

        drawSquare(margin.x, -margin.y + 1.2, 0.3, GRAY);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 1, "Wheelchairs");

        drawSquare(margin.x, -margin.y + 0.2, 0.3, BLACK);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y, "The blind");
    }
    else
    {
        drawSquare(margin.x, -margin.y + 2.2, 0.3, GREEN);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 2, "Hospital staff");

        drawSquare(margin.x, -margin.y + 1.2, 0.3, BLACK);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y + 1, "Patients' relatives");

        drawSquare(margin.x, -margin.y + 0.2, 0.3, RED);
        glColor3f(0.0, 0.0, 0.0);
        drawText(margin.x + 0.5, -margin.y, "Patient");
    }
}

void drawText(float x, float y, const char text[])
{
    glDisable(GL_LIGHTING); // Disable lighting for proper display of 'drawText()'
    glDisable(
        GL_DEPTH_TEST); // Disable depth test for proper display of 'drawText()'

    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glScalef(0.0045F, 0.0045F, 0.0);
    glLineWidth(0.8F);

    int idx = 0;
    while (text[idx] != '\0')
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[idx++]);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST); // Enable hidden surface removal
    glEnable(GL_LIGHTING);   // Prepare OpenGL to perform lighting calculations
}

void reshape(int width, int height)
{
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Initialize projection matrix to identity matrix
    gluPerspective(65.0, static_cast<GLfloat>(width) / height, 1.0,
                   100.0); // Create matrix for symmetric perspective-view frustum

    glMatrixMode(GL_MODELVIEW);

    winWidth = width;
    winHeight = height;
}

void normalKey(unsigned char key, int xMousePos, int yMousePos)
{
    switch (key)
    {
    case 'a': // Animate or inanimate scene
        animate = (!animate) ? true : false;
        startTime = currTime;
        break;

    case 27: // ASCII character for Esc key
        delete socialForce;
        socialForce = 0;

        exit(0); // Terminate program
        break;
    }
}

void update()
{
    int frameTime;       // Store time in milliseconds
    static int prevTime; // Stores time in milliseconds

    currTime = glutGet(GLUT_ELAPSED_TIME); // Get time in milliseconds since 'glutInit()' called
    frameTime = currTime - prevTime;
    prevTime = currTime;

    int count_agents = 0, count_agvs = 0;

    std::vector<Agent *> agents = socialForce->getCrowd();
    for (Agent *agent : agents)
    {
        Point3f src = agent->getPosition();
        Point3f des = agent->getDestination();

        if (Utility::isPositionErr(src, walkwayWidth, juncData.size()))
        {
            socialForce->removeAgent(agent->getId());
            continue;
        }

        if (agent->getVelocity().length() < LOWER_SPEED_LIMIT + 0.2 &&
            agent->getMinDistanceToWalls(socialForce->getWalls(), src, agent->getRadius()) < 0.2 &&
            (agent->interDes).size() == 0)
        {
            Point3f intermediateDes = Utility::getIntermediateDes(src, walkwayWidth, walkwayWidth);

            (agent->interDes).push_back(intermediateDes);
            agent->setPath(intermediateDes.x, intermediateDes.y, 1.0);
            agent->setPath(des.x, des.y, 1.0);
        }

        if ((agent->interDes).size() > 0)
        {
            float distanceToInterDes = src.distance((agent->interDes).front());
            if (distanceToInterDes <= 1)
            {
                (agent->interDes).clear();
            }
        }

        float distanceToTarget = src.distance(des);
        if (distanceToTarget <= 1 || isnan(distanceToTarget))
        {
            agent->setIsMoving(false);
            if (!agent->getStopAtCorridor())
            {
                socialForce->removeAgent(agent->getId());
            }
            count_agents = count_agents + 1;
        }
    }

    std::vector<AGV *> agvs = socialForce->getAGVs();
    for (AGV *agv : agvs)
    {
        Point3f src = agv->getPosition();
        Point3f des = agv->getDestination();

        float distance = src.distance(des);
        if (distance <= 1 || isnan(distance))
        {
            if (agv->getIsMoving())
            {
                agv->setTravelingTime(glutGet(GLUT_ELAPSED_TIME) - agv->getTravelingTime());
                agv->setIsMoving(false);
                socialForce->removeCrowd();
                createAgents();
            }
            count_agvs = count_agvs + 1;
        }
    }
    if (count_agvs == agvs.size())
    {
        Utility::writeEnd("data/end.txt", juncName, inputData[6], agvs);
        std::cout << "Maximum speed: " << maxSpeed << " - Minimum speed: " << minSpeed << endl;
        std::cout << "Finish in: " << Utility::convertTime(currTime - startTime) << endl;
        delete socialForce;
        socialForce = 0;

        exit(0); // Terminate program
    }

    if (animate)
    {
        socialForce->moveCrowd(static_cast<float>(frameTime) / 1000); // Perform calculations and move agents
        socialForce->moveAGVs(static_cast<float>(frameTime) / 1000);
    }
    computeFPS();
    glutPostRedisplay();
    glutIdleFunc(update); // Continuously execute 'update()'
}

void computeFPS()
{
    static int frameCount = 0; // Stores number of frames
    int currTime, frameTime;   // Store time in milliseconds
    static int prevTime;       // Stores time in milliseconds

    frameCount++;

    currTime = glutGet(
        GLUT_ELAPSED_TIME); // Get time in milliseconds since 'glutInit()' called
    frameTime = currTime - prevTime;

    if (frameTime > 1000)
    {
        fps = frameCount /
              (static_cast<float>(frameTime) / 1000); // Compute the number of FPS
        prevTime = currTime;
        frameCount = 0; // Reset number of frames
    }
}
