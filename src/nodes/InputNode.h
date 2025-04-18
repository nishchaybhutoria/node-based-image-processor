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
    InputNode(int id, const std::string& defaultPath = "", const std::string& name = "Input");

    void process() override;
    cv::Mat getOutput() const override;
    void renderPropertiesUI() override;
    GLuint getTextureID() const { return textureID; }
    void preview() override;

    ~InputNode() override {
        if (textureID) glDeleteTextures(1, &textureID);
    }
};