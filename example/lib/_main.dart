import 'package:flutter/material.dart';

import 'package:image_picker/image_picker.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:async';
import 'package:flutter/material.dart';
import 'package:file_picker/file_picker.dart';
import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';

import 'package:graphics/graphics.dart' as graphics;

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  File? _image;
  final ImagePicker _picker = ImagePicker();

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

  Future<void> _processImage() async {
    final tempDir = await getTemporaryDirectory();
    final processedImagePath = '${tempDir.path}/processed_image.jpg';
    final processImage = await _image!.copy(processedImagePath);

    final imagePathPointer = processImage.path.toNativeUtf8();
    final result = graphics.processImage(imagePathPointer);
    calloc.free(imagePathPointer);

    if (result != 0) {
      throw Exception('Image processing failed');
    }

    setState(() {
      _image = processImage;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Image Processing with FFI'),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              if (_image != null)
                Image.file(_image!)
              else
                const Text('No image selected.'),
              const SizedBox(height: 20),
              ElevatedButton(
                onPressed: () => _pickImage(ImageSource.gallery),
                child: const Text('Pick Image from Gallery'),
              ),
              const SizedBox(height: 10),
              ElevatedButton(
                onPressed: () => _pickImage(ImageSource.camera),
                child: const Text('Pick Image from Camera'),
              ),
              ElevatedButton(
                onPressed: () => _processImage(),
                child: const Text('gray scale image'),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
