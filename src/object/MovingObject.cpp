#include "MovingObject.h"

const float PI = 3.14159265359F;

MovingObject::MovingObject() {
    color.set(0.0, 0.0, 0.0);
    position.set(0.0, 0.0, 0.0);
    velocity.set(0.0, 0.0, 0.0);
}

MovingObject::~MovingObject() { }

void MovingObject::setColor(float red, float green, float blue) {
    color.set(red / 255, green / 255, blue / 255);
}

void MovingObject::setPosition(float x, float y) {
    position.set(x, y, 0.0);
}

void MovingObject::setPath(float x, float y, float radius) {
    path.push_back({Point3f(x, y, 0.0), radius});
}

void MovingObject::setDestination(float x, float y) {
    destination.set(x, y, 0.0);
}

void MovingObject::setDesiredSpeed(float speed)
{
    desiredSpeed = speed;
}

void MovingObject::setIsMoving(bool isMoving)
{
    this->isMoving = isMoving;
}

Point3f MovingObject::getPath() {
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

Point3f MovingObject::getAheadVector() const {
    return (velocity + position);
}

float MovingObject::getOrientation() {
    return (atan2(velocity.y, velocity.x) * (180 / PI));
}
