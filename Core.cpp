#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <cstring>
#include <GL/glut.h>
#include "SocialForce.h"
using namespace std;

// Global Constant Variables
const float PI = 3.14159265359F;

// Global Variables
GLsizei winWidth = 992;	 // Window width (16:9 ratio)
GLsizei winHeight = 620; // Window height (16:9 ratio)
SocialForce *socialForce;
float fps = 0;		  // Frames per second
bool animate = false; // Animate scene flag

int numOfPeople[] = {3, 5, 7, 4, 5, 9, 2, 4, 5, 3, 6, 4};
float minSpeed = 0.81;
float maxSpeed = 1.4;
float perLowSpeed = 0.3;

// Function Prototypes
void init();
void createWalls();
void createAgents();
void createAGVs();
void display();
void drawAgents();
void drawAGVs();
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
	glutInit(&argc, argv);									   // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // Set display mode  Default mode used
	glutInitWindowSize(winWidth, winHeight);				   // Set window width and height
	glutInitWindowPosition(90, 90);							   // Set window position
	glutCreateWindow("Crowd Simulation using Social Force");   // Set window title and create display window

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
	createAGVs();
}

void createWalls()
{
	Wall *wall;

	// Upper Wall
	wall = new Wall(-25.0, 5.0, -5.0, 5.0); // Step 1: Create wall and define its coordinates (param: x1, y1, x2, y2)
	socialForce->addWall(wall);				// Step 2: Add wall to SFM

	wall = new Wall(5.0, 5.0, 25.0, 5.0);
	socialForce->addWall(wall);

	// Lower Wall
	wall = new Wall(-25.0, -5.0, -5.0, -5.0);
	socialForce->addWall(wall);

	wall = new Wall(5.0, -5.0, 25.0, -5.0);
	socialForce->addWall(wall);

	// Left Wall
	wall = new Wall(-5.0, 15.0, -5.0, 5.0);
	socialForce->addWall(wall);

	wall = new Wall(-5.0, -5.0, -5.0, -15.0);
	socialForce->addWall(wall);

	// Right Wall
	wall = new Wall(5.0, 15.0, 5.0, 5.0);
	socialForce->addWall(wall);

	wall = new Wall(5.0, -5.0, 5.0, -15.0);
	socialForce->addWall(wall);
}

float calSpeed(int pos, int numLowSpeed)
{
	float speed;
	if (pos < numLowSpeed)
	{
		speed = randomFloat(minSpeed, minSpeed + (maxSpeed - minSpeed) / 3);
	}
	else
	{
		speed = randomFloat(minSpeed + (maxSpeed - minSpeed) / 3, maxSpeed);
	}
	return speed;
}

void setFlowFeature(Agent *agent, int numOfPeople, float initPosX, float initPosY, float targetPosX, float targetPosY)
{
	int numLowSpeed = perLowSpeed * numOfPeople;
	for (int temp = 0; temp < numOfPeople; temp++)
	{
		agent = new Agent;
		float speed = calSpeed(temp, numLowSpeed);
		agent->setPosition(initPosX, initPosY);
		agent->setPath(targetPosX, targetPosY, 2.0);
		agent->setDesiredSpeed(speed);
		socialForce->addAgent(agent);
	}
}

