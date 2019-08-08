# Computer Shader Ray Tracing
This project was undertaken due to a masters research project in which I am comparing the performance in ms/frame of ray tracing with the compute shader when used with OpenGL 4.6 and Vulkan 1.1.

## Introduction
Ray tracing is a concept heavily researched from the 80's to the present day, yet only around now is it becoming a technique usable within the video games industry with the introduction of NVIDIA's RTX series of GPUs. There is still one problem however and that is having the expensive equipment to actually support ray tracing at a pleasant render speed. The aims of this project is to highlight a graphics library (OpenGL 4.6 or Vulkan 1.1) which can perform better than the other when performing the ray tracing algorithm on the GPU. 

## Ray Tracing Algorithm
In order to understand how the algorithm goes, it is important to understand the preliminary concepts behind the algorithm.
### Backwards Ray Tracing
Ray Tracing was derived from the understanding of how light works to allow us to see the environment around us. In a general case, light is emitted from a **light source**,  is bounced off an object in the **environment**, and finally the light hits our **viewpoint**.  This is what happens in the real world, but this would be too much computing to do on the GPU as there are millions of possible rays which could come out of that one light source let alone having multiple light sources. The correct way to render with rays would be to shoot a ray backwards from the view port till it reaches its destination, which results in a faster compute.

