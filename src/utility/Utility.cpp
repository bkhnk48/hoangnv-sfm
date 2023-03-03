#include "Utility.h"

#include <bits/stdc++.h>

#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "src/constant/Constant.h"

using namespace std;
using namespace Constant;
using namespace Utility;

// random float number between particular range
float Utility::randomFloat(float lowerBound, float upperBound)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lowerBound, upperBound);
    return dis(gen);
}

// read map data file
std::map<std::string, std::vector<float>> Utility::readMapData(
    const char *fileName)
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

std::vector<json> Utility::convertMapData(
    std::map<std::string, std::vector<float>> mapData)
{
    std::vector<json> data;
    for (auto elem : mapData)
    {
        if (elem.first == "numI" || elem.first == "walkwayWidth")
        {
            continue;
        }
        int numOfHallway = elem.second.size();
        for (int i = 0; i < numOfHallway; i++)
        {
            if (!hallwaySameCharExists(elem.second[i], data))
            {
                json temp;
                temp[elem.first + "_" + std::to_string(i)] = elem.second[i];
                data.push_back(temp);
            }
        }
    }
    return data;
}

bool Utility::hallwaySameCharExists(float hallwayLength,
                                    std::vector<json> data)
{
    for (auto elem : data)
    {
        if (fabs(hallwayLength - (float)(elem.items().begin().value())) < 0.1)
        {
            return true;
        }
    }
    return false;
}

// read input file
json Utility::readInputData(const char *fileName)
{
    std::ifstream f(fileName);
    json data = json::parse(f);

    return data;
}

// write end file
void Utility::writeResult(const char *fileName, string name, int mode,
                          std::vector<AGV *> agvs,
                          std::vector<json> juncDataList,
                          int agvRunConcurrently, int runMode,
                          int numRunPerHallway, int totalRunningTime)
{
    ofstream output(fileName, ios::app);

    int easyReadingMode = 1;
    std::string delimiter = " - ";
    std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (runMode == 0)
    {
        output << "#" << name << " run completed on " << std::ctime(&now);

        for (AGV *agv : agvs)
        {
            string array1[] = {"From Left", "From Bottom", "From Right", "From Top"};
            string array2[] = {"Turn Right", "Go straight", "Turn Left"};
            string direction = array1[(int)(agv->getDirection().x)] + "-" +
                               array2[(int)(agv->getDirection().y)];
            output << name << delimiter << mode << delimiter << direction << delimiter
                   << convertTime(agv->getTravelingTime()) << delimiter
                   << agv->getNumOfCollision() << endl;
        }
    }
    else
    {
        if (easyReadingMode == 1)
        {
            output << "\n\t*#* Completed on " << std::ctime(&now);
        }

        string hallwayName;
        float hallwayLength;
        vector<int> travelingTimeList;

        int juncIndexTemp = 0;

        for (AGV *agv : agvs)
        {
            if (agv->getNumOfCollision() == 0)
            {
                agv->setTotalStopTime(0);
            }
            int marker = numRunPerHallway * (juncIndexTemp + 1) - 1;
            if (agvRunConcurrently == 1)
            {
                marker = numRunPerHallway * 2 * (juncIndexTemp + 1) - 1;
            }
            travelingTimeList.push_back(agv->getTravelingTime());

            if (juncIndexTemp < juncDataList.size())
            {
                hallwayName = juncDataList[juncIndexTemp].items().begin().key();
                hallwayLength = juncDataList[juncIndexTemp].items().begin().value();

                if (easyReadingMode == 1)
                {
                    output << hallwayName << delimiter << hallwayLength << ": AGV ID "
                           << agv->getId() << delimiter
                           << convertTime(agv->getTravelingTime()) << delimiter
                           << "Collisions " << agv->getNumOfCollision() << delimiter
                           << "Total stop time " << convertTime(agv->getTotalStopTime()) << endl;
                }
            }

            if (agv->getId() == marker)
            {
                int minValue = *std::min_element(travelingTimeList.begin(),
                                                 travelingTimeList.end());
                int maxValue = *std::max_element(travelingTimeList.begin(),
                                                 travelingTimeList.end());

                int sum = std::accumulate(travelingTimeList.begin(),
                                          travelingTimeList.end(), 0);
                double avgTime = static_cast<double>(sum) / travelingTimeList.size();

                if (easyReadingMode == 1)
                {
                    output << "Shortest: " << convertTime(minValue)
                           << " - Longest: " << convertTime(maxValue) << endl;
                    output << "Average time to travel through the hallway " << hallwayName
                           << " is " << convertTime((int)avgTime) << "\n"
                           << endl;
                } else {
                    output << hallwayLength << " " << minValue << " " << maxValue << " " << avgTime << endl;
                }

                travelingTimeList.clear();
                juncIndexTemp = juncIndexTemp + 1;
            }
        }
    }

    if (easyReadingMode == 1)
    {
        output << "\t==> Total running time:  " << convertTime(totalRunningTime) << endl;
    }

    output.close();
}

