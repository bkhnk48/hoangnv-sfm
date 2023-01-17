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
    float impatient;
    bool stopAtCorridor;

public:
    Agent();

    ~Agent();

    std::vector<Point3f> interDes;

    void setRadius(float radius);

    float getRadius() const { return radius; }

    void setImpatient(float impatient);

    float getImpatient() const { return impatient; }

    void setStopAtCorridor(bool stopAtCorridor);

    float getStopAtCorridor() const { return stopAtCorridor; }

    float getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius);

    Vector3f getDrivingForce(const Point3f destination);
    Vector3f getAgentInteractForce(std::vector<Agent *> agents);
    Vector3f getWallInteractForce(std::vector<Wall *> walls);
    Vector3f getAgvInteractForce(std::vector<AGV *> agvs);

    using MovingObject::move;
    void move(std::vector<Agent *> agents, std::vector<Wall *> walls, std::vector<AGV *> agvs, float stepTime);
};

#endif
