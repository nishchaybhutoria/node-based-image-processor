#pragma once
#include "../core/Node.h"
#include <opencv2/opencv.hpp>
#include <GL/gl.h>

class BlurNode : public Node {
private:
    cv::Mat inputImage, outputImage;
    GLuint textureID = 0;

    int blurRadius = 5;
    bool directional = false; // false = uniform, true = horizontal only

public:
    BlurNode(int id, const std::string& name = "Blur");

    void setInputs(const std::vector<cv::Mat>& input) override;
    void process() override;
    cv::Mat getOutput() const override;
    void preview() override;
    void renderPropertiesUI() override;

    ~BlurNode() override {
        if (textureID) glDeleteTextures(1, &textureID);
    }
};
