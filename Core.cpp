#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <cstring>
#include <random>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <array>
#if defined(__linux__)
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#include "SocialForce.h"
#include "Utility.h"

using namespace std;

// Global Constant Variables
const float PI = 3.14159265359F;

// Global Variables
GLsizei winWidth = 992;	 // Window width (16:9 ratio)
GLsizei winHeight = 620; // Window height (16:9 ratio)
SocialForce *socialForce;
float fps = 0;		  // Frames per second
bool animate = false; // Animate scene flag

std::vector<double> inputData;

// int numOfPeople[] = {3, 5, 7, 4, 5, 9, 2, 4, 5, 3, 6, 4};
std::array<int, 12> numOfPeople;
float minSpeed = -1;
float maxSpeed = -1;

// Function Prototypes
void init();
void createWalls();
void createAgents();
void display();
void drawAgents();
void drawCylinder(float x, float y, float radius = 0.2, int slices = 15, float height = 0.5);
void drawWalls();
void showInformation();
void drawText(float x, float y, const char text[]);
void reshape(int width, int height);
void normalKey(unsigned char key, int xMousePos, int yMousePos);
float randomFloat(float lowerBound, float upperBound);
void update();
void computeFPS();

int main(int argc, char **argv)
{
	inputData = Utility::readInput("input.txt");

	glutInit(&argc, argv);									   // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // Set display mode  Default mode used
	glutInitWindowSize(winWidth, winHeight);				   // Set window width and height
	glutInitWindowPosition(90, 90);							   // Set window position
	glutCreateWindow("Crowd Simulation using Social Force");   // Set window title and create display window

	animate = true;
	if ((int)inputData[6] == 0)
	{
		glutHideWindow();
	}

	init();					  // Initialization
	glutDisplayFunc(display); // Send graphics to display window
	glutReshapeFunc(reshape); // Maintain aspect ratio when window first created, resized and moved

	glutKeyboardFunc(normalKey);
	glutIdleFunc(update); // Continuously execute 'update()'
	glutMainLoop();		  // Enter GLUT's main loop

	return 0;
}

void init()
{
	// General Light Intensity
	GLfloat gnrlAmbient[] = {0.8F, 0.8F, 0.8F, 1.0}; // Ambient (R, G, B, A) light intensity of entire scene

	// Object Light Intensity
	GLfloat lghtDiffuse[] = {0.7F, 0.7F, 0.7F, 1.0}; // Diffuse (R, G, B, A) light intensity

	// Light Position
	GLfloat lghtPosition[] = {4.0, -4.0, 4.0, 0.0};

	glClearColor(1.0, 1.0, 1.0, 0.0); // Set colour used when colour buffer cleared
	glShadeModel(GL_SMOOTH);		  // Set shading option

	// General Lighting
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gnrlAmbient);

	// Object Lighting
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lghtDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lghtPosition);

	glEnable(GL_DEPTH_TEST); // Enable hidden surface removal
	glEnable(GL_NORMALIZE);	 // Normalize normal vector
	glEnable(GL_LIGHTING);	 // Prepare OpenGL to perform lighting calculations
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHT0);

	glCullFace(GL_BACK);	// Specify face to be culled
	glEnable(GL_CULL_FACE); // Enable culling of specified face

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	srand(1604010629); // Seed to generate random numbers

	socialForce = new SocialForce;
	createWalls();
	createAgents();
}

void createWalls()
{
	Wall *wall;

	float walkwayWidth = (float)inputData[2];
	vector<float> coors = Utility::getWallCoordinates(walkwayWidth);

	// Upper Wall
	wall = new Wall(coors[0], coors[1], coors[2], coors[3]);
	socialForce->addWall(wall);

	wall = new Wall(coors[4], coors[5], coors[6], coors[7]);
	socialForce->addWall(wall);

	// Lower Wall
	wall = new Wall(coors[8], coors[9], coors[10], coors[11]);
	socialForce->addWall(wall);

	wall = new Wall(coors[12], coors[13], coors[14], coors[15]);
	socialForce->addWall(wall);

	// Left Wall
	wall = new Wall(coors[16], coors[17], coors[18], coors[19]);
	socialForce->addWall(wall);

	wall = new Wall(coors[20], coors[21], coors[22], coors[23]);
	socialForce->addWall(wall);

	// Right Wall
	wall = new Wall(coors[24], coors[25], coors[26], coors[27]);
	socialForce->addWall(wall);

	wall = new Wall(coors[28], coors[29], coors[30], coors[31]);
	socialForce->addWall(wall);
}

// void setFlowFeature(Agent *agent, int numOfPeople, float initPosX, float initPosY, float targetPosX, float targetPosY)
// {
// 	for (int temp = 0; temp < numOfPeople; temp++)
// 	{
// 		agent = new Agent;
// 		agent->setPosition(initPosX, initPosY);
// 		agent->setPath(targetPosX, targetPosY, 2.0);
// 		agent->setDesiredSpeed(speed);
// 		socialForce->addAgent(agent);
// 	}
// }

