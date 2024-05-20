Topics in computerized graphics  project- Febuary 2023

Overview 
I made a 3D scene from a heightmap image with the ability to increase/decrease the quality    by adding/removing triangles  (meaning ,  we sample more points and thus have more triangles) .
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
 
![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/71270b52-200a-448a-9ff5-312ffb08b9b3)





Top view camera
 

![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/2f2ec500-2fe3-4960-8be8-bb9bb0d22984)











Free Roam Camera

![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/7a22436e-dd9b-4dc7-8313-fd38482f14de)


Up Arrow – move forward
Down Arrow – move backward
Left Arrow – rotate left
Right Arrow – rotate right 
Y – scale up
H – scale down

















I have also implemented the following environmental simulations
Rain&Fog
With rain

![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/ad9404b9-69a3-4a5d-b9b1-1b9eabf6137a)

 
Without Rain

![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/dc6c7800-cd9c-46f4-9bd8-f564240755f1)

 
In order to strat rain press L
In order to stop rain press L again
In order to increase to amount of rain press P
In order to decrease to amount of rain press O










With fog

 ![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/5adf88e1-e4db-4f50-a988-93bbf58fabe7)

![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/18d7ea01-3744-4ad9-bdb0-2d6eb4d9b51f)

 
In order to create fog press K
In order to stop the fog press K again
The fog is design in a way that the farther we get away from the map, the fogiest it gets















Grid Resolution
Up sampling
![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/7a3e4e90-809b-4c5d-bce6-45974d20ac6f)

 
Down Sampling
![image](https://github.com/noamst/Landspace-Rendering/assets/19705801/e2089d98-41bf-41f1-9d1c-0c10d2f61407)

 
From high to low resolution

In order to change the resolution from highe to low press C
In order to change the resolution from low to high press B 








Path Navigation & Picking Mode
 
 
 
In order to find the path between two points mark two triangles and press V


References
 Heightmap: https://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16 006/ 
:Picking
 http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-openglhack/
