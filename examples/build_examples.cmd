set PATH=%PATH%;%CD%\..\bin

rmdir build


sketchup2room --out build --shader lighting.frag --html lightingtest.html lightingtest.skp

sketchup2room --out build --html stadium.html stadium.skp
sketchup2room --out build --html building.html building.skp 
sketchup2room --out build --html index.html index.skp

pause