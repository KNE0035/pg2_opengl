#version 460 core
layout (location = 0) in vec3 position;

uniform mat4 mvp; //mvp matrix

void main( void )
{
	


	gl_Position =  mvp * vec4(position.x, position.y, position.z , 1.0f);


	//Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	//gl_Position = vec4( position.x, -position.y, position.z, 1.0f );
	//gl_Position = vec4( position, 1.0f );
}
