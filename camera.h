#pragma once
// Code from https://www.learnopengl.com

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLES3/gl3.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Defines several possible options for camera angles.
enum View_Angles
{
    FRONT,
    BACK,
    TOP,
    FIRSTPERSON
};

// Default camera values
constexpr float YAW = 90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 25.0f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    int viewAngle = 0;

    // constructor with vectors
    Camera(glm::vec3 positionIn = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yawIn = YAW, float pitchIn = PITCH)
        :front(glm::vec3(0.0f, 0.0f, -1.0f))
        ,movementSpeed(SPEED)
        ,mouseSensitivity(SENSITIVITY)
        ,zoom(ZOOM)
    {
        position = positionIn;
        worldUp = up;
        yaw = yawIn;
        pitch = pitchIn;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yawIn, float pitchIn)
        :front(glm::vec3(0.0f, 0.0f, -1.0f))
        ,movementSpeed(SPEED)
        ,mouseSensitivity(SENSITIVITY)
        ,zoom(ZOOM)
    {
        position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw = yawIn;
        pitch = pitchIn;
        updateCameraVectors();
    }

    void SetPosition(const glm::vec3 pos)
    {
        position = pos;
        updateCameraVectors();
    }

    void SetRotation(const float yawIn, const float pitchIn)
    {
        yaw = yawIn;
        pitch = pitchIn;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, up); }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(const Camera_Movement direction, const float deltaTime)
    {
        const float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)  { position += front * velocity; }
        if (direction == BACKWARD) { position -= front * velocity; }
        if (direction == LEFT)     { position -= right * velocity; }
        if (direction == RIGHT)    { position += right * velocity; }
        if (direction == DOWN)     { position -= up    * velocity; }
        if (direction == UP)    { position += up    * velocity; }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, const GLboolean constrainPitch = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (yaw >= 360.0f) { yaw = 0.0f; }
        if (yaw < 0.0f) { yaw = 359.9f; }

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (pitch > 89.0f)  { pitch = 89.0f; }
            if (pitch < -89.0f) { pitch = -89.0f; }
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        zoom -= (float)yoffset;
        if (zoom < 1.0f)  { zoom = 1.0f; }
        if (zoom > 45.0f) { zoom = 45.0f; }
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 frontNew;
        frontNew.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        frontNew.y = static_cast<float>(sin(glm::radians(pitch)));
        frontNew.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
        front = glm::normalize(frontNew);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
};