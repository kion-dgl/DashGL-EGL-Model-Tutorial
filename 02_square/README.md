# Square

I guess in this tutorial we can start talking about the actual OpenGL aspect of
working with these tutorials. But before that we have some code to clean up, so 
we're going to move the ```init_ogl``` code to a header file and try to clean up
the code that we're looking attry and make things simpler. So we're going to create
a directory called ```lib``` and then create a ```dashgl.c``` and a ```dashgl.h```.
In general I should explain there is no _official_ dashgl library, it's more something
that I use as a name for tutorials, but maybe someday there could be a ```apt-get
install lib_dashgl``` and that install a specific script to /usr/lib or something and
that would be pretty cool.

As for the actual OpenGL stuff in this tutorial, it would probably make sense to talk
about the shaders. Shaders is kind of a weird word when you first hear it. In general
it's a pretty good idea to point out that there's no special sauce that we're doing 
on the GPU. For 3d graphics, it's basically a bunch of matrix mulitplication, so there's
nothing that says you can't do it all on the CPU and in general up to the mid-90's this
was generally the way everything was done. The problem is you run your game logic on
the cpu, and then you take a ton of cycles on the cpu dedicated to just multiplying
matrices to draw things on the screen and start the name frame. 

So in general we're not doing anything special on the GPU, more like offloading what
we would be doing on the CPU to the GPU. To do that we need to first share the 
information that the CPU has access to the GPU, this is what we do when be bind
buffers, we take the vertex information and store it in a place where the GPU
has access to. We get back a buffer index, so when the CPU needs to tell the GPU
to run a draw call, we can say "use these here to do this". 

For the "hey do this" part, we use shaders. Shaders are basically a program that
runs on the GPU that defines how an area of memory should be colored in or "shaded",
thus the term shaders. It's a litter weird to get familiar to hearing the term
in this context, but if you're reading this tutorial there's a good chance you're
already familiar with it, and that just me being crazy.

So next we need to go over how stuff actually gets drawn onto the screen. The easiest
way to think of this as a image. So if the screen is 800x400 then we basically have a
800x400 image, that gets populated with stuff that we tell the GPU to draw on top of
it every frame. By default the coordinates are set from -1 to 1 going from left to right
and from 1 to -1 going from up to down. So we when we tell the GPU to draw something we
map it onto these coordinates. That's why our "square" ends up looking like a rectangle.

What this implies is what you were wondering, "what, I thought OpenGL was an API for
drawing a 3d context?". And the answer is that it both is and it isn't. That is to 
say OpenGL is primarily an API for offloading calculations from the CPU to the GPU
in order to draw on the screen. The way we do that is telling the GPU to do matrix 
calculations which defines where coordinates to be drawn on the screen. So this tutorial
will define how to use shaders to draw a simple 3d context.

And lastly when drawing we use triangles. Which we will cover in future tutorials.
And if you're thinking, well what about quads? The answer is quads are stupid,
we use triangles. More on why in the next tutorial.
