# COMP3015-Coursework-2-mblenkhorn-COMP3015-CW2-Optimised-Developer-Tool-with-Documentation
This is a GitHub repository for COMP3015 Coursework 2: Optimised Developer Tool with Documentation


## How does the user interact with your executable? How do you open and control the software you wrote (exe file)?

The user can interact with the executable by bringing up the command prompt, type cd then the directory where the executable is stored, and press ENTER. The user will now need to type the executable's name and if done correctly, it will bring up a window showing a bridge with the waves being animated and going over the bridge. 


## How does the program code work? How do the classes and functions fit together and who does what?

### SceneBasic_Uniform.h

This class contains the variables and functions that the SceneBasic_Uniform.cpp needs to use to render the shaders. The file also contains several includes for the different helper files that SceneBasic_Uniform.cpp also needs such as object meshes or cubes. Plus, the SceneBasic_Uniform.h is a header which means SceneBasic_Uniform.cpp can include it in its class to access its functionality. 

### SceneBasic_Uniform.cpp

This class contains the variables and functions derived from SceneBasic_Uniform.h and adds functionality to them. The initScene() function calls the compile function and the initializeScene function. 
The compile() function simply compiles all the shaders that the program will use. 
The update() function gets the time for the vertex animation as well as rotating the camera around. 
The renderCubes() function simply renders the cubes by calling shader uniform with the uniform passed in as an argument and then the setMatrices function for the shadows is called and renders the cube onscreen. The generateShadowMap calls the lightFrustum functions to get the view and projection matrix, then it binds the buffer and starts the drawing the scene by calling the drawBuildingScene(). The render() function calls the program's use function to render the shader, then it renders the shadow map, sets the shadow shader's uniform, draws the building scene again by calling drawBuildingScene(), then it calls renderCubes(), and lastly, starts up a new shader initialization to render the frustums. 

The initaliseScene calls the setFBO fnction and gets the shader subroutines. Then it creates a shadowscale, and light position and then calls the orient and setPerspective functions from the lightFrustum. Then it gets the lightPV value from a multiplication sum. The function setFBO draws the buffer texture for the shadows and assigns a depth buffer to texture index 0. Then it creates the FBO and draws the buffers. The drawBuildingScene calls the shader code for the shadows, and then it renders the mesh by calling the matrices function and mesh render function. The setMatrice function creates the model matrix for the shadow and the waves respectively. The drawWaves function uses the water wave shader and calls the intensity uniform on the shader. The renderWaves function calls the uniform variables on the shader and then renders the plane for the animation to be visible. 

### Frustum Shader
The frustum shader simply creates a frustum image to show where the shadows cast to. 

### Wave Shader
The Vertex Shader starts by getting the vertex position. Then it translates the vertices on y coordinates and the coordinates are stored in a variable called u. The coordinates are calculated by wave frequency multiplied by x position subtracted by wave velocity multiplied by wave time. Then it calculates calculates the vertices by multiplying the amplitude by the sin of u and storing it in the y position. Then it computes normal vector. Then it passes values to the fragment shader which the position, normal, and texture coordinates. Then it multiplies waveMVP by the position and stores it in gl_Position. 

The Fragment Shader starts by storing the normal in a variable called n. Then it calculates the light vector by subtracting the light's position by the vertex's position. Then it normalizes the light vector and the position vector, and then reflects them. It calculates the dot product of the light and position vectors. Then it calculates the diffuse by multiplying the intensity by the material's diffuse by the dot product. If the dot product is greater than 0, the spec is calculated by multiplying the intensity by the Material's specular highlights by the powers of the dot product product of the position and reflectance vector. Then it returns the sum of Material.Ka * Light.Intensity + diffuse + spec. 


### Shadow Shader
The vertex shader gets the Position by multiplying the ModelViewMatrix by the vertexPosition's Position, gets the normal by normalize the result of NormalMatrix * VertexNormal, gets the shadowCoord by multiply ShadowMatrix by the VertexPosition, and gets the gl_Position by multiplying MVP by the VertexPosition. 

