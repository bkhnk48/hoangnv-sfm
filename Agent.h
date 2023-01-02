#ifndef AGENT_H
#define AGENT_H

#include "vecmath.h"
#include <deque>
#include <vector>
#include "Wall.h"
#include "AGV.h"

struct Waypoint {
    Point3f position;
    float radius;
};

class Agent {
private:
    static int crowdIdx;    // Keep track of 'crowd' vector index in 'SocialForce.h'

    int id;
    float radius;
    float desiredSpeed;
    Color3f colour;

    Point3f position;
    Point3f destination;
    std::deque <Waypoint> path;
    Vector3f velocity;

public:
    Agent();

    ~Agent();

    bool hadInterDes;

    void setRadius(float radius);

    void setDesiredSpeed(float speed);

    void setColour(float red, float green, float blue);

    void setPosition(float x, float y);

    void setDestination(float x, float y);

    void setPath(float x, float y, float radius);

    int getId() const { return id; }

    float getRadius() const { return radius; }

    float getDesiredSpeed() const { return desiredSpeed; }

    Color3f getColour() const { return colour; }

    Point3f getPosition() const { return position; }

    Point3f getPath();

    Point3f getDestination() const { return destination; };

    Vector3f getVelocity() const { return velocity; }

    float getOrientation();

    Point3f getAheadVector() const;

    float getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius);

    void move(std::vector<Agent *> agents, std::vector<Wall *> walls, std::vector<AGV *> agvs, float stepTime);
};

#endif