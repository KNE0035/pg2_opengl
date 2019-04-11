#version 450 core
in vec3 uniform_normal;

out vec4 FragColor;

void main( void )
{
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//FragColor = vec4((uniform_normal.x + 1) * 0.5, (uniform_normal.y + 1) * 0.5, (uniform_normal.z + 1) * 0.5, 1.0f );
}
