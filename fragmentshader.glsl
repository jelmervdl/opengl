#version 120

varying vec3 N;
varying vec3 v; 

float limit(float value, float step_size)
{
	return value - mod(value, step_size);
}

void main()
{
	// Source: http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php

	vec3 L = normalize(gl_LightSource[0].position.xyz - v);
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L,N));
  
    //calculate Ambient Term:
    vec4 Iamb = gl_FrontLightProduct[0].ambient;
 
    //calculate Diffuse Term:
    vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);
    
    // calculate Specular Term:
    vec4 Ispec = gl_FrontLightProduct[0].specular
                 * pow(max(dot(R,E), 0.0), 0.3 * gl_FrontMaterial.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);

    // write Total Color:
    vec4 color = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;

    gl_FragColor = color;
}
