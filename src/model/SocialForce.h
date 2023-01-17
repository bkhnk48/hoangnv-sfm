#ifndef SOCIAL_FORCE_H
#define SOCIAL_FORCE_H

#include "src/agv/AGV.h"
#include "src/agent/Agent.h"
#include "src/wall/Wall.h"
#include <vector>
#include <algorithm>

class SocialForce {
private:
    std::vector<Agent *> crowd;
    std::vector<Wall *> walls;
    std::vector<AGV *> agvs;

public:
    // SocialForce();
    ~SocialForce();

    void addAgent(Agent *agent);

    void addWall(Wall *wall);

    void addAGV(AGV *agv);

    const std::vector<Agent *> getCrowd() const { return crowd; }

    int getCrowdSize() const { return crowd.size(); }

    const std::vector<Wall *> getWalls() const { return walls; }

    int getNumWalls() const { return walls.size(); }

    const std::vector<AGV *> getAGVs() const { return agvs; }

    int getNumAGVs() const { return agvs.size(); }

    void removeAgent(int agentId);
    void removeCrowd(); // Remove all individuals and groups
    void removeWalls();

    void removeAGV();
    void removeAGVs();

    void moveCrowd(float stepTime);

    void moveAGVs(float stepTime);
};

#endif