//standard libraries
#include <iostream>
using namespace std;
//opengl headers
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GLU.h>

//opengl mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//functions for shader compilation and linking
#include "shaderhelper.h"
//object for drawing
#include "Planet.h"
#include "commons.h"
#include "Camera.h"
#include "PlanetParameters.h"

float time;
float timeSpeed = 1.0f;

float distanceScale = 15;
float radiusScale = 1.0f / 10000;

bool realScale = false;
bool orbitsOn = true;
bool userPause = false;

const int N_SATELLITES = 6;
const int N_PLANETS = 10;

Camera camera;

Planet *planets;
Planet *stars;
Planet *satellites;
const int satellitePlanet[N_SATELLITES] = {3, 5, 5, 5, 5, 6};
const int materialPlanetReal[N_PLANETS + N_SATELLITES] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // planets
0, 0, 0, 0, 0, 0}; //satellites

const int materialPlanetSchematic[N_PLANETS + N_SATELLITES] = 
{2, 1, 1, 1, 1, 5, 3, 3, 3, 3, // planets
1, 4, 4, 4, 4, 4}; //satellites

//struct for loading shaders
ShaderProgram planetShader;
ShaderProgram simpleShader;
ShaderProgram simpleTextureShader;

//window size
int windowWidth = 1024;
int windowHeight = 768;

//last mouse coordinates
int mouseX, mouseY;

//matrices
glm::mat4x4 modelViewMatrix;
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewProjectionMatrix;
glm::mat4x4 normalMatrix;

