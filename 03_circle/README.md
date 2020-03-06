# Circle

So we're now into our third tutorial, and in the first tutorial we drew a
single triangle and there wasn't a lot to go on in terms of describing a
pattern. And then we drew a square, which was two triangles, and anyone 
observant would probably notice a trend in terms of how the square was
being drawn. In this tutorial we draw a "circle" which can be an arbitrary
number of triangles. In this case our "circle" is actually an octagon, or
a decagon, or how ever many number of triangles we decide to generate
for the number of sides.

So before we do anything, let's do a quick review for how we're generating
the circle. Basically what we're doing is we're drawing a circle by generating
pieces of a pie in a circle. So we make a triangle by starting on the outside
of the circle, move into the center for the next vertex and then finish the
triangle on the outside of the circle. So using a very small number of triangles
we ends up with a hexagon or an octagon, and the more triangles that we use
the more and more circular the circle will look. Except, we aren't doing anything
to account for the screen width, so we end up dawing an oval more than anything.

We can take this oppertunity to review what we're doing for the bind the and draw 
mechanics since an example with arbitrary number of triangles provides a bit of hint.
We described the way the circle was drawn for each triangle. We store these triangles
in a buffer. Each vertex is 3 floats. Each float has a sie of 4 bytes. And each 
triangle has 3 vertices. So we take the number of triangles, multiply that by 
3 by 3 by 4 and that's the size of the buffer where we save the vertices to the
buffer.

When we goto actually draw our circle, we first need to tell the GPU which program
we want it to use. So we first tell it which shader to use, which if it brings up
the subject, then yes it is possible to define multiple shaders to for rendering
different styles for different draw calls. But we don't need to worry about that
too much right now since we only have one shader that we're dealing with. So
we tell the GPU which shader that we want it to be using, and then we tell it
which buffer that we want it to use, so we direct it to use the circle vertices
vertex that we saved earlier.

Then when we tell the GPU to actually draw the vertex, we need to let the
GPU know what actually fits into what part of the program. This is probably
a bad explanation, but the way I visualize it is like fitting materia into
armor slots in final fantasy seven. Like for example for our position we
have one armor with three slots, so we put in the materia for x, y, and z
respectively in those slots. And then we also have a another piece of armor
for color, so we put in the r, g, and b materia into those slots. And then
since we have six materia that we want to use at any give time, we need to
set the stride to fit in the slots for each triangle. I guess this is where
the materia analogy kind of breaks down, and becomes more like a string of
beads or something. I get the feeling there could be some kind of anal bead
analogy that could be explanded on to explain this, but I digress.

Once we have these elements in place, the shader which describes what we want
to offload to the GPU, the buffer which provides the GPU with access to
memory for the vertices and color information. And then we tell the GPU
how the attributes of the position and color are spaced in the buffer, and
last we implement the actual draw call by telling the GPU how many triangles we
want it to draw. And that results in our weird polygonal oval thing being drawn 
on the screen. So we've done single shapes, let's go ahead an try drawing multiple
shapes in the next tutorial.
