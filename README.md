# raytracer
C++ Raytracer using GLM library

# to build:
`g++ -o main main.cpp`

# to run:
`./main sample.txt`

output will be in `sample.ppm`

to view ppm file, use a vscode extension or photoshop or an online viewer

# output:
see sample.txt for formatting:
- NEAR,LEFT,RIGHT,BOTTOM,TOP define the near plane and its distance
- RES defines the resolution of the output image (x and y)
- SPHERE defines sphere objects in the scene - in order:
  - x pos, y pos, z pos, x size, y size, z size, red*, green*, blue*, ambient*, diffuse*, specular*, reflect*, specular exponent
  - \* means the range is 0-1
- LIGHT defines light objects in the scene - in order:
  - x pos, y pos, z pos, red*, green*, blue*
  - \* means the range is 0-1
- BACK defines the background colour
- AMBIENT defines ambient lighting
- OUTPUT defines the output file name

# samples:
![sample.txt rendered](https://raw.githubusercontent.com/kukuminer/raytracer/main/samples/sample.png)
![whale.txt rendered](https://raw.githubusercontent.com/kukuminer/raytracer/main/samples/whale.png)
