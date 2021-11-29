# Xinmapper-ghsol
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
█▄▀█▀▄██▄██░▄▄▀█░▄▀▄░█░▄▄▀█▀▄▄▀█▀▄▄▀█░▄▄█░▄▄▀
███░████░▄█░██░█░█▄█░█░▀▀░█░▀▀░█░▀▀░█░▄▄█░▀▀▄
█▀▄█▄▀█▄▄▄█▄██▄█▄███▄█▄██▄█░████░████▄▄▄█▄█▄▄
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
   
<h1>Xinmapper is an xbox controller keyboard input sim app </h1>
<b>*NOTE:</b> Latest development build code is in the timerbuild branch
if you want to follow the progress or see what is upcoming.
</br>
<b>*NOTE:</b> Project uses Visual Studio 2019 with C++20 language features, 
the upgrade to VS2022 will be coming in the near future.

<b>*NOTE:</b> This project no longer uses the Boost library as the features 
of the library that were used by this app are now 
standard C++ library features. See the "build help" folder if you
have trouble building the project.

You will need to build the project to edit the Map string at this time, 
the tokens mapping a controller button to keyboard and mouse input
are of the form: 

<b>LTHUMB:LEFT:NORM:a   <-- maps left thumbstick left direction to keyboard key "a" </b> </br>
<b>LTHUMB:RIGHT:NORM:d   <-- maps left thumbstick right direction to keyboard key "d"</b> </br>
<b>LTHUMB:UP:NORM:w    <-- maps left thumbstick up direction to keyboard key "w"</b> </br>
<b>LTHUMB:DOWN:NORM:s  <-- maps left thumbstick down direction to keyboard key "s"</b> </br>


There is also the ability to specify virtual keycodes in the mapping, these are of the form:

<b>START:NONE:NORM:VK27  <-- decimal 27 is Hex 1B which is the "Escape" key</b> </br>
<b>BACK:NONE:NORM:VK8    <-- decimal 8 is Hex 8 which is the "Backspace" key</b> </br>
<b>LTRIGGER:NONE:NORM:VK2  <-- decimal 2 is Hex 2 which is the Right Mouse Button</b> </br>


A full list of virtual keycodes can be found on the MSDN here*: 
https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

<b>*NOTE:</b> the list is in Hex and will need to be translated to decimal for use in the Map string.</br>

Remember to initialize the "Mapper" with the "MapInformation" string you built with 
the above tokens before enabling processing.
Set the mouse sensitivity with mouse->SetSensitivity(int).


More info or build help, see the "Xinmapper Build Help" folder. Or the included main() function code.
