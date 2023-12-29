attribute vec4 in_position;
attribute vec4 in_uv;
varying vec4 uv;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float poissonTaille;

void main() {
   uv = in_uv;
   mat4 modelView = viewMatrix * modelMatrix;
   vec4 pos = vec4(vec3(in_position) * poissonTaille, 1.0);
   gl_Position = projectionMatrix * modelView * pos;
}