void createAgents()
{
	Agent *agent;

	for (int idx = 0; idx < 12; idx++)
	{
		switch (idx % 12)
		{
		// From Left to Right
		case 0:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(-5.0, -3.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(22.0, 25.0), randomFloat(-5.0, -3.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 1:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(-3.0, 3.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(22.0, 25.0), randomFloat(-3.0, 3.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 2:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0), randomFloat(22.0, 25.0), randomFloat(3.0, 5.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-20.3F, -6.0), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(22.0, 25.0), randomFloat(3.0, 5.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		// From Right to Left
		case 3:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(-5.0, -3.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(-25.0, -22.0), randomFloat(-5.0, -3.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 4:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(-3.0, 3.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(-25.0, -22.0), randomFloat(-3.0, 3.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 5:
		{
			// setFlowFeature(agent, numOfPeople[idx], randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0), randomFloat(-25.0, -22.0), randomFloat(3.0, 5.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(6.0, 20.3F), randomFloat(-4.0, 4.0));
				agent->setPath(randomFloat(-25.0, -22.0), randomFloat(3.0, 5.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}

		// From Top to Bottom
		case 6:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(-5.0, -3.0), randomFloat(-18.0, -15.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F));
				agent->setPath(randomFloat(-5.0, -3.0), randomFloat(-18.0, -15.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 7:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(-3.0, 3.0), randomFloat(-18.0, -15.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F));
				agent->setPath(randomFloat(-3.0, 3.0), randomFloat(-18.0, -15.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 8:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F), randomFloat(3.0, 5.0), randomFloat(-18.0, -15.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(6.0, 12.3F));
				agent->setPath(randomFloat(3.0, 5.0), randomFloat(-18.0, -15.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}

		// From Bottom to Top
		case 9:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(-5.0, -3.0), randomFloat(15.0, 18.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0));
				agent->setPath(randomFloat(-5.0, -3.0), randomFloat(15.0, 18.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 10:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(-3.0, 3.0), randomFloat(15.0, 18.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0));
				agent->setPath(randomFloat(-3.0, 3.0), randomFloat(15.0, 18.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		case 11:
		{ // setFlowFeature(agent, numOfPeople[idx], randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0), randomFloat(3.0, 5.0), randomFloat(15.0, 18.0));
			int numLowSpeed = perLowSpeed * numOfPeople[idx];
			for (int temp = 0; temp < numOfPeople[idx]; temp++)
			{
				agent = new Agent;
				float speed = calSpeed(temp, numLowSpeed);
				agent->setPosition(randomFloat(-4.0, 4.0), randomFloat(-12.3F, -6.0));
				agent->setPath(randomFloat(3.0, 5.0), randomFloat(15.0, 18.0), 2.0);
				agent->setDesiredSpeed(speed);
				socialForce->addAgent(agent);
			}
			break;
		}
		default:
			break;
		}
	}
}

void createAGVs() {
	AGV *agv = new AGV;
	agv->setPosition(randomFloat(-6.3F, -2.0), randomFloat(-12.3F, -6.0));
	agv->setPath(randomFloat(3.0, 5.0), randomFloat(15.0, 18.0), 2.0);
	agv->setVelocity(0,1.0F);
	socialForce->addAGV(agv);
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
		glColor3f(agent->getColour().x, agent->getColour().y, agent->getColour().z);
		drawCylinder(agent->getPosition().x, agent->getPosition().y, agent->getRadius(), 15, 0.0);
	}
}

void drawAGVs()
{
	vector<AGV *> agvs = socialForce->getAGVs();

	for (AGV *agv : agvs)
	{
		// Draw AGVs
		glColor3f(agv->getColour().x, agv->getColour().y, agv->getColour().z);
		// drawCylinder(agv->getPosition().x, agv->getPosition().y, agv->getRadius(), 15, 0.0);
		float x, y, w, h;
		x = agv->getPosition().x;
		y = agv->getPosition().y;
		w = agv->getDimension().x;
		h = agv->getDimension().y;
		glBegin(GL_QUADS);
		glVertex3f(x - w / 2, y + h / 2, 0);
		glVertex3f(x - w / 2, y - h / 2, 0);
		glVertex3f(x + w / 2, y - h / 2, 0);
		glVertex3f(x + w / 2, y + h / 2, 0);
		glEnd();
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

	if (animate)
	{
		socialForce->moveCrowd(static_cast<float>(frameTime) / 1000); // Perform calculations and move agents
		socialForce->moveAGV(static_cast<float>(frameTime) / 1000);
	}
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
