# C Gravity Simulator
An improvement over my [old gravity simulator in Haskell](https://github.com/FuzzyCat444/HaskellGravitySim).

![](/images/image1.png)
![](/images/image2.png)
![](/images/image3.png)

## Controls

- WASDQE - move camera
- RMB - rotate camera
- LMB - launch body
- 1 - select planet
- 2 - select star
- 3 - select black hole
- SCROLL WHEEL - change size of body
- MIDDLE MOUSE BUTTON on body - attach camera to body/focus on body
- MIDDLE MOUSE BUTTON on empty - detach camera from body/reset relative velocity of universe to camera
- SPACE - pause/resume simulation
- CTRL + SCROLL WHEEL - change simulation speed
- CTRL + MIDDLE MOUSE BUTTON - reset simulation speed
- G - show/hide grid
- TAB - show/hide body crosshairs
- CTRL + X - delete body under cursor
- CTRL + Z - undo
- BACKSPACE- delete all bodies and reset camera
- CTRL + BACKSPACE - delete all bodies outside of visible grid radius
- LEFT/RIGHT ARROWS - switch focus to a different body

## Building

If the mouse sensitivity is off and you need to adjust it, you will need to edit Constants.h and recompile the program. Download the necessary libraries and set up the folders according to the library/include paths in build.bat, and the run build.bat. You will need GLFW3, GLAD, linmath.h, and lodepng. If you use a compiler other than mingw than you may need to further modify the compile command.

## Credit

Skybox generated using [this space skybox maker](http://wwwtyro.github.io/space-3d/) with the seed "1tc6mu0dea68".
