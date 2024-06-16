#include "graphics.hpp"
#include <opencv2/opencv.hpp>

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
}
