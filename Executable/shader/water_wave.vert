#version 460

layout (location = 0 ) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

//wave parameters
uniform float waveTime;
uniform float waveFreq = 2.5;
uniform float waveVelocity = 2.5;
uniform float waveAmp = 0.6;

uniform mat4 waveModelViewMatrix;
uniform mat3 waveNormalMatrix;
uniform mat4 waveMVP;


void CalculateVertices(vec4 pos, float u)
{
    pos.y = waveAmp * sin( u );
}

void CalculateMatrices(vec4 pos, vec3 n)
{
    Position = waveModelViewMatrix * pos;
    Normal = waveNormalMatrix * n;
    TexCoord = VertexTexCoord;
}


void main()
{
    vec4 pos = vec4(VertexPosition,1.0);

    //translate vertices on y coordinates
    //the variable U stores the current vertice
    //pos.y stores the y coordinates
    //float u = waveFreq * pos.x - waveVelocity * waveTime;
    //pos.y = waveAmp * sin( u );
    float u = waveFreq * pos.x - waveVelocity * waveTime;
    CalculateVertices(pos, u);

    //compute the normal vector
    //the vector needs to be normalized before passing values
    //vec3 n = vec3(0.0);
    //n.xy = normalize(vec2(cos( u ), 1.0));
    vec3 n = vec3(0.0);
    n.xy = normalize(vec2(cos( u ), 1.0));


    //pass values to the fragment shader
    CalculateMatrices(pos, n);
    //Position = waveModelViewMatrix * pos;
    //Normal = waveNormalMatrix * n;
    //TexCoord = VertexTexCoord;
    
    gl_Position = waveMVP * pos;
}