/////////////////////////////////////////////////////////////////////
///is called when program starts
void init()
{
    //initialize shader programs
    planetShader.init("Vertex.vert", "Fragment.frag");
    simpleShader.init("Simple.vert", "Simple.frag");
    simpleTextureShader.init("SimpleTexture.vert", "SimpleTexture.frag");
    
    //enable texturing and zero slot
    glEnable(GL_TEXTURE0);

    //initialize planets and sun
    planets = new Planet[N_PLANETS];
    for (int i = 0; i < N_PLANETS; ++i) {
        if (i == 0) {
            planets[i].invertNormals();
        }
        planets[i].initGLBuffers(planetShader.programObject, "pos", "nor", "tex");
        planets[i].initTexture(1, &planetTextures[i]);
        planets[i].initPlanetParameters(radiiSchematic[i], rotationSpeeds[i], orbitalPeriods[i], meanAnomalies[i], axialTilts[i]);
        planets[i].initOrbitParameters(semiMajorAxesSchematic[i], eccentricities[i], inclinations[i], periapsisArgs[i], ascendingNodesLongs[i]);
    }

    // initalize planet's orbits
    for (int i = 1; i < N_PLANETS; ++i) {
        planets[i].orbit.initGLBuffers(simpleShader.programObject, "pos", meanAnomalies[i]);
    }

    //initialize stars
    stars = new Planet[1];
    stars->initGLBuffers(simpleTextureShader.programObject, "pos", "nor", "tex");
    stars->initTexture(5, starTextures);
    
    //initalize satellites
    satellites = new Planet[N_SATELLITES];
    for (int i = 0; i < N_SATELLITES; ++i) {
        satellites[i].initGLBuffers(planetShader.programObject, "pos", "nor", "tex");
        satellites[i].initTexture(1, &satelliteTextures[i]);

        int idx = i + N_PLANETS;
        satellites[i].initPlanetParameters(radiiSchematic[idx], rotationSpeeds[idx], orbitalPeriods[idx], meanAnomalies[idx], axialTilts[idx]);
        satellites[i].initOrbitParameters(semiMajorAxesSchematic[idx], eccentricities[idx], inclinations[idx], periapsisArgs[idx], ascendingNodesLongs[idx]);

        satellites[i].orbit.initGLBuffers(simpleShader.programObject, "pos", meanAnomalies[idx]);
    }

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

/////////////////////////////////////////////////////////////////////
///called when window size is changed
void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    //set viewport to match window size
    glViewport(0, 0, width, height);
  
    float fieldOfView = 45.0f;
    float aspectRatio = float(width)/float(height);
    float zNear = 0.1f;
    float zFar = 200000.0f;
    //set projection matrix
    projectionMatrix = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

////////////////////////////////////////////////////////////////////
///actions for single frame
void display()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // let there be light!
    glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 lightView;
    
    int locMV, locN, locP, locV, locLight, locLightView, locAmb, locDiff, locSpec, locShin, loca0, loca1, loca2;
    float amb, diff, spec, shininess, a0, a1, a2;

    glUseProgram(planetShader.programObject);
    locMV = glGetUniformLocation(planetShader.programObject, "modelViewMatrix");
    locN = glGetUniformLocation(planetShader.programObject,"normalMatrix");
    locP = glGetUniformLocation(planetShader.programObject, "modelViewProjectionMatrix");
    locV = glGetUniformLocation(planetShader.programObject, "viewMatrix");
    locLight = glGetUniformLocation(planetShader.programObject, "lightPosition");
    locLightView = glGetUniformLocation(planetShader.programObject, "lightViewPosition");

    locAmb = glGetUniformLocation(planetShader.programObject, "materialAmbient");
    locDiff = glGetUniformLocation(planetShader.programObject, "materialDiffuse");
    locSpec = glGetUniformLocation(planetShader.programObject, "materialSpecular");
    locShin = glGetUniformLocation(planetShader.programObject, "materialShininess");
    loca0 = glGetUniformLocation(planetShader.programObject, "a0");
    loca1 = glGetUniformLocation(planetShader.programObject, "a1");
    loca2 = glGetUniformLocation(planetShader.programObject, "a2");
    
    glm::mat4x4 modelMatrix, viewMatrix;

    const int *materialPlanet = realScale ? materialPlanetReal : materialPlanetSchematic;
    //_____________________________________________________________________________________________

    float minDist = 1e20f;
    //now we draw the planets
    for (int i = 0; i < N_PLANETS; ++i) {
        viewMatrix = camera.getViewMatrix();
        modelMatrix = planets[i].getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(modelViewMatrix);
        modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
        amb = materials[materialPlanet[i]][0];
        diff = materials[materialPlanet[i]][1];
        spec = materials[materialPlanet[i]][2];
        shininess = materials[materialPlanet[i]][3];
        a0 = materials[materialPlanet[i]][4];
        a1 = materials[materialPlanet[i]][5];
        a2 = materials[materialPlanet[i]][6];

        minDist = std::min(glm::length(glm::vec3(modelViewMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))), minDist);

        lightView = glm::vec3(viewMatrix * glm::vec4(lightPosition, 1.0));

        glUniformMatrix4fv(locMV,1,0,glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(locN,1,0,glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));
        glUniformMatrix4fv(locV,1,0,glm::value_ptr(viewMatrix));

        glUniform3f(locLight, lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(locLightView, lightView.x, lightView.y, lightView.z);
        glUniform3f(locAmb, amb, amb, amb);
        glUniform3f(locDiff, diff, diff, diff);
        glUniform3f(locSpec, spec, spec, spec);
        glUniform1f(locShin, shininess);
        glUniform1f(loca0, a0);
        glUniform1f(loca1, a1);
        glUniform1f(loca2, a2);
 
        planets[i].Draw();
    }

    // now we are going to draw some satellites
    for (int i = 0; i < N_SATELLITES; ++i) {
        viewMatrix = camera.getViewMatrix();
        modelMatrix = planets[satellitePlanet[i]].getSatelliteModelMatrix() * satellites[i].getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(modelViewMatrix);
        modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

        int idx = i + N_PLANETS;
        amb = materials[materialPlanet[idx]][0];
        diff = materials[materialPlanet[idx]][1];
        spec = materials[materialPlanet[idx]][2];
        shininess = materials[materialPlanet[idx]][3];
        a0 = materials[materialPlanet[idx]][4];
        a1 = materials[materialPlanet[idx]][5];
        a2 = materials[materialPlanet[idx]][6];

        lightView = glm::vec3(viewMatrix * glm::vec4(lightPosition, 1.0));

        glUniformMatrix4fv(locMV,1,0,glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(locN,1,0,glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));
        glUniformMatrix4fv(locV,1,0,glm::value_ptr(viewMatrix));

        glUniform3f(locLight, lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(locLightView, lightView.x, lightView.y, lightView.z);
        glUniform3f(locAmb, amb, amb, amb);
        glUniform3f(locDiff, diff, diff, diff);
        glUniform3f(locSpec, spec, spec, spec);
        glUniform1f(locShin, shininess);
        glUniform1f(loca0, a0);
        glUniform1f(loca1, a1);
        glUniform1f(loca2, a2);

        satellites[i].Draw();
    }

    if (orbitsOn) {
        // let's draw the orbits of the planets
        glUseProgram(simpleShader.programObject);
        for (int i = 1; i < N_PLANETS; ++i) {
            viewMatrix = camera.getViewMatrix();
            modelMatrix = planets[i].getOrbitModelMatrix() * planets[i].orbit.getModelMatrix();
            modelViewMatrix = viewMatrix * modelMatrix;
            modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

            locP = glGetUniformLocation(simpleShader.programObject,"modelViewProjectionMatrix");
            glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));

            planets[i].orbit.Draw();
        }

        // let's draw the orbits of the satellites
        for (int i = 0; i < N_SATELLITES; ++i) {
            viewMatrix = camera.getViewMatrix();
            modelMatrix = planets[satellitePlanet[i]].getSatelliteModelMatrix() * satellites[i].getOrbitModelMatrix() * 
                            satellites[i].orbit.getModelMatrix();
            modelViewMatrix = viewMatrix * modelMatrix;
            modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

            locP = glGetUniformLocation(simpleShader.programObject,"modelViewProjectionMatrix");
            glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));

            satellites[i].orbit.Draw();
        }
    }

    // we draw the stars    
    glUseProgram(simpleTextureShader.programObject);
    locMV = glGetUniformLocation(simpleTextureShader.programObject, "modelViewMatrix");
    locN = glGetUniformLocation(simpleTextureShader.programObject,"normalMatrix");
    locP = glGetUniformLocation(simpleTextureShader.programObject, "modelViewProjectionMatrix");

    modelMatrix = glm::scale(glm::mat4x4(1.0), glm::vec3(100000.0f, 100000.0f, 100000.0f));
    viewMatrix = camera.getRotationMatrix();
    modelViewMatrix = viewMatrix * modelMatrix;
    normalMatrix = glm::inverseTranspose(modelViewMatrix);
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(locMV,1,0,glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(locN,1,0,glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));

    stars[0].Draw();

    //_____________________________________________________________________________________________

    glutSwapBuffers();
    time = time + timeSpeed * (1 - userPause);
    camera.setMaxMovementSpeed(minDist);
}

