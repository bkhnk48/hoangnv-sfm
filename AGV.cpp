#include "AGV.h"

using namespace std;

int AGV::agvIdx = -1;

AGV::AGV()
{
    MovingObject();
    id = ++agvIdx;

    totalTime = 0;
    collision = 0;
    dimension = Vector3f(0.5F, 1.5F, 0.0);
    distance = 0;
    isRunning = false;
    stop = false;

    instantaneous_velocity.set(0.0, 0.0, 0.0);
}

AGV::~AGV()
{
    agvIdx--;
}

void AGV::setTotalTime(int totalTime) { this->totalTime = totalTime; }

void AGV::setCollision(int collision) { this->collision = collision; }

void AGV::setAcceleration(float acceleration)
{
    this->acceleration = acceleration;
}

void AGV::setDistance(float distance) { this->distance = distance; }

void AGV::setDimension(Vector3f dimension) { this->dimension = dimension; }

void AGV::setBorderPoint(Vector3f d1, Vector3f d2, Vector3f d3, Vector3f d4)
{
    this->d1 = d1;
    this->d2 = d2;
    this->d3 = d3;
    this->d4 = d4;
}

void AGV::setIsRunning(bool isRunning) { this->isRunning = isRunning; }

void AGV::setDirection(float x, float y) { this->direction.set(x, y, 0.0F); }

Point3f AGV::getNearestPoint(Point3f position_i) const
{
    vector<Vector3f> edges;
    edges.push_back(d1);
    edges.push_back(d2);
    edges.push_back(d3);
    edges.push_back(d4);
    Vector3f relativeEnd, relativePos, relativeEndScal, relativePosScal;
    float dotProduct;
    vector<Point3f> nearestPoint;
    Point3f point;
    for (int i = 0; i < edges.size(); i++)
    {
        if (i < edges.size() - 1)
        {
            relativeEnd = edges[i + 1] - edges[i];
        }
        else
        {
            relativeEnd = edges[0] - edges[i];
        }

        relativePos = position_i - edges[i];

        relativeEndScal = relativeEnd;
        relativeEndScal.normalize();

        relativePosScal = relativePos * (1.0F / relativeEnd.length());

        dotProduct = relativeEndScal.dot(relativePosScal);

        if (dotProduct < 0.0)
            nearestPoint.push_back(edges[i]);
        else if (dotProduct > 1.0)
        {
            if (i < edges.size() - 1)
            {
                nearestPoint.push_back(edges[i + 1]);
            }
            else
            {
                nearestPoint.push_back(edges[0]);
            }
        }
        else
            nearestPoint.push_back((relativeEnd * dotProduct) + edges[i]);
    }

    nearestPoint.push_back(position);

    point = nearestPoint[0];

    for (int i = 0; i < nearestPoint.size(); i++)
    {
        if (position_i.distance(nearestPoint[i]) < position_i.distance(point))
        {
            point = nearestPoint[i];
        }
    }

    return point;
}

bool AGV::checkNearAgent(vector<Point3f> position_list)
{
    for (Point3f p : position_list)
    {
        if (getNearestPoint(p).distance(p) < distance)
        {
            return true;
        }
    }
    return false;
}

void AGV::move(float stepTime, vector<Point3f> position_list)
{
    Vector3f vector_acceleration, e_ij;

    e_ij = getPath() - position;
    e_ij.normalize();

    velocity = e_ij * desiredSpeed;
    vector_acceleration = e_ij * acceleration;
    vector_acceleration *= stepTime;

    if (getDestination().distance(position) < 1.0F)
    {
        position = getDestination();
    }
    else
    {
        if (checkNearAgent(position_list))
        {
            if (!stop)
            {
                collision++;
            }
            stop = true;
            if (abs(instantaneous_velocity.x) >= abs(vector_acceleration.x) &&
                abs(instantaneous_velocity.y) >= abs(vector_acceleration.y))
            {
                position = position + instantaneous_velocity * stepTime;
                instantaneous_velocity = instantaneous_velocity - vector_acceleration;
            }
            else
            {
                instantaneous_velocity.set(0, 0, 0);
            }
        }
        else
        {
            stop = false;
            if (instantaneous_velocity.length() < velocity.length())
            {
                position = position + instantaneous_velocity * stepTime;
                instantaneous_velocity = instantaneous_velocity + vector_acceleration;
            }
            else
            {
                instantaneous_velocity = velocity;
                position = position + velocity * stepTime;
            }
        }
    }
}
