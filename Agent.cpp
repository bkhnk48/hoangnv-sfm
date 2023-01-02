#include <random>
#include "Agent.h"
#include "Force.h"

using namespace std;

const float PI = 3.14159265359F;

int Agent::crowdIdx = -1;
default_random_engine generator;

Agent::Agent() {
    id = ++crowdIdx;

    radius = 0.2F;

    hadInterDes = false;

    // Desired Speed Based on (Moussaid et al., 2009)
    normal_distribution<float> distribution(1.29F,
                                            0.19F); // Generate random value of mean 1.29 and standard deviation 0.19
    desiredSpeed = distribution(generator);

    colour.set(0.0, 0.0, 0.0);

    position.set(0.0, 0.0, 0.0);
    velocity.set(0.0, 0.0, 0.0);
}

Agent::~Agent() {
    path.clear(); // Remove waypoints
    crowdIdx--;
}

void Agent::setRadius(float radius) {
    this->radius = radius;
}

void Agent::setDesiredSpeed(float speed) {
    desiredSpeed = speed;
}

void Agent::setColour(float red, float green, float blue) {
    colour.set(red / 255, green / 255, blue / 255);
}

void Agent::setPosition(float x, float y) {
    position.set(x, y, 0.0);
}

void Agent::setDestination(float x, float y) {
    destination.set(x, y, 0.0);
}

void Agent::setPath(float x, float y, float radius) {
    path.push_back({Point3f(x, y, 0.0), radius});
}

Point3f Agent::getPath() {
    Vector3f distanceCurr, distanceNext;

    distanceCurr = path[0].position - position; // Distance to current waypoint

    if (path.size() > 2) {
        distanceNext = path[1].position - position; // Distance to next waypoint

        // Set Next Waypoint as Current Waypoint if Next Waypoint is Nearer
        if (distanceNext.lengthSquared() < distanceCurr.lengthSquared()) {
            path.push_back(path.front());
            path.pop_front();

            distanceCurr = distanceNext;
        }
    }

    // Move Front Point to Back if Within Radius
    if (distanceCurr.lengthSquared() < (path.front().radius * path.front().radius)) {
        path.push_back(path.front());
        path.pop_front();
    }

    return path.front().position;
}

float Agent::getOrientation() {
    return (atan2(velocity.y, velocity.x) * (180 / PI));
}

Point3f Agent::getAheadVector() const {
    return (velocity + position);
}

float Agent::getMinDistanceToWalls(vector<Wall *> walls, Point3f position, float radius) {
    Point3f nearestPoint;
    Vector3f vector;
    float distanceSquared, minDistanceSquared = INFINITY;

    for (Wall *wall: walls) {
        nearestPoint = wall->getNearestPoint(position);
        vector = position - nearestPoint; // Vector from wall to agent i
        distanceSquared = vector.lengthSquared();

        // Store Nearest Wall Distance
        if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
        }
    }

    return sqrt(minDistanceSquared) - radius; // Distance between wall and agent i
}

void Agent::move(vector<Agent *> agents, vector<Wall *> walls, vector<AGV *> agvs, float stepTime) {
    Vector3f acceleration;

    // Compute Social Force
    acceleration = AgentForce::drivingForce(getPath(), position, desiredSpeed, velocity) +
                   AgentForce::agentInteractForce(agents, id, position, velocity) +
                   AgentForce::wallInteractForce(walls, position, radius) +
                   AgentForce::agvInteractForce(agvs, position, velocity, radius);

    // Compute New Velocity
    velocity = velocity + acceleration * stepTime;

    // Truncate Velocity if Exceed Maximum Speed (Magnitude)
    if (velocity.lengthSquared() > (desiredSpeed * desiredSpeed)) {
        velocity.normalize();
        velocity *= desiredSpeed;
    }

    // Compute New Position
    position = position + velocity * stepTime;
}
