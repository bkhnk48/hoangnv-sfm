#ifndef AGV_H
#define AGV_H

#include "vecmath.h"
#include <deque>
#include <vector>
#include "MovingObject.h"

class AGV : public MovingObject
{
private:
    static int agvIdx;
    int totalTime, collision;
    float acceleration, distance;
    Vector3f dimension, instantaneous_velocity, d1, d2, d3, d4;
    bool stop;
    Vector3f direction;

public:
    AGV();

    ~AGV();

    void setTotalTime(int totalTime);

    void setCollision(int collision);

    void setAcceleration(float acceleration);

    void setDistance(float distance);

    void setDimension(Vector3f dimension);

    void setBorderPoint(Vector3f d1, Vector3f d2, Vector3f d3, Vector3f d4);

    void setDirection(float x, float y);

    int getTotalTime() { return totalTime; }

    int getCollision() { return collision; }

    double getAcceleration() { return acceleration; }

    float getDistance() { return distance; }

    Vector3f getDimension() const { return dimension; }

    Vector3f getDirection() { return direction; }

    Point3f getNearestPoint(Point3f position_i) const;

    bool checkNearAgent(std::vector<Point3f> position_list);

    void move(float stepTime, std::vector<Point3f> position_list);
};

#endif
