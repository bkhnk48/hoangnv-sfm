#ifndef UTILITES_H
#define UTILITES_H

#include <vector>
#include <iostream>
#include <array>
#include "vecmath.h"

class Utility {
public:
    Utility();

    virtual ~Utility();

    static constexpr const float MEAN = 1.2;
    static constexpr const float HEIGHT_LIMIT = 13;
    static constexpr const float WIDTH_LIMIT = 20;
    static constexpr const double UPPER_SPEED_LIMIT = 1.8;
    static constexpr const double LOWER_SPEED_LIMIT = 0.6;

    static float randomFloat(float lowerBound, float upperBound);

    static std::vector<double> readInput(const char *fileName);

    static std::array<int, 12> getNumPedesInFlow(int totalPedestrian);

    static std::vector<double> getPedesVelocity(int numPedes, double n_dist);

    static std::vector<float> getWallCoordinates(float walkwayWidth);

    static std::string convertTime(int milliseconds);

    static std::vector<float> getPedesDestination(int direction, int side, float walkwayWidth);

    static std::vector<float> getPedesSource(int direction, float totalLength, float subLength, float caravanWidth);

    static std::vector<float> getPedesColor(float maxSpeed, float minSpeed, float desiredSpeed);

    static std::vector <Point3f> getRouteAGV(int src, int turningDirection, float walkwayWidth);

    static Point3f getIntermediateDes(Point3f position, float verWalkwayWidth, float horWalkwayWidth);
};

#endif