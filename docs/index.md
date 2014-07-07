

Please bear with us, this documentation is far from complete.

[For now here are the slides from the tutorial room.](slides.html)


Quick start
---------------

1. Create and save your SketchUp file in the "workspace" folder
2. Double click on build.cmd, you html will be created in a directory called build
3. When you make changes delete the build folder and run build.cmd again

How to compose your SketchUp file for use with sketchup2room
=======

Whilst naively making a room in SketchuUp should more or less work, there is a way to get the best results.

Components Basics
----

Sketchup2room will compile any ungrouped and grouped geometry into a main.obj file. Geometry in "components" will be saved as external OBJ files and have HTML for them generated. It is very important that when you have repeated features, that you make a single component for it and then create multiple instances of that component. Its also a good idea to make large pieces of geometry that are not repeated a component to cut down on load times. To create a component select some things in sketchup make a group and then select the make component option. I highly recommend changing the name from Group#x default name.

You can create "placeholders" components by putting a '!' in front of the component name. For example you could create a link without any door frame by making a component and calling it "!door" and setting the instance name as described below. This is quite often used to place lights.

As well as creating place holders you can make a component non solid by adding '+' infront of the component name. It is advised to do this with any highly complex models you create to improve room frame rates.

Please check out the examples skp files, to see how this works in practice.

Default Material
-----

Sketchup2room will cull faces that have the Default Material, to make sure faces are exported apply a material to them

Making dynamic objects.
=====

By setting a name on individual instances of components you can tell sketchup2room to set the location of the entrance portal and create links to new rooms.

The format is, where the pipe character "|" separates sections:

> $type|option1,option2=value|Text or URL

$origin type
----

Give an instance the name $origin to specify the location of the entrance portal. The system will use the components local x (green) axis. So make sure you create your prop with the right orientation (green axis facing outwards)


$link type
-----

Simple:

> $link|room3.html

Advanced:

> $link|noglow,notext|room3.html

**TODO: Document parameters**

$text or $t type
-----

Creates a text element at the given position.

Example:

> $text|text_col=1 1 1|JanusVR is cool

**TODO: Document parameters**

$paragraph or $p type
-----

Exactly like $text but create a paragraph.

**TODO: Document parameters**


$image or $i type
------

Embeds an image, no options only needed for animated gifs or to link to external content

Default width is 2m, and height is fixed by aspect ratio of the image data, create your placeholder to fit that size and then scale instances to resize the image.

Example:

> $i|http://example.com/image.png


$video or $v type
------

Embeds a video, can specify thumbnail.

Default width is 2m, and height is fixed by aspect ratio of thumbnail/video file, create you placeholder/prop to fit that size and then scale instances to resize the video.

So, if you wanted to have a 4x3 video in your room, create a TV model with a 4x3 screen which is to meters wide and the components origin in the center of the screen. You can then position instances of that component with any size and the video should fit.


Example:

> $v|thumb=images/thumb.jpg|http://example.com/video.mp4


$nosolid
-----

*This is deprecated append '+' to the name of the component to create non-solid objects'*

Create an instance of a model but do not use the geometry for collision detection. This is very important for high poly models.


Lighting Types
=============

$ambient
--------

Set the ambient lighting level for the room

Example, dimly lit room:

> $ambient|0.2 0.2 0.2

$directional
--------

Like ambient light, but will shade face based on there orientation to the light source.

See tut_image.skp for usage.


$light
------

This is your basic lighting element, a point light that casts light in all direction.

**TODO: Document parameters**

$spotlight
------

Directional spotlight, light will be case along the green axis of the component

**TODO: Document parameters**
