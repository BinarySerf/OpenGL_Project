#version 460 core

in vec2 TexCoord;
in vec3 ourColor;
out vec4 FragColor;

uniform sampler2D texture1;   
uniform vec2 mousePos;        // Mouse pos (normalized 0-1)
uniform float time;           

void main()
{
    vec2 uv = TexCoord;

    // Distance from mouse to this fragment
    float dist = distance(uv, mousePos);

    // Ripple effect: a sine wave expanding outward from the mouse
    float ripple = sin(30.0 * dist - time * 5.0) * 2;

    // Make ripple stronger near mouse, fade away further
    ripple *= exp(-10.0 * dist);

    // Distort the UV based on the ripple
    vec2 offset = normalize(uv - mousePos) * ripple * 0.02;
    vec2 finalUV = uv + offset;

    FragColor = texture(texture1, finalUV);

    //FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); 
}
