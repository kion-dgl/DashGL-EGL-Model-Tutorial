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

So in general we're not doing anything special on the GPU, more like offl
