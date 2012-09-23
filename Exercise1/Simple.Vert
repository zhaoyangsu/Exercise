const char* SimpleVertexShader = STR(
attribute vect4 Position;
attribute vect4 SourceColor;
varying vect4 DestinationColor;
uniform mat4 Projection;
uniform mat4 Modelview;
void main(void)
{
    DestinationColor = SourceColor;
    gl_Position = Projection * Modelview * Position;
}
);