#include "Utility.h"
#include <array>
#include <bits/stdc++.h>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

using namespace std;

Utility::Utility() {}

Utility::~Utility() {}

// random float number between particular range
float Utility::randomFloat(float lowerBound, float upperBound) {
  return (lowerBound +
          (static_cast<float>(rand()) / RAND_MAX) * (upperBound - lowerBound));
}

// read input file
std::vector<double> Utility::readInput(const char *fileName) {
  vector<double> v;
  ifstream input(fileName);

  std::string delimiter = " ";

  for (std::string line; getline(input, line);) {
    if (line.rfind("#", 0) == 0) // pos=0 limits the search to the prefix
    {
      continue;
    }
    // cout << "Line: " <<line << endl;
    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
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

// calculate number of people in each flow
std::array<int, 12> Utility::getNumPedesInFlow(int totalPedestrian) {
  std::array<int, 12> arr = {};
  int j = 0;
  for (int i = 0; i < totalPedestrian; i++) {
    arr[j] = arr[j] + 1;
    j = j + 1;
    if (j == 12) {
      j = 0;
    }
  }
  return arr;
}

// get list velocity of all pedestrians
std::vector<double> Utility::getPedesVelocity(int numPedes, double n_dist) {
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
  for (int i = 0; i < numPedes; ++i) {
    ++histogram[distr(gen)];
  }

  // std::cout << "Distribution for " << numPedes << " samples:" << std::endl;
  int counter = 0;
  for (const auto &elem : histogram) {
    // std::cout << std::fixed << std::setw(11) << ++counter << ": "
    //           << std::setw(14) << std::setprecision(3) << elem.first <<
    //           std::endl;
    double velocity = std * elem.first * 0.1 + Utility::MEAN;
    if (velocity > 2) {
      velocity = 2.2;
    } else if (velocity < 0.8) {
      velocity = 0.6;
    }

    v.push_back(velocity);
  }

  return v;
}

std::vector<float> Utility::getWallCoordinates(float wWidth) {
  std::vector<float> v;
  float temp = wWidth / 2;
  // Upper Wall
  v.insert(v.end(), {-25, temp, -temp, temp});
  v.insert(v.end(), {temp, temp, 25, temp});
  // Lower Wall
  v.insert(v.end(), {-25, -temp, -temp, -temp});
  v.insert(v.end(), {temp, -temp, 25, -temp});
  // Left Wall
  v.insert(v.end(), {-temp, 25, -temp, temp});
  v.insert(v.end(), {-temp, -temp, -temp, -25});
  // Right Wall
  v.insert(v.end(), {temp, 25, temp, temp});
  v.insert(v.end(), {temp, -temp, temp, -25});

  return v;
}

// Convert miliseconds to pretty form
std::string Utility::convertTime(int ms) {
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

// direction: 0 Left - Right, 1 Right - Left, 2 Top - Bottom, 3 Bottom - Top
// side: 0 Left side, 1 Center, 2 Right side
std::vector<float> Utility::getPedesDestination(int direction, int side,
                                                float walkwayWidth) {
  std::vector<float> v;

  float radius = 3.0;
  switch (direction) {
    // Left - Right
  case 0:
    switch (side) {
    case 0:
      v.insert(v.end(),
               {Utility::randomFloat(20.0, 25.0),
                Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                     walkwayWidth / 2),
                radius});
      return v;
      break;
    case 1:
      v.insert(v.end(),
               {Utility::randomFloat(20.0, 25.0),
                Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                     walkwayWidth / 2 - walkwayWidth / 3),
                radius});
      return v;
      break;
    case 2:
      v.insert(v.end(),
               {Utility::randomFloat(20.0, 25.0),
                Utility::randomFloat(-walkwayWidth / 2,
                                     -walkwayWidth / 2 + walkwayWidth / 3),
                radius});
      return v;
      break;
    default:
      break;
    }
    break;
    // Right - Left
  case 1:
    switch (side) {
    case 0:
      v.insert(v.end(),
               {Utility::randomFloat(-25.0, -20.0),
                Utility::randomFloat(-walkwayWidth / 2,
                                     -walkwayWidth / 2 + walkwayWidth / 3),
                radius});
      return v;
      break;
    case 1:
      v.insert(v.end(),
               {Utility::randomFloat(-25.0, -20.0),
                Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                     walkwayWidth / 2 - walkwayWidth / 3),
                radius});
      return v;
      break;
    case 2:
      v.insert(v.end(),
               {Utility::randomFloat(-25.0, -20.0),
                Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                     walkwayWidth / 2),
                radius});
      return v;
      break;
    default:
      break;
    }
    break;
    // Top - Bottom
  case 2:
    switch (side) {
    case 0:
      v.insert(v.end(),
               {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                     walkwayWidth / 2),
                Utility::randomFloat(-16, -13), radius});
      return v;
      break;
    case 1:
      v.insert(v.end(),
               {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                     walkwayWidth / 2 - walkwayWidth / 3),
                Utility::randomFloat(-16, -13), radius});
      return v;
      break;
    case 2:
      v.insert(v.end(),
               {Utility::randomFloat(-walkwayWidth / 2,
                                     -walkwayWidth / 2 + walkwayWidth / 3),
                Utility::randomFloat(-16, -13), radius});
      return v;
      break;
    default:
      break;
    }
    break;
    // Bottom - Top
  case 3:
    switch (side) {
    case 0:
      v.insert(v.end(),
               {Utility::randomFloat(-walkwayWidth / 2,
                                     -walkwayWidth / 2 + walkwayWidth / 3),
                Utility::randomFloat(13, 16), radius});
      return v;
      break;
    case 1:
      v.insert(v.end(),
               {Utility::randomFloat(-walkwayWidth / 2 + walkwayWidth / 3,
                                     walkwayWidth / 2 - walkwayWidth / 3),
                Utility::randomFloat(13, 16), radius});
      return v;
      break;
    case 2:
      v.insert(v.end(),
               {Utility::randomFloat(walkwayWidth / 2 - walkwayWidth / 3,
                                     walkwayWidth / 2),
                Utility::randomFloat(13, 16), radius});
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
std::vector<float> Utility::getPedesSource(int direction, float totalLength,
                                           float subLength,
                                           float caravanWidth) {
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

  // Calculate coordinates from Head to Tail of each Caravan
  switch (direction) {
    // From Left to Right
  case 0:
    switch (sampled_value) {
    case 0:
      v.insert(
          v.end(),
          {Utility::randomFloat(-20 + centerLength / 2, -20 + totalLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    case 1:
      v.insert(
          v.end(),
          {Utility::randomFloat(-20 - centerLength / 2, -20 + centerLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    case 2:
      v.insert(
          v.end(),
          {Utility::randomFloat(-20 - totalLength / 2, -20 - centerLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    default:
      break;
    }
    break;

    // From Right to Left
  case 1:
    switch (sampled_value) {
    case 0:
      v.insert(
          v.end(),
          {Utility::randomFloat(20 - totalLength / 2, 20 - centerLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    case 1:
      v.insert(
          v.end(),
          {Utility::randomFloat(20 - centerLength / 2, 20 + centerLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    case 2:
      v.insert(
          v.end(),
          {Utility::randomFloat(20 + centerLength / 2, 20 + totalLength / 2),
           Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2)});
      return v;
      break;
    default:
      break;
    }
    break;

    // From Top to Bottom
  case 2:
    switch (sampled_value) {
    case 0:
      v.insert(
          v.end(),
          {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
           Utility::randomFloat(15 - totalLength / 2, 15 - centerLength / 2)});
      return v;
      break;
    case 1:
      v.insert(
          v.end(),
          {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
           Utility::randomFloat(15 - centerLength / 2, 15 + centerLength / 2)});
      return v;
      break;
    case 2:
      v.insert(
          v.end(),
          {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
           Utility::randomFloat(15 + centerLength / 2, 15 + totalLength / 2)});
      return v;
      break;
    default:
      break;
    }
    break;

    // From Bottom to Top
  case 3:
    switch (sampled_value) {
    case 0:
      v.insert(v.end(),
               {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                Utility::randomFloat(-15 + centerLength / 2,
                                     -15 + totalLength / 2)});
      return v;
      break;
    case 1:
      v.insert(v.end(),
               {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                Utility::randomFloat(-15 - centerLength / 2,
                                     -15 + centerLength / 2)});
      return v;
      break;
    case 2:
      v.insert(v.end(),
               {Utility::randomFloat(-caravanWidth / 2, caravanWidth / 2),
                Utility::randomFloat(-15 - totalLength / 2,
                                     -15 - centerLength / 2)});
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