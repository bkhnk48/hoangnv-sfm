#ifndef UTILITES_H
#define UTILITES_H

#include <vector>
#include <iostream>
#include <array>
#include "lib/vecmath/vecmath.h"
#include <map>
#include "src/agv/AGV.h"

#pragma once
namespace Utility
{
    float randomFloat(float lowerBound, float upperBound);

    std::map<std::string, std::vector<float>> readMapData(const char *fileName);

    std::vector<double> readInput(const char *fileName);

    void writeEnd(const char *fileName, std::string name, int mode, std::vector<AGV *> data);

    std::vector<int> getNumPedesInFlow(int junctionType, int totalPedestrian);

    std::vector<double> getPedesVelocity(int type, std::vector<double> inputData);

    std::vector<double> getPedesVelocityBasedTDis(int numPedes, double n_dist);

    std::vector<double> getPedesVelocityBasedDDis(std::vector<double> inputData);

    std::vector<float> getWallCoordinates(float walkwayWidth, std::vector<float> juncData);

    std::string convertTime(int milliseconds);

    std::vector<float> getMapLimit(float walkwayWidth, std::vector<float> juncData);

    std::vector<float> getPedesDestination(int direction, int side, float walkwayWidth, std::vector<float> juncData, bool stopAtCorridor);

    std::vector<float> getPedesSource(int direction, float totalLength, float subLength, float caravanWidth, float walkwayWidth, std::vector<float> juncData);

    std::vector<float> getPedesColor(float maxSpeed, float minSpeed, float desiredSpeed, int type);

    std::vector<Point3f> getRouteAGV(int junctionType, int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    std::vector<Point3f> getRouteAGVCrossRoad(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    std::vector<Point3f> getRouteAGVTJunction(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    Point3f getIntermediateDes(Point3f position, float verWalkwayWidth, float horWalkwayWidth);

    bool isPositionErr(Point3f position, float walkwayWidth, int junctionType);

    int randomInt(int from, int to);
};

#endif