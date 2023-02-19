#include "Renderer.h"

using namespace Renderer;

void Renderer::drawAgents(SocialForce *socialForce)
{
    vector<Agent *> agents = socialForce->getCrowd();

    for (Agent *agent : agents)
    {
        // Draw Agents
        glColor3f(agent->getColor().x, agent->getColor().y, agent->getColor().z);
        drawCylinder(
            agent->getPosition().x, agent->getPosition().y,
            agent->getRadius(), 15, 0.0);
    }
}

void Renderer::drawAGVs(
    SocialForce *socialForce,
    std::vector<float> juncData,
    int agvRunConcurrently,
    int runMode)
{
    vector<AGV *> agvs = socialForce->getAGVs();
    Vector3f e_ij;

    if (runMode == 1)
    // if (false)
    {
        AGV *agv1 = NULL;
        AGV *agv2 = NULL;
        int i;
        vector<int> j;

        vector<AGV *> runningAGV;

        for (i = 0; i < agvs.size(); i++)
        {
            if (agvs[i]->getIsMoving() && agvs[i]->getTravelingTime() != 0)
            {
                agv1 = agvs[i];
                if (i < agvs.size() - 1 && agvRunConcurrently == 1 && i % 2 == 0)
                {
                    agv2 = agvs[i + 1];
                    runningAGV.clear();
                    runningAGV.insert(runningAGV.end(), {agv1, agv2});
                }
                else
                {
                    runningAGV.clear();
                    runningAGV.insert(runningAGV.end(), {agv1});
                }

                break;
            }
            else if (!agvs[i]->getIsMoving() && agvs[i]->getTravelingTime() == 0)
            {
                j.push_back(i);
            }
        }

        if (i == agvs.size())
        {
            if (j.size() > 0)
            {
                agv1 = agvs[j.front()];
                if (agvRunConcurrently == 1)
                {
                    agv2 = agvs[agv1->getId() + 1];
                    runningAGV.clear();
                    runningAGV.insert(runningAGV.end(), {agv1, agv2});
                }
                else
                {
                    runningAGV.clear();
                    runningAGV.insert(runningAGV.end(), {agv1});
                }
            }
        }

        if (runningAGV.size() > 0)
        {
            for (AGV *agv : runningAGV)
            {
                if (agv->getTravelingTime() == 0)
                {
                    agv->setIsMoving(true);
                    cout << "AGV ID " << agv->getId() << " is running..." << endl;
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
    else if (juncData.size() == 2 && agvRunConcurrently == 1)
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
}

void Renderer::drawCylinder(float x, float y, float radius, int slices, float height)
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

void Renderer::drawWalls(SocialForce *socialForce)
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

void Renderer::drawText(float x, float y, const char text[])
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

void Renderer::drawSquare(double x1, double y1, double sidelength, Color3f color)
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

void Renderer::showInformation(
    SocialForce *socialForce,
    float fps,
    bool animate,
    int currTime,
    int startTime,
    int classificationType,
    GLsizei winWidth,
    GLsizei winHeight)
{
    Point3f margin;

    margin.x = static_cast<float>(-winWidth) / 60;
    margin.y = static_cast<float>(winHeight) / 60 - 0.75F;

    glColor3f(0.0, 0.0, 0.0);

    // Total Agents
    drawText(margin.x, margin.y - 0.9F, "Total agents:");
    std::string s = std::to_string(socialForce->getCrowdSize());
    drawText(
        margin.x + 4.0F, margin.y - 0.9F, // totalAgentsStr
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

    drawText(
        margin.x + 5.0F, margin.y - 1.8F, // totalAgentsStr
        s.c_str());

    // Appendix
    if (classificationType == 0)
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

void Renderer::computeFPS(float *fps)
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
        *fps = frameCount /
               (static_cast<float>(frameTime) / 1000); // Compute the number of FPS
        prevTime = currTime;
        frameCount = 0; // Reset number of frames
    }
}
