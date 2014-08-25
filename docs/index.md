

Please bear with us, this documentation is far from complete.

[For now here are the slides from the tutorial room.](slides.html)


Quick start
---------------

1. Create and save your SketchUp file in the "workspace" folder
2. Double click on build.cmd, your html will be created in a directory called build
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

Sketchup2room will cull faces that have the Default Material, so make sure faces are exported apply a material to them

Making dynamic objects.
=====

By setting a name on individual instances of components you can tell sketchup2room to set the location of the entrance portal and create links to new rooms.

The format is as follows, where the pipe character "|" separates sections:

> $type|option1,option2=value|Text or URL

Note that RGB values or width and height are space delimited and range from 0 to 1, e.g.: col=.2 .5 .3

$origin type
----

Give an instance the name $origin to specify the location of the entrance portal. The system will use the components local x (green) axis. So make sure you create your prop with the right orientation (green axis facing outwards)


$link type
-----

Simple:

> $link|room3.html

Advanced:

> $link|title=ROOM 3,noglow,notext,autoload,col=.5 .5 .5,thumb=door.jpg,size=1 2.2|room3.html


Creates a portal to a JanusVR room.

Optional Parameters:
* title - Sets the title of the link (e.g. title=Tutorial Room)
* noglow - Turns off the glow effect that normally indicates a clickable portal
* notext - Draws no text on the door
* autoload - Automatically loads the destination room, as though the link has been clicked
* col - Sets the color of the glow effect using RGB values
* thumb - The Path to the thumbnail to be used as the door image
* size - Sets the size (width height) of the portal.  Default is 1.8 2.5.


$text or $t type
-----

Creates a text element at the given position.

Simple:

> $text|text_col=1 1 1|JanusVR is cool

Advanced:
> $text|font_size=20,back_alpha=0.5,back_col=0.5 0 0,text_col=1 1 1|Welcome to My Room

Optional Parameters:
* font_size - the point font size of the text
* back_alpha - the alpha value of the background color, where 0 = transparent and 1 = opaque
* back_col - the color value of the background. R G B values from 0 to 1
* text_col - the color value of the text. R G B values from 0 to 1


$paragraph or $p type
-----

Exactly like $text but create a paragraph.



$image or $i type
------

Embeds an image, no options only needed for animated gifs or to link to external content

Default width is 2m, and height is fixed by aspect ratio of the image data, create your placeholder to fit that size and then scale instances to resize the image.

Example:

> $i|http://example.com/image.png


$video or $v type
------

Embeds a video, can specify thumbnail.

Default width is 2m, and height is fixed by aspect ratio of thumbnail/video file, create your placeholder/prop to fit that size and then scale instances to resize the video.

If you wanted to have a 4x3 video in your room, create a TV model with a 4x3 screen which is 2 meters wide and the components origin in the center of the screen. You can then position instances of that component with any size and the video should fit.


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

Like ambient light, but will shade face based on there orientation to the light source.  Direction is indicated by the green axis of the component.

> $directional|0.2 0.2 0.2

See tut_image.skp for an example.


$light
------

This is your basic lighting element, a point light that casts light in all direction.

Optional Parameters:

* func - Sets the type of light. Default value is pointlight. Using torchlight will make a flickering light
* range - Maximum distance (meters) to apply the light. Default is 20. To increase performance, set this to the minimum that does not cause graphical artifacts.  Shorter distances can create interesting effects.
* falloff - Specifies the falloff power factor, where 1=1/x, 2 = 1/(x^2), 3 = 1/(x^3), and so on.
* cond - Allows you to enable or disable the light with some custom condition. Takes a GLSL expression.

> $light|func=torchlight|2 1.5 0.6

$spotlight
------

Directional spotlight, light will be case along the green axis of the component

Optional Parameters:
Takes all the parameters of $light, plus
* outerCone - The angle in degrees that the light is emitted from the spot light.  Default is 40
* innerCone - The angle in degrees that receives maximum intensity. There will be a gradual fall off from this to the outerCone. Default is outerCone-2
