# ffxi
ffxi dat reverse engineering

19 May 2016
------------
1) fix a bug that cause premature stop in extracting dat.  Main reason why some dat have missing MMB & img.

2) add a default VAO creation, to fix no Model/Map display for window 10.  User need to update latest glm/glfw3 as well, else it will generate compile error.

3) fix DXT3 convert error, should be using BCD2Decode.


NOTE:
for user that encounter GLFW3 crash when calling glfwInit(), u have to goto Project Properties -> C/C++ -> Code Generation ->
Runtime Library, change /MDd to /MD

7 June 2016
------------
1) fix transparency for mapViewer

2) add func 'v' to view individual Model within each MMB.

Note: mapViewer uses VisualStudio NuGet Manager for opengl, glfw2, glew.  The packages is 'nupengl.core'
access thru 'Tools -> NuGet Package Manager -> Manage NuGet Packages for Solution'

Remember to change --- char ffxidir[512]="E:\\Program Files (x86)\\PlayOnline2\\SquareEnix\\FINAL FANTASY XI\\";
to your folder.
