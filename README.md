# Simulator measures the influence of people on AGVs using Social Force Model

## Social Force Model
The Social Force Model (SFM) is a mathematical model used to describe the movement of individuals in crowded spaces. It was first proposed by Dirk Helbing and Péter Molnár in 1995. 

The model assumes that individuals move based on two types of forces: physical forces and social forces. Physical forces are those that are governed by laws of physics, such as inertia, friction, and gravity. Social forces, on the other hand, are those that are influenced by the behavior of other individuals in the crowd

The SFM model also takes into account the personal preferences of individuals, such as their desired walking speed and preferred direction of movement. These preferences are included in the model as parameters that can be adjusted to simulate different scenarios.

Throughout the years, many improvements were made to the original model. One of the latest improvement was made
by [Moussaïd *et al.* (2009)](https://doi.org/10.1098/rspb.2009.0405). In their research, the model parameters were
calibrated to match the results of the experiment they have conducted on the real-world crowd. This simulator is created based on this research.

## Getting Started

### Prerequisites

This project requires the following library.
- [Open Graphics Library (OpenGL)](https://www.opengl.org/)

This project also requires users to use compilers that support C++ 11.

### Run simulation

**The emulator is run with the following input:**
- input.json: The configuration of the emulator scenarios
- map.txt: Map data (extracted from the OMNeT++ simulator map)

<!-- **Create a Pointer to the <code>SocialForce</code> Object** -->
**Main commands** <br/>
To compile program
```cpp
make
```
To run simulator

```cpp
./app
```