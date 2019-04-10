#version 460 core
layout (location = 0) in vec3 position;

//uniform mat4 MVP;

void main( void )
{
	
	gl_Position =  MVP * vec4(position,1);
	//Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	//gl_Position = vec4( position.x, -position.y, position.z, 1.0f );
	//gl_Position = vec4( position, 1.0f );
}