The fragment shader gets the ambient light by multiplying the light intensity by the Material's ambient highlights. Then it gets the diff and spec highlights by normalize the light and position vectors, then it reflects them, then it gets the dot product of the light and position vectors, then it calculates the diffuse by multiplying intensity by material diffuse highlights and the dot. Then it calculates the specular highlights and return the sum of the function. If the shadow coordinates on the z are greater than or equal to 0, then it gets the sum of all the shadow coordinates and multiplies the sum by a quarter. Then it will generate the shadows and perform gamma correction. 


## What makes your shader program special and how does it compare to similar things? (Where did you get the idea from? What did you start with? How did you make yours unique? Did you start with a given shader/project?)

I got the idea from two sources: a bridge scene from the Grand Theft Auto franchise and a level from Super Mario Odyssey that had waves that go other the platforms. These two games were the main source of inspiration for this assignment.  

Grand Theft Auto Inspiration: https://www.google.co.uk/search?q=super+mario+odyssey+poison+level&tbm=isch&sxsrf=ALiCzsYf6s5undPCzLqM9v5py3da0Sg-3A%3A1652698655986&source=hp&biw=1707&bih=849&ei=Hy6CYsjLOYmbgQaI5o_gBg&iflsig=AJiK0e8AAAAAYoI8LzYF1oO_jTyD5EyU4avbHfTmF0kB&ved=0ahUKEwiI17ui7uP3AhWJTcAKHQjzA2wQ4dUDCAc&uact=5&oq=super+mario+odyssey+poison+level&gs_lcp=CgNpbWcQAzoECCMQJzoICAAQsQMQgwE6BAgAEAM6CAgAEIAEELEDOgsIABCABBCxAxCDAToFCAAQgAQ6BAgAEBg6BAgAEB5QAFjAKGDzKWgEcAB4AIABT4gBgQ-SAQIzNpgBAKABAaoBC2d3cy13aXotaW1n&sclient=img#imgrc=Wqt3hElmSR4sVM

Super Mario Odyssey Inspiration: https://www.google.co.uk/search?q=super+mario+odyssey+poison+level&tbm=isch&sxsrf=ALiCzsYf6s5undPCzLqM9v5py3da0Sg-3A%3A1652698655986&source=hp&biw=1707&bih=849&ei=Hy6CYsjLOYmbgQaI5o_gBg&iflsig=AJiK0e8AAAAAYoI8LzYF1oO_jTyD5EyU4avbHfTmF0kB&ved=0ahUKEwiI17ui7uP3AhWJTcAKHQjzA2wQ4dUDCAc&uact=5&oq=super+mario+odyssey+poison+level&gs_lcp=CgNpbWcQAzoECCMQJzoICAAQsQMQgwE6BAgAEAM6CAgAEIAEELEDOgsIABCABBCxAxCDAToFCAAQgAQ6BAgAEBg6BAgAEB5QAFjAKGDzKWgEcAB4AIABT4gBgQ-SAQIzNpgBAKABAaoBC2d3cy13aXotaW1n&sclient=img#imgrc=Wqt3hElmSR4sVM

I started with the code from the OpenGL-Shading-Language Cookbook Third Edition by applying the code in chapter 10 (waves) and in chapter 8 (pcf). I then cleaned up the code in the shaders to make it readable and more maintainable if I want to add more things to it. I also started with the scenebasic header and cpp files and applied the code that was in that chapter. I then added more functions to make it easier to read, more maintainable, and less lines of code cluttering the functions.

https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition/tree/master/chapter10

https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition/tree/master/chapter08

I made my shader program unique and special by doing a combination of the bridge scene and the animated wave effect as well as adding scenery to the scene. I added cubes to make them feel like platforms from a level. The animated waves that go under the bridge feel like an actual river which was the effect I was going for. Also, the effect of the vertex animation for the waves had a cartoonish feel to it which is what I also going for. The bridge was given anti-aliasing shadows and a yellow colour to make it stand out from the scene, and to create the bridge scene that is common in video games which the water travelling below it. 


## YouTube Link
https://youtu.be/6Gt8KaMCjHk