// calculate number of people in each flow
std::vector<int> Utility::getNumPedesInFlow(int junctionType,
                                            int totalPedestrian)
{
    int numFlow = 0;
    if (junctionType == 2)
    {
        numFlow = 6;
    }
    else if (junctionType == 3)
    {
        numFlow = 18;
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

// get list velocity of all pedestrians: type 0 - Discrete distribution, type 1
// - T distribution
std::vector<double> Utility::getPedesVelocity(int type, json inputData,
                                              float deviationParam)
{
    if (type == 0)
    {
        return getPedesVelocityBasedDDis(inputData, deviationParam);
    }
    else
    {
        return getPedesVelocityBasedTDis(int(inputData["numOfAgents"]["value"]),
                                         inputData["TDDegree"]["value"]);
    }
}

std::vector<double> Utility::getPedesVelocityBasedDDis(json inputData,
                                                       float deviationParam)
{
    vector<double> v;
    float perNoDisabilityWithoutOvertaking =
        float(inputData["p1"]["value"]) * deviationParam;
    float perNoDisabilityWithOvertaking =
        float(inputData["p2"]["value"]) * deviationParam;
    float perWalkingWithCrutches =
        float(inputData["p3"]["value"]) * deviationParam;
    float perWalkingWithSticks = float(inputData["p4"]["value"]) * deviationParam;
    float perWheelchairs = float(inputData["p5"]["value"]) * deviationParam;
    // float perTheBlind = inputData["p6"]["value"];
    float perTheBlind =
        100 - (perNoDisabilityWithoutOvertaking + perNoDisabilityWithOvertaking +
               perWalkingWithCrutches + perWalkingWithSticks + perWheelchairs);

    const int nrolls = 10000; // number of experiments
    const int numPedes =
        int(int(inputData["numOfAgents"]["value"]) *
            deviationParam); // maximum number of pedes to distribute

    std::default_random_engine generator;
    std::discrete_distribution<int> distribution{perNoDisabilityWithoutOvertaking,
                                                 perNoDisabilityWithOvertaking,
                                                 perWalkingWithCrutches,
                                                 perWalkingWithSticks,
                                                 perWheelchairs,
                                                 perTheBlind};

    int p[6] = {};

    for (int i = 0; i < nrolls; ++i)
    {
        int number = distribution(generator);
        ++p[number];
    }

    std::map<int, float> map;
    map[0] = V1;
    map[1] = V2;
    map[2] = V3;
    map[3] = V4;
    map[4] = V5;
    map[5] = V6;

    for (int i = 0; i < 6; ++i)
    {
        // std::cout << i << ": " << p[i] * numPedes / nrolls << std::endl;
        // std::cout << i << ": " << std::string(p[i] * numPedes / nrolls, '*') <<
        // std::endl;
        for (int j = 0; j < p[i] * numPedes / nrolls; j++)
        {
            v.push_back(map[i]);
        }
    }
    int curSize = v.size();
    for (int i = 0; i < numPedes - curSize; i++)
    {
        v.push_back(map[0]);
    }

    return v;
}

std::vector<double> Utility::getPedesVelocityBasedTDis(int numPedes,
                                                       double n_dist)
{
    vector<double> v;
    double std = sqrt(n_dist / (n_dist + 2));

    std::mt19937 gen(1701);

    std::student_t_distribution<> distr(n_dist);

    // std::cout << "min() == " << distr.min() << std::endl;
    // std::cout << "max() == " << distr.max() << std::endl;
    // std::cout << "n() == " << std::fixed << std::setw(11) <<
    // std::setprecision(10) << distr.n() << std::endl;

    // generate the distribution as a histogram
    std::map<double, int> histogram;
    for (int i = 0; i < numPedes; ++i)
    {
        ++histogram[distr(gen)];
    }

    // std::cout << "Distribution for " << numPedes << " samples:" << std::endl;
    // int counter = 0;
    for (const auto &elem : histogram)
    {
        // std::cout << std::fixed << std::setw(11) << ++counter << ": "
        //           << std::setw(14) << std::setprecision(3) << elem.first <<
        //           std::endl;
        double velocity = std * elem.first * 0.1 + MEAN;
        if (velocity > 1.8)
        {
            velocity = UPPER_SPEED_LIMIT;
        }
        else if (velocity < 0.6)
        {
            velocity = LOWER_SPEED_LIMIT;
        }

        v.push_back(velocity);
    }

    return v;
}

std::vector<float> Utility::getWallCoordinates(float walkwayWidth,
                                               std::vector<float> juncData)
{
    std::vector<float> v;

    std::vector<float> mapLimit = getMapLimit(walkwayWidth, juncData);

    float temp = walkwayWidth / 2;

    float leftWidthLimit = mapLimit[0];
    float rightWidthLimit = mapLimit[1];

    float lowerHeightLimit = mapLimit[2];
    float upperHeightLimit = mapLimit[3];

    if (juncData.size() == 2)
    {
        // Upper Wall
        v.insert(v.end(), {leftWidthLimit, temp, rightWidthLimit, temp});
        // Lower Wall
        v.insert(v.end(), {leftWidthLimit, -temp, rightWidthLimit, -temp});
        return v;
    }

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

    return std::to_string(hr) + std::string("h ") + std::to_string(min) +
           std::string("m ") + std::to_string(sec) + std::string("s ") +
           std::to_string(ms) + std::string("ms");
}

// Position 0/1/2/3: Left/Right/Lower/Upper Limit
std::vector<float> Utility::getMapLimit(float walkwayWidth,
                                        std::vector<float> juncData)
{
    std::vector<float> v;
    float leftWidthLimit = -1;
    float rightWidthLimit = -1;
    float lowerHeightLimit = -1;
    float upperHeightLimit = -1;

    float temp = walkwayWidth / 2;

    if (juncData.size() == 2)
    {
        leftWidthLimit = -juncData[0] - temp;
        rightWidthLimit = juncData[1] + temp;
    }
    else
    {
        leftWidthLimit = -juncData[0] - temp;
        rightWidthLimit = juncData[2] + temp;

        lowerHeightLimit = -juncData[1] - temp;
        upperHeightLimit = juncData[3] + temp;
    }

    v.insert(v.end(), {leftWidthLimit, rightWidthLimit, lowerHeightLimit,
                       upperHeightLimit});

    return v;
}

// direction: 0 To Right, 1 To Left, 2 To Bottom, 3 To Top
// side: 0 Left side, 1 Center, 2 Right side
std::vector<float> Utility::getPedesDestination(int direction, int side,
                                                float walkwayWidth,
                                                std::vector<float> juncData,
                                                bool stopAtCorridor)
{
    std::vector<float> v;

    std::vector<float> mapLimit = getMapLimit(walkwayWidth, juncData);
    float leftWidthLimit = mapLimit[0];
    float rightWidthLimit = mapLimit[1];
    float lowerHeightLimit = mapLimit[2];
    float upperHeightLimit = mapLimit[3];

    float radius = 0.5;
    switch (direction)
    {
        // To Right
    case 0:
    {
        float latitude;
        if (stopAtCorridor)
        {
            // latitude = Utility::randomFloat(0, rightWidthLimit - 2);
            latitude = Utility::randomInt(leftWidthLimit + 2, rightWidthLimit - 2);
        }
        else
        {
            latitude =
                Utility::randomFloat(rightWidthLimit + 1, rightWidthLimit + 2);
        }
        switch (side)
        {
        case 0:
        {
            v.insert(v.end(),
                     {latitude,
                      //    Utility::randomFloat(walkwayWidth / 2 - walkwayWidth /
                      //    3, walkwayWidth / 2),
                      Utility::randomFloat(-walkwayWidth / 2, walkwayWidth / 2),
                      radius});
            return v;
            break;
        }
        case 1:
        {
            v.insert(v.end(),
                     {latitude,
                      Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                           walkwayWidth / 2 - walkwayWidth / 3),
                      radius});
            return v;
            break;
        }
        case 2:
        {
            v.insert(v.end(),
                     {latitude,
                      Utility::randomFloat(-walkwayWidth / 2,
                                           -walkwayWidth / 2 + walkwayWidth / 3),
                      radius});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
        // To Left
    case 1:
    {
        float latitude;
        if (stopAtCorridor)
        {
            latitude = Utility::randomFloat(leftWidthLimit + 2, 0);
        }
        else
        {
            latitude = Utility::randomFloat(leftWidthLimit - 2, leftWidthLimit - 1);
        }
        switch (side)
        {
        case 0:
        {
            v.insert(v.end(),
                     {latitude,
                      Utility::randomFloat(-walkwayWidth / 2,
                                           -walkwayWidth / 2 + walkwayWidth / 3),
                      radius});
            return v;
            break;
        }
        case 1:
        {
            v.insert(v.end(),
                     {latitude,
                      Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                           walkwayWidth / 2 - walkwayWidth / 3),
                      radius});
            return v;
            break;
        }
        case 2:
        {
            v.insert(v.end(),
                     {latitude,
                      Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                           walkwayWidth / 2),
                      radius});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
        // To Bottom
    case 2:
    {
        float longitude;
        if (stopAtCorridor)
        {
            longitude = Utility::randomFloat(lowerHeightLimit + 2, 0);
        }
        else
        {
            longitude =
                Utility::randomFloat(lowerHeightLimit - 2, lowerHeightLimit - 1);
        }
        switch (side)
        {
        case 0:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                           walkwayWidth / 2),
                      longitude, radius});
            return v;
            break;
        }
        case 1:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                           walkwayWidth / 2 - walkwayWidth / 3),
                      longitude, radius});
            return v;
            break;
        }
        case 2:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(-walkwayWidth / 2,
                                           -walkwayWidth / 2 + walkwayWidth / 3),
                      longitude, radius});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
        // To Top
    case 3:
    {
        float longitude;
        if (stopAtCorridor)
        {
            longitude = Utility::randomFloat(0, upperHeightLimit - 2);
        }
        else
        {
            longitude =
                Utility::randomFloat(upperHeightLimit + 1, upperHeightLimit + 2);
        }
        switch (side)
        {
        case 0:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(-walkwayWidth / 2,
                                           -walkwayWidth / 2 + walkwayWidth / 3),
                      longitude, radius});
            return v;
            break;
        }
        case 1:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                           walkwayWidth / 2 - walkwayWidth / 3),
                      longitude, radius});
            return v;
            break;
        }
        case 2:
        {
            v.insert(v.end(),
                     {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                           walkwayWidth / 2),
                      longitude, radius});
            return v;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        return v;
        break;
    }
    return v;
}

