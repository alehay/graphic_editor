import 'package:flutter/material.dart';
import 'package:image_picker/image_picker.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:async';
import 'package:file_picker/file_picker.dart';
import 'dart:io';
import 'package:ffi/ffi.dart';
import 'package:graphics/graphics.dart' as graphics;
import 'dart:ffi' as ffi;
import 'dart:ui' as ui;
import 'package:image_size_getter/image_size_getter.dart';
import 'package:image_size_getter/file_input.dart';
import 'package:full_screen_image/full_screen_image.dart';
import 'dart:math';
import 'package:flutter_cache_manager/flutter_cache_manager.dart';
import 'package:gallery_saver/gallery_saver.dart';

void main() {
  int res = graphics.libGraphInit();
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class PointPainter extends CustomPainter {
  final List<Offset> points;
  final double scale;

  PointPainter(this.points, this.scale);

  @override
  void paint(Canvas canvas, ui.Size size) {
    final paint = Paint()
      ..color = Colors.green
      ..strokeCap = StrokeCap.round
      ..strokeWidth = 3.0 / scale;

    for (int i = 0; i < points.length - 1; i++) {
      canvas.drawLine(points[i] * scale, points[i + 1] * scale, paint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return true;
  }
}

Future<void> saveImage(File file) async {
  // Save to gallery
  final result = await GallerySaver.saveImage(file.path);

  if (result != null && result) {
    print("Image saved to gallery successfully");
  } else {
    print("Failed to save image to gallery");
  }
}

class _MyAppState extends State<MyApp> {
  File? _image;
  final ImagePicker _picker = ImagePicker();
  List<Offset> _points = [];
  final GlobalKey _imageKey = GlobalKey();
  double _scaleWidth = 1;
  double _scaleHeight = 1;
  double _offsetX = 0.0;
  double _offsetY = 0.0;
  bool _isDrawing = false; // State variable to track drawing mode

  double _scale = 1.0;
  final TransformationController _transformationController =
      TransformationController();

  Future<void> _pickImage(ImageSource source) async {
    var appDir = (await getTemporaryDirectory()).path;
    new Directory(appDir).delete(recursive: true);

    final pickedFile = await _picker.pickImage(source: source);
    if (pickedFile != null) {
      final imageFile = File(pickedFile.path);
      final tempDir = await getTemporaryDirectory();
      final copyImagePath = '${tempDir.path}/copy_image.jpg';

      // Copy the image to the new location
      final copiedImage = await imageFile.copy(copyImagePath);

      // Get the dimensions of the image
      final size = ImageSizeGetter.getSize(FileInput(copiedImage));
      final imageWidth = size.width.toDouble();
      final imageHeight = size.height.toDouble();
      double screenWidth = MediaQuery.of(context).size.width;

      double scale_img = imageWidth / screenWidth;
      print("scale img = ${scale_img}");

      if (scale_img < 1.0) {
        scale_img = 1.0;
      }

      setState(() {
        _image = copiedImage;
        _scaleWidth = scale_img;
      });

      _resetZoom();
    }
  }

  void _addPoint(Offset point) {
    setState(() {
      _points.add(point);
    });
  }

  void _resetZoom() {
    _transformationController.value = Matrix4.identity();
    setState(() {
      _scale = 1.0;
    });
  }

  void _zoomOut() {
    final currentScale = _transformationController.value.getMaxScaleOnAxis();
    if (currentScale > 0.5) {
      _transformationController.value =
          _transformationController.value.scaled(0.8);
      setState(() {
        _scale = currentScale * 0.8;
      });
    }
  }

  void _zoomIn() {
    final currentScale = _transformationController.value.getMaxScaleOnAxis();
    if (currentScale < 4.0) {
      _transformationController.value =
          _transformationController.value.scaled(1.2);
      setState(() {
        _scale = currentScale * 1.2;
      });
    }
  }

  void _moveLeft() {
    final Matrix4 currentMatrix = _transformationController.value;
    final Offset translation = Offset(50.0 / _scale, 0);
    final Matrix4 newMatrix = Matrix4.copy(currentMatrix)
      ..translate(translation.dx, translation.dy);
    _transformationController.value = newMatrix;
  }

  void _moveRight() {
    final Matrix4 currentMatrix = _transformationController.value;
    final Offset translation = Offset(-50.0 / _scale, 0);
    final Matrix4 newMatrix = Matrix4.copy(currentMatrix)
      ..translate(translation.dx, translation.dy);
    _transformationController.value = newMatrix;
  }

  void _moveUp() {
    final Matrix4 currentMatrix = _transformationController.value;
    final Offset translation = Offset(0, 50.0 / _scale);
    final Matrix4 newMatrix = Matrix4.copy(currentMatrix)
      ..translate(translation.dx, translation.dy);
    _transformationController.value = newMatrix;
  }

  void _moveDown() {
    final Matrix4 currentMatrix = _transformationController.value;
    final Offset translation = Offset(0, -50.0 / _scale);
    final Matrix4 newMatrix = Matrix4.copy(currentMatrix)
      ..translate(translation.dx, translation.dy);
    _transformationController.value = newMatrix;
  }

  @override
  void dispose() {
    DefaultCacheManager().emptyCache();
    super.dispose();
  }

  String _generateRandomName(int length) {
    final random = Random();
    const chars =
        'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    return String.fromCharCodes(Iterable.generate(
        length, (_) => chars.codeUnitAt(random.nextInt(chars.length))));
  }

  Future<void> _processImage(String key) async {
    final tempDir = await getTemporaryDirectory();
    final randomName =
        _generateRandomName(10); // Generate a 10 character random name
    final processedImagePath = '${tempDir.path}/$randomName.jpg';
    final processImage = await _image!.copy(processedImagePath);

    final imagePathPointer = processImage.path.toNativeUtf8();
    final pointsPointer = _convertPointsToNative(_points);

    int result = -1;
    if (key == "GRAY") {
      result = graphics.processImageWithPointsGrayScale(
          imagePathPointer, pointsPointer, _points.length);
    }

    if (key == "DRAW") {
      result = graphics.processImageWithPoints(
          imagePathPointer, pointsPointer, _points.length);
    }

    if (key == "BLUR") {
      result = graphics.processImageGaussianBlur(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "GRUB_CUT") {
      result = graphics.processImageGrabcut(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "GRUB_CUT_FOREGRAUND") {
      result = graphics.processImageGrabcutWhiteForeground(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "SEPIA") {
      result = graphics.processImageSepia(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "VIGNETTE") {
      result = graphics.processImageVignette(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "SHARPEN") {
      result = graphics.processImageSharpen(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    if (key == "EMBOSS") {
      result = graphics.processImageEmboss(
          imagePathPointer, pointsPointer, _points.length, 3);
    }
    if (key == "CARTOON") {
      result = graphics.processImageCartoon(
          imagePathPointer, pointsPointer, _points.length, 3);
    }

    calloc.free(imagePathPointer);
    calloc.free(pointsPointer);
    final temp = _image!;
    if (result != 0) {
      throw Exception('Image processing failed');
    }
    setState(() {
      _image = processImage;
      _points.clear();
    });

    temp.delete();
  }

  ffi.Pointer<ffi.Float> _convertPointsToNative(List<Offset> points) {
    final pointer = calloc<ffi.Float>(points.length * 2);
    for (int i = 0; i < points.length; i++) {
      // Apply both scale and offset
      pointer[i * 2] = (points[i].dx * _scale + _offsetX) * _scaleWidth;
      pointer[i * 2 + 1] = (points[i].dy * _scale + _offsetY) * _scaleWidth;
    }
    return pointer;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Image Processing with FFI'),
          actions: [
            IconButton(
              icon: Icon(_isDrawing ? Icons.edit_off : Icons.edit),
              onPressed: () {
                setState(() {
                  _isDrawing = !_isDrawing;
                });
              },
            ),
            IconButton(
              icon: const Icon(Icons.refresh),
              onPressed: () => _processImage("DRAW"),
            ),
            IconButton(
              onPressed: () => _pickImage(ImageSource.gallery),
              icon: const Icon(Icons.photo),
            ),
            const SizedBox(height: 10),
            IconButton(
              onPressed: () => _pickImage(ImageSource.camera),
              icon: const Icon(Icons.camera),
            ),
            IconButton(
              icon: const Icon(Icons.save),
              onPressed: () => saveImage(_image!),
            ),
          ],
        ),
        body: Center(
          child: InteractiveViewer(
            transformationController: _transformationController,
            boundaryMargin: EdgeInsets.all(20.0),
            minScale: 0.5,
            maxScale: 4.0,
            scaleEnabled: true,
            panEnabled: true,
            onInteractionUpdate: (details) {
              setState(() {
                _scale = _transformationController.value.getMaxScaleOnAxis();
                final translation =
                    _transformationController.value.getTranslation();
                _offsetX = translation.x;
                _offsetY = translation.y;
              });
            },
            child: GestureDetector(
              onPanUpdate: (details) {
                if (_isDrawing) {
                  RenderBox renderBox = context.findRenderObject() as RenderBox;
                  Offset localPosition =
                      renderBox.globalToLocal(details.localPosition);
                  _addPoint(localPosition / _scale);
                }
              },
              child: Stack(
                children: [
                  if (_image != null)
                    Image.file(_image!, key: _imageKey, fit: BoxFit.contain)
                  else
                    const Text('No image selected.'),
                  CustomPaint(
                    painter: PointPainter(_points, _scale),
                    size: MediaQuery.of(context).size,
                  ),
                ],
              ),
            ),
          ),
        ),
        floatingActionButton: SizedBox(
          width: 56,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              IconButton(
                icon: const Icon(Icons.zoom_out_map),
                onPressed: _resetZoom,
              ),
              IconButton(
                icon: const Icon(Icons.zoom_in),
                onPressed: _zoomIn,
              ),
              IconButton(
                icon: const Icon(Icons.zoom_out),
                onPressed: _zoomOut,
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  IconButton(
                    icon: Icon(Icons.arrow_upward),
                    onPressed: _moveUp,
                  ),
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  IconButton(
                    icon: Icon(Icons.arrow_back),
                    onPressed: _moveLeft,
                  ),
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  IconButton(
                    icon: Icon(Icons.arrow_forward),
                    onPressed: _moveRight,
                  ),
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  IconButton(
                    icon: Icon(Icons.arrow_downward),
                    onPressed: _moveDown,
                  ),
                ],
              ),
              DropdownMenuItem<String>(
                value: 'submenu1',
                child: SubmenuButton(
                  menuStyle: MenuStyle(
                    padding: MaterialStateProperty.all(
                      EdgeInsets.symmetric(horizontal: 24, vertical: 12),
                    ),
                  ),
                  child: Icon(Icons.satellite),
                  menuChildren: [
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('gray_scale'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("GRAY");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('gaussian blur'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("BLUR");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('grab_cut'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("GRUB_CUT");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('sepia'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("SEPIA");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Vignette'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("VIGNETTE");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Sharpen'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("SHARPEN");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Emboss'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("EMBOSS");
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Cartoon'),
                        ],
                      ),
                      onPressed: () {
                        _processImage("CARTOON");
                      },
                    ),
                  ],
                ),
              ),
              SizedBox(height: 6), // Add some spacing between the buttons
              DropdownMenuItem<String>(
                value: 'submenu1',
                child: SubmenuButton(
                  menuStyle: MenuStyle(
                    padding: MaterialStateProperty.all(
                      EdgeInsets.symmetric(horizontal: 24, vertical: 12),
                    ),
                  ),
                  child: Icon(Icons.satellite),
                  menuChildren: [
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Option 1.1'),
                        ],
                      ),
                      onPressed: () {
                        // Handle Option 1.1
                      },
                    ),
                    MenuItemButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all(EdgeInsets.zero),
                      ),
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Icon(Icons.star, size: 20),
                          SizedBox(width: 8),
                          Text('Option 1.2'),
                        ],
                      ),
                      onPressed: () {
                        // Handle Option 1.2
                      },
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
