#include "commons.h"

const float meanAnomalies[] = 
{
    0.0f, // Sun and stars
    174.796f / 180 * PI, //Mercury
     50.115f / 180 * PI, //Venus
  357.51716f / 180 * PI, //Earth
    19.3564f / 180 * PI, //Mars
     18.818f / 180 * PI, //Jupiter
    320.346f / 180 * PI, //Saturn
    142.955f / 180 * PI, //Uranus
    267.767f / 180 * PI, //Neptune
     14.860f / 180 * PI, //Pluto

     // now for the satellites
  134.96292f / 180 * PI, // Moon
        0.0f / 180 * PI, // Io
        0.0f / 180 * PI, // Europa
        0.0f / 180 * PI, // Callisto
        0.0f / 180 * PI, // Ganymede
        0.0f / 180 * PI, // Titan
};

const float orbitalPeriods[] = // already in time units
{
    1.0f,
    87.969f * timeScale,
    224.698f * timeScale,
    365.256366f * timeScale,
    686.98f * timeScale,	
    4332.589f * timeScale,
    10759.22f * timeScale,
    30685.4f * timeScale,
    60190.03f * timeScale,
    90613.3f * timeScale,

    27.321f * timeScale,
    1.7f * timeScale,
    3.551f * timeScale,
    16.69f * timeScale,
    7.155f * timeScale,
    15.95f * timeScale,
};

const float rotationSpeeds[] = // in radians per time unit
{
    2 * PI / (25.38f * timeScale),
    2 * PI / (58.646f * timeScale),
   -2 * PI / (243.023f * timeScale),
    2 * PI / (0.99726968f * timeScale),
    2 * PI / (1.029f * timeScale),
    2 * PI / (0.4147f * timeScale),
    2 * PI / (0.4416f * timeScale),
   -2 * PI / (0.71833f * timeScale),
    2 * PI / (0.6653f * timeScale),
   -2 * PI / (6.38723f * timeScale),

    2 * PI / (27.321f * timeScale),
    2 * PI / (1.7f * timeScale),
    2 * PI / (3.551f * timeScale),
    2 * PI / (16.69f * timeScale),
    2 * PI / (7.155f * timeScale),
    2 * PI / (15.95f * timeScale),
};

const float axialTilts[] = 
{
    7.25f / 180 * PI,
    2.11f / 180 * PI,
    177.36f / 180 * PI,
    23.44f / 180 * PI,
    25.1919f / 180 * PI,
    3.13f / 180 * PI,
    26.73f / 180 * PI,
    97.77f / 180 * PI,
    28.32f / 180 * PI,
    119.591f / 180 * PI,

    6.687f / 180 * PI,
    0.0f / 180 * PI,
    0.016f / 180 * PI,
    0.356f / 180 * PI,
    0.068f / 180 * PI,
    0.6f / 180 * PI,
};

const float eccentricities[] = 
{
    1.0f,
    0.20563069f,
    0.0068f,
    0.01671123f,
    0.0933941f,
    0.048775f,
    0.055723f,
    0.044406f,
    0.011214f,
    0.24880766f,

    0.0549f,
    0.0041f,
    0.0094f,
    0.0074f,
    0.0013f,
    0.0288f,
};

const float inclinations[] = 
{
    0.0f,
    7.0f / 180 * PI,
    3.39458f / 180 * PI,
    0.0f / 180 * PI,
    1.85061f / 180 * PI,
    1.03f / 180 * PI,
    2.485f / 180 * PI,
    0.773f / 180 * PI,
    1.767975f / 180 * PI,
    17.14175f / 180 * PI,

    5.145f / 180 * PI, 
    0.05f / 180 * PI,
    0.466f / 180 * PI,
    0.192f / 180 * PI,
    0.177f / 180 * PI,
    0.28f / 180 * PI,
};

const float periapsisArgs[] = 
{
    0.0f,
    29.124279f / 180 * PI,
    54.85229f / 180 * PI,
    114.20783f / 180 * PI,
    286.4623f / 180 * PI,
    275.066f / 180 * PI,
    336.013f / 180 * PI,
    96.541318f / 180 * PI,
    265.646853f / 180 * PI,
    113.76329f / 180 * PI,

    318.15f / 180 * PI,
    84.129f / 180 * PI,
    88.97f / 180 * PI,
    52.643f / 180 * PI,
    192.417f / 180 * PI,
    185.671f / 180 * PI,
};