// direction: 0 From Left, 1 From Right, 2 From Top, 3 From Bottom
std::vector<float> Utility::getPedesSource(int direction, float totalLength,
                                           float subLength, float caravanWidth,
                                           float walkwayWidth,
                                           std::vector<float> juncData)
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

    std::vector<float> mapLimit = getMapLimit(walkwayWidth, juncData);
    float rightWidthLimit = mapLimit[1];
    float upperHeightLimit = mapLimit[3];

    float horLandmark = rightWidthLimit;
    float verLandmark = upperHeightLimit;
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
                     {Utility::randomFloat(-horLandmark + centerLength / 2,
                                           -horLandmark + totalLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(-horLandmark - centerLength / 2,
                                           -horLandmark + centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(-horLandmark - totalLength / 2,
                                           -horLandmark - centerLength / 2),
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
                     {Utility::randomFloat(horLandmark - totalLength / 2,
                                           horLandmark - centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(horLandmark - centerLength / 2,
                                           horLandmark + centerLength / 2),
                      Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(horLandmark + centerLength / 2,
                                           horLandmark + totalLength / 2),
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
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                      Utility::randomFloat(verLandmark - totalLength / 2,
                                           verLandmark - centerLength / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                      Utility::randomFloat(verLandmark - centerLength / 2,
                                           verLandmark + centerLength / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
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
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                      Utility::randomFloat(-verLandmark + centerLength / 2,
                                           -verLandmark + totalLength / 2)});
            return v;
            break;
        case 1:
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                      Utility::randomFloat(-verLandmark - centerLength / 2,
                                           -verLandmark + centerLength / 2)});
            return v;
            break;
        case 2:
            v.insert(v.end(),
                     {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
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

std::vector<float> Utility::getPedesColor(float maxSpeed, float minSpeed,
                                          float desiredSpeed, int type)
{
    std::vector<float> v;

    if (type == 1)
    {
        float oneThirdVeloRange = (maxSpeed - minSpeed) / 3;

        if (desiredSpeed >= MEAN)
        {
            v.insert(v.end(), {GREEN.x, GREEN.y, GREEN.z}); // Green
            return v;
        }
        else if (desiredSpeed < MEAN &&
                 desiredSpeed >= minSpeed + oneThirdVeloRange)
        {
            v.insert(v.end(), {BLACK.x, BLACK.y, BLACK.z}); // Black
            return v;
        }
        else
        {
            v.insert(v.end(), {RED.x, RED.y, RED.z}); // Red
            return v;
        }
    }
    else
    {
        // cout << fabs(V1 - desiredSpeed)  << endl;
        if (fabs(V1 - desiredSpeed) < 0.01)
        {
            v.insert(v.end(), {GREEN.x, GREEN.y, GREEN.z});
            return v;
        }
        else if (fabs(V2 - desiredSpeed) < 0.01)
        {
            v.insert(v.end(), {PURPLE.x, PURPLE.y, PURPLE.z});
            return v;
        }
        else if (fabs(V3 - desiredSpeed) < 0.01)
        {
            v.insert(v.end(), {RED.x, RED.y, RED.z});
            return v;
        }
        else if (fabs(V4 - desiredSpeed) < 0.01)
        {
            v.insert(v.end(), {WOOD.x, WOOD.y, WOOD.z});
            return v;
        }
        else if (fabs(V5 - desiredSpeed) < 0.01)
        {
            v.insert(v.end(), {GRAY.x, GRAY.y, GRAY.z});
            return v;
        }
        else
        {
            v.insert(v.end(), {BLACK.x, BLACK.y, BLACK.z});
            return v;
        }
    }
}

float getCoor(float x, float verAsymtote, float horAsymtote)
{
    return horAsymtote * x / (x - verAsymtote);
}

std::vector<Point3f> Utility::getRouteAGV(int src, int turningDirection,
                                          float walkwayWidth,
                                          std::vector<float> juncData)
{
    std::vector<Point3f> v;
    int junctionType = juncData.size();
    if (junctionType == 2)
    {
        v = getRouteAGVHallway(src, walkwayWidth, juncData);
    }
    else if (junctionType == 3)
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
std::vector<Point3f> Utility::getRouteAGVHallway(int src, float walkwayWidth,
                                                 std::vector<float> juncData)
{
    std::vector<Point3f> v;

    float leftWidthLimit = -juncData[0] - walkwayWidth / 2;
    float rightWidthLimit = juncData[1] + walkwayWidth / 2;

    if (src == 0)
    {
        v.insert(v.end(), {Point3f(leftWidthLimit - 1, -walkwayWidth / 3, 0.0),
                           Point3f(rightWidthLimit + 1, -walkwayWidth / 3, 0.0)});
        v.insert(v.end(), {Point3f(rightWidthLimit + 2, -walkwayWidth / 3, 0.0)});
        return v;
    }
    else
    {
        v.insert(v.end(), {Point3f(rightWidthLimit + 1, walkwayWidth / 3, 0.0),
                           Point3f(leftWidthLimit - 1, walkwayWidth / 3, 0.0)});
        v.insert(v.end(), {Point3f(leftWidthLimit - 2, walkwayWidth / 3, 0.0)});
        return v;
    }

    return v;
}

// src = {0, 1, 2} ~ Go from Left, Bottom, Right side
// turningDirection = {0, 1, 2} - Turn Left, Go Straight, Turn Right
std::vector<Point3f> Utility::getRouteAGVTJunction(
    int src, int turningDirection, float walkwayWidth,
    std::vector<float> juncData)
{
    std::vector<Point3f> v;

    float horWalkwayWidth = walkwayWidth;
    float verWalkwayWidth = walkwayWidth;

    float posVerAsymtote = verWalkwayWidth / 2;
    float negVerAsymtote = -verWalkwayWidth / 2;

    float posHorAsymtote = horWalkwayWidth / 2;
    // float negHorAsymtote = -horWalkwayWidth / 2;

    float leftWidthLimit = -juncData[0] - walkwayWidth / 2;
    float rightWidthLimit = juncData[2] + walkwayWidth / 2;

    float lowerHeightLimit = -juncData[1] - walkwayWidth / 2;

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
            v.insert(v.end(),
                     {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                      Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(
                v.end(),
                {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                 Point3f(-verWalkwayWidth / 2, -horWalkwayWidth / 3, 0.0),
                 Point3f(-verWalkwayWidth / 3.5, -horWalkwayWidth / 2.5, 0.0),
                 Point3f(-verWalkwayWidth / 3.5, -horWalkwayWidth / 2, 0.0)});
            v.insert(v.end(),
                     {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
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
            v.insert(v.end(),
                     {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                      Point3f(verWalkwayWidth / 3, -horWalkwayWidth / 2, 0.0),
                      Point3f(verWalkwayWidth / 2.5, -horWalkwayWidth / 3.5, 0.0),
                      Point3f(verWalkwayWidth / 2, -horWalkwayWidth / 3.5, 0.0)});
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0),
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
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                      Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(),
                     {Point3f(leftWidthLimit - 2, horWalkwayWidth / 3, 0.0)});
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
std::vector<Point3f> Utility::getRouteAGVCrossRoad(
    int src, int turningDirection, float walkwayWidth,
    std::vector<float> juncData)
{
    float horWalkwayWidth = walkwayWidth;
    float verWalkwayWidth = walkwayWidth;

    float posVerAsymtote = verWalkwayWidth / 2;
    float negVerAsymtote = -verWalkwayWidth / 2;

    float posHorAsymtote = horWalkwayWidth / 2;
    float negHorAsymtote = -horWalkwayWidth / 2;

    float leftWidthLimit = -juncData[0] - walkwayWidth / 2;
    float rightWidthLimit = juncData[2] + walkwayWidth / 2;

    float lowerHeightLimit = -juncData[1] - walkwayWidth / 2;
    float upperHeightLimit = juncData[3] + walkwayWidth / 2;

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
            v.insert(v.end(),
                     {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                      Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 2, -horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(
                v.end(),
                {Point3f(leftWidthLimit - 1, -horWalkwayWidth / 3, 0.0),
                 Point3f(-verWalkwayWidth / 2, -horWalkwayWidth / 3, 0.0),
                 Point3f(-verWalkwayWidth / 3.5, -horWalkwayWidth / 2.5, 0.0),
                 Point3f(-verWalkwayWidth / 3.5, -horWalkwayWidth / 2, 0.0)});
            v.insert(v.end(),
                     {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
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
            v.insert(v.end(),
                     {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                      Point3f(verWalkwayWidth / 3, upperHeightLimit + 1, 0.0)});
            v.insert(v.end(),
                     {Point3f(verWalkwayWidth / 3, upperHeightLimit + 2, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(),
                     {Point3f(verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0),
                      Point3f(verWalkwayWidth / 3, -horWalkwayWidth / 2, 0.0),
                      Point3f(verWalkwayWidth / 2.5, -horWalkwayWidth / 3.5, 0.0),
                      Point3f(verWalkwayWidth / 2, -horWalkwayWidth / 3.5, 0.0)});
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 1, -horWalkwayWidth / 3, 0.0),
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
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                      Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0)});
            v.insert(v.end(),
                     {Point3f(leftWidthLimit - 2, horWalkwayWidth / 3, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(),
                     {Point3f(rightWidthLimit + 1, horWalkwayWidth / 3, 0.0),
                      Point3f(verWalkwayWidth / 2, horWalkwayWidth / 3, 0.0),
                      Point3f(verWalkwayWidth / 3.5, horWalkwayWidth / 2.5, 0.0),
                      Point3f(verWalkwayWidth / 3.5, horWalkwayWidth / 2, 0.0)});
            v.insert(v.end(),
                     {Point3f(verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
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
            v.insert(v.end(),
                     {Point3f(-verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
                      Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 1, 0.0)});
            v.insert(v.end(),
                     {Point3f(-verWalkwayWidth / 3, lowerHeightLimit - 2, 0.0)});
            return v;
            break;
        }
            // Turn Right
        case 2:
        {
            v.insert(v.end(),
                     {Point3f(-verWalkwayWidth / 3, upperHeightLimit + 1, 0.0),
                      Point3f(-verWalkwayWidth / 3, horWalkwayWidth / 2, 0.0),
                      Point3f(-verWalkwayWidth / 2.5, horWalkwayWidth / 3.5, 0.0),
                      Point3f(-verWalkwayWidth / 2, horWalkwayWidth / 3.5, 0.0)});
            v.insert(v.end(),
                     {Point3f(leftWidthLimit - 1, horWalkwayWidth / 3, 0.0),
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

Point3f Utility::getIntermediateDes(Point3f position, float verWalkwayWidth,
                                    float horWalkwayWidth)
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

bool Utility::isPositionErr(Point3f position, float hallwayWidth,
                            int junctionType, std::vector<AGV *> agvs)
{
    float posLimit = hallwayWidth / 2;
    float negLimit = -hallwayWidth / 2;
    float x = position.x;
    float y = position.y;
    if (junctionType == 4)
    {
        bool con1 = x >= posLimit && y >= posLimit;
        bool con2 = x >= posLimit && y <= negLimit;
        bool con3 = x <= negLimit && y >= posLimit;
        bool con4 = x <= negLimit && y <= negLimit;
        if (con1 || con2 || con3 || con4)
        {
            return true;
        }
    }
    else if (junctionType == 3)
    {
        bool con1 = y >= posLimit;
        bool con2 = x >= posLimit && y <= negLimit;
        bool con3 = x <= negLimit && y <= negLimit;
        if (con1 || con2 || con3)
        {
            return true;
        }
    }
    else if (junctionType == 2)
    {
        if (y >= posLimit || y <= negLimit)
        {
            return true;
        }
    }

    for (AGV *agv : agvs)
    {
        if (!agv->getIsMoving())
        {
            continue;
        }
        float distance = (agv->getWidth() > agv->getLength()) ? agv->getWidth()
                                                              : agv->getLength();
        if (position.distance(agv->getPosition()) < distance / 2)
        {
            return true;
        }
    }

    return false;
}

int Utility::getNumAGVCompleted(std::vector<AGV *> agvs)
{
    int count = 0;
    for (AGV *agv : agvs)
    {
        if (!agv->getIsMoving() && agv->getTravelingTime() != 0)
        {
            count = count + 1;
        }
    }
    return count;
}

int Utility::randomInt(int from, int to)
{
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(from, to);
    return distr(gen);
}
