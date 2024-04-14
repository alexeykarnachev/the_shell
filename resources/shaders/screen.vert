#version 460 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

uniform vec2 screen_size;

out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    vec2 position = vec2(vertexPosition.x, screen_size.y - vertexPosition.y);
    position = (position / screen_size) * 2.0 - 1.0;

    gl_Position = vec4(position, 0.0, 1.0);
}
