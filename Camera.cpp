#include "Camera.hpp"

namespace gps {

    double pitchG = 0.0f, yawG = 0.0f;
    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f,1.0f,0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    void Camera::prezentare() {
        //TODO
        yawG = 0.0f;
        pitchG= -0.0f;
        cameraPosition = glm::vec3(0.0f, 2.0f, 2.0f);
        cameraUpDirection = glm::vec3(0.0f, 1.f, 0.0f);
        cameraTarget.x = (float)sin(0.0f) * cos(-0.45f);
        cameraTarget.z = - (float)cos(0.0f) * cos(-0.45f);
        cameraTarget.y = (float)sin(-0.45f);
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
        return;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction)
        {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;
        default:
            break;
        }

    }
    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(double pitch, double yaw) {

        //glm::vec3 front;
        //front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        //front.y = sin(glm::radians(pitch));
        //front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        // cameraTarget.y = cameraPosition.y + sin(pitchG);
        //  cameraTarget.z = cos(pitchG);
        // Recalculate the right and up vectors after rotation
        yawG += yaw;
        pitchG += pitch;
        //printf("%f\n", pitchG);
        if (pitchG > 1.568f)
            pitchG = 1.568f;
        else if (pitchG < -1.568f)
            pitchG = -1.568f;
        
        cameraTarget.x = cameraPosition.x + (float)sin(yawG) * cos(pitchG);
        cameraTarget.z = cameraPosition.z - (float)cos(yawG) * cos(pitchG);
        cameraTarget.y = cameraPosition.y + (float)sin(pitchG);

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
