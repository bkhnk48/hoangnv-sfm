#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "lib/vecmath/vecmath.h"
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
    Color3f color;
    float desiredSpeed;
    std::deque<Waypoint> path;
    Point3f destination;
    Point3f position;
    Vector3f velocity;
    bool isMoving;

public:
    MovingObject();
    ~MovingObject();

    int getId() const { return id; }

    Color3f getColor() const { return color; }

    void setColor(float red, float green, float blue);

    Point3f getPosition() const { return position; }

    void setPosition(float x, float y);

    Point3f getPath();

    void setPath(float x, float y, float radius);

    Point3f getDestination() const { return destination; };

    void setDestination(float x, float y);

    float getDesiredSpeed() const { return desiredSpeed; }

    void setDesiredSpeed(float speed);

    bool getIsMoving() const { return isMoving; }

    void setIsMoving(bool isMoving);

    Vector3f getVelocity() const { return velocity; }

    Point3f getAheadVector() const;

    float getOrientation();

    virtual void move(){};
};

#endif
