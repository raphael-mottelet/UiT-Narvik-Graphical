 # task7

 I added a UV-sphere mesh and switched to a Phong pipeline. The vertex shader transforms each vertex to clip space and passes world-space position and a normalized world-space normal (using a normal matrix). 
 
 The fragment shader then does per-fragment Phong lighting: ambient + diffuse (Lambert) + specular (Phong), using the camera position for the view vector and a single point light.

Light setup: position (2, 2, 2). Ambient (0.10, 0.10, 0.12) is low so the sphere isn’t flat but shadows aren’t crushed. Diffuse (1, 1, 1) is white so the sphere keeps its blue color. Specular (1, 1, 1) gives a clean white highlight.

Material (blue sphere): ambient (0.02, 0.04, 0.10), diffuse (0.20, 0.40, 1.00), specular (0.90, 0.90, 0.95), shininess 32. This makes it look like a slightly glossy plastic—defined highlight without being mirror-like. 

Per-fragment shading keeps the highlight smooth and accurate compared to per-vertex. Depth test and back-face culling are enabled for correct rendering.