const float ascendingNodesLongs[] = 
{
    0.0f,
    48.33167f / 180 * PI,
    76.67069f / 180 * PI,
    348.73936f / 180 * PI,
    49.57854f / 180 * PI,
    100.55615f / 180 * PI,
    113.642f / 180 * PI,
    73.989821f / 180 * PI,
    131.79431f / 180 * PI,
    110.30347f / 180 * PI,

    125.08f / 180 * PI,
    43.97f / 180 * PI,
    219.106f / 180 * PI,
    298.848f / 180 * PI,
    63.552f / 180 * PI,
    24.502f / 180 * PI,
};

const float radii[] =  // in kilometers
{
    695990.0f,
    2439.7f,
    6051.8f,
    6371.0f,
    3389.5f,
    69911.0f,
    57316.0f,
    25266.0f,
    24552.5f,
    1195.0f,

    1737.1f,
    1821.6f,
    1560.8f,
    2410.3f,
    2631.2f,
    2575.5f,
};

const float radiiSchematic[] =  // in kilometers
{
    695990.0f / 20,
    2439.7f,
    6051.8f,
    6371.0f,
    3389.5f,
    69911.0f / 2,
    57316.0f / 2,
    25266.0f,
    24552.5f,
    1195.0f * 10,

    1737.1f,
    1821.6f,
    1560.8f,
    2410.3f,
    2631.2f,
    2575.5f,
};

const float semiMajorAxes[] =  // in astron. units
{
    0.0f,
    0.38709830982f,
    0.72332981996f,
    1.00000101778f,
    1.52367934191f,
    5.20260319132f,
    9.55490959574f,
    19.21844606178f,
    30.11038686942f,
    39.5181761979f,

    0.00257f,
    0.00282f,
    0.004486f,
    0.012585f,
    0.007155f,
    0.008168f,
};

const float semiMajorAxesSchematic[] =  // in astron. units
{
    0.0f,
    0.38709830982f,
    0.72332981996f,
    1.00000101778f,
    1.52367934191f,
    5.20260319132f / 1.5f,
    9.55490959574f / 1.5f,
    19.21844606178f / 2,
    30.11038686942f / 2,
    39.5181761979f / 2,

    0.00257f * 50,
    0.00282f * 100,
    0.004486f * 100,
    0.012585f * 100,
    0.007155f * 100,
    0.008168f * 50,
};

// amb, diff, spec, shininess
const float materials[][7] = 
{
    {0.2f, 0.8f, 0.0f, 128 * 0.7f, 1.0f, 0.0f, 0.0f}, // the configuration without ligth depending from distance, used in real mode
    {0.2f, 0.8f, 0.0f, 128 * 0.7f, 0.4f, 0.0f, 0.001f}, // ligth depends from distance like inverted square
    {1.0f, 1.0f, 1.0f, 128 * 0.7f, 1.0f, 0.0f, 0.0f}, // the sun
    {0.3f, 1.0f, 0.8f, 128 * 0.2f, 0.0f, 0.0f, 0.0001f}, // far planets with high albedo
    {0.2f, 0.8f, 0.0f, 128 * 0.7f, 0.0f, 0.01f, 0.0f}, // ligth depends from distance linearly
    {0.2f, 0.9f, 0.8f, 128 * 1.0f, 0.0f, 0.01f, 0.0f}, // Jupiter 
};

const char *planetTextures[] = 
{
    "Sun.png",
    "Mercury.png", 
    "Venus.png",
    "Earth.png",
    "Mars.png",
    "Jupiter.png",
    "Saturn.png",
    "Uranus.png",
    "Neptune.png",
    "Pluto.png",
};

const char* starTextures[5] = 
{
    "sky03_04.png",
    "sky03_03.png",
    "sky03_02.png",
    "sky03_01.png",
    "sky03_00.png",
};

const char* satelliteTextures[] = 
{
    "Moon.png",
    "Io.png",
    "Europa.png",
    "Callisto.png",
    "Ganymede.png",
    "Titan.png",
};