#version 460

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;

out vec3 Normal;
out vec3 Position;
out vec4 ShadowCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ShadowMatrix;

void main()
{
    //multiply the ModelViewMatrix by the vertexPosition's Position
    Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;

    //normalize the result of NormalMatrix * VertexNormal
    Normal = normalize( NormalMatrix * VertexNormal );

    //multiply ShadowMatrix by the VertexPosition
    ShadowCoord = ShadowMatrix * vec4(VertexPosition,1.0);

    gl_Position = MVP * vec4(VertexPosition,1.0);
}