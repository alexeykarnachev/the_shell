#version 460 core

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;

vec4 texture2DAA(sampler2D tex, vec2 uv) {
    vec2 texsize = vec2(textureSize(tex,0));
    vec2 uv_texspace = uv*texsize;
    vec2 seam = floor(uv_texspace+.5);
    uv_texspace = (uv_texspace-seam)/fwidth(uv_texspace)+seam;
    uv_texspace = clamp(uv_texspace, seam-.5, seam+.5);
    return texture(tex, uv_texspace/texsize);
}

void main() {
    vec4 texture_color = texture2DAA(texture0, fragTexCoord);
    if (texture_color.a < 0.99) discard;

    float plain_color_weight = fragColor.a;
    float texture_color_weight = 1.0 - plain_color_weight;

    float w = fragColor.a;
    vec3 color = fragColor.rgb * w + (1.0 - w) * texture_color.rgb;

    finalColor = vec4(color, 1.0);
}
