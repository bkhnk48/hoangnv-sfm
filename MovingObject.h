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

    void setColor(float red, float green, float blue);

    void setPosition(float x, float y);

    void setPath(float x, float y, float radius);

    void setDestination(float x, float y);

    void setDesiredSpeed(float speed);
    
    void setIsMoving(bool isMoving);

    bool getIsMoving() const { return isMoving; }

    int getId() const { return id; }

    float getDesiredSpeed() const { return desiredSpeed; }

    Color3f getColor() const { return color; }

    Point3f getPosition() const { return position; }

    Point3f getPath();

    Point3f getDestination() const { return destination; };

    Vector3f getVelocity() const { return velocity; }

    Point3f getAheadVector() const;

    float getOrientation();

    virtual void move(){};
};

#endif
