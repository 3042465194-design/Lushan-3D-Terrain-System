#version 330 core
in vec2 TexCoord;
in float Height;
out vec4 FragColor;

uniform sampler2D texSampler;
uniform bool useElevationColor;
uniform float minHeight;
uniform float maxHeight;

void main()
{
    vec4 texColor = texture(texSampler, TexCoord);
    if (useElevationColor) {
        float t = clamp((Height - minHeight) / (maxHeight - minHeight), 0.0, 1.0);
        vec3 lowColor = vec3(0.2, 0.4, 0.1);   // 低处绿色
        vec3 highColor = vec3(0.8, 0.7, 0.5);  // 高处土黄
        vec3 elevationColor = mix(lowColor, highColor, t);
        // 混合纹理颜色和高程颜色，这里简单叠加（可调整权重）
        vec3 finalColor = mix(texColor.rgb, elevationColor, 0.5);
        FragColor = vec4(finalColor, 1.0);
    } else {
        FragColor = texColor;
    }
}