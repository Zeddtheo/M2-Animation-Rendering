attribute vec4 in_position;
attribute vec4 in_uv;
varying vec4 uv;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float particleSize;

uniform float lifeTimeSize;
uniform float grosse;
uniform float indexTex;

void main() {
    uv = in_uv;
    uv.s = uv.s + mod(indexTex, 4.0f) * 0.25;
    uv.t = uv.t + floor(indexTex/4.0f) * 0.25f;

    mat4 modelView = viewMatrix * modelMatrix;
    modelView[0][0] = 1.0;
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    modelView[1][0] = 0.0;
    modelView[1][1] = 1.0;
    modelView[1][2] = 0.0;

    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = 1.0;

    vec4 pos = vec4(vec3(in_position) * particleSize, 1.0);
    gl_Position = projectionMatrix * modelView * pos;
}
