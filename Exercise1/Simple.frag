const char* SimpleFragmentShader = STR(
varying lowp vec4 DestinationColor;
void main(void)
{
    gl_FragColor = DestinationColor;
}
);