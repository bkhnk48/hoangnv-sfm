#include <random>
#include "AGV.h"
using namespace std;

const float PI = 3.14159265359F;

int AGV::crowdIdx = -1;

AGV::AGV()
{
    id = ++crowdIdx;

    dimension = Vector3f(0.5F, 0.5F, 0.0);

    colour.set(0.0, 0.0, 0.0);

    position.set(0.0, 0.0, 0.0);
    velocity.set(0.0, 0.0, 0.0);
}

AGV::~AGV()
{
    path.clear(); // Remove waypoints
    crowdIdx--;
}

void AGV::setDimension(Vector3f dimension)
{
    this->dimension = dimension;
}

void AGV::setColour(float red, float green, float blue)
{
    colour.set(red, green, blue);
}

void AGV::setPosition(float x, float y)
{
    position.set(x, y, 0.0);
}

void AGV::setPath(float x, float y, float radius)
{
    path.push_back({Point3f(x, y, 0.0), radius});
}

void AGV::setVelocity(float x, float y)
{
    velocity.set(x, y, 0.0);
}

Point3f AGV::getPath()
{
    Vector3f distanceCurr, distanceNext;

    distanceCurr = path[0].position - position; // Distance to current waypoint

    if (path.size() > 2)
    {
        distanceNext = path[1].position - position; // Distance to next waypoint

        // Set Next Waypoint as Current Waypoint if Next Waypoint is Nearer
        if (distanceNext.lengthSquared() < distanceCurr.lengthSquared())
        {
            path.push_back(path.front());
            path.pop_front();

            distanceCurr = distanceNext;
        }
    }

    // Move Front Point to Back if Within Radius
    if (distanceCurr.lengthSquared() < (path.front().radius * path.front().radius))
    {
        path.push_back(path.front());
        path.pop_front();
    }

    return path.front().position;
}

float AGV::getOrientation()
{
    return (atan2(velocity.y, velocity.x) * (180 / PI));
}

Point3f AGV::getAheadVector() const
{
    return (velocity + position);
}

void AGV::move(float stepTime)
{
    // Compute New Position
    position = position + velocity * stepTime;
}
