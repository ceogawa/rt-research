/* Pseudocode:
1. Find bounds of scene (furthest point from camera in camera direction)
2. Assign k centers to the planes of the scene perpendicular to the camera 
    at furthest/k intervals 
3. Update k centers based on vertices added to each layer
4. Find center of mass for each layer 
5. Place lights at each, ensuring that there is an object in front of the light 

- 4 lights at the center, have all lights move towards corner 
- sorting libraries */