//////////////////////////////////////////////////////////////////////////
///IdleFunction
void update()
{
    //make animation
    glutPostRedisplay();
}

void changeMode()
{
    const float *axis;
    const float *radius;

    if (realScale) {
        radiusScale = 1.0f / 10000;
        distanceScale = 15;
        axis = semiMajorAxesSchematic;
        radius = radiiSchematic;
        camera.setView(glm::vec3(-100.0f, 75.0f, 350.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    } else {
        radiusScale = 1000.0f / AU;
        distanceScale = 1000;
        axis = semiMajorAxes;
        radius = radii;
        camera.setView(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }
        
    for (int i = 0; i < N_PLANETS; ++i) {
        planets[i].initPlanetParameters(radius[i], rotationSpeeds[i], orbitalPeriods[i], meanAnomalies[i], axialTilts[i]);
        planets[i].initOrbitParameters(axis[i], eccentricities[i], inclinations[i], periapsisArgs[i], ascendingNodesLongs[i]);
        planets[i].orbit.initGLBuffers(simpleShader.programObject, "pos", meanAnomalies[i]);
    }
    for (int i = 0; i < N_SATELLITES; ++i) {
        int idx = i + N_PLANETS;
        satellites[i].initPlanetParameters(radius[idx], rotationSpeeds[idx], orbitalPeriods[idx], meanAnomalies[idx], axialTilts[idx]);
        satellites[i].initOrbitParameters(axis[idx], eccentricities[idx], inclinations[idx], periapsisArgs[idx], ascendingNodesLongs[idx]);
        satellites[i].orbit.initGLBuffers(simpleShader.programObject, "pos", meanAnomalies[idx]);
    }      
}

/////////////////////////////////////////////////////////////////////////
///is called when key on keyboard is pressed
void keyboard(unsigned char key, int mx, int my)
{
    int cnt = 0;
    switch(key) {
    case ' ':
        userPause = !userPause;
        break;
    case 27:        //ESC key
        glutLeaveMainLoop();
        break;
    case 13:       //Enter Key
        changeMode();
        realScale = !realScale;
        break;      
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        stars->switchTexture(key - '0' - 1);
        break;
    case '=':
        timeSpeed = std::min(timeSpeed * 1.2f, 90000.0f);
        break;
    case '-':
        timeSpeed = std::max(timeSpeed / 1.2f, 0.01f);
        break;
    case 'o':
        orbitsOn = !orbitsOn;
        break;
    case 'v':
        ++cnt;
    case 'm':
        ++cnt;
    case 'e':
        ++cnt;
    case 'a':
        ++cnt;
    case 'j':
        ++cnt;
    case 's':
        ++cnt;
    case 'u':
        ++cnt;
    case 'p':
        ++cnt;
    case 'n':
        {
            glm::vec3 planetPos = glm::vec3(planets[9 - cnt].getModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            camera.setView( planetPos + glm::vec3(0.0f, 0.0f, (realScale) ? 1.0f : 10.0f), planetPos);
        }
        break;  
    default:
        break;
    }
}

void specialKeyboard(int key, int mx, int my)
{
    switch(key) {
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
    case GLUT_KEY_RIGHT:
    case GLUT_KEY_LEFT:
        camera.processKeyPress(key, mx, my);
        break;
    default:
        break;
    }
}

void specialKeyboardUp(int key, int mx, int my)
{
    switch(key) {
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
    case GLUT_KEY_RIGHT:
    case GLUT_KEY_LEFT:
        camera.processKeyUp(key, mx, my);
        break;
    default:
        break;
    }
}


void mouseMove(int x, int y)
{
    camera.processMouse(x, y);
}

////////////////////////////////////////////////////////////////////////
///this function is used in case of InitializationError
void emptydisplay()
{
}

////////////////////////////////////////////////////////////////////////
///entry point
int main (int argc, char* argv[])
{
  glutInit(&argc, argv);
#ifdef __APPLE__
  glutInitDisplayMode( GLUT_2_1_CORE_PROFILE| GLUT_RGBA | GLUT_DOUBLE );
#else
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
  glutInitContextVersion (2, 1);  
  glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
  glewExperimental = GL_TRUE;
#endif
  glutCreateWindow("Model of the Solar System");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape); 
  glutReshapeWindow(windowWidth,windowHeight);
  glutIdleFunc(update);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyboard);
  glutSpecialUpFunc(specialKeyboardUp);

  glutPassiveMotionFunc(mouseMove);

    glewInit();
    const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION );
    cout << "GLSL Version: " << slVer << endl;

    try
    {
        init();
    }
    catch (const char *str)
    {
        cout << "Error During Initialiation: " << str << endl;
        delete planets;
        //start main loop with empty screen
        glutDisplayFunc(emptydisplay);
        glutMainLoop();
        return -1;
    }
    camera.setView(glm::vec3(-100.0f, 75.0f, 350.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    glutSetCursor(GLUT_CURSOR_NONE);
    try
    {
        glutMainLoop();
    }
    catch (const char *str)
    {
        cout << "Error During Main Loop: " << str << endl;
    }
    //release memory
    delete planets;
    delete stars;
    delete satellites;
    glDeleteProgram(planetShader.programObject);
    glDeleteProgram(simpleShader.programObject);
    glDeleteProgram(simpleTextureShader.programObject);
    return 0;
}