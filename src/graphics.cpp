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

  FFI_PLUGIN_EXPORT int process_image_gaussian_blur(const char *image_path, const float *points, int num_points, int kernel)
  {
    kernel = kernel * 2 + 1;
    LOG(INFO) << "process_image_gaussian_blur " << std::endl;
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
    cv::Mat blur_image;
    cv::GaussianBlur(image, blur_image, cv::Size(kernel, kernel), 0, 0);

    // Blend the original image and the grayscale image using the mask
    cv::Mat result;
    image.copyTo(result);
    blur_image.copyTo(result, mask);

    // Save the processed image
    cv::imwrite(image_path, result);

    LOG(INFO) << "Process image blur done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_edge_detection(const char *image_path, const float *points, int num_points, int kernel)
  {
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
    cv::Mat invert_mask;
    cv::bitwise_not(mask, invert_mask);

    // Convert the image to grayscale
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

    // Apply Sobel operator
    cv::Mat grad_x, grad_y;
    cv::Sobel(gray_image, grad_x, CV_16S, 1, 0, kernel);
    cv::Sobel(gray_image, grad_y, CV_16S, 0, 1, kernel);

    // Convert back to CV_8U
    cv::Mat abs_grad_x, abs_grad_y;
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    // Combine gradients
    cv::Mat grad;
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // Apply Laplacian operator
    cv::Mat laplacian;
    cv::Laplacian(grad, laplacian, CV_16S, kernel);
    cv::convertScaleAbs(laplacian, laplacian);

    // Create a result image initialized with the original image
    cv::Mat result = image.clone();

    // Copy the edge-detected result to the result image using the mask
    laplacian.copyTo(result, mask);

    image.copyTo(result, invert_mask);
    // Save the processed image
    cv::imwrite(image_path, result);

    std::cout << "Process image edge detection done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_enhanced_edge_detection(const char *image_path, const float *points, int num_points, int kernel)
  {
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

    // Apply Sobel operator
    cv::Mat grad_x, grad_y;
    cv::Sobel(gray_image, grad_x, CV_16S, 1, 0, kernel);
    cv::Sobel(gray_image, grad_y, CV_16S, 0, 1, kernel);

    // Convert back to CV_8U
    cv::Mat abs_grad_x, abs_grad_y;
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    // Combine gradients
    cv::Mat grad;
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // Apply Laplacian operator
    cv::Mat laplacian;
    cv::Laplacian(grad, laplacian, CV_16S, kernel);
    cv::convertScaleAbs(laplacian, laplacian);

    // Blend the original image and the edge-detected image using the mask
    cv::Mat result;
    image.copyTo(result);
    laplacian.copyTo(result, mask);

    // Save the processed image
    cv::imwrite(image_path, result);

    std::cout << "Process image enhanced edge detection done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_gradient_magnitude(const char *image_path, const float *points, int num_points, int kernel)
  {
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

    // Apply Sobel operator
    cv::Mat grad_x, grad_y;
    cv::Sobel(gray_image, grad_x, CV_16S, 1, 0, kernel);
    cv::Sobel(gray_image, grad_y, CV_16S, 0, 1, kernel);

    // Convert back to CV_8U
    cv::Mat abs_grad_x, abs_grad_y;
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    // Compute gradient magnitude
    cv::Mat grad_magnitude;
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad_magnitude);

    // Apply Laplacian operator
    cv::Mat laplacian;
    cv::Laplacian(grad_magnitude, laplacian, CV_16S, kernel);
    cv::convertScaleAbs(laplacian, laplacian);

    // Blend the original image and the gradient magnitude image using the mask
    cv::Mat result;
    image.copyTo(result);
    laplacian.copyTo(result, mask);

    // Save the processed image
    cv::imwrite(image_path, result);

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_sepia(const char *image_path, const float *points, int num_points, int kernel)
  {
    LOG(INFO) << "process_image_sepia " << std::endl;
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Could not open or find the image" << std::endl;
      return 1;
    }

    cv::Mat sepia_image = image.clone();

    // Create the transformation matrix explicitly
    cv::Mat sepia_kernel = (cv::Mat_<float>(3, 3) << 0.272, 0.534, 0.131,
                            0.349, 0.686, 0.168,
                            0.393, 0.769, 0.189);

    cv::transform(image, sepia_image, sepia_kernel);

    cv::imwrite(image_path, sepia_image);
    LOG(INFO) << "Process image sepia done!" << std::endl;

    return 0;
  }

FFI_PLUGIN_EXPORT int process_image_vignette(const char *image_path, const float *points, int num_points, int kernel)
{
  LOG(INFO) << "process_image_vignette " << std::endl;
  cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
  if (image.empty())
  {
    std::cerr << "Could not open or find the image" << std::endl;
    return 1;
  }

  // Convert image to float type
  cv::Mat image_float;
  image.convertTo(image_float, CV_32F);

  cv::Mat vignette_image = image_float.clone();
  cv::Mat mask = cv::Mat::zeros(image.size(), CV_32F);
  cv::Point center = cv::Point(image.cols / 2, image.rows / 2);
  double max_dist = std::sqrt(center.x * center.x + center.y * center.y);

  for (int y = 0; y < image.rows; y++)
  {
    for (int x = 0; x < image.cols; x++)
    {
      double dist = std::sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));
      mask.at<float>(y, x) = 1.0 - (dist / max_dist);
    }
  }

  cv::Mat channels[3];
  cv::split(vignette_image, channels);
  for (int i = 0; i < 3; i++)
  {
    channels[i] = channels[i].mul(mask);
  }
  cv::merge(channels, 3, vignette_image);

  // Convert vignette_image back to 8-bit type before saving
  vignette_image.convertTo(vignette_image, CV_8UC3);
  cv::imwrite(image_path, vignette_image);
  LOG(INFO) << "Process image vignette done!" << std::endl;

  return 0;
}


  FFI_PLUGIN_EXPORT int process_image_sharpen(const char *image_path, const float *points, int num_points, int )
  {
    LOG(INFO) << "process_image_sharpen " << std::endl;
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Could not open or find the image" << std::endl;
      return 1;
    }

    cv::Mat sharpened_image;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    cv::filter2D(image, sharpened_image, image.depth(), kernel);

    cv::imwrite(image_path, sharpened_image);
    LOG(INFO) << "Process image sharpen done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_emboss(const char *image_path, const float *points, int num_points, int )
  {
    LOG(INFO) << "process_image_emboss " << std::endl;
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Could not open or find the image" << std::endl;
      return 1;
    }

    cv::Mat embossed_image;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
    cv::filter2D(image, embossed_image, image.depth(), kernel);

    cv::imwrite(image_path, embossed_image);
    LOG(INFO) << "Process image emboss done!" << std::endl;

    return 0;
  }

  FFI_PLUGIN_EXPORT int process_image_cartoon(const char *image_path, const float *points, int num_points, int kernel)
  {
    LOG(INFO) << "process_image_cartoon " << std::endl;
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty())
    {
      std::cerr << "Could not open or find the image" << std::endl;
      return 1;
    }

    cv::Mat gray, edges, cartoon;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, gray, 7);
    cv::adaptiveThreshold(gray, edges, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 9);
    cv::bilateralFilter(image, cartoon, 9, 75, 75);
    cv::bitwise_and(cartoon, cartoon, cartoon, edges);

    cv::imwrite(image_path, cartoon);
    LOG(INFO) << "Process image cartoon done!" << std::endl;

    return 0;
  }
}