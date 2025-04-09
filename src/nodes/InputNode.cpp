#include "InputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>

InputNode::InputNode(int id, const std::string& path, const std::string& name) : Node(id, name.empty() ? std::filesystem::path(path).filename().string() : name), filepath(path) {}

GLuint matToTexture(const cv::Mat& mat) {
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

void InputNode::process() {
    image = cv::imread(filepath);
    if (image.empty()) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
    }

    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

cv::Mat InputNode::getOutput() const {
    return image;
}