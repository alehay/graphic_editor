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

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class PointPainter extends CustomPainter {
  final List<Offset> points;

  PointPainter(this.points);

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = Colors.red
      ..strokeCap = StrokeCap.round
      ..strokeWidth = 5.0;

    for (var point in points) {
      canvas.drawCircle(point, 5.0, paint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return true;
  }
}

class _MyAppState extends State<MyApp> {
  File? _image;
  final ImagePicker _picker = ImagePicker();
  List<Offset> _points = [];

  Future<void> _pickImage(ImageSource source) async {
    final pickedFile = await _picker.pickImage(source: source);
    if (pickedFile != null) {
      final imageFile = File(pickedFile.path);
      final tempDir = await getTemporaryDirectory();
      final copyImagePath = '${tempDir.path}/copy_image.jpg';

      // Copy the image to the new location
      final copiedImage = await imageFile.copy(copyImagePath);

      setState(() {
        _image = copiedImage;
      });
    }
  }

  void _addPoint(Offset point) {
    setState(() {
      _points.add(point);
    });
  }

  Future<void> _processImage() async {
    final tempDir = await getTemporaryDirectory();
    final processedImagePath = '${tempDir.path}/processed_image.jpg';
    final processImage = await _image!.copy(processedImagePath);

    final imagePathPointer = processImage.path.toNativeUtf8();
    final pointsPointer = _convertPointsToNative(_points);
    final result = graphics.processImageWithPoints(imagePathPointer, pointsPointer, _points.length);
    calloc.free(imagePathPointer);
    calloc.free(pointsPointer);

    if (result != 0) {
      throw Exception('Image processing failed');
    }

    setState(() {
      _image = processImage;
    });
  }

  ffi.Pointer<ffi.Float> _convertPointsToNative(List<Offset> points) {
    final pointer = calloc<ffi.Float>(points.length * 2);
    for (int i = 0; i < points.length; i++) {
      pointer[i * 2] = points[i].dx;
      pointer[i * 2 + 1] = points[i].dy;
    }
    return pointer;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Image Processing with FFI'),
        ),
        body: Center(
          child: GestureDetector(
            onPanUpdate: (details) {
              RenderBox renderBox = context.findRenderObject() as RenderBox;
              Offset localPosition = renderBox.globalToLocal(details.globalPosition);
              _addPoint(localPosition);
            },
            child: Stack(
              children: [
                if (_image != null)
                  Image.file(_image!)
                else
                  const Text('No image selected.'),
                CustomPaint(
                  painter: PointPainter(_points),
                  child: Container(),
                ),
              ],
            ),
          ),
        ),
        floatingActionButton: Column(
          mainAxisAlignment: MainAxisAlignment.end,
          children: [
            FloatingActionButton(
              onPressed: () => _pickImage(ImageSource.gallery),
              child: const Icon(Icons.photo),
            ),
            const SizedBox(height: 10),
            FloatingActionButton(
              onPressed: () => _pickImage(ImageSource.camera),
              child: const Icon(Icons.camera),
            ),
            const SizedBox(height: 10),
            FloatingActionButton(
              onPressed: () => _processImage(),
              child: const Icon(Icons.edit),
            ),
          ],
        ),
      ),
    );
  }
}
