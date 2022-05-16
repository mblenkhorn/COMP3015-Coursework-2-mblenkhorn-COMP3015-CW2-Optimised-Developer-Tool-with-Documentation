#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 nightVisionModelViewMatrix;
uniform mat3 nightVisionNormalMatrix;
uniform mat4 nightVisionProjectionMatrix;
uniform mat4 nightVisionMVP;

void main()
{
    TexCoord = VertexTexCoord;
    Normal = normalize( nightVisionNormalMatrix * VertexNormal);
    Position = vec3( nightVisionModelViewMatrix * vec4(VertexPosition,1.0) );

    gl_Position = nightVisionMVP * vec4(VertexPosition,1.0);
}