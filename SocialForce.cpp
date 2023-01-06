#include "SocialForce.h"

using namespace std;

SocialForce::~SocialForce()
{
    removeCrowd();
    removeWalls();
    removeAGVs();
}

void SocialForce::addAgent(Agent *agent) { crowd.push_back(agent); }

void SocialForce::addWall(Wall *wall) { walls.push_back(wall); }

void SocialForce::addAGV(AGV *agv) { agvs.push_back(agv); }

void SocialForce::removeAgent()
{
    int lastIdx;

    if (!crowd.empty())
    {
        lastIdx = crowd.size() - 1; // Assign index of last element

        delete crowd[lastIdx];
        crowd.pop_back();
    }
}

void SocialForce::removeCrowd()
{
    for (unsigned int idx = 0; idx < crowd.size(); idx++)
        delete crowd[idx];

    crowd.clear();
}

void SocialForce::removeWalls()
{
    for (unsigned int idx = 0; idx < walls.size(); idx++)
        delete walls[idx];

    walls.clear();
}

void SocialForce::removeAGV()
{
    int lastIdx;
    if (!agvs.empty())
    {
        lastIdx = agvs.size() - 1; // Assign index of last element

        delete agvs[lastIdx];
        agvs.pop_back();
    }
}

void SocialForce::removeAGVs()
{
    for (unsigned int idx = 0; idx < crowd.size(); idx++)
        delete agvs[idx];

    agvs.clear();
}

void SocialForce::moveCrowd(float stepTime)
{
    for (unsigned int idx = 0; idx < crowd.size(); idx++)
        crowd[idx]->move(crowd, walls, agvs, stepTime);
}

void SocialForce::moveAGVs(float stepTime)
{
    int lastIdx;
    vector<Point3f> position_list;
    for (Agent *agent : crowd)
    {
        if (agent->getPosition().distance(Vector3f(0, 0, 0)) > 12.0F)
            continue;
        position_list.push_back(agent->getPosition());
    }

    if (!agvs.empty())
    {
        lastIdx = agvs.size() - 1; // Assign index of last element
        agvs[lastIdx]->move(stepTime, position_list);

        Point3f src = agvs[lastIdx]->getPosition();
        Point3f des = agvs[lastIdx]->getDestination();

        float distance = src.distance(des);
        if (distance <= 1 || isnan(distance))
        {
            delete agvs[lastIdx];
            agvs.pop_back();
        }
    }
}