#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

struct OrbitVertexData
{
	 glm::vec3 pos;
};

class Orbit
{
    OrbitVertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

    unsigned int vbo;    // VertexBufferObject one for VertexData
	unsigned int vao;    // one VertexArrayObject

    void buildOrbit(int nPoints, float MA0);
public:
    float semiMajorAxis;
    float semiMinorAxis;
    float eccentricity;
    float inclination;
    float periapsisArg;
    float ascNodeLongitude;

    Orbit();
    ~Orbit();
    void initGLBuffers(unsigned int programId, const char* posName, float MA0);
    glm::mat4x4 getModelMatrix();
    void Draw();
};