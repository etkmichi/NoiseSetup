# NoiseSetup
GUI with controls for libnoise

Attention:
  * This is an old project of mine, code structure is bad and a lot of things are not implemented!

  * Saving any setup is not implemented!

  * Buggy implementations! The app was developed to get some C++/qt experience and at the moment its not my intention to update/improve the code
  * for sure there are some memory leaks and access violations!
  
![logo]

[logo]: https://github.com/etkmichi/NoiseSetup/blob/main/Bildschirmfoto_2023-04-11_10-51-51.jpg

<br><br>
Created with qtcreator and qt base installation.<br><br>
For compiling and starting the gui the two qt-projects must be configured!<br>
1. The directory /WorldGenerator provides a library setup which can be used for setting up noises with gui.
2. The directory /WorldGeneratorGUI provides code for starting the GUI

With QtCreator:<br>
To compile the first project, open /WorldGenerator/WorldGenerator.pro with the qtcreator and configure the project.<br>
To compile the second project, open /WorldGeneratorGUI/WorldGeneratorGUI.pro with qtcreator and configure the project.<br>
<br><br>
Linux with cmd-line (Debug):<br>
qmake /WorldGenerator/WorldGenerator/WorldGenerator.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug<br>
make -f /build-WorldGenerator-Desktop-Debug/Makefile qmake_all<br>
cd /build-WorldGenerator-Desktop-Debug/<br>
make -j24<br>

Additional libraries:<br>
libnoise<br>
glew<br>
opengl<br>
GLU<br>
glfw<br>
qt5-base<br>
