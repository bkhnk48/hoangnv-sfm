#ifndef AGV_H
#define AGV_H

#include "vecmath.h"
#include <deque>
#include <vector>

class AGV
{
private:
    static int agvIdx;

    int id;
    double acceleration;
    float distance;
    Vector3f dimension;
    Color3f colour;

    Point3f position;
    Vector3f velocity;
    Vector3f instantaneous_velocity;

public:
    AGV();
    ~AGV();

    void setAcceleration(double acceleration);
    void setDistance(float distance);
    void setDimension(Vector3f dimension);
    void setColour(float red, float green, float blue);
    void setPosition(float x, float y);
    void setVelocity(float x, float y);

    int getId() const { return id; }
    double getAcceleration() { return acceleration; }
    float getDistance() { return distance; }
    Vector3f getDimension() const { return dimension; }
    Color3f getColour() const { return colour; }
    Point3f getPosition() const { return position; }
    Vector3f getVelocity() const { return velocity; }
    float getOrientation();
    Point3f getAheadVector() const;
    Point3f getNearestPoint(Point3f position_i) const;

    bool checkNearAgent(std::vector<Point3f> position_list);

    void move(float stepTime, std::vector<Point3f> position_list);
};

#endif
