#ifndef AGV_H
#define AGV_H

#include "vecmath.h"
#include <deque>
#include <vector>

struct Endpoint
{
    Point3f position;
    float radius;
};

class AGV
{
private:
    static int crowdIdx; // Keep track of 'crowd' vector index in 'SocialForce.h'

    int id;
    Vector3f dimension;
    Color3f colour;

    Point3f position;
    std::deque<Endpoint> path;
    Vector3f velocity;

public:
    AGV();
    ~AGV();

    void setDimension(Vector3f dimension);
    void setColour(float red, float green, float blue);
    void setPosition(float x, float y);
    void setPath(float x, float y, float radius);
    void setVelocity(float x, float y);

    int getId() const { return id; }
    Vector3f getDimension() const { return dimension; }
    Color3f getColour() const { return colour; }
    Point3f getPosition() const { return position; }
    Point3f getPath();
    Vector3f getVelocity() const { return velocity; }
    float getOrientation();
    Point3f getAheadVector() const;
    Point3f getNearestPoint(Point3f position_i) const;

    void move(float stepTime);
};

#endif
