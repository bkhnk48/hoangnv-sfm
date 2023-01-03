#ifndef AGV_H
#define AGV_H

#include "vecmath.h"
#include <deque>
#include <vector>

class AGV {
private:
    static int agvIdx;

    int id;
    float acceleration, tangential_velocity, distance;
    Vector3f dimension, velocity, instantaneous_velocity, d1, d2, d3, d4;
    Color3f colour;

    Point3f position;
    std::deque <Point3f> path;

public:
    AGV();

    ~AGV();

    void setAcceleration(float acceleration);

    void setTangentialVelocity(float v);

    void setDistance(float distance);

    void setDimension(Vector3f dimension);

    void setBorderPoint(Vector3f d1, Vector3f d2, Vector3f d3, Vector3f d4);

    void setColour(float red, float green, float blue);

    void setPosition(float x, float y);

    void setPath(float x, float y);

    int getId() const { return id; }

    double getAcceleration() { return acceleration; }

    float getTangentialVelocity() const { return tangential_velocity; }

    float getDistance() { return distance; }

    Vector3f getDimension() const { return dimension; }

    Vector3f getVelocity() const { return velocity; }

    Color3f getColour() const { return colour; }

    Point3f getPosition() const { return position; }

    Point3f getPath();

    float getOrientation();

    Point3f getAheadVector() const;

    Point3f getNearestPoint(Point3f position_i) const;

    bool checkNearAgent(std::vector <Point3f> position_list);

    void move(float stepTime, std::vector <Point3f> position_list);
};

#endif
