Topics in computerized graphics  project- Febuary 2023

Overview 
We made a 3D scene from a heightmap image with the ability to increase/decrease the quality    by adding/removing triangles  (meaning ,  we sample more points and thus have more triangles) .
:Goals
Build a 3D height map from an image  
Interaction – mouse picking of triangle and 3D model translate, scale, rotate  
Grid resolution (down-sample, up-sample)  
Path Navigation –shortest geodesic path   
Environmental simulations –rain, fog  

Specifications
The project was building in CPP with OpenGL and OpenCV 
   For path navigation, we used Dijkstra's Alogrithm as the norm is the weight between two edges in the grid
For the rain we've used a stack that keeps the particles and redraws them in every updete
  We've used built in openGL fucntions in the function initfog to create the fog 

Viewports  
Side view camera
 





Top view camera
 















Free Roam Camera
 
    Up Arrow – move forward
Down Arrow – move backward
 Left Arrow – rotate left
 Right Arrow – rotate right 
 Y – scale up
 H – scale down

















We have also implemented the following environmental simulations
Rain&Fog
With rain
 
Without Rain
 
In order to strat rain press L
In order to stop rain press L again
In order to increase to amount of rain press P
In order to decrease to amount of rain press O










With fog
 
 
In order to create fog press K
In order to stop the fog press K again
The fog is design in a way that the farther we get away from the map, the fogiest it gets















Grid Resolution
Up sampling
 
Down Sampling
 
From high to low resolution

In order to change the resolution from highe to low press C
In order to change the resolution from low to high press B 








Path Navigation & Picking Mode
 
 
 
In order to find the path between two points mark two triangles and press V


References
 Heightmap: https://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16 006/ 
:Picking
 http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-openglhack/
