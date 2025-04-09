#pragma once
#include <opencv2/opencv.hpp>
#include "../core/Node.h"
#include <GL/gl.h>

class OutputNode : public Node {
private:
    cv::Mat image;
    GLuint textureID = 0;

public:
    OutputNode(int id, const std::string& name = "Output");

    void setInput(const cv::Mat& input);
    void process() override;

    cv::Mat getOutput() const override;
    GLuint getTextureID() const { return textureID; }

    ~OutputNode() override {
        if (textureID) glDeleteTextures(1, &textureID);
    }
};