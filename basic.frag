#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;

uniform bool useTex;    // flag: true - crta se tekstura, false - crta se boja
uniform vec4 uCol;      // boja (rgba) kada nema teksture
uniform float uAlpha;   // dodatna providnost za teksture
uniform bool gradient;  // blagi gradijent za vodu (tamnije na dnu)

void main()
{
    if (useTex) {
        vec4 texCol = texture(uTex, chTex);
        outCol = vec4(texCol.rgb, texCol.a * uAlpha);
    }
    else {
        outCol = uCol;
        if (gradient) outCol.rgb = uCol.rgb * (0.7 + 0.3 * chTex.t); // t ide od 0 (dno) do 1 (vrh)
    }
}
