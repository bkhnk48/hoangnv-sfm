#include "Utility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <iomanip>
#include <string>
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>

using namespace std;

Utility::Utility()
{
}

Utility::~Utility()
{
}

// random float number between particular range
float Utility::randomFloat(float lowerBound, float upperBound)
{
    return (lowerBound + (static_cast<float>(rand()) / RAND_MAX) * (upperBound - lowerBound));
}

// read map data file
// std::map<std::string, float[]> Utility::readMapData(const char * fileName) {
std::map<std::string, std::vector<float>> Utility::readMapData(const char *fileName)
{
    map<std::string, std::vector<float>> map;
    ifstream input(fileName);

    std::string delimiter = " ";

    int lineNo = 1;

    for (std::string line; getline(input, line);)
    {
        // cout << "Line: " << line << endl;
        vector<float> v;
        if (lineNo == 1)
        {
            v.push_back(stof(line));
            map["numI"] = v;
        }
        else if (lineNo == 2)
        {
            v.push_back(stof(line));
            map["walkwayWidth"] = v;
        }
        else
        {
            // cout << "Line: " <<line << endl;
            size_t pos = 0;
            std::string token;
            int count = 0;
            std::string juncId;
            while ((pos = line.find(delimiter)) != std::string::npos)
            {
                token = line.substr(0, pos);
                // std::cout << token << std::endl;
                if (count == 0)
                {
                    juncId.assign(token);
                }

                if (count > 1)
                {
                    v.push_back(stof(token));
                }
                line.erase(0, pos + delimiter.length());
                count++;
            }
            // cout << line << endl;
            v.push_back(stof(line));
            map[juncId] = v;
        }
        lineNo++;
    }

    return map;
}

// read input file
std::vector<double> Utility::readInput(const char *fileName)
{
    vector<double> v;
    ifstream input(fileName);

    std::string delimiter = " ";

    for (std::string line; getline(input, line);)
    {
        if (line.rfind("#", 0) == 0) // pos=0 limits the search to the prefix
        {
            continue;
        }
        // cout << "Line: " <<line << endl;
        size_t pos = 0;
        std::string token;
        while ((pos = line.find(delimiter)) != std::string::npos)
        {
            token = line.substr(0, pos);
            // std::cout << token << std::endl;
            v.push_back(stod(token));
            line.erase(0, pos + delimiter.length());
        }
        // cout << line << endl;
        v.push_back(stod(line));
    }

    return v;
}

// write end file
void Utility::writeEnd(const char *fileName, string name, int mode, std::vector<AGV *> data)
{
    vector<double> v;
    ofstream output(fileName, ios::app);

    std::string delimiter = " ";
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    output << "#" << name << " run completed on " << std::ctime(&now);

    for (AGV *agv : data)
    {
        string array1[] = {"Left", "Bottom", "Right", "Top"};
        string array2[] = {"Turn Right", "Go straight", "Turn Left"};
        string direction = array1[(int)(agv->getDirection().x)] + "-" + array2[(int)(agv->getDirection().y)];
        output << name << delimiter << mode << delimiter << direction << delimiter << convertTime(agv->getTotalTime()) << delimiter << agv->getCollision() << endl;
    }

    output.close();
}

// calculate number of people in each flow
std::vector<int> Utility::getNumPedesInFlow(int junctionType, int totalPedestrian)
{
    int numFlow;
    if (junctionType == 3)
    {
        numFlow = 6;
    }
    else if (junctionType == 4)
    {
        numFlow = 12;
    }

    std::vector<int> v(numFlow, 0);
    int j = 0;
    for (int i = 0; i < totalPedestrian; i++)
    {
        v[j] = v[j] + 1;
        j = j + 1;
        if (j == numFlow)
        {
            j = 0;
        }
    }
    return v;
}

