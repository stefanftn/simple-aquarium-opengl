#version 330 core

layout(location = 0) in vec2 inPos; // pozicija temena oblika (jedinicne velicine oko (0, 0))
layout(location = 1) in vec2 inTex; // koordinate teksela
out vec2 chTex;

uniform vec2 uPos;   // centar objekta u NDC
uniform vec2 uScale; // sirina i visina objekta u NDC (negativna sirina = ogledalo)

void main()
{
    gl_Position = vec4(inPos.x * uScale.x + uPos.x, inPos.y * uScale.y + uPos.y, 0.0, 1.0);
    chTex = inTex;
}
