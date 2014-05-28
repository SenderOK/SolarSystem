#include "Planet.h"
#include "commons.h"
#include "lodepng.h" //functions to load png textures

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

#include <iostream>

Planet::Planet():
    dataCount(0),
    indexesCount(0),
    pData(0),
    pIndexes(0)
{
    generateData(18, 36);
}


Planet::~Planet(void)
{
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(textureCount, texId);
    if (pData)
    {
        delete[] pData;
        delete[] pIndexes;
    }
}

void Planet::initPlanetParameters(float _radius, float _rotationVelocity, float _orbitalPeriod, float _meanAnomaly, float _axialTilt)
{
    radius = _radius;
    rotationVelocity = _rotationVelocity;
    orbitalPeriod = _orbitalPeriod;
    startMeanAnomaly = _meanAnomaly;
    axialTilt = _axialTilt;
}

void Planet::initOrbitParameters(float a, float e, float i, float pArg, float ascNodeLong)
{
    orbit.semiMajorAxis = a;
    orbit.eccentricity = e;
    orbit.inclination = i;
    orbit.periapsisArg = pArg;
    orbit.ascNodeLongitude = ascNodeLong;

    orbit.semiMinorAxis = a * std::sqrt(1 - e * e);
}

void Planet::initGLBuffers(GLuint programId, const char* posName,const char* norName,const char* texName)
{
    shaderId = programId;

    glGenVertexArrays( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers ( 2, &vbo[0]);
    
    glBindBuffer ( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData ( GL_ARRAY_BUFFER, dataCount*sizeof(VertexData), pData, GL_STATIC_DRAW );
        
    glEnable(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vbo[1] );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, indexesCount * sizeof(unsigned int), pIndexes, GL_STATIC_DRAW );
    
    int	loc = glGetAttribLocation(programId, posName);
    if (loc>-1)
    {
        glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)0);
        glEnableVertexAttribArray (loc);
    }
    int loc2 = glGetAttribLocation(programId, norName);
    if (loc2>-1)
    {
        glVertexAttribPointer(loc2,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*3));
        glEnableVertexAttribArray (loc2);
    }
    int loc3 = glGetAttribLocation(programId, texName);
    if (loc3>-1)
    {
        glVertexAttribPointer(loc3,2,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*6));
        glEnableVertexAttribArray (loc3);
    }
}

void Planet::initTexture(int count, const char **filenames)
{
    textureCount = count;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(count, texId);
    
    for (unsigned i = 0; i < textureCount; ++i) {
        std::vector<unsigned char> image;
        unsigned width, height;
        unsigned error = lodepng::decode(image, width, height, filenames[i], LCT_RGB);

        if (error) {
            std::cerr << "Something has gone wrong while trying to load " << filenames[i] << " texture" << std::endl;
            std::cerr << lodepng_error_text(error) << std::endl;
            exit(1);
        }
 
        //make texId current 
        glBindTexture(GL_TEXTURE_2D, texId[i]);
    
        //don't use alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //build mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
    
        // Set nearest filtering mode for texture minification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        //set Texture Data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
    }
    currTexture = 0;
}

void Planet::switchTexture(int mode)
{
    currTexture = mode;
}