// get list velocity of all pedestrians
std::vector<double> Utility::getPedesVelocity(int numPedes, double n_dist)
{
    vector<double> v;
    double std = sqrt(n_dist / (n_dist + 2));

    std::mt19937 gen(1701);

    std::student_t_distribution<> distr(n_dist);

    // std::cout << "min() == " << distr.min() << std::endl;
    // std::cout << "max() == " << distr.max() << std::endl;
    // std::cout << "n() == " << std::fixed << std::setw(11) << std::setprecision(10) << distr.n() << std::endl;

    // generate the distribution as a histogram
    std::map<double, int> histogram;
    for (int i = 0; i < numPedes; ++i)
    {
        ++histogram[distr(gen)];
    }

    // std::cout << "Distribution for " << numPedes << " samples:" << std::endl;
    int counter = 0;
    for (const auto &elem : histogram)
    {
        // std::cout << std::fixed << std::setw(11) << ++counter << ": "
        //           << std::setw(14) << std::setprecision(3) << elem.first << std::endl;
        double velocity = std * elem.first * 0.1 + Utility::MEAN;
        if (velocity > 1.8)
        {
            velocity = Utility::UPPER_SPEED_LIMIT;
        }
        else if (velocity < 0.6)
        {
            velocity = Utility::LOWER_SPEED_LIMIT;
        }

        v.push_back(velocity);
    }

    return v;
}

std::vector<float> Utility::getWallCoordinates(float wWidth, std::vector<float> juncData)
{
    std::vector<float> v;
    float temp = wWidth / 2;

    float leftWidthLimit = -juncData[0];
    float rightWidthLimit = juncData[2];

    float lowerHeightLimit = -juncData[1];
    float upperHeightLimit = juncData[3];

    // Upper Wall
    v.insert(v.end(), {leftWidthLimit, temp, -temp, temp});
    v.insert(v.end(), {temp, temp, rightWidthLimit, temp});
    // Lower Wall
    v.insert(v.end(), {leftWidthLimit, -temp, -temp, -temp});
    v.insert(v.end(), {temp, -temp, rightWidthLimit, -temp});
    // Left Wall
    v.insert(v.end(), {-temp, upperHeightLimit, -temp, temp});
    v.insert(v.end(), {-temp, -temp, -temp, lowerHeightLimit});
    // Right Wall
    v.insert(v.end(), {temp, upperHeightLimit, temp, temp});
    v.insert(v.end(), {temp, -temp, temp, lowerHeightLimit});

    return v;
}

// Convert miliseconds to pretty form
std::string Utility::convertTime(int ms)
{
    // 3600000 milliseconds in an hour
    long hr = ms / 3600000;
    ms = ms - 3600000 * hr;

    // 60000 milliseconds in a minute
    long min = ms / 60000;
    ms = ms - 60000 * min;

    // 1000 milliseconds in a second
    long sec = ms / 1000;
    ms = ms - 1000 * sec;

    return std::to_string(hr) + std::string("h ") + std::to_string(min) + std::string("m ") + std::to_string(sec) +
           std::string("s ") + std::to_string(ms) + std::string("ms");
}

