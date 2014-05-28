#pragma once

extern float time;      
// counted in (1 day / timeScale) units
// if timeScale == 1440 then it's minutes

extern float timeSpeed;
const float timeScale = 1440.0f; 
// this coefficient is used to transform time from days to time units

const float PI = 3.1415926535f;

const float AU = 149597870.0f; // kilometers in astron. unit
extern float distanceScale;
extern float radiusScale;

extern bool realScale;



//window size
extern int windowWidth;
extern int windowHeight;