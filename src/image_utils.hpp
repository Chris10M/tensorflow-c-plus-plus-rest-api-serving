#ifndef H_IMAGE_UTILS
#define H_IMAGE_UTILS

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


namespace ImageUtils {
    // vgg16 imagenet preprocessing function
    cv::Mat preprocess_image(cv::Mat image) {
        cv::Mat float_image;
        image.convertTo(float_image, CV_32FC3);

        float mean[] = {103.939, 116.779, 123.68};

        for (int y = 0; y < float_image.cols; ++y) {      
        for (int x = 0; x < float_image.rows; ++x) {

            cv::Vec3f pix_value = float_image.at<cv::Vec3f>(y, x);

            float red = pix_value.val[0] - mean[0];
            float green = pix_value.val[1] - mean[1];
            float blue = pix_value.val[2] - mean[2];
            
            float_image.at< cv::Vec3f>(y, x) = cv::Vec3f(blue, green, red);
            }
        }

        return float_image;
    }

    std::vector<float> cv_mat_to_float_array(cv::Mat float_image) {
        std::vector<float> array;

        array.assign((float*)float_image.datastart,
                    (float*)float_image.dataend);

        return array;
    }

    template <typename T>
    int argmax(std::vector<T> data) {
        int arg_max = 0;

        if (!data.size())
            return arg_max;

        T max_value = data[0];
        for (int i = 1; i < data.size(); ++i) {
            max_value = std::max(data[i], max_value);
            
            if (max_value == data[i])
                arg_max = i;
        }

        return arg_max;
    }

    cv::Rect rect_union(cv::Rect a, cv::Rect b) {
        auto x = std::min(a.x, b.x);
        auto y = std::min(a.y, b.y);
                
        auto w = std::max(a.x + a.width, b.x + b.width) - x;
        auto h = std::max(a.y + a.height, b.y + b.height) - y;

        return cv::Rect(x, y, w, h);
     }
}

#endif

