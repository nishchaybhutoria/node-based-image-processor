#pragma once
#include <opencv2/opencv.hpp>
#include "../core/Node.h"
#include <GL/gl.h>

class BrightnessContrastNode : public Node {
private:
    cv::Mat inputImage, outputImage;
    GLuint textureID = 0;

    float brightness = 0;
    float contrast = 1;

public:
    BrightnessContrastNode(int id, const std::string& name = "Brightness/Contrast");

    void setInput(const cv::Mat& input) override;
    void process() override;
    cv::Mat getOutput() const override;
    GLuint getTextureID() const { return textureID; }
    void preview() override;
    void renderPropertiesUI() override;

    ~BrightnessContrastNode() override {
        if (textureID) glDeleteTextures(1, &textureID);
    }
};