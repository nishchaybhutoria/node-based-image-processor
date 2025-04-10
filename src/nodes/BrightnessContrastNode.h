#pragma once
#include <opencv2/opencv.hpp>
#include "../core/Node.h"
#include <GL/gl.h>

class BrightnessContrastNode : public Node {
private:
    cv::Mat inputImage, outputImage;
    GLuint textureID = 0;

    float brightness = -100;
    float contrast = 100;

public:
    BrightnessContrastNode(int id, const std::string& name = "Brightness/Contrast");

    void setInput(const cv::Mat& input);
    void process() override;

    cv::Mat getOutput() const override;
    GLuint getTextureID() const { return textureID; }
    void preview() override;

    ~BrightnessContrastNode() override {
        if (textureID) glDeleteTextures(1, &textureID);
    }
};