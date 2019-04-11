#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvp; // mvp matrix
uniform mat4 mvn;

out vec3 uniform_normal;

void main( void )
{
	gl_Position =  mvp * vec4(position.x, position.y, position.z , 1.0f);

	uniform_normal = mvn * normal;
	uniform_normal = normalize(uniform_normal);
}
