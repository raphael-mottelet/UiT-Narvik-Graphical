# Task Geometry (Box & Pyramid), little explications

I replaced the Task 1 red triangle with two new meshes: a unit box and a square-base pyramid. Each mesh is built from raw vertex positions and an index buffer. 

I kept the same minimal shader pair from Task 1: the vertex shader transforms positions with an MVP matrix, and the fragment shader uses a uniform uColor to color each object. 

The camera and input code are basically a copy pasta of Task 1; arrow keys move the scene (via camera motion), A/D rotate the box around Y, and Q/E rotate the pyramid. 

Finally, I draw each object by setting its own model transform and uColor before issuing glDrawElements.