// direction: 0 Left - Right, 1 Right - Left, 2 Top - Bottom, 3 Bottom - Top
// side: 0 Left side, 1 Center, 2 Right side
std::vector<float> Utility::getPedesDestination(int direction, int side, float walkwayWidth)
{
    std::vector<float> v;

    float radius = 3.0;
    switch (direction)
    {
    // Left - Right
    case 0:
        switch (side)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(Utility::WIDTH_LIMIT, Utility::WIDTH_LIMIT + 3),
                               Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3, walkwayWidth / 2),
                               radius});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(Utility::WIDTH_LIMIT, Utility::WIDTH_LIMIT + 3),
                               Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                                    walkwayWidth / 2 - walkwayWidth / 3),
                               radius});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(Utility::WIDTH_LIMIT, Utility::WIDTH_LIMIT + 3),
                               Utility::randomFloat(-walkwayWidth / 2, -walkwayWidth / 2 + walkwayWidth / 3),
                               radius});
            return v;
            break;
        default:
            break;
        }
        break;
        // Right - Left
    case 1:
        switch (side)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(-Utility::WIDTH_LIMIT - 3, -Utility::WIDTH_LIMIT),
                               Utility::randomFloat(-walkwayWidth / 2, -walkwayWidth / 2 + walkwayWidth / 3),
                               radius});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(-Utility::WIDTH_LIMIT - 3, -Utility::WIDTH_LIMIT),
                               Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                                    walkwayWidth / 2 - walkwayWidth / 3),
                               radius});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(-Utility::WIDTH_LIMIT - 3, -Utility::WIDTH_LIMIT),
                               Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3, walkwayWidth / 2),
                               radius});
            return v;
            break;
        default:
            break;
        }
        break;
        // Top - Bottom
    case 2:
        switch (side)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3, walkwayWidth / 2),
                               Utility::randomFloat(-Utility::HEIGHT_LIMIT - 3, -Utility::HEIGHT_LIMIT),
                               radius});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                                    walkwayWidth / 2 - walkwayWidth / 3),
                               Utility::randomFloat(-Utility::HEIGHT_LIMIT - 3, -Utility::HEIGHT_LIMIT),
                               radius});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(-walkwayWidth / 2, -walkwayWidth / 2 + walkwayWidth / 3),
                               Utility::randomFloat(-Utility::HEIGHT_LIMIT - 3, -Utility::HEIGHT_LIMIT),
                               radius});
            return v;
            break;
        default:
            break;
        }
        break;
        // Bottom - Top
    case 3:
        switch (side)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(-walkwayWidth / 2, -walkwayWidth / 2 + walkwayWidth / 3),
                               Utility::randomFloat(Utility::HEIGHT_LIMIT, Utility::HEIGHT_LIMIT + 3), radius});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                                    walkwayWidth / 2 - walkwayWidth / 3),
                               Utility::randomFloat(Utility::HEIGHT_LIMIT, Utility::HEIGHT_LIMIT + 3), radius});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3, walkwayWidth / 2),
                               Utility::randomFloat(Utility::HEIGHT_LIMIT, Utility::HEIGHT_LIMIT + 3), radius});
            return v;
            break;
        default:
            break;
        }
        break;
    default:
        return v;
        break;
    }
    return v;
}

