#pragma once
#include <glm/glm.hpp>

//helper struct for Vertex
//contains position, normal and texture coordinates
struct VertexData
{
	 glm::vec3 pos;
	 glm::vec3 nor;
	 glm::vec2 tex;
};

//some object for drawing
class MyObject
{
	VertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

	unsigned int* pIndexes;	//pointer to indexes (list of vetrices) 
	unsigned int indexesCount;
	
	unsigned int vbo[2];//VertexBufferObject one for MeshVertexData, another for Indexes
	unsigned int vao;//one VertexArrayObject

public:
	MyObject(void);
	~MyObject(void);
	//function for initialization
	void initGLBuffers(unsigned int programId, const char* posName,const char* norName,const char* texName);
	//function for drawing
	void Draw();

private:
	//helper function generates two triangles
	void generateData();

};

