#version 460

uniform vec4 frustumColour;

layout ( location = 0 ) out vec4 FragColor;

void main()
{
    FragColor = frustumColour; //passes in the colour to the fragment colour
}