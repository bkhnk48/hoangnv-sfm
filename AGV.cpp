#include "AGV.h"
using namespace std;

const float PI = 3.14159265359F;

int AGV::agvIdx = -1;

AGV::AGV()
{
  id = ++agvIdx;

  dimension = Vector3f(0.5F, 1.5F, 0.0);
  distance = 0;

  colour.set(0.0, 0.0, 0.0);

  position.set(0.0, 0.0, 0.0);
  velocity.set(0.0, 0.0, 0.0);
  instantaneous_velocity.set(0.0, 0.0, 0.0);
}

AGV::~AGV() { agvIdx--; }

void AGV::setAcceleration(double acceleration)
{
  this->acceleration = acceleration;
}

void AGV::setDistance(float distance) { this->distance = distance; }

void AGV::setDimension(Vector3f dimension) { this->dimension = dimension; }

void AGV::setColour(float red, float green, float blue)
{
  colour.set(red, green, blue);
}

void AGV::setPosition(float x, float y) { position.set(x, y, 0.0); }

void AGV::setPath(float x, float y)
{
  path.push_back({Point3f(x, y, 0.0)});
}

void AGV::setTangentialVelocity(float v) { tangential_velocity = v; }

Point3f AGV::getPath()
{
  Vector3f distanceCurr, distanceNext;

  distanceCurr = path[0].position - position; // Distance to current Pathpoint

  if (path.size() > 2)
  {
    distanceNext = path[1].position - position; // Distance to next Pathpoint

    // Set Next Pathpoint as Current Pathpoint if Next Pathpoint is Nearer
    if (distanceNext.lengthSquared() < distanceCurr.lengthSquared())
    {
      path.push_back(path.front());
      path.pop_front();

      distanceCurr = distanceNext;
    }
  }

  // Move Front Point to Back if Within Radius
  if (distanceCurr.lengthSquared() < 0.5F)
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

Point3f AGV::getAheadVector() const { return (velocity + position); }

Point3f AGV::getNearestPoint(Point3f position_i) const
{
  vector<Vector3f> edges;
  float w, l;
  Vector3f top, bottom, a, b, d1, d2, d3, d4, e_ij;
  w = getDimension().x;
  l = getDimension().y;
  e_ij = path.front().position - getPosition();
  e_ij.normalize();
  top = getPosition() + e_ij * l * 0.5F;
  bottom = getPosition() - e_ij * l * 0.5F;

  a = Vector3f(e_ij.y, -e_ij.x, 0.0F);
  a.normalize();
  b = Vector3f(-e_ij.y, e_ij.x, 0.0F);
  b.normalize();

  d1 = top + a * w * 0.5F;
  d2 = top + b * w * 0.5F;
  d3 = bottom + b * w * 0.5F;
  d4 = bottom + a * w * 0.5F;
  edges.push_back(Vector3f(d1.x, d1.y, 0.0F));
  edges.push_back(Vector3f(d2.x, d2.y, 0.0F));
  edges.push_back(Vector3f(d3.x, d3.y, 0.0F));
  edges.push_back(Vector3f(d4.x, d4.y, 0.0F));
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
  Vector3f vector_acceleration, e_i;

  e_i = getPath() - position;
  e_i.normalize();

  velocity = e_i * tangential_velocity;
  vector_acceleration = e_i * acceleration;

  if (checkNearAgent(position_list))
  {
    if (instantaneous_velocity.length() > Vector3f(vector_acceleration * stepTime).length())
    {
      position = position + instantaneous_velocity * stepTime;
      instantaneous_velocity = instantaneous_velocity - vector_acceleration * stepTime;
    }
    else
    {
      instantaneous_velocity.set(0, 0, 0);
      position = position;
    }
  }
  else
  {
    if (instantaneous_velocity.length() < velocity.length())
    {
      position = position + instantaneous_velocity * stepTime;
      instantaneous_velocity = instantaneous_velocity + vector_acceleration * stepTime;
    }
    else
    {
      instantaneous_velocity = velocity;
      position = position + velocity * stepTime;
    }
  }
}
