#include <random>
#include "Agent.h"

using namespace std;

int Agent::crowdIdx = -1;
default_random_engine generator;

Agent::Agent()
{
    MovingObject();
    id = ++crowdIdx;

    radius = 0.2F;
    isMoving = true;
    stopAtCorridor = false;
    impatient = 0;

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

void Agent::setImpatient(float impatient)
{
    this->impatient = impatient / (1 / exp(-5)) > 1 ? 1 : impatient / (1 / exp(-5));
}

void Agent::setStopAtCorridor(bool stopAtCorridor)
{
    this->stopAtCorridor = stopAtCorridor;
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
    acceleration = getDrivingForce(getPath()) +
                   getAgentInteractForce(agents) +
                   getWallInteractForce(walls) +
                   getAgvInteractForce(agvs);

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

Vector3f Agent::getDrivingForce(const Point3f destination)
{
    const float T = 0.54F; // Relaxation time based on (Moussaid et al., 2009)
    Vector3f e_i, f_i;

    // Compute Desired Direction
    // Formula: e_i = (destination - position_i) / ||(destination - position_i)||
    e_i = destination - position;
    e_i.normalize();

    // Compute Driving Force
    // Formula: f_i = ((desiredSpeed * e_i) - velocity_i) / T
    f_i = ((desiredSpeed * e_i) - velocity) * (1 / T);

    return f_i;
};

Vector3f Agent::getAgentInteractForce(vector<Agent *> agents)
{
    // Constant Values Based on (Moussaid et al., 2009)
    const float lambda = 2.0;  // Weight reflecting relative importance of velocity vector against position vector
    const float gamma = 0.35F; // Speed interaction
    const float n_prime = 3.0; // Angular interaction
    const float n = 2.0;       // Angular intaraction
    const float A = 4.5;       // Modal parameter A

    Vector3f distance_ij, e_ij, D_ij, t_ij, n_ij, f_ij;
    float B, theta, f_v, f_theta;
    int K;
    int numOfAgents = 0;

    f_ij.set(0.0, 0.0, 0.0);

    for (const Agent *agent_j : agents)
    {
        // Do Not Compute Interaction Force to Itself
        if (agent_j->getId() != id)
        {
            // Compute Distance Between Agent j and i
            distance_ij = agent_j->getPosition() - position;
            agent_j->getPosition();

            // Skip Computation if Agents i and j are Too Far Away
            if (distance_ij.lengthSquared() > (2.0 * 2.0))
                continue;

            numOfAgents++;

            // Compute Direction of Agent j from i
            // Formula: e_ij = (position_j - position_i) / ||position_j - position_i||
            e_ij = distance_ij;
            e_ij.normalize();

            // Compute Interaction Vector Between Agent i and j
            // Formula: D = lambda * (velocity_i - velocity_j) + e_ij
            D_ij = lambda * (velocity - agent_j->getVelocity()) + e_ij;

            // Compute Modal Parameter B
            // Formula: B = gamma * ||D_ij||
            B = gamma * D_ij.length();

            // Compute Interaction Direction
            // Formula: t_ij = D_ij / ||D_ij||
            t_ij = D_ij;
            t_ij.normalize();

            // Compute Angle Between Interaction Direction (t_ij) and Vector Pointing from Agent i to j (e_ij)
            theta = t_ij.angle(e_ij);

            // Compute Sign of Angle 'theta'
            // Formula: K = theta / |theta|
            K = (theta == 0) ? 0 : static_cast<int>(theta / abs(theta));

            // Compute Amount of Deceleration
            // Formula: f_v = -A * exp(-distance_ij / B - ((n_prime * B * theta) * (n_prime * B * theta)))
            f_v = -A * exp(-distance_ij.length() / B - ((n_prime * B * theta) * (n_prime * B * theta)));

            // Compute Amount of Directional Changes
            // Formula: f_theta = -A * K * exp(-distance_ij / B - ((n * B * theta) * (n * B * theta)))
            f_theta = -A * K * exp(-distance_ij.length() / B - ((n * B * theta) * (n * B * theta)));

            // Compute Normal Vector of Interaction Direction Oriented to the Left
            n_ij.set(-t_ij.y, t_ij.x, 0.0);

            // Compute Interaction Force
            // Formula: f_ij = f_v * t_ij + f_theta * n_ij
            f_ij += f_v * t_ij + f_theta * n_ij;
        }
    }

    setImpatient(1 / exp(-numOfAgents / (3.14159265359F * 4)));

    return f_ij;
}

Vector3f Agent::getWallInteractForce(vector<Wall *> walls)
{
    // const float repulsionRange = 0.3F;	// Repulsion range based on (Moussaid et al., 2009)
    const int a = 3;
    const float b = 0.1F;

    Point3f nearestPoint;
    Vector3f vector_wi, minVector_wi;
    float distanceSquared, minDistanceSquared = INFINITY, d_w, f_iw;

    for (Wall *wall : walls)
    {
        nearestPoint = wall->getNearestPoint(position);
        vector_wi = position - nearestPoint; // Vector from wall to agent i
        distanceSquared = vector_wi.lengthSquared();

        // Store Nearest Wall Distance
        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
            minVector_wi = vector_wi;
        }
    }

    d_w = sqrt(minDistanceSquared) - radius; // Distance between wall and agent i

    // Compute Interaction Force
    // Formula: f_iw = a * exp(-d_w / b)
    f_iw = a * exp(-d_w / b);
    minVector_wi.normalize();

    return f_iw * minVector_wi;
}

Vector3f Agent::getAgvInteractForce(vector<AGV *> agvs)
{
    // Constant Values Based on (Moussaid et al., 2009)
    const float lambda = 2.0; // Weight reflecting relative importance of velocity
    // vector against position vector
    const float gamma = 0.35F; // Speed interaction
    const float n_prime = 3.0; // Angular interaction
    const float n = 2.0;       // Angular intaraction
    const float A = 4.5;       // Modal parameter A

    Vector3f distance_ij, e_ij, D_ij, t_ij, n_ij, f_ij;
    float B, theta, f_v, f_theta;
    int K;

    f_ij.set(0.0, 0.0, 0.0);

    for (AGV *agv : agvs)
    {
        if (agv->getIsMoving())
        {
            // Compute Distance Between Agent j and i
            distance_ij = agv->getNearestPoint(position) - position;

            // Skip Computation if Agents i and j are Too Far Away
            if (distance_ij.lengthSquared() > (2.0 * 2.0))
                continue;

            if (distance_ij.length() < 0.3F)
            {
                const int a = 3;
                const float b = 0.1F;

                Point3f nearestPoint;
                Vector3f vector_wi, minVector_wi;
                float distanceSquared, minDistanceSquared = INFINITY, d_w, f_iw;

                nearestPoint = agv->getNearestPoint(position);
                vector_wi = position - nearestPoint;
                distanceSquared = vector_wi.lengthSquared();

                if (distanceSquared < minDistanceSquared)
                {
                    minDistanceSquared = distanceSquared;
                    minVector_wi = vector_wi;
                }

                d_w = sqrt(minDistanceSquared) - radius;

                f_iw = a * exp(-d_w / b);
                minVector_wi.normalize();

                f_ij += f_iw * minVector_wi;
            }
            else
            {
                // Compute Direction of Agent j from i
                // Formula: e_ij = (position_j - position_i) / ||position_j - position_i||
                e_ij = distance_ij;
                e_ij.normalize();

                // Compute Interaction Vector Between Agent i and j
                // Formula: D = lambda * (velocity_i - velocity_j) + e_ij
                D_ij = lambda * (velocity - agv->getVelocity()) + e_ij;

                // Compute Modal Parameter B
                // Formula: B = gamma * ||D_ij||
                B = gamma * D_ij.length();

                // Compute Interaction Direction
                // Formula: t_ij = D_ij / ||D_ij||
                t_ij = D_ij;
                t_ij.normalize();

                // Compute Angle Between Interaction Direction (t_ij) and Vector Pointing
                // from Agent i to j (e_ij)
                theta = t_ij.angle(e_ij);

                // Compute Sign of Angle 'theta'
                // Formula: K = theta / |theta|
                K = (theta == 0) ? 0 : static_cast<int>(theta / abs(theta));

                // Compute Amount of Deceleration
                // Formula: f_v = -A * exp(-distance_ij / B - ((n_prime * B * theta) *
                // (n_prime * B * theta)))
                f_v = -A * exp(-distance_ij.length() / B -
                               ((n_prime * B * theta) * (n_prime * B * theta)));

                // Compute Amount of Directional Changes
                // Formula: f_theta = -A * K * exp(-distance_ij / B - ((n * B * theta) * (n
                // * B * theta)))
                f_theta =
                    -A * K *
                    exp(-distance_ij.length() / B - ((n * B * theta) * (n * B * theta)));

                // Compute Normal Vector of Interaction Direction Oriented to the Left
                n_ij.set(-t_ij.y, t_ij.x, 0.0);

                // Compute Interaction Force
                // Formula: f_ij = f_v * t_ij + f_theta * n_ij
                f_ij += (f_v * t_ij + f_theta * n_ij) * exp(-getImpatient());
            }
            break;
        }
    }

    return f_ij;
}
