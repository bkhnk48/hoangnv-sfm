#ifndef RENDERER_H
#define RENDERER_H

#if defined(__linux__)
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <vector>
#include "src/model/SocialForce.h"
#include "src/constant/Constant.h"
#include "src/utility/Utility.h"

using namespace Constant;
using namespace Utility;

#pragma once
namespace Renderer
{
    void drawAgents(SocialForce *socialForce);

    void drawAGVs(
        SocialForce *socialForce,
        std::vector<float> juncData,
        int agvRunConcurrently,
        int runMode);

    void drawCylinder(
        float x, float y, float radius = 0.2, int slices = 15,
        float height = 0.5);

    void drawWalls(SocialForce *socialForce);

    void drawText(float x, float y, const char text[]);

    void drawSquare(double x1, double y1, double sidelength, Color3f color);

    void showInformation(
        SocialForce *socialForce,
        float fps,
        bool animate,
        int currTime,
        int startTime,
        int classificationType,
        GLsizei winWidth,
        GLsizei winHeight);

    void computeFPS(float *fps);
}

#endif
