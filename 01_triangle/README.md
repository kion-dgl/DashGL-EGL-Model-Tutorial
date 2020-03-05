# 01 Triangle

Okay, so being honest, this tutorial is actually one of the scariest for 
providing an explanation in how it works. And to provide a parrallel example
for why I feel that way is the the classic "hello world" example for 
every programming language. And this is because "hello world" is simultaniously
simple and complex programs in existence. Syntax-wise the programs are short
and simple, often not requiring more than five lines and generally only requiring
a single line.

But it's also very complex in that your brain questions how it's possible. If
the computer is breaking text down into something that can be assembled, linked
and executed. You get this peak into the world below where you get a glimpse of
assembly, binary, and wonder specifically how the CPU works and how the operating
system works and the massive amount of logic that goes into putting those few
letters that make up "hello world" on the screen. And how does the computer even
know what letters look like? And how does it know to place them on the screen and
where?

So the blue triangle is a similar experience in OpenGL. Syntax-wise there's not a
lot going on, but there is a lot of code required to get the context set up and
working, and you're introduced to shaders, programs, attributes, coordinates that
OpenGL uses, and the concept of putting information into buffers to be used later.
And to make it worse, there's not a lot of context to work with to provide specific
examples of how everything works. So like "hello world", it gives you a small peak
into the world below. But at the same time, similar to "hello world", once you
are introduced into more examples like "if else", then you learn more patterns
and are able to use the syntax. This is kind of similar to OpenGL as once you start
getting into more examples, you might not get a complete understanding of the
underlying mechanics, but you will become familiar enough with the mechanics to
have a general idea of what's going on and understand the patterns for how to
manipulate the API.

In this tutorial the biggest elephant in the room is the ```init_ogl``` function.
In that function we have a lot of boiler plate where we start the EGL context
and get OpenGL to work without a desktop environment. We have a config, a display,
a surface and a context, so we need to at least attempt to try and explain the
relationship between these elements. The remaining functions for the init-shaders
and draw triangles we can probably shove the explanation into tutorial 02.
