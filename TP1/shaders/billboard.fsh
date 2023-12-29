uniform sampler2D texture;
varying vec4 uv;
uniform float alpha;
uniform float numTexture;
uniform float elapsed;

void main() {
    gl_FragColor = texture2D(texture, uv.st) ;
    gl_FragColor.a *= alpha;
}
