#include "AGV.h"
using namespace std;

const float PI = 3.14159265359F;

int AGV::agvIdx = -1;

AGV::AGV() {
  id = ++agvIdx;

  dimension = Vector3f(1.0F, 1.0F, 0.0);
  distance = 0;

  colour.set(0.0, 0.0, 0.0);

  position.set(0.0, 0.0, 0.0);
  velocity.set(0.0, 0.0, 0.0);
  instantaneous_velocity.set(0.0, 0.0, 0.0);
}

AGV::~AGV() { agvIdx--; }

void AGV::setAcceleration(double acceleration) {
  this->acceleration = acceleration;
}

void AGV::setDistance(float distance) { this->distance = distance; }

void AGV::setDimension(Vector3f dimension) { this->dimension = dimension; }

void AGV::setColour(float red, float green, float blue) {
  colour.set(red, green, blue);
}

void AGV::setPosition(float x, float y) { position.set(x, y, 0.0); }

void AGV::setVelocity(float x, float y) { velocity.set(x, y, 0.0); }

float AGV::getOrientation() {
  return (atan2(velocity.y, velocity.x) * (180 / PI));
}

Point3f AGV::getAheadVector() const { return (velocity + position); }

Point3f AGV::getNearestPoint(Point3f position_i) const {
  vector<Vector3f> edges;
  float x, y, w, h;
  x = position.x;
  y = position.y;
  w = dimension.x;
  h = dimension.y;
  edges.push_back(Vector3f(x - w / 2, y + h / 2, 0.0F));
  edges.push_back(Vector3f(x + w / 2, y + h / 2, 0.0F));
  edges.push_back(Vector3f(x + w / 2, y - h / 2, 0.0F));
  edges.push_back(Vector3f(x - w / 2, y - h / 2, 0.0F));
  Vector3f relativeEnd, relativePos, relativeEndScal, relativePosScal;
  float dotProduct;
  vector<Point3f> nearestPoint;
  Point3f point;
  for (int i = 0; i < edges.size(); i++) {
    if (i < edges.size() - 1) {
      relativeEnd = edges[i] - edges[i + 1];
    } else {
      relativeEnd = edges[i] - edges[0];
    }

    relativePos = position_i - edges[i];

    relativeEndScal = relativeEnd;
    relativeEndScal.normalize();

    relativePosScal = relativePos * (1.0F / relativeEnd.length());

    dotProduct = relativeEndScal.dot(relativePosScal);

    if (dotProduct < 0.0)
      nearestPoint.push_back(edges[i]);
    else if (dotProduct > 1.0) {
      if (i < edges.size() - 1) {
        nearestPoint.push_back(edges[i + 1]);
      } else {
        nearestPoint.push_back(edges[0]);
      }
    } else
      nearestPoint.push_back((relativeEnd * dotProduct) + edges[i]);
  }

  point = nearestPoint[0];

  for (int i = 0; i < nearestPoint.size(); i++) {
    if (position_i.distance(nearestPoint[i]) < position_i.distance(point)) {
      point = nearestPoint[i];
    }
  }

  return point;
}

bool AGV::checkNearAgent(vector<Point3f> position_list) {
  for (Point3f p : position_list) {
    if (getNearestPoint(p).distance(p) < distance) {
      return true;
    }
  }
  return false;
}

void AGV::move(float stepTime, vector<Point3f> position_list) {
  Vector3f vector_acceleration;
  if (velocity.x == 0 || velocity.y == 0) {
    if (velocity.x == 0) {
      vector_acceleration.set(0, acceleration, 0);
    } else if (velocity.y == 0) {
      vector_acceleration.set(acceleration, 0, 0);
    } else {
      vector_acceleration.set(0, 0, 0);
    }
  } else {
    double angle = velocity.angle(Vector3f(0.5F, 0, 0));
    if (angle > PI / 2) {
      angle = PI - angle;
    }

    double x, y;
    x = sqrt(acceleration * acceleration / (1 + tan(angle)));
    y = abs(tan(angle) * x);

    vector_acceleration.set(x, y, 0);

    if (velocity.x < 0)
      vector_acceleration.x = -x;
    if (velocity.y < 0)
      vector_acceleration.y = -y;
  }

  if (checkNearAgent(position_list)) {
    if (instantaneous_velocity.length() > 0) {
      position = position + instantaneous_velocity * stepTime -
                 vector_acceleration * stepTime * stepTime * 0.5F;
      instantaneous_velocity =
          instantaneous_velocity - vector_acceleration * stepTime;
    } else {
      position = position;
    }
  } else {
    if (instantaneous_velocity.length() < velocity.length()) {
      position = position + instantaneous_velocity * stepTime +
                 vector_acceleration * stepTime * stepTime * 0.5F;
      instantaneous_velocity =
          instantaneous_velocity + vector_acceleration * stepTime;
    } else {
      position = position + velocity * stepTime;
    }
  }
}
