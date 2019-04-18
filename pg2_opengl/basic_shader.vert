#version 460 core
layout ( location = 0 ) in vec4 in_position_ms;
layout ( location = 1 ) in vec3 in_normal_ms;
layout ( location = 2 ) in vec3 in_color;
layout ( location = 3 ) in vec2 in_texcoord;
layout ( location = 4 ) in vec3 in_tangent;layout ( location = 5 ) in int in_material_index;

uniform mat4 MVP;
uniform mat4 MVN;

out vec3 unified_normal;
out vec2 texcoord;
flat out int material_index;

void main( void )
{
	gl_Position =  MVP * vec4(in_position_ms.x, in_position_ms.y, in_position_ms.z , 1.0f);
	
	unified_normal = normalize((MVN * vec4(in_normal_ms.x, in_normal_ms.y, in_normal_ms.z, 0.0f)).xyz);
	vec4 hit_es = MVN * vec4(in_position_ms.x, in_position_ms.y, in_position_ms.z , 1.0f);
	
	vec3 omega_i_es = normalize( hit_es.xyz / hit_es.w );
	if ( dot( unified_normal, omega_i_es ) > 0.0f )
	{
		unified_normal *= -1.0f;
	}
	
	texcoord = vec2( in_texcoord.x, 1.0f - in_texcoord.y ); // 3ds max fix
	material_index = in_material_index;
}
