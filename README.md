What is this?
-------------

A tool for creating VR "rooms" from google SketchUp files.

This utility will take a SketchUp .skp file and create a complete set of .obj and html files that make up a JanusVR room.


[Check out the VR now to see it in action](http://static.croxford.me/VRSite/)

[Documentation in the docs Folder](docs/index.md)

[Download version 3](https://github.com/lisa-lionheart/sketchup2room/archive/v3.zip)

Please feel to open an issue on github or post in reddit thread on [/r/JanusVR](http://www.reddit.com/r/janusVR)

Version 3:
----------

- Update for being able to create Image elements for images not imported as texture
- Fixed shader issues with viewing room through portals.
- Major performance improvements, better HTML generation
- Better support for video, ability to set thumbnails

Version 2:

- First public release

How?
----

From the command line

sketchup2room myFile.skp


or

sketchup2room --out www --html room.html  myRoom.skp
