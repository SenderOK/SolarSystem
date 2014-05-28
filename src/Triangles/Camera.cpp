#include "Camera.h"
#include "commons.h"

#include <GL/freeglut.h>
#include <iostream>

Camera::Camera() : 
    eye(0.0f, 20.0f, 200.0f),
    frontMovementSpeed(0.0f),
    sideMovementSpeed(0.0f),
    upKey(false),
    downKey(false),
    rightKey(false),
    leftKey(false),
    maxSpeed(10.0f),
    acceleration(5.0f),
    cnt(0)
{
    updateDirections();
}

Camera::~Camera()
{
}

void Camera::setView(glm::vec3 position, glm::vec3 aim)
{
    eye = position;

    forward = glm::normalize(aim - position);

    theta = std::acos(forward.y);
    phi = std::atan2(-forward.z, forward.x);
    if (phi < -PI / 2 + 0.01) {
        phi += 2 * PI;
    }
    
    mouseX = -(phi - (3 * PI / 2)) * windowWidth / (2 * PI);
    mouseY = windowHeight * theta / PI;

    glutWarpPointer(int(mouseX), int(mouseY));
}

void Camera::updateDirections()
{
    forward = glm::vec3(sin(theta) * cos(phi), cos(theta), -sin(phi) * sin(theta));
    right = glm::vec3(cos(phi - PI / 2), 0.0f, -sin(phi - PI / 2));
    up = glm::cross(right, forward);
}

glm::mat4x4 Camera::getRotationMatrix()
{
    phi = PI / 2 + PI * float(windowWidth / 2 - mouseX) / (windowWidth / 2);
	theta = PI / 2 - (PI / 2 - 0.01f) * float(windowHeight / 2 - mouseY) / (windowHeight / 2);

    updateDirections();

    return glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f) + forward, up);
}

glm::mat4x4 Camera::getViewMatrix()
{
    int newTimeSinceStart = glutGet(GLUT_ELAPSED_TIME); 
    float deltaTime = float(std::max(newTimeSinceStart - timeSinceStart, 1)) / 1000;
    
    timeSinceStart = newTimeSinceStart;

    if (rightKey) {
        if (sideMovementSpeed < 0)
            sideMovementSpeed = std::min(maxSpeed, sideMovementSpeed + 2 * acceleration * deltaTime);
        else 
            sideMovementSpeed = std::min(maxSpeed, sideMovementSpeed + acceleration * deltaTime);
    }
   
    if (leftKey) {
        if (sideMovementSpeed > 0)
            sideMovementSpeed = std::max(-maxSpeed, sideMovementSpeed - 2 * acceleration * deltaTime);
        else 
            sideMovementSpeed = std::max(-maxSpeed, sideMovementSpeed - acceleration * deltaTime);
    }

    if (!rightKey && !leftKey) {
        if (sideMovementSpeed < 0)
            sideMovementSpeed = std::min(0.0f, sideMovementSpeed + acceleration * deltaTime);
        if (sideMovementSpeed > 0)
            sideMovementSpeed = std::max(0.0f, sideMovementSpeed - acceleration * deltaTime);
    }

    if (upKey) {
        if (frontMovementSpeed < 0)
            frontMovementSpeed = std::min(maxSpeed, frontMovementSpeed + 2 * acceleration * deltaTime);
        else
            frontMovementSpeed = std::min(maxSpeed, frontMovementSpeed + acceleration * deltaTime);
    }
    if (downKey) {
        if (frontMovementSpeed > 0)
            frontMovementSpeed = std::max(-maxSpeed, frontMovementSpeed - 2 * acceleration * deltaTime);
        else
            frontMovementSpeed = std::max(-maxSpeed, frontMovementSpeed - acceleration * deltaTime);
    }

    if (!upKey && !downKey) {
        if (frontMovementSpeed < 0)
            frontMovementSpeed = std::min(0.0f, frontMovementSpeed + acceleration * deltaTime);
        if (frontMovementSpeed > 0)
            frontMovementSpeed = std::max(0.0f, frontMovementSpeed - acceleration * deltaTime);
    }    

    phi = PI / 2 + PI * float(windowWidth / 2 - mouseX) / (windowWidth / 2);
	theta = PI / 2 - (PI / 2 - 0.01f) * float(windowHeight / 2 - mouseY) / (windowHeight / 2);

    updateDirections();
    
    eye += forward * deltaTime * frontMovementSpeed;
    eye += right * deltaTime * sideMovementSpeed;
    
    return glm::lookAt(eye, eye + forward, up);
}

void Camera::processKeyPress(int key, int mx, int my)
{
    switch(key) {
    case GLUT_KEY_UP:
        upKey = true;
        break;
    case GLUT_KEY_DOWN:
        downKey = true;
        break;
    case GLUT_KEY_RIGHT:
        rightKey = true;
        break;
    case GLUT_KEY_LEFT:
        leftKey = true;
        break;
    default:
        break;
    }
}

void Camera::processKeyUp(int key, int mx, int my)
{
    switch(key) {
    case GLUT_KEY_UP:
        upKey = false;
        break;
    case GLUT_KEY_DOWN:
        downKey = false;
        break;
    case GLUT_KEY_RIGHT:
        rightKey = false;
        break;
    case GLUT_KEY_LEFT:
        leftKey = false;
        break;
    default:
        break;
    }
}

const int horBorderSize = (1 * windowWidth) / 100;

void Camera::processMouse(int x, int y)
{
    if (x > windowWidth - horBorderSize) {
        glutWarpPointer(horBorderSize, int(mouseY));
        mouseX = 0;
    } else if (x < horBorderSize) {
        glutWarpPointer(windowWidth - horBorderSize, int(mouseY));
        mouseX = float(windowWidth);
    } else {
        mouseX = float(x);
        mouseY = float(y);
    }
}

void Camera::setMaxMovementSpeed(float minDist)
{
    if (!realScale) {
        if (minDist > 500.0) {
            maxSpeed = 20.0;
            acceleration = 5.0;
        } else {
            maxSpeed = std::max(minDist / 2, 10.0f);
            acceleration = std::max(minDist, 10.0f);
        }
    } else {
        if (minDist > 41000) {
            maxSpeed = 100.0f;
            acceleration = 10.0f;
        } else if (minDist > 90.0f) {
            maxSpeed = minDist / 3;
            acceleration = minDist / 3;
        } else {
            maxSpeed = 30.0f;
            acceleration = 10.0f;
        }

    }
}