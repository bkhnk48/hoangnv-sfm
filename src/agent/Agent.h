#ifndef AGENT_H
#define AGENT_H

#include "lib/vecmath/vecmath.h"
#include <deque>
#include <vector>
#include "src/wall/Wall.h"
#include "src/agv/AGV.h"
#include "src/object/MovingObject.h"

class Agent : public MovingObject
{
private:
    static int crowdIdx; // Keep track of 'crowd' vector index in 'SocialForce.h'
    float radius;
    bool stopAtCorridor;

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

    void setStopAtCorridor(bool stopAtCorridor);

    float getStopAtCorridor() const { return stopAtCorridor; }

    float getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius);

    using MovingObject::move;
    void move(std::vector<Agent *> agents, std::vector<Wall *> walls, std::vector<AGV *> agvs, float stepTime);
};

#endif
