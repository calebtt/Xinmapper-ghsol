# Xinmapper-ghsol
Xinmapper is an xbox controller keyboard and mouse input sim app
*NOTE: This project no longer uses the Boost library as the features 
of the library that were used by this app
are now standard C++ library features. Ignore the Boost lib 
dependency in the "Xinmapper Build Help" folder instructions.

You will need to build the project to edit the Map string at this time, 
the tokens mapping a controller button to keyboard and mouse input
are of the form: 

LTHUMB:LEFT:NORM:a   <-- maps left thumbstick left direction to keyboard key "a" 

LTHUMB:RIGHT:NORM:d   <-- maps left thumbstick right direction to keyboard key "d"

LTHUMB:UP:NORM:w    <-- maps left thumbstick up direction to keyboard key "w"

LTHUMB:DOWN:NORM:s  <-- maps left thumbstick down direction to keyboard key "s"


There is also the ability to specify virtual keycodes in the mapping, these are of the form:

START:NONE:NORM:VK27  <-- decimal 27 is Hex 1B which is the "Escape" key

BACK:NONE:NORM:VK8    <-- decimal 8 is Hex 8 which is the "Backspace" key

LTRIGGER:NONE:NORM:VK2  <-- decimal 2 is Hex 2 which is the Right Mouse Button


A full list of virtual keycodes can be found on the MSDN here*: 
https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

*Note the list is in Hex and will need to be translated to decimal for use in the Map string.

Remember to initialize the "Mapper" with the "MapInformation" string you built with 
the above tokens before enabling processing.
Set the mouse sensitivity with mouse->SetSensitivity(int).


More info or build help, see the "Xinmapper Build Help" folder.
