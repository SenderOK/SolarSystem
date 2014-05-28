#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Orbit.h"

//helper struct for Vertex
//contains position, normal and texture coordinates
struct VertexData
{
	 glm::vec3 pos;
	 glm::vec3 nor;
	 glm::vec2 tex;
};

//a planet for drawing
class Planet
{
	VertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

	unsigned int* pIndexes;	//pointer to indexes (list of vetrices)
	unsigned int indexesCount;
	
	unsigned int vbo[2];    // VertexBufferObject one for MeshVertexData, another for Indexes
	unsigned int vao;       // one VertexArrayObject
	float rotationVelocity; // should be negative for Venus, Uranus, Pluto
    float orbitalPeriod;
    float startMeanAnomaly;
    float axialTilt;
    float radius;

	unsigned int texId[5];
    unsigned int textureCount;
    unsigned int currTexture;

    unsigned int shaderId;

public:
    Orbit orbit;

	Planet();
	~Planet(void);
	//function for initialization
	void initGLBuffers(unsigned int programId, const char* posName,const char* norName,const char* texName);
	void initTexture(int count, const char **filenames);
    void initPlanetParameters(float _radius, float _rotationVelocity, float _orbitalPeriod, float _meanAnomaly, float _axialTilt);
	void initOrbitParameters(float a, float e, float i, float pArg, float ascNodeLong);
	//function for drawing
	void Draw();
    void invertNormals();
    void switchTexture(int mode);
	glm::mat4x4 getModelMatrix();
    glm::mat4x4 getSatelliteModelMatrix();
    glm::mat4x4 getOrbitModelMatrix();

private:
	// helper function builds the sphere
	void generateData(int nParallels, int nMeridians);

};