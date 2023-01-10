#ifndef UTILITES_H
#define UTILITES_H

#include <vector>
#include <iostream>
#include <array>
#include "vecmath.h"
#include <map>
#include "AGV.h"

class Utility
{
public:
    Utility();

    virtual ~Utility();

    static constexpr const float MEAN = 1.2;
    static constexpr const double UPPER_SPEED_LIMIT = 1.8;
    static constexpr const double LOWER_SPEED_LIMIT = 0.6;

    static float randomFloat(float lowerBound, float upperBound);

    static std::map<std::string, std::vector<float>> readMapData(const char *fileName);

    static std::vector<double> readInput(const char *fileName);

    static void writeEnd(const char *fileName, std::string name, int mode, std::vector<AGV *> data);

    static std::vector<int> getNumPedesInFlow(int junctionType, int totalPedestrian);

    static std::vector<double> getPedesVelocity(int type, std::vector<double> inputData);

    static std::vector<double> getPedesVelocityBasedTDis(int numPedes, double n_dist);

    static std::vector<double> getPedesVelocityBasedDDis(std::vector<double> inputData);

    static std::vector<float> getWallCoordinates(float walkwayWidth, std::vector<float> juncData);

    static std::string convertTime(int milliseconds);

    static std::vector<float> getMapLimit(float walkwayWidth, std::vector<float> juncData);

    static std::vector<float> getPedesDestination(int direction, int side, float walkwayWidth, std::vector<float> juncData, bool stopAtCorridor);

    static std::vector<float> getPedesSource(int direction, float totalLength, float subLength, float caravanWidth, float walkwayWidth, std::vector<float> juncData);

    static std::vector<float> getPedesColor(float maxSpeed, float minSpeed, float desiredSpeed);

    static std::vector<Point3f> getRouteAGV(int junctionType, int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    static std::vector<Point3f> getRouteAGVCrossRoad(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    static std::vector<Point3f> getRouteAGVTJunction(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData);

    static Point3f getIntermediateDes(Point3f position, float verWalkwayWidth, float horWalkwayWidth);

    static int randomInt(int from, int to);
};

#endif