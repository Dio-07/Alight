# Alight
Vex V5 robotics library add on for LEMLIB and PROS

This add on depends on the use of lemlib as the sensors work to reset coordinates
Alight is built on top of **PROS** and designed to work alongside **LEMlib**

Lemlib DOCS:
https://lemlib.readthedocs.io/en/stable/index.html

## Installation 
1. follow [this](https://lemlib.readthedocs.io/en/stable/tutorials/1_getting_started.html) tutorial on setitng up lemlib 
2. download this repo
3. unzip the file
4. inside the folder move:
   - alight folder into include
   - move all files inside alight src folder into your project src

tip: Some files like main.cpp will get replaced

in the end your relevant format should look like this:

<img width="150" height="400" alt="image" src="https://github.com/user-attachments/assets/f4d24537-d755-4591-8c42-3b2c4935ff0e" />



## Features
- non-coordinate based drive functions
- distance sensor corrections
- controller display
- brain auton buttons
- Auton Selector
- non-obstructive piston toggles

## main.cpp Template
This add on contains a more advanced template code found [here](https://github.com/Dio-07/Alight/blob/main/src/main.cpp)