void createAgents()
{
	Agent *agent;

	numOfPeople = Utility::getNumPedesInFlow(int(inputData[0]));
	vector<double> velocityList = Utility::getPedesVelocity(int(inputData[0]), inputData[1]);
	minSpeed = velocityList[0];
	maxSpeed = velocityList[velocityList.size() - 1];
	auto rng = std::default_random_engine{};
	std::shuffle(velocityList.begin(), velocityList.end(), rng);

	int pedesCount = 0;

	// test

	// setFlowFeature(agent, numOfPeople[0], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(-5.0, -3.0));
	// for (int temp = 0; temp < 1; temp++)
	// {
	// 	agent = new Agent;
	// 	// agent->setPosition(randomFloat(-20.3F, -6.0), randomFloat(-2.0, 2.0));
	// 	agent->setPosition(randomFloat(-3.0, -2.0), randomFloat(4.0, 5.0));
	// 	agent->setPath(randomFloat(-20.3F, -6.0), randomFloat(-2.0, 2.0), 1.0);
	// 	// agent->setPath(randomFloat(22.0, 25.0), randomFloat(-3.0, -2.0), 1.0);
	// 	agent->setDesiredSpeed(1.3);
	// 	std::vector<float> color = Utility::getPedesColor(displayMode, maxSpeed, minSpeed, agent->getDesiredSpeed());
	// 	agent->setColour(color[0], color[1], color[2]);
	// 	socialForce->addAgent(agent);
	// }

	// test

	for (int idx = 0; idx < 12; idx++)
	{
		switch (idx % 12)
		{
		// From Left to Right
		case 0:
		{

			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(-5.0, -3.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(0, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(0, 0, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 1:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(-3.0, 3.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(0, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(0, 1, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 2:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(3.0, 5.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(0, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(0, 2, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		// From Right to Left
		case 3:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(-5.0, -3.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(1, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(1, 0, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 4:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(-3.0, 3.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(1, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(1, 1, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 5:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(3.0, 5.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(1, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(1, 2, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}

		// From Top to Bottom
		case 6:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(-5.0, -3.0), randomFloat(-18.0, -15.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(2, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(2, 0, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 7:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(-3.0, 3.0), randomFloat(-18.0, -15.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(2, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(2, 1, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 8:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(3.0, 5.0), randomFloat(-18.0, -15.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(2, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(2, 2, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}

		// From Bottom to Top
		case 9:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(-5.0, -3.0), randomFloat(15.0, 18.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(3, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(3, 0, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 10:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(-3.0, 3.0), randomFloat(15.0, 18.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(3, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(3, 1, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 11:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(3.0, 5.0), randomFloat(15.0, 18.0));
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				vector<float> position = Utility::getPedesSource(3, (float)inputData[3], (float)inputData[4], (float)inputData[5]);
				agent->setPosition(position[0], position[1]);
				vector<float> desList = Utility::getPedesDestination(3, 2, (float)inputData[2]);
				agent->setPath(desList[0], desList[1], desList[2]);
				agent->setDesiredSpeed(velocityList[pedesCount]);
				pedesCount = pedesCount + 1;
				std::vector<float> color = Utility::getPedesColor(maxSpeed, minSpeed, agent->getDesiredSpeed());
				agent->setColour(color[0], color[1], color[2]);
				socialForce->addAgent(agent);
			}
			break;
		}
		default:
			break;
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the colour and depth buffer
	glLoadIdentity();									// Initialize modelview matrix to identity matrix

	// Camera
	gluLookAt(0.0, 0.0, 18.0, // Position
			  0.0, 0.0, 0.0,  // Look-at point
			  0.0, 1.0, 0.0); // Up-vector

	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);

	drawAgents();
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
		glColor3f(agent->getColour().x, agent->getColour().y, agent->getColour().z);
		drawCylinder(agent->getPosition().x, agent->getPosition().y, agent->getRadius(), 15, 0.0);
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
	current.y = 0;		// Set initial point

	for (float angle = sliceAngle; angle <= 360; angle += sliceAngle)
	{
		next.x = radius * cos(angle * PI / 180); // Compute next point
		next.y = radius * sin(angle * PI / 180); // Compute next point

		// Top Cover
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0, 0.0, height);			  // Centre of triangle
		glVertex3f(current.x, current.y, height); // Left of triangle
		glVertex3f(next.x, next.y, height);		  // Right of triangle
		glEnd();

		// Body
		glBegin(GL_QUADS);
		glVertex3f(current.x, current.y, height); // Top-left of quadrilateral
		glVertex3f(current.x, current.y, 0.0);	  // Bottom-left of quadrilateral
		glVertex3f(next.x, next.y, 0.0);		  // Bottom-right of quadrilateral
		glVertex3f(next.x, next.y, height);		  // Top-right of quadrilateral
		glEnd();

		// Bottom Cover
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0, 0.0, 0.0);			   // Centre of triangle
		glVertex3f(current.x, current.y, 0.0); // Left of triangle
		glVertex3f(next.x, next.y, 0.0);	   // Right of triangle
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

void showInformation()
{
	Point3f margin;
	char totalAgentsStr[5] = "\0", fpsStr[8] = "\0", frctnStr[6] = "\0";

	margin.x = static_cast<float>(-winWidth) / 50;
	margin.y = static_cast<float>(winHeight) / 50 - 0.75F;

	glColor3f(0.0, 0.0, 0.0);

	// Total Agents
	drawText(margin.x, margin.y, "Total agents:");
	//_itoa_s
	// std::_itoa(socialForce->getCrowdSize(), totalAgentsStr, 10);
	std::string s = std::to_string(socialForce->getCrowdSize());
	drawText(margin.x + 4.0F, margin.y, // totalAgentsStr
			 s.c_str());

	// FPS
	drawText(margin.x, margin.y - 0.9F, "FPS:");
	//_itoa_s(static_cast<int>(fps), fpsStr, 10);								// Convert integer portion into char
	s = std::to_string(static_cast<int>(fps));
	// strcat_s(fpsStr, ".");													// Append decimal mark
	s = s + ".";
	//_itoa_s((fps - static_cast<int>(fps)) * 100000, frctnStr, 10);			// Convert fractional portion into char
	s = s + std::to_string(fps - static_cast<int>(fps) * 100000);
	// strncat_s(fpsStr, frctnStr, sizeof(fpsStr) - (strlen(fpsStr) + 1));		// Append fractional portion
	fpsStr[7] = '\0';
	drawText(margin.x + 1.7F, margin.y - 0.9F, s.c_str() /*fpsStr*/);
}

void drawText(float x, float y, const char text[])
{
	glDisable(GL_LIGHTING);	  // Disable lighting for proper display of 'drawText()'
	glDisable(GL_DEPTH_TEST); // Disable depth test for proper display of 'drawText()'

	glPushMatrix();
	glTranslatef(x, y, 0.0);
	glScalef(0.0045F, 0.0045F, 0.0);
	glLineWidth(1.4F);

	int idx = 0;
	while (text[idx] != '\0')
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[idx++]);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST); // Enable hidden surface removal
	glEnable(GL_LIGHTING);	 // Prepare OpenGL to perform lighting calculations
}

void reshape(int width, int height)
{
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();														// Initialize projection matrix to identity matrix
	gluPerspective(65.0, static_cast<GLfloat>(width) / height, 1.0, 100.0); // Create matrix for symmetric perspective-view frustum

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
		break;

	case 27: // ASCII character for Esc key
		delete socialForce;
		socialForce = 0;

		exit(0); // Terminate program
		break;
	}
}

float randomFloat(float lowerBound, float upperBound)
{
	return (lowerBound + (static_cast<float>(rand()) / RAND_MAX) * (upperBound - lowerBound));
}

void update()
{
	int currTime, frameTime; // Store time in milliseconds
	static int prevTime;	 // Stores time in milliseconds

	currTime = glutGet(GLUT_ELAPSED_TIME); // Get time in milliseconds since 'glutInit()' called
	frameTime = currTime - prevTime;
	prevTime = currTime;

	int count = 0;

	std::vector<Agent *> agents = socialForce->getCrowd();
	for (Agent *agent : agents)
	{
		Point3f src = agent->getPosition();
		Point3f des = agent->getPath();
		float distance = src.distance(des);
		if (distance > 1)
		{
			break;
		}
		else
		{
			count = count + 1;
		}
	}
	if (count == agents.size())
	{
		std::cout << "Maximum speed: " << maxSpeed << " - Minimum speed: " << minSpeed << endl;
		std::cout << "Finish in: " << Utility::convertTime(currTime) << endl;
		delete socialForce;
		socialForce = 0;

		exit(0); // Terminate program
	}

	if (animate)
		socialForce->moveCrowd(static_cast<float>(frameTime) / 1000); // Perform calculations and move agents

	computeFPS();
	glutPostRedisplay();
	glutIdleFunc(update); // Continuously execute 'update()'
}

void computeFPS()
{
	static int frameCount = 0; // Stores number of frames
	int currTime, frameTime;   // Store time in milliseconds
	static int prevTime;	   // Stores time in milliseconds

	frameCount++;

	currTime = glutGet(GLUT_ELAPSED_TIME); // Get time in milliseconds since 'glutInit()' called
	frameTime = currTime - prevTime;

	if (frameTime > 1000)
	{
		fps = frameCount / (static_cast<float>(frameTime) / 1000); // Compute the number of FPS
		prevTime = currTime;
		frameCount = 0; // Reset number of frames
	}
}
