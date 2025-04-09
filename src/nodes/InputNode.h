#pragma once
#include <opencv2/opencv.hpp>
#include "../core/Node.h"
#include <filesystem>
#include <GL/gl.h>

class InputNode : public Node {
private:
    cv::Mat image;
    std::string filepath;
    GLuint textureID = 0;

public:
    InputNode(int id, const std::string& path, const std::string& name = "");

    void process() override;
    cv::Mat getOutput() const override;

    GLuint getTextureID() const { return textureID; }
};