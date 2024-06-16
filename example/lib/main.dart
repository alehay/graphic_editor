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
      final processedImagePath = await _processImage(imageFile);
      setState(() {
        _image = File(processedImagePath);
      });
    }
  }

  Future<String> _processImage(File imageFile) async {
    final tempDir = await getTemporaryDirectory();
    final processedImagePath = '${tempDir.path}/processed_image.jpg';

    final imagePathPointer = imageFile.path.toNativeUtf8();
    final result = graphics.processImage(imagePathPointer);
    calloc.free(imagePathPointer);

    if (result != 0) {
      throw Exception('Image processing failed');
    }

    return processedImagePath;
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
            ],
          ),
        ),
      ),
    );
  }
}
