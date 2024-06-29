#include "graphics.hpp"
#include <opencv2/opencv.hpp>
#include "aixlog.hpp"

extern "C"
{

    FFI_PLUGIN_EXPORT int process_image_grabcut(const char *image_path, const float *points, int num_points, int iterations)
    {
        LOG(INFO) << "process_image_grabcut " << std::endl;
        cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
        if (image.empty())
        {
            std::cerr << "Could not open or find the image" << std::endl;
            return 1;
        }

        // Convert points to a vector of cv::Point
        std::vector<cv::Point> cv_points;
        for (int i = 0; i < num_points; i++)
        {
            cv_points.push_back(cv::Point(points[i * 2], points[i * 2 + 1]));
        }

        // Create a mask using the polygon defined by the points
        cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
        cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{cv_points}, cv::Scalar(cv::GC_PR_FGD));

        // Define background and foreground models
        cv::Mat bgdModel, fgdModel;

        // Apply GrabCut algorithm
        cv::grabCut(image, mask, cv::Rect(), bgdModel, fgdModel, iterations, cv::GC_INIT_WITH_MASK);

        // Create a binary mask of the foreground
        cv::Mat foreground_mask = (mask == cv::GC_FGD) | (mask == cv::GC_PR_FGD);

        // Create a white background
        cv::Mat white_background(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

        // Blend the original image with the white background using the foreground mask
        cv::Mat result;
        image.copyTo(result);
        white_background.copyTo(result, ~foreground_mask);

        // Save the processed image
        cv::imwrite(image_path, result);

        LOG(INFO) << "Process image grabcut with white background done!" << std::endl;

        return 0;
    }

    FFI_PLUGIN_EXPORT int process_image_grabcut_white_foreground(const char *image_path, const float *points, int num_points, int iterations)
    {
        LOG(INFO) << "process_image_grabcut_white_foreground " << std::endl;
        cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
        if (image.empty())
        {
            std::cerr << "Could not open or find the image" << std::endl;
            return 1;
        }

        // Convert points to a vector of cv::Point
        std::vector<cv::Point> cv_points;
        for (int i = 0; i < num_points; i++)
        {
            cv_points.push_back(cv::Point(points[i * 2], points[i * 2 + 1]));
        }

        // Create a mask using the polygon defined by the points
        cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
        cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{cv_points}, cv::Scalar(cv::GC_PR_FGD));

        // Define background and foreground models
        cv::Mat bgdModel, fgdModel;

        // Apply GrabCut algorithm
        cv::grabCut(image, mask, cv::Rect(), bgdModel, fgdModel, iterations, cv::GC_INIT_WITH_MASK);

        // Create a binary mask of the foreground
        cv::Mat foreground_mask = (mask == cv::GC_FGD) | (mask == cv::GC_PR_FGD);

        // Create a white foreground
        cv::Mat white_foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

        // Blend the white foreground with the original image using the foreground mask
        cv::Mat result;
        image.copyTo(result);
        white_foreground.copyTo(result, foreground_mask);

        // Save the processed image
        cv::imwrite(image_path, result);

        LOG(INFO) << "Process image grabcut with white foreground done!" << std::endl;

        return 0;
    }
}