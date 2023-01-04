#include <random>
#include "Agent.h"
#include "Force.h"

using namespace std;

int Agent::crowdIdx = -1;
default_random_engine generator;

Agent::Agent()
{
    MovingObject();
    id = ++crowdIdx;

    radius = 0.2F;

    hadInterDes = false;

    // Desired Speed Based on (Moussaid et al., 2009)
    normal_distribution<float> distribution(1.29F,
                                            0.19F); // Generate random value of mean 1.29 and standard deviation 0.19
    desiredSpeed = distribution(generator);
}

Agent::~Agent()
{
    path.clear(); // Remove waypoints
    crowdIdx--;
}

void Agent::setRadius(float radius)
{
    this->radius = radius;
}

float Agent::getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius)
{
    Point3f nearestPoint;
    Vector3f vector;
    float distanceSquared, minDistanceSquared = INFINITY;

    for (Wall *wall : walls)
    {
        nearestPoint = wall->getNearestPoint(position);
        vector = position - nearestPoint; // Vector from wall to agent i
        distanceSquared = vector.lengthSquared();

        // Store Nearest Wall Distance
        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
        }
    }

    return sqrt(minDistanceSquared) - radius; // Distance between wall and agent i
}

void Agent::move(vector<Agent *> agents, vector<Wall *> walls, vector<AGV *> agvs, float stepTime)
{
    Vector3f acceleration;

    // Compute Social Force
    acceleration = AgentForce::drivingForce(getPath(), position, desiredSpeed, velocity) +
                   AgentForce::agentInteractForce(agents, id, position, velocity) +
                   AgentForce::wallInteractForce(walls, position, radius) +
                   AgentForce::agvInteractForce(agvs, position, velocity, radius);

    // Compute New Velocity
    velocity = velocity + acceleration * stepTime;

    // Truncate Velocity if Exceed Maximum Speed (Magnitude)
    if (velocity.lengthSquared() > (desiredSpeed * desiredSpeed))
    {
        velocity.normalize();
        velocity *= desiredSpeed;
    }

    // Compute New Position
    position = position + velocity * stepTime;
}
