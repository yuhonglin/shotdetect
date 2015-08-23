ShortDetect
=========

ShotDetect is "shot detection and keyframe extraction" program

  - It is fast, deveploped by C++ and is directly based on libavcodec and openCV etc.
  - I can deal with all formats of online videos (thanks to libavcodec etc.)
  - The performace is quite good (f1 score > 80% from a test on 400 shots)


Version
----
1.0

Compilation
--------------

### First, install dependencies ###
Install ffmpeg and openCV

### Second, compile the program ###
```sh
mkdir build
cd build
cmake ..
make -j 3
```
The program is in ```bin``` folder.

Usage
------
User ```--help``` options to see all the configurations. The basic options are two

 - ```-i``` : the input file path ( use ```//``` instead of ```/``` in the path ).
 - ```-o``` : the output path, where the the keyframes are stored.
You can use the other parameters as default because they are tuned by experiments already.

Output format
-------------
Semantic of the keyframe's filename: ```videoFileName``` + ```@``` + ```index``` + ```@``` + ```shot's starting time``` + ```@``` + ```keyframe's time``` + ```@``` + ```shot's ending time``` + ```.jpg```

Currently, the keyframe is always the middle frame in time.

License
----
MIT
