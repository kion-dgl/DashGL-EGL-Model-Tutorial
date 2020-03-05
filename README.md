# Dashie

I guess we should start filling in what we can for the read me files for the
first tutorial, and then we can come back and polish them in the online page.
To start off we should probably explain the purpose of this tutorial, which
could probably best be described as a way to get familiar with OpenGL by starting
with a simple triangle and moving up to the point of being able to display a
simple 3d model.  

Next it would probably be a good idea to explain what technology we will be
using, which is a Raspberry Pi, which uses either Raspbian or RetroPie (which
is based on Raspbian anyways). Specifically we want to create a program that we
can list under RetroPie, run in a full screen context with hardware acceleration
and then exit and return to RetroPie.

We should probably also provide a table for which versions of the Raspberry Pi
work, under which versions of Raspbian and RetroPie respectively, and what to do
with the Raspberry Pi Zero to get the examples to be able to compile on the 
device. 

We should also have either a screenshot or an introduction video which shows the
complete output of what this tutorial will look like once it's complete.

It would also make sense to describe the languages we chose and why we're using
it. So first we're generally trying to replicate what it would be like to program
for a console or a handheld on the Raspberry Pi. So while it is possible to create
homebrew for the Playstation, or Dreamcast we can treat the Raspberry Pi like a
platform and program to a similar set of specifications like we would be programming
on one of the other consoles.

So to replicate working with these consoles we'll be using the C programming language.
It's possible to go higher with C++ or Java, but for programming exercises we 
don't want to use higher languages. We could also go lower with assembler, but we
want to finish sometime this decade, so that's not really an option either.

We're using EGL because it allows us to skip on having to mess around with a
desktop environment. We have GTK, FreeGLUT, Raylib, GLFW, and SDL2. SDL2 makes the
most sense but running without a desktop requires it to be compiled, so if that
ever becomes mainlined, then I'll probably switch tutorials over to that.

In terms of OpenGL, we'll be using OpenGL 2.0. OpenGL 1.0 is a CPU-bound library
and I think it helps having a little bit of experience with that as it helps with
understanding OpenGL 2.0 a little bit, but in this tutorial we'll be jumping into
OpenGL 2.0 directly. And we won't be using OpenGL 3.0 because we won't be using
the features, namely vertex array objects (VAO's) which are required in OpenGL 3.0.

So that should be enough to cover the basics, if you're ready let's get started 
with a simple triangle.

