#version 460

uniform struct LightInfo {
    vec4 Position; //the light's position
    vec3 Intensity; //how bright the light will be
} Light;

uniform struct MaterialInfo {
    vec3 Ka; //ambient highlights
    vec3 Kd; //diffuse highlights
    vec3 Ks; //specular highlights
    float Shininess; //how shiny the material will be
} Material;

uniform sampler2DShadow ShadowMap; //the shadowmap that will be applied to the shadow coordinates

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

layout (location = 0) out vec4 FragColor;

vec3 CalculateLightVector()
{
  return vec3(Light.Position) - Position;
}

float CalculateDotProduct(vec3 s, vec3 n)
{
   return max( dot(s,n), 0.0 );
}

vec3 CalculateDiffuse(float sDotN)
{
   return Light.Intensity * Material.Kd * sDotN;
}


vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal; //get the Normal

    vec3 s = normalize(CalculateLightVector()); //calculate and normalize light vector
    vec3 v = normalize(-Position.xyz); //normalize the inverted surface vector
    vec3 r = reflect( -s, n ); //get the reflectance vector

    float sDotN = CalculateDotProduct(s,n);

    vec3 diffuse = CalculateDiffuse(sDotN);

    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return diffuse + spec; //return the sum of diffuse and spec
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


vec3 CalculateAmbient()
{
  return Light.Intensity * Material.Ka;
}

void SumTexels(float sum, float shadow)
{
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,-1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,-1));
     shadow = sum * 0.25; //multiply the sum by a quarter and store it in shadow (this will be the shadows)
}


subroutine (RenderPassType)
void shadeWithShadow()
{
    vec3 ambient = CalculateAmbient(); //calculate ambient highlights
    vec3 diffAndSpec = phongModelDiffAndSpec(); //run the function and get the sum 

    //lookup any nearby texels 
    float sum = 0;
    float shadow = 1.0;

    //if the shadowCoordinates on the z are greater than or equal to 0
    if(ShadowCoord.z >= 0 ) {
        SumTexels(sum, shadow); //get the sum of all the shadowCoordinates
    }

    FragColor = vec4( ambient + diffAndSpec * shadow, 1.0 ); //this will generate shadows from the shadow map

    //perform gamma correction by getting the power of the fragColor and the vec4 subtraction 1.0 / 2.2
    FragColor = pow( FragColor, vec4(1.0 / 2.2) );
}

subroutine (RenderPassType)
void recordDepth()
{
    
}

void main() {
    
    RenderPass();
}