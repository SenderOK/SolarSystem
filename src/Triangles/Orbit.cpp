#include "Orbit.h"
#include "commons.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

Orbit::Orbit() :
    dataCount(0),
    pData(0), 
    vao(0),
    vbo(0)
{
}

Orbit::~Orbit()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    if (pData)
        delete[] pData;
}

void Orbit::initGLBuffers(unsigned int programId, const char* posName, float MA0)
{
    buildOrbit(5000, MA0);

    if (vao == 0)
        glGenVertexArrays( 1, &vao );

    glBindVertexArray ( vao );

    if (vbo == 0)
        glGenBuffers ( 1, &vbo);
    
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glBufferData ( GL_ARRAY_BUFFER, dataCount * sizeof(OrbitVertexData), pData, GL_STATIC_DRAW );

    int	loc = glGetAttribLocation(programId, posName);
    if (loc>-1)
    {
        glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE, 0,(GLuint *)0);
        glEnableVertexAttribArray (loc);
    }
}

void Orbit::Draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, dataCount);
    glBindVertexArray(0);
}

void Orbit::buildOrbit(int nPoints, float MA0)
{
    if (pData) {
        delete[] pData;
        pData = 0;
    }

    dataCount = nPoints;
    pData = new OrbitVertexData [dataCount];
    float shift = 2 * PI / nPoints;
    for (int i = 0; i < nPoints; ++i) {
        /*pData[i].pos.x = semiMajorAxis * cos(shift * i);
        pData[i].pos.y = 0.0f;
        pData[i].pos.z = -semiMinorAxis * sin(shift * i);*/

        float meanAnomaly = MA0 + i * shift;

        float e = eccentricity;
        float trueAnomaly = meanAnomaly + (2 * e - 0.25 * pow(e, 3) + 5/96 * pow(e, 5)) * sin(meanAnomaly)
                                    + (5 / 4 * pow(e, 2) - 11/24 * pow(e, 4)) * sin(2 * meanAnomaly)
                                    + (13 / 12 * pow(e, 3) - 43/64 * pow(e, 5)) * sin(3 * meanAnomaly)
                                    + 103/96 * pow(e, 4) * sin(4 * meanAnomaly)
                                    + 1097/960 * pow(e, 5) * sin(5 * meanAnomaly);
        pData[i].pos.x = semiMajorAxis * cos(trueAnomaly);
        pData[i].pos.y = 0.0f;
        pData[i].pos.z = -semiMinorAxis * sin(trueAnomaly);
    }
}

glm::mat4x4 Orbit::getModelMatrix()
{
    return glm::scale(glm::mat4x4(1.0), glm::vec3(distanceScale));
}