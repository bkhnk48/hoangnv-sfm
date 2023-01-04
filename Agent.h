#ifndef AGENT_H
#define AGENT_H

#include "vecmath.h"
#include <deque>
#include <vector>
#include "Wall.h"
#include "AGV.h"
#include "MovingObject.h"

class Agent: public MovingObject {
private:
    static int crowdIdx;    // Keep track of 'crowd' vector index in 'SocialForce.h'

    float radius;

    Point3f destination;

public:
    Agent();

    ~Agent();

    bool hadInterDes;

    void setRadius(float radius);

    float getRadius() const { return radius; }

    float getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius);

    void move(std::vector<Agent *> agents, std::vector<Wall *> walls, std::vector<AGV *> agvs, float stepTime);
};

#endif