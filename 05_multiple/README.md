# Multiple

Okay, now that we have a geneal idea of the basic pattern of how we bind buffers
and and then fit those buffers to attributes to be drawn on the screen. Now we
might as well take the oppertunity to make multiple draw calls. There's really not
much to it, we generally repeat the same pattern four times. We bind four buffers,
and then repeat the same draw call each time for each buffer to draw each shape.

There really isn't that much too it, but what often happens with these tutorials is
that only one shape ends up being gone so you're not provided with any context for 
how to scale up and what happens and how you're supposed to do that. And to be honest
this is only one of the ways you can do this. In each case we need to bind another 
buffer before we can set the attributes and make the draw calls. We can actually bind
all of the shapes to a single buffer, and that way we only need to set the buffer
and attributes and then draw four calls, one for each shape at a specific offset of
the buffer. 

We also might as well take the oppertunity to talk about vertex color while we're here.
We've been setting the color of each shape using vertex color, and that's generally the
approach of defining the color in the buffer and then using that as an attribute to
define the color. This is a pretty easy way to do things. But we can also take the approach
of defninig a uniform.  A uniform is a value that stays static until it's updated by
the CPU side of things. 

So say we have a vec3 uniform that we set as "red", then the uniform will be red for
every shape we draw, and all of the colors will be red. And then if we set it to blue
then all of the colors we draw will be blue until we update it again. There's a whole
can of worms involved in terms of best practices and performance. But right now since
we're working on basics and don't have to worry about performance too much, the 
focus is on providing context for what options are available and how to implement them
and then worry about best practices when angry people on the internet yell at us.
