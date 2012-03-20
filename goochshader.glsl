#version 120

varying vec3 N;
varying vec3 v;

float b = 0.55;
float y = 0.3;

float alpha = 0.25;
float beta = 0.5; 

void main()
{
    // light in object space
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);

    // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 E = normalize(-v);

    // Reflected ray
    vec3 R = normalize(-reflect(L,N));
  
    // Angle between object's normal and light
    float angle = dot(N,L);

    vec4 kd = gl_FrontLightModelProduct.sceneColor;

    vec4 kCool = vec4(0, 0, b, 0) * alpha * kd;
    vec4 kWarm = vec4(y, y, 0, 0) * beta * kd;

    vec4 color = 128 * (kCool * ((1.0 - angle) / 2) + kWarm * ((1.0 + angle) / 2));
    
    // Angle between reflection and eye
    float reflection_angle = dot(R,E);

    // Specular
    if (reflection_angle > 0.0)
        color += gl_FrontLightProduct[0].specular * pow(reflection_angle, gl_FrontMaterial.shininess);
    
    gl_FragColor = color;
}
