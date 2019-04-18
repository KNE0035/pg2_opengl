#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 MVN;

out vec4 uniform_normal;

void main( void )
{
	gl_Position =  MVP * vec4(position.x, position.y, position.z , 1.0f);
	
	uniform_normal = MVN * vec4(normal.x, normal.y, normal.z, 1.0f);

	uniform_normal.xyz = normalize(uniform_normal.xyz);
}
