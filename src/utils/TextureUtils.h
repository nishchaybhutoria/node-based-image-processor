#pragma once
#include <opencv2/opencv.hpp>
#include <GL/gl.h>

inline GLuint matToTexture(const cv::Mat& mat) {
    if (mat.empty()) return 0;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum inputFormat = mat.channels() == 3 ? GL_BGR : GL_LUMINANCE;

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows,
        0, inputFormat, GL_UNSIGNED_BYTE, mat.ptr()
    );

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}