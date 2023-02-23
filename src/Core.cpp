#if defined(__linux__)
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <random>
#include <lib/nlohmann/json.hpp>

#include "model/SocialForce.h"
#include "constant/Constant.h"
#include "renderer/Renderer.h"

using namespace std;
using namespace Constant;
using namespace Renderer;
using json = nlohmann::json;

// Global Variables
GLsizei winWidth = 1080; // Window width (16:10 ratio)
GLsizei winHeight = 660; // Window height (16:10 ratio)
SocialForce *socialForce;
float fps = 0; // Frames per second
int currTime = 0;
int startTime = 0;
bool animate = false; // Animate scene flag
float speedConsiderAsStop = 0.2;

json inputData;
std::map<std::string, std::vector<float>> mapData;
std::vector<json> juncDataList;
std::vector<float> juncData;
std::string juncName;
int juncIndex = 0;
float walkwayWidth;

int classificationType = 0;

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

void reshape(int width, int height);

void normalKey(unsigned char key, int xMousePos, int yMousePos);

void update();

int main(int argc, char **argv)
{
    inputData = Utility::readInputData("data/input.json");
    mapData = Utility::readMapData("data/map.txt");
    std::string input1;

    if ((int)inputData["runMode"]["value"] == 0)
    {
        do
        {
            cout << "Select the type of traffic you want to simulate" << endl;
            cout << "1. Hallway" << endl;
            cout << "2. Junction" << endl;
            cout << "Your choice: ";
            getline(cin, input1);
            if (input1 == "1")
            {
                walkwayWidth = (float)inputData["hallwayWidth"]["value"];
                float length1Side = ((float)inputData["hallwayLength"]["value"]) / 2;
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
    }
    else
    {
        juncDataList = Utility::convertMapData(mapData);
        float hallwayLength = juncDataList[juncIndex].items().begin().value();

        walkwayWidth = (float)inputData["hallwayWidth"]["value"];
        float length1Side = (hallwayLength) / 2;
        juncData = {length1Side, length1Side};
    }

    float deviationParam = randomFloat(1 - (float)inputData["experimentalDeviation"]["value"] / 100, 1 + (float)inputData["experimentalDeviation"]["value"] / 100);
    // Threshold people stopping at the corridor
    threshold = int(inputData["numOfAgents"]["value"]) * deviationParam * (float)(inputData["stopAtHallway"]["value"]) / 100;

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
    if ((int)inputData["graphicsMode"]["value"] == 0)
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

void setAgentsFlow(Agent *agent, float desiredSpeed, float maxSpeed, float minSpeed, int caseJump)
{
    // if (socialForce->getCrowdSize() < threshold)
    // {
    //     agent->setStopAtCorridor(true);
    // }

    int codeSrc = 0;
    int codeDes = 0;

    int juncType = juncData.size();

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

    vector<float> position = Utility::getPedesSource(
        codeSrc,
        (float)inputData["totalCrowdLength"]["value"],
        (float)inputData["headCrowdLength"]["value"],
        (float)inputData["crowdWidth"]["value"],
        walkwayWidth,
        juncData);
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
    std::vector<float> color = getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed(), classificationType);
    agent->setColor(color[0], color[1], color[2]);
    socialForce->addAgent(agent);
}

void createAgents()
{
    Agent *agent;

    float deviationParam = randomFloat(1 - (float)inputData["experimentalDeviation"]["value"] / 100, 1 + (float)inputData["experimentalDeviation"]["value"] / 100);
    // cout << "Deviation: "<< deviationParam <<" - Num agents: "<< int(int(inputData["numOfAgents"]["value"]) * deviationParam) << endl;
    numOfPeople = Utility::getNumPedesInFlow(juncData.size(), int(int(inputData["numOfAgents"]["value"]) * deviationParam));
    vector<double> velocityList = Utility::getPedesVelocity(classificationType, inputData, deviationParam);
    if (classificationType == 0)
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
    //     std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed(), classificationType);
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
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx);
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
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx);
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
                setAgentsFlow(agent, velocityList[pedesCount], maxSpeed, minSpeed, idx);
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
    if ((int)inputData["runMode"]["value"] == 0)
    {
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
                vector<Point3f> route = Utility::getRouteAGV(i, j, walkwayWidth, juncData);
                agv->setDirection(i, j);
                agv->setPosition(route[0].x, route[0].y);

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

                agv->setDestination(route[route.size() - 1].x, route[route.size() - 1].y);
                agv->setDesiredSpeed(0.6F);
                agv->setAcceleration(inputData["acceleration"]["value"]);
                agv->setThresholdDisToPedes((float)inputData["thresDistance"]["value"]);
                for (int i = 1; i < route.size(); i++)
                {
                    agv->setPath(route[i].x, route[i].y, 1.0);
                }
                socialForce->addAGV(agv);
            }
        }
    }
    else
    {
        int numOfHallway = juncDataList.size();
        int numRunPerHallway = (int)inputData["noRunPerHallway"]["value"];
        int juncIndexTemp = 0;
        float hallwayLength = juncDataList[juncIndexTemp].items().begin().value();
        cout << "*****=> " << juncDataList[juncIndex].items().begin().key() << ": " << hallwayLength << endl;
        float length1Side = (hallwayLength) / 2;
        vector<float> juncDataTemp = {length1Side, length1Side};
        int numAGVPerRun = 1;
        if ((int)inputData["runConcurrently"]["value"] == 1)
        {
            numAGVPerRun = 2;
        }
        for (int i = 0; i < numOfHallway * numRunPerHallway; i++)
        {
            for (int j = 0; j < numAGVPerRun; j++)
            {
                agv = new AGV();
                vector<Point3f> route = Utility::getRouteAGV(j, 1, walkwayWidth, juncDataTemp); // Just need the source
                agv->setDirection(j, 1);
                agv->setPosition(route[0].x, route[0].y);

                agv->setDestination(route[route.size() - 1].x, route[route.size() - 1].y);
                agv->setDesiredSpeed((float)inputData["agvDesiredSpeed"]["value"]);
                agv->setAcceleration(inputData["acceleration"]["value"]);
                agv->setThresholdDisToPedes((float)inputData["thresDistance"]["value"]);
                for (int i = 1; i < route.size(); i++)
                {
                    agv->setPath(route[i].x, route[i].y, 1.0);
                }
                socialForce->addAGV(agv);

                int marker = numRunPerHallway * (juncIndexTemp + 1) - 1;
                if ((int)inputData["runConcurrently"]["value"] == 1)
                {
                    marker = numRunPerHallway * 2 * (juncIndexTemp + 1) - 1;
                }
                if (agv->getId() == marker)
                {
                    juncIndexTemp = juncIndexTemp + 1;
                    if (juncIndexTemp == juncDataList.size())
                    {
                        juncIndexTemp = 0;
                    }
                    hallwayLength = juncDataList[juncIndexTemp].items().begin().value();
                    length1Side = (hallwayLength) / 2;
                    juncDataTemp = {length1Side, length1Side};
                }
            }
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

    drawAgents(socialForce);
    drawAGVs(socialForce, juncData, (int)inputData["runConcurrently"]["value"], (int)inputData["runMode"]["value"]);
    drawWalls(socialForce);
    glPopMatrix();

    showInformation(socialForce, fps, animate, currTime, startTime, classificationType, winWidth, winHeight);

    glutSwapBuffers();
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

        if (Utility::isPositionErr(src, walkwayWidth, juncData.size(), socialForce->getAGVs()))
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
        if (agv->getCollisionStartTime() == 0 && agv->getVelocity().length() < speedConsiderAsStop && agv->getIsMoving())
        {
            agv->setCollisionStartTime(glutGet(GLUT_ELAPSED_TIME));
            // cout << "- Start collision: " << convertTime(agv->getCollisionStartTime()) << endl;
        }

        if (agv->getCollisionStartTime() != 0 && agv->getVelocity().length() > speedConsiderAsStop && agv->getIsMoving())
        {
            agv->setTotalStopTime(agv->getTotalStopTime() + glutGet(GLUT_ELAPSED_TIME) - agv->getCollisionStartTime());
            // cout << "- Stop collision: " << convertTime(glutGet(GLUT_ELAPSED_TIME)) << endl;
            // cout << "=> Total collision: " << convertTime(agv->getTotalStopTime()) << endl;
            agv->setCollisionStartTime(0);
        }

        Point3f src = agv->getPosition();
        Point3f des = agv->getDestination();

        float distance = src.distance(des);
        if (distance <= 1 || isnan(distance))
        {
            if (agv->getIsMoving())
            {
                agv->setTravelingTime(glutGet(GLUT_ELAPSED_TIME) - agv->getTravelingTime());
                agv->setIsMoving(false);

                int numAGVCompleted = getNumAGVCompleted(socialForce->getAGVs());

                int marker = (int)inputData["noRunPerHallway"]["value"];
                if ((int)inputData["runConcurrently"]["value"] == 1)
                {
                    marker = (int)inputData["noRunPerHallway"]["value"] * 2;
                    if (numAGVCompleted % 2 == 0)
                    {
                        socialForce->removeCrowd();
                        createAgents();
                    }
                }
                else
                {
                    socialForce->removeCrowd();
                    createAgents();
                }

                if (numAGVCompleted > 0 && numAGVCompleted % marker == 0)
                {
                    juncIndex = juncIndex + 1;
                    if (juncIndex == juncDataList.size())
                    {
                        juncIndex = 0;
                    }
                    socialForce->removeWalls();
                    float hallwayLength = juncDataList[juncIndex].items().begin().value();
                    if (numAGVCompleted + 1 <= agvs.size())
                    {
                        cout << "*****=> " << juncDataList[juncIndex].items().begin().key() << ": " << hallwayLength << endl;
                    }
                    float length1Side = (hallwayLength) / 2;
                    juncData = {length1Side, length1Side};
                    createWalls();
                    // cout << agv->getId() << " - Remove and re-create agent" << endl;
                }
            }
            count_agvs = count_agvs + 1;
        }
    }
    if (count_agvs == agvs.size())
    {
        int totalRunningTime = currTime - startTime;
        Utility::writeResult(
            "data/end.txt", juncName, inputData["graphicsMode"]["value"], agvs,
            juncDataList,
            (int)inputData["runConcurrently"]["value"],
            (int)inputData["runMode"]["value"],
            (int)inputData["noRunPerHallway"]["value"],
            totalRunningTime);

        std::cout << "Maximum speed: " << maxSpeed << " - Minimum speed: " << minSpeed << endl;
        std::cout << "Finish in: " << Utility::convertTime(totalRunningTime) << endl;
        delete socialForce;
        socialForce = 0;

        exit(0); // Terminate program
    }

    if (animate)
    {
        socialForce->moveCrowd(static_cast<float>(frameTime) / 1000); // Perform calculations and move agents
        socialForce->moveAGVs(static_cast<float>(frameTime) / 1000);
    }
    computeFPS(&fps);
    glutPostRedisplay();
    glutIdleFunc(update); // Continuously execute 'update()'
}