// direction: 0 Left - Right, 1 Right - Left, 2 Top - Bottom, 3 Bottom - Top
std::vector<float> Utility::getPedesSource(int direction, float totalLength, float subLength, float caravanWidth)
{
    std::vector<float> v;
    float totalArea = totalLength * caravanWidth;
    float centerLength = totalLength - 2 * subLength;

    std::mt19937 gen(std::random_device{}());
    std::vector<double> chances{subLength * caravanWidth / totalArea,
                                centerLength * caravanWidth / totalArea,
                                subLength * caravanWidth / totalArea};
    // Initialize to same length.
    std::vector<int> choices(chances.size());
    choices = {0, 1, 2};
    // size_t is suitable for indexing.
    std::discrete_distribution<std::size_t> d{chances.begin(), chances.end()};

    int sampled_value = choices[d(gen)];
    // cout << "sampled_value " << sampled_value << endl;

    float horLandmark = Utility::WIDTH_LIMIT;
    float verLandmark = Utility::HEIGHT_LIMIT;
    if (totalLength > 40)
    {
        horLandmark = totalLength / 2;
        verLandmark = totalLength / 2;
    }
    // Calculate coordinates from Head to Tail of each Caravan
    switch (direction)
    {
    // From Left to Right
    case 0:
        switch (sampled_value)
        {
        case 0:
            v.insert(v.end(),
                     {Utility::randomFloat(-horLandmark + centerLength / 2, -horLandmark + totalLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(-horLandmark - centerLength / 2, -horLandmark + centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(-horLandmark - totalLength / 2, -horLandmark - centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        default:
            break;
        }
        break;

        // From Right to Left
    case 1:
        switch (sampled_value)
        {
        case 0:
            v.insert(v.end(),
                     {Utility::randomFloat(horLandmark - totalLength / 2, horLandmark - centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(horLandmark - centerLength / 2, horLandmark + centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(horLandmark + centerLength / 2, horLandmark + totalLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        default:
            break;
        }
        break;

        // From Top to Bottom
    case 2:
        switch (sampled_value)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(verLandmark - totalLength / 2,
                                                    verLandmark - centerLength / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(verLandmark - centerLength / 2,
                                                    verLandmark + centerLength / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(verLandmark + centerLength / 2,
                                                    verLandmark + totalLength / 2)});
            return v;
            break;
        default:
            break;
        }
        break;

        // From Bottom to Top
    case 3:
        switch (sampled_value)
        {
        case 0:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(-verLandmark + centerLength / 2,
                                                    -verLandmark + totalLength / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(-verLandmark - centerLength / 2,
                                                    -verLandmark + centerLength / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(), {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                               Utility::randomFloat(-verLandmark - totalLength / 2,
                                                    -verLandmark - centerLength / 2)});
            return v;
            break;
        default:
            break;
        }
        break;
    default:
        return v;
        break;
    }
    return v;
}

std::vector<float> Utility::getPedesColor(float maxSpeed, float minSpeed, float desiredSpeed)
{
    std::vector<float> v;
    float oneThirdVeloRange = (maxSpeed - minSpeed) / 3;

    if (desiredSpeed >= Utility::MEAN)
    {
        v.insert(v.end(), {0.0, 128.0, 0.0}); // Green
        return v;
    }
    else if (desiredSpeed < Utility::MEAN && desiredSpeed >= minSpeed + oneThirdVeloRange)
    {
        v.insert(v.end(), {0.0, 0.0, 0.0}); // Black
        return v;
    }
    else
    {
        v.insert(v.end(), {216.0, 32.0, 42.0}); // Red
        return v;
    }
}

float getCoor(float x, float verAsymtote, float horAsymtote)
{
    return horAsymtote * x / (x - verAsymtote);
}

std::vector<Point3f> Utility::getRouteAGV(int junctionType, int src, int turningDirection, float walkwayWidth, std::vector<float> juncData)
{
    std::vector<Point3f> v;
    if (junctionType == 3)
    {
        v = getRouteAGVTJunction(src, turningDirection, walkwayWidth, juncData);
    }
    else if (junctionType == 4)
    {
        v = getRouteAGVCrossRoad(src, turningDirection, walkwayWidth, juncData);
    }
    return v;
}

// src = {0, 1, 2} ~ Go from Left, Bottom, Right side
// turningDirection = {0, 1, 2} - Turn Left, Go Straight, Turn Right
std::vector<Point3f> Utility::getRouteAGVTJunction(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData)
{
    std::vector<Point3f> v;

    float horWalkwayWidth = walkwayWidth;
    float verWalkwayWidth = walkwayWidth;

    float posVerAsymtote = verWalkwayWidth / 2;
    float negVerAsymtote = -verWalkwayWidth / 2;

    float posHorAsymtote = horWalkwayWidth / 2;
    float negHorAsymtote = -horWalkwayWidth / 2;

    float leftWidthLimit = -juncData[0];
    float rightWidthLimit = juncData[2];

    float lowerHeightLimit = -juncData[1];

    switch (src)
    {
        // Go From Left side
    case 0:
    {
        switch (turningDirection)
        {
        case 0:
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(-verWalkwayWidth / 5, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                               Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 2, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
    // Go From Bottom side
    case 1:
    {
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = verWalkwayWidth / 3; x >= leftWidthLimit - 1; x--)
            {
                float y = getCoor(x, posVerAsymtote, posHorAsymtote);
                if (y > horWalkwayWidth / 3)
                {
                    y = horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.front();
            if (checker.y > lowerHeightLimit)
            {
                v.insert(v.begin(), Point3f(checker.x, lowerHeightLimit - 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                               Point3f(verWalkwayWidth / 3, -horWalkwayWidth / 5, 0.0)});
            v.insert(v.end(), {Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
    // Go from Right side
    case 2:
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = rightWidthLimit + 1; x >= -verWalkwayWidth / 3; x--)
            {
                float y = getCoor(x, negVerAsymtote, posHorAsymtote);
                if (y > horWalkwayWidth / 3)
                {
                    y = horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.back();
            if (checker.y > lowerHeightLimit)
            {
                v.push_back(Point3f(checker.x, lowerHeightLimit - 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                               Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(leftWidthLimit - 2, horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }

    return v;
}

// src = {0, 1, 2, 3} ~ Go from Left, Bottom, Right, Top side
// turningDirection = {0, 1, 2} - Turn Left, Go Straight, Turn Right
std::vector<Point3f> Utility::getRouteAGVCrossRoad(int src, int turningDirection, float walkwayWidth, std::vector<float> juncData)
{
    float horWalkwayWidth = walkwayWidth;
    float verWalkwayWidth = walkwayWidth;

    float posVerAsymtote = verWalkwayWidth / 2;
    float negVerAsymtote = -verWalkwayWidth / 2;

    float posHorAsymtote = horWalkwayWidth / 2;
    float negHorAsymtote = -horWalkwayWidth / 2;

    float leftWidthLimit = -juncData[0];
    float rightWidthLimit = juncData[2];

    float lowerHeightLimit = -juncData[1];
    float upperHeightLimit = juncData[3];

    std::vector<Point3f> v;
    switch (src)
    {
    // Go from Left side
    case 0:
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = leftWidthLimit - 1; x <= verWalkwayWidth / 3; x++)
            {
                float y = getCoor(x, posVerAsymtote, negHorAsymtote);
                if (y < -horWalkwayWidth / 3)
                {
                    y = -horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.back();
            if (checker.y < upperHeightLimit)
            {
                v.push_back(Point3f(checker.x, upperHeightLimit + 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(-verWalkwayWidth / 5, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                               Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 2, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;

        // Go from Bottom side
    case 1:
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = verWalkwayWidth / 3; x >= leftWidthLimit - 1; x--)
            {
                float y = getCoor(x, posVerAsymtote, posHorAsymtote);
                if (y > horWalkwayWidth / 3)
                {
                    y = horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.front();
            if (checker.y > lowerHeightLimit)
            {
                v.insert(v.begin(), Point3f(checker.x, lowerHeightLimit - 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                               Point3f(verWalkwayWidth / 3, upperHeightLimit + 1, 0.0)});
            v.insert(v.end(), {Point3f(verWalkwayWidth / 3, upperHeightLimit + 2, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                               Point3f(verWalkwayWidth / 3, -horWalkwayWidth / 5, 0.0)});
            v.insert(v.end(), {Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0),
                               Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;

        // Go from Right side
    case 2:
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = rightWidthLimit + 1; x >= -verWalkwayWidth / 3; x--)
            {
                float y = getCoor(x, negVerAsymtote, posHorAsymtote);
                if (y > horWalkwayWidth / 3)
                {
                    y = horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.back();
            if (checker.y > lowerHeightLimit)
            {
                v.push_back(Point3f(checker.x, lowerHeightLimit - 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                               Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(leftWidthLimit - 2, horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                               Point3f(verWalkwayWidth / 5, horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(), {Point3f(verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
                               Point3f(verWalkwayWidth / 3, upperHeightLimit + 2, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;

        // Go from Top side
    case 3:
        switch (turningDirection)
        {
        // Turn Left
        case 0:
        {
            for (int x = -verWalkwayWidth / 3; x <= rightWidthLimit + 1; x++)
            {
                float y = getCoor(x, negVerAsymtote, negHorAsymtote);
                if (y < -horWalkwayWidth / 3)
                {
                    y = -horWalkwayWidth / 3;
                }
                v.push_back(Point3f(x, y, 0.0));
            }
            Point3f checker = v.front();
            if (checker.y < upperHeightLimit)
            {
                v.insert(v.begin(), Point3f(checker.x, upperHeightLimit + 1, 0.0));
            }
            return v;
            break;
        }
            // Go Straight
        case 1:
        {
            v.insert(v.end(), {Point3f(-verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
                               Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0)});
            v.insert(v.end(), {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 2, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(), {Point3f(-verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
                               Point3f(-verWalkwayWidth / 3, horWalkwayWidth / 5, 0.0)});
            v.insert(v.end(), {Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0),
                               Point3f(leftWidthLimit - 2, horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }
    return v;
}

Point3f Utility::getIntermediateDes(Point3f position, float verWalkwayWidth, float horWalkwayWidth)
{
    if (position.x > 0 && position.y > 0)
    {
        return Point3f(verWalkwayWidth / 3, horWalkwayWidth / 3, 0.0);
    }
    else if (position.x < 0 && position.y > 0)
    {
        return Point3f(-verWalkwayWidth / 3, horWalkwayWidth / 3, 0.0);
    }
    else if (position.x < 0 && position.y < 0)
    {
        return Point3f(-verWalkwayWidth / 3, -horWalkwayWidth / 3, 0.0);
    }
    else
    {
        return Point3f(verWalkwayWidth / 3, -horWalkwayWidth / 3, 0.0);
    }
}

int Utility::randomInt(int from, int to)
{
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(from, to);
    return distr(gen);
}