void Planet::Draw()
{
    int texLoc = glGetUniformLocation(shaderId, "texture");
    if (texLoc>-1)
        glUniform1ui(texLoc, texId[currTexture]);

    glBindTexture(GL_TEXTURE_2D, texId[currTexture]);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Planet::generateData(int nParallels, int nMeridians)
{
    if (pData)
    {
        delete[] pData;
        delete[] pIndexes;
    }

    dataCount = (nParallels + 1) * (nMeridians + 1);
    unsigned int nTriangles = nParallels * nMeridians * 2;
    indexesCount = 3 * nTriangles;

    pData = new VertexData [dataCount];
    pIndexes = new unsigned int [indexesCount];

    int currData = 0;

    float phiShift = (2 * PI) / nMeridians;
    float thetaShift = PI / nParallels;

    for (int phi = 0; phi <= nMeridians; ++phi) {
        for (int theta = 0; theta <= nParallels; ++theta) {
            float x = sin(theta * thetaShift) * cos(phi * phiShift);
            float y = cos(theta * thetaShift);
            float z = sin(theta * thetaShift) * sin(phi * phiShift);
            pData[currData].pos = glm::vec3(x, y, z);
            pData[currData].nor = glm::vec3(x, y, z);
            pData[currData].tex = glm::vec2(1 - phi * phiShift / (2 * PI), theta * thetaShift / PI);
            ++currData;
        }
    }

    int currIndex = 0;
    int currPoint = 0;
    for (int i = 0; i < nMeridians; ++i) {
        for (int j = 0; j < nParallels; ++j) {
            pIndexes[currIndex] = currPoint;
            pIndexes[currIndex + 1] = currPoint + nParallels + 1;
            pIndexes[currIndex + 2] = currPoint + nParallels + 2;
            currIndex = currIndex + 3;

            pIndexes[currIndex] = currPoint;
            pIndexes[currIndex + 1] = currPoint + 1;
            pIndexes[currIndex + 2] = currPoint + nParallels + 2;
            currIndex = currIndex + 3;
            ++currPoint;
        }
        ++currPoint;
    }
}

void Planet::invertNormals()
{
    for (unsigned i = 0; i < dataCount; ++i) {
        pData[i].nor = -pData[i].nor;
    }
}

glm::mat4x4 Planet::getModelMatrix()
{
    glm::vec3 axisX(1.0f, 0.0f, 0.0f), 
              axisY(0.0f, 1.0f, 0.0f),
              axisZ(0.0f, 0.0f, 1.0f);
    glm::mat4x4 Model = glm::mat4x4(1.0f);

    Model = glm::rotate(Model, glm::degrees(orbit.ascNodeLongitude - 3 * PI / 2), axisY);

    Model = glm::rotate(Model, glm::degrees(orbit.inclination), axisZ);

    Model = glm::rotate(Model, glm::degrees(orbit.periapsisArg - PI / 2), axisY);

    float meanAnomaly = startMeanAnomaly + time * (2 * PI) / orbitalPeriod;

    float e = orbit.eccentricity;
    float trueAnomaly = meanAnomaly + (2 * e - 0.25 * pow(e, 3) + 5/96 * pow(e, 5)) * sin(meanAnomaly)
                                    + (5 / 4 * pow(e, 2) - 11/24 * pow(e, 4)) * sin(2 * meanAnomaly)
                                    + (13 / 12 * pow(e, 3) - 43/64 * pow(e, 5)) * sin(3 * meanAnomaly)
                                    + 103/96 * pow(e, 4) * sin(4 * meanAnomaly)
                                    + 1097/960 * pow(e, 5) * sin(5 * meanAnomaly);

    /*float trueAnomaly = meanAnomaly + (2 * e - pow(e, 3) / 4) * sin(meanAnomaly) + 
                                       5 / 4 * pow(e, 2) * sin(2 * meanAnomaly) +
                                       13 / 12 * pow(e, 3) * sin(3 * meanAnomaly);*/
    // Approximate solution of Kepler's equasion
    float currX = orbit.semiMajorAxis * distanceScale * cos(trueAnomaly);
    float currY = orbit.semiMinorAxis * distanceScale * sin(trueAnomaly);
    glm::vec3 shiftVec = glm::vec3(currX, 0.0f, -currY) + 
                         glm::vec3(-orbit.eccentricity * orbit.semiMajorAxis * distanceScale, 0.0f, 0.0f);
    Model = glm::translate(Model, shiftVec);
    
    Model = glm::rotate(Model, glm::degrees(-axialTilt), axisZ);
    Model = glm::rotate(Model, glm::degrees(time * rotationVelocity), axisY);

    Model = glm::scale(Model, glm::vec3(radius * radiusScale, radius * radiusScale, radius * radiusScale));

    return Model;
}

glm::mat4x4 Planet::getSatelliteModelMatrix()
{
    glm::vec3 axisX(1.0f, 0.0f, 0.0f), 
              axisY(0.0f, 1.0f, 0.0f),
              axisZ(0.0f, 0.0f, 1.0f);
    glm::mat4x4 Model = glm::mat4x4(1.0f);

    Model = glm::rotate(Model, glm::degrees(orbit.ascNodeLongitude - 3 * PI / 2), axisY);

    Model = glm::rotate(Model, glm::degrees(orbit.inclination), axisZ);

    Model = glm::rotate(Model, glm::degrees(orbit.periapsisArg - PI / 2), axisY);

    float meanAnomaly = startMeanAnomaly + time * (2 * PI) / orbitalPeriod;

    float e = orbit.eccentricity;
    float trueAnomaly = meanAnomaly + (2 * e - 0.25 * pow(e, 3) + 5/96 * pow(e, 5)) * sin(meanAnomaly)
                                    + (5 / 4 * pow(e, 2) - 11/24 * pow(e, 4)) * sin(2 * meanAnomaly)
                                    + (13 / 12 * pow(e, 3) - 43/64 * pow(e, 5)) * sin(3 * meanAnomaly)
                                    + 103/96 * pow(e, 4) * sin(4 * meanAnomaly)
                                    + 1097/960 * pow(e, 5) * sin(5 * meanAnomaly);

    /*float trueAnomaly = meanAnomaly + (2 * e - pow(e, 3) / 4) * sin(meanAnomaly) + 
                                       5 / 4 * pow(e, 2) * sin(2 * meanAnomaly) +
                                       13 / 12 * pow(e, 3) * sin(3 * meanAnomaly);*/
    // Approximate solution of Kepler's equasion
    float currX = orbit.semiMajorAxis * distanceScale * cos(trueAnomaly);
    float currY = orbit.semiMinorAxis * distanceScale * sin(trueAnomaly);
    glm::vec3 shiftVec = glm::vec3(currX, 0.0f, -currY) + 
                         glm::vec3(-orbit.eccentricity * orbit.semiMajorAxis * distanceScale, 0.0f, 0.0f);
    Model = glm::translate(Model, shiftVec);

    return Model;
}

glm::mat4x4 Planet::getOrbitModelMatrix()
{
    glm::vec3 axisX(1.0f, 0.0f, 0.0f), 
              axisY(0.0f, 1.0f, 0.0f),
              axisZ(0.0f, 0.0f, 1.0f);
    glm::mat4x4 Model = glm::mat4x4(1.0f);

    Model = glm::rotate(Model, glm::degrees(orbit.ascNodeLongitude - 3 * PI / 2), axisY);
    Model = glm::rotate(Model, glm::degrees(orbit.inclination), axisZ);
    Model = glm::rotate(Model, glm::degrees(orbit.periapsisArg - PI / 2), axisY);
    glm::vec3 shiftVec = glm::vec3(-orbit.eccentricity * orbit.semiMajorAxis * distanceScale, 0.0f, 0.0f);
    Model = glm::translate(Model, shiftVec);

    return Model;
}