#version 120

varying vec3 N;
varying vec3 v; 

float limit(float value, float step_size)
{
	return value - mod(value, step_size);
}

void main()
{
	// Source for variable names:
    // http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php

    // light in object space
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);

    // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 E = normalize(-v);

    // Reflected ray
    vec3 R = normalize(-reflect(L,N));
  
    vec4 color = gl_FrontLightModelProduct.sceneColor;

    // Ambient:
    color += gl_FrontLightProduct[0].ambient;
 
    // Angle between object's normal and light
    float angle = dot(N,L);

    // Diffuse
    if (angle > 0.0)
        color += gl_FrontLightProduct[0].diffuse * angle;
    
    // Angle between reflection and eye
    float light_angle = dot(R,E);

    // Specular
    if (light_angle > 0.0)
        color += gl_FrontLightProduct[0].specular * pow(light_angle, gl_FrontMaterial.shininess);
    
    gl_FragColor = color;
}
