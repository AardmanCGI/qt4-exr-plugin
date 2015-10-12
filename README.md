# EXR Image format plugin for Qt4
A Qt plugin for loading EXR images via OpenEXR.

## Building with CMake
From the qt4-exr-plugin directory, run:
```
mkdir build
cd build
cmake ..
cmake --build .
```

If qmake is not in your path or if you are using a different qt install,
make sure `/path/to/qmake -query` returns the correct paths to the installation.
If not then change the qt.conf file in the same directory as qmake to:
```
[Paths]
Prefix=..
```
Then run
```
cmake -DQT_QMAKE_EXECUTABLE=/path/to/qmake ..
cmake --build .
```

To specify the path to OpenEXR pass the
```
-DIlmBase_ROOT=/path/to/ilmbase -DOpenEXR_ROOT=/path/to/openexr
```
arguments to cmake.

It is also possible to compile this plugin into a single shared library by
compiling in the openexr static libraries.
```
 -DIlmBase_USE_STATIC_LIBS=ON -DOpenEXR_USE_STATIC_LIBS=ON
```
Note that OpenEXR will also have to have been built with the
`-DIlmBase_USE_STATIC_LIBS=ON` option.

## Installing
### Automatic (Recommended)
Once built, run:
```
cmake --build . --target install
```

### Manual
Copy the compiled qexr4.dll into your Qt installation's
`plugins/imageformats` directory.

## License
BSD 3-clause Revised

Copyright (c) 2015, Aardman Animations Ltd

All rights reserved.
