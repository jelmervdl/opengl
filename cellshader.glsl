#version 120

varying vec3 N;
varying vec3 v; 

float step(float value, float step_size)
{
	return value + (step_size - mod(value, step_size));
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
    
    if (dot(N, E) < 0.4)
        color = vec4(0, 0, 0, 1.0);

    gl_FragColor = vec4(
        step(color.r, 0.2),
        step(color.g, 0.2),
        step(color.b, 0.2), 1.0);
}
