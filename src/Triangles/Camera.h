#include "commons.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class Camera
{
    //camera position
    glm::vec3 eye;
    //forward vector direction
    glm::vec3 forward;
    //up vector direction (head of observer)
    glm::vec3 up;
    //right vector direction
    glm::vec3 right;

    float frontMovementSpeed;
    float sideMovementSpeed;
    float maxSpeed;
    float acceleration;

    float phi;          // horizontal angle
    float theta;        // veritcal angle

    float mouseX, mouseY;
    float currX, currY, prevX, prevY;
    int cnt;

    int timeSinceStart;

    bool upKey;
    bool downKey;
    bool rightKey;
    bool leftKey;

    void updateDirections(); // update directions according to the angles

public:
    Camera();
    ~Camera();

    glm::mat4x4 getViewMatrix();
    glm::mat4x4 getRotationMatrix();
    void setMaxMovementSpeed(float speed);
    void setView(glm::vec3 position, glm::vec3 aim);
    void processKeyPress(int key, int mx, int my);
    void processKeyUp(int key, int mx, int my);
    void processMouse(int x, int y);
};