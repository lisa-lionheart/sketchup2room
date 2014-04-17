set PATH=%PATH%;%CD%\..\bin

rmdir test
rmdir testlevel


sketchup2room --out dodgers_stadium --html index.html dodgers_stadium.skp

sketchup2room --out building --html index.html building.skp 
sketchup2room --out testlevel --html index.html testlevel.skp

pause