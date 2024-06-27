#include "graphics.hpp"
#include <opencv2/opencv.hpp>
#include "aixlog.hpp"

extern "C"
{
  // A very short-lived native function.
  //
  // For very short-lived functions, it is fine to call them on the main isolate.
  // They will block the Dart execution while running the native function, so
  // only do this for native functions which are guaranteed to be short-lived.
  FFI_PLUGIN_EXPORT int sum(int a, int b) { return a + b; }

  // A longer-lived native function, which occupies the thread calling it.
  //
  // Do not call these kind of native functions in the main isolate. They will
  // block Dart execution. This will cause dropped frames in Flutter applications.
  // Instead, call these native functions on a separate isolate.
  FFI_PLUGIN_EXPORT int sum_long_running(int a, int b)
  {
    // Simulate work.
#if _WIN32
    Sleep(5000);
#else
    usleep(5000 * 1000);
#endif
    return a + b;
  }

  FFI_PLUGIN_EXPORT int init()
  {

    AixLog::Severity aix_log_level = AixLog::Severity::info;
    ;

    auto cout_sink = std::make_shared<AixLog::SinkCout>(
        aix_log_level,
        "%Y-%m-%d %H-%M-%S.#ms [#severity] (#function) #message");

    auto native =
        std::make_shared<AixLog::SinkNative>("native_log", aix_log_level);

    AixLog::Log::init({cout_sink, native});

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image(const char *image_path)
  {
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Could not open or find the image" << std::endl;
      return 1;
    }

    // Example processing: Convert to grayscale
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

    // Save the processed image
    cv::imwrite(image_path, gray_image);

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_with_points(const char *image_path, const float *points, int num_points)
  {
    LOG(INFO) << "input path " << image_path << std::endl;
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      LOG(INFO) << "Could not open or find the image" << std::endl;
      return 1;
    }

    // Convert points to a vector of cv::Point
    std::vector<cv::Point> cv_points;
    for (int i = 0; i < num_points; i++)
    {
      cv_points.push_back(cv::Point(points[i * 2], points[i * 2 + 1]));
    }

    // Example processing: Draw a polygon around the points
    cv::polylines(image, cv_points, true, cv::Scalar(0, 255, 0), 2);

    // Save the processed image
    cv::imwrite(image_path, image);

    LOG(INFO) << "process image done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_gray_scale(const char *image_path, const float *points, int num_points)
  {

    LOG(INFO) << "process_image_gray_scale " << std::endl;
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
    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{cv_points}, cv::Scalar(255));

    // Convert the image to grayscale
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

    // Convert grayscale back to BGR for blending
    cv::Mat gray_bgr;
    cv::cvtColor(gray_image, gray_bgr, cv::COLOR_GRAY2BGR);

    // Blend the original image and the grayscale image using the mask
    cv::Mat result;
    image.copyTo(result);
    gray_bgr.copyTo(result, mask);

    // Save the processed image
    cv::imwrite(image_path, result);

    LOG(INFO) << "Process image done!" << std::endl;

    return 0;
  }
}