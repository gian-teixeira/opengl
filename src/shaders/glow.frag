#version 120

uniform sampler2D inputTexture;
uniform vec2 textureSize;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0);
    float blurSize = 1.0 / textureSize.x;

    for (float i = -4.0; i <= 4.0; i += 1.0)
    {
        color += texture2D(inputTexture, vec2(texCoord.x + i * blurSize, texCoord.y)) * 0.1
            + texture2D(inputTexture, vec2(texCoord.x, texCoord.y + i * blurSize)) * 0.1;
    }

    gl_FragColor = color;
}