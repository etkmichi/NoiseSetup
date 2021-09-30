# NoiseSetup
GUI with controls for libnoise
<br><br>
Created with qtcreator and qt base installation.<br><br>
For compiling and starting the gui the two qt-projects must be configured!
1. The directory /WorldGenerator provides a library setup which can be used for setting up noises with gui.
2. The directory /WorldGeneratorGUI provides code for starting the GUI

With QtCreator:
To compile the first project, open /WorldGenerator/WorldGenerator.pro with the qtcreator and configure the project.
To compile the second project, open /WorldGeneratorGUI/WorldGeneratorGUI.pro with qtcreator and configure the project.
<br><br>
Linux with cmd-line (Debug):
qmake /WorldGenerator/WorldGenerator/WorldGenerator.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug
make -f /build-WorldGenerator-Desktop-Debug/Makefile qmake_all
cd /build-WorldGenerator-Desktop-Debug/
make -j24
<br><br>

Additional libraries:<br>
libnoise<br>
glew<br>
opengl<br>
GLU<br>
glfw<br>
qt5-base<br>
