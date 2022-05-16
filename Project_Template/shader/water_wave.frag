#version 460


uniform struct LightInfo {
    vec4 Position; //the light's position
    vec3 Intensity; //how bright the light will be
} Light;

uniform struct MaterialInfo {
    vec3 Ka; //ambient reflectivity
    vec3 Kd; //diffuse reflectivity
    vec3 Ks; //specular reflectivity
    float Shininess; //how shiny the material will look
} Material;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform float Time; //the Time variable for the vertices on the water_wave vertex shader

layout ( location = 0 ) out vec4 FragColor;


vec3 phongModel(vec3 kd) {
    vec3 n = Normal;
    vec3 s = normalize(Light.Position.xyz - Position.xyz);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return Material.Ka * Light.Intensity + diffuse + spec;
}

void main()
{
    FragColor = vec4( phongModel(Material.Kd) , 1.0 );
}