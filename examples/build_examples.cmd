set PATH=%PATH%;%CD%\..\bin

rmdir build


sketchup2room --out build --html stadium.html stadium.skp
sketchup2room --out build --html building.html building.skp 
sketchup2room --out build --html testlevel.html testlevel.skp
sketchup2room --out build --html street.html street.skp
sketchup2room --out build --html index.html index.skp

pause