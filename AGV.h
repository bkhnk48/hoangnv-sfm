#ifndef AGV_H
#define AGV_H

#include "vecmath.h"
#include <deque>
#include <vector>
#include "MovingObject.h"

class AGV: public MovingObject {
private:
    static int agvIdx;

    float acceleration, distance;
    Vector3f dimension, instantaneous_velocity, d1, d2, d3, d4;
    
public:
    AGV();

    ~AGV();

    void setAcceleration(float acceleration);

    void setDistance(float distance);

    void setDimension(Vector3f dimension);

    void setBorderPoint(Vector3f d1, Vector3f d2, Vector3f d3, Vector3f d4);

    double getAcceleration() { return acceleration; }

    float getDistance() { return distance; }

    Vector3f getDimension() const { return dimension; }

    Point3f getNearestPoint(Point3f position_i) const;

    bool checkNearAgent(std::vector <Point3f> position_list);

    void move(float stepTime, std::vector <Point3f> position_list);
};

#endif
