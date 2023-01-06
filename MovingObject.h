#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "vecmath.h"
#include <deque>
#include <vector>

struct Waypoint
{
    Point3f position;
    float radius;
};

class MovingObject
{
protected:
    int id;
    Color3f colour;
    float desiredSpeed;
    std::deque<Waypoint> path;
    Point3f destination;
    Point3f position;
    Vector3f velocity;

public:
    MovingObject();
    ~MovingObject();

    void setColour(float red, float green, float blue);

    void setPosition(float x, float y);

    void setPath(float x, float y, float radius);

    void setDestination(float x, float y);

    void setDesiredSpeed(float speed);

    int getId() const { return id; }

    float getDesiredSpeed() const { return desiredSpeed; }

    Color3f getColour() const { return colour; }

    Point3f getPosition() const { return position; }

    Point3f getPath();

    Point3f getDestination() const { return destination; };

    Vector3f getVelocity() const { return velocity; }

    Point3f getAheadVector() const;

    float getOrientation();
};

#endif
