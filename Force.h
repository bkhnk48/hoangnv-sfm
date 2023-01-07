#ifndef FORCE_H
#define FORCE_H

#include "vecmath.h"

class Force
{
public:
    virtual Vector3f getDrivingForce() { return Vector3f(0, 0, 0); };       // Computes f_i
    virtual Vector3f getAgentInteractForce() { return Vector3f(0, 0, 0); }; // Computes f_ij
    virtual Vector3f getWallInteractForce() { return Vector3f(0, 0, 0); };  // Computes f_iw
    virtual Vector3f getAgvInteractForce() { return Vector3f(0, 0, 0); };
};

#endif
