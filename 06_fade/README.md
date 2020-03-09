# Fade

So we're on to a pretty basic example that uses a uniform. In this case we're
using a single float value as a uniform to control the opacity of the triangle.
In this case we can use the CPU to calculate what the fade should be based on
the time as we increment the value. And on each frame before every draw call, 
we update the value of the uniform. The GPU will then remember that as a fixed
value until we update it again, which happens to be on each frame.

As far as opacity goes, this example doesn't exactly work. I'm not sure why the
example shows the console under the triangle each time. So I think there could
be some setting that I'm missing when setting up the OpenGL context.
