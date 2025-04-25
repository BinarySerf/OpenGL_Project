#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

// 2x2 Bayer Matrix
float thresholdMatrix(vec2 pos)
{
    // Pattern:
    // [ 0.25, 0.75 ]
    // [ 1.00, 0.50 ]
    int x = int(mod(pos.x, 2.0));
    int y = int(mod(pos.y, 2.0));
    
    float thresholds[2][2] = float[2][2](
        float[2](0.25, 0.75),
        float[2](1.00, 0.50)
    );
    
    return thresholds[y][x];
}

void main()
{
    vec4 color = texture(texture1, TexCoord);
    
    // Get fragment position
    vec2 fragCoord = gl_FragCoord.xy;

    // Calculate brightness
    float brightness = dot(color.rgb, vec3(0.299, 0.587, 0.114));

    // Lookup threshold
    float threshold = thresholdMatrix(fragCoord);

    // Compare brightness to threshold
    float finalColor = brightness > threshold ? 1.0 : 0.0;

    FragColor = vec4(vec3(finalColor), 1.0);
}
