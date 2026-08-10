/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../include/HammerEngine/HammerEngine.h"

void HammerEngine::updateCameraDefault3D(){
    glfwPollEvents();

    glfwGetCursorPos(window, &mouseX, &mouseY);

    float velocity = cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPosition += cameraFront * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPosition -= cameraFront * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    }

    if (glfwGetKey(window,  GLFW_KEY_ESCAPE) == GLFW_PRESS){
        mouse_capture = false;
    }

    if (glfwGetKey(window,  GLFW_KEY_P) == GLFW_PRESS){
        mouse_capture = true;
    }

    if (mouse_capture) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCallback(mouseX, mouseY);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void HammerEngine::updateCameraDefaultGravety3D(bool fall){
    glfwPollEvents();

    glfwGetCursorPos(window, &mouseX, &mouseY);

    float velocity = cameraSpeed;



    // Convert yaw from degrees to radians
    float yawRad = yaw * 3.14159265f / 180.0f;

    // Forward direction (OpenGL-style: -Z is forward)
    float forwardX = sinf(yawRad);
    float forwardZ = -cosf(yawRad);  // NEGATIVE because -Z is forward

    // Right direction (yaw + 90 degrees)
    float rightX = sinf(yawRad + 3.14159265f / 2.0f);
    float rightZ = -cosf(yawRad + 3.14159265f / 2.0f);

    // W - forward
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPosition.x += forwardX * velocity;
        cameraPosition.z += forwardZ * velocity;
    }

    // S - backward
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPosition.x -= forwardX * velocity;
        cameraPosition.z -= forwardZ * velocity;
    }

    // A - strafe left (opposite of right)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPosition.x -= rightX * velocity;
        cameraPosition.z -= rightZ * velocity;
    }

    // D - strafe right
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPosition.x += rightX * velocity;
        cameraPosition.z += rightZ * velocity;
    }


    if (glfwGetKey(window,  GLFW_KEY_ESCAPE) == GLFW_PRESS){
        mouse_capture = false;
    }

    if (glfwGetKey(window,  GLFW_KEY_P) == GLFW_PRESS){
        mouse_capture = true;
    }

    if (mouse_capture) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCallback(mouseX, mouseY);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if(!fall){
        cameraPosition.y -= cameraFallSpeed;
    }
}

void HammerEngine::updateCameraDefault2D(){
    glfwPollEvents();

    float velocity = cameraSpeed;
    mouse_capture = false;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPosition.x += velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPosition.x -= velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPosition.y += velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPosition.y -= velocity;
    }

}
