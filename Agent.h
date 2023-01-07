#ifndef AGENT_H
#define AGENT_H

#include "vecmath.h"
#include <deque>
#include <vector>
#include "Wall.h"
#include "AGV.h"
#include "MovingObject.h"
#include "Force.h"

class Agent : public MovingObject, public Force
{
private:
    static int crowdIdx; // Keep track of 'crowd' vector index in 'SocialForce.h'
    float radius;

    Vector3f getDrivingForce(const Point3f position_target);
    Vector3f getAgentInteractForce(std::vector<Agent *> agents);
    Vector3f getWallInteractForce(std::vector<Wall *> walls);
    Vector3f getAgvInteractForce(std::vector<AGV *> agvs);

public:
    Agent();

    ~Agent();

    std::vector<Point3f> interDes;

    void setRadius(float radius);

    float getRadius() const { return radius; }

    float getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius);

    void move(std::vector<Agent *> agents, std::vector<Wall *> walls, std::vector<AGV *> agvs, float stepTime);
};

#endif
