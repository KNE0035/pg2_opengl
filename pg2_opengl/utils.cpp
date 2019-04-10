#include "pch.h"
#include "utils.h"

using std::mt19937;
using std::uniform_real_distribution;

typedef mt19937                                     Engine;
typedef uniform_real_distribution<float>            Distribution;

auto uniform_generator = std::bind( Distribution( 0.0f, 1.0f ), Engine( 1 ) );


float Random( const float range_min, const float range_max )
{
	float ksi;

//#pragma omp critical ( random ) 
	{
		//ksi = static_cast<float>( rand() ) / ( RAND_MAX + 1 );		
		ksi = static_cast<float>( uniform_generator() );

		/*static float randoms[] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
		static int next = 0;
		ksi = randoms[next];
#pragma omp atomic
		next++;
		if ( next > 10 ) next = 0;*/
	}

	return ksi * ( range_max - range_min ) + range_min;
}

long long GetFileSize64( const char * file_name )
{
	FILE * file = fopen( file_name, "rb" );

	if ( file != NULL )
	{		
		_fseeki64( file, 0, SEEK_END ); // pøesun na konec souboru
		long long file_size = _ftelli64( file ); // zjištìní aktuální pozice
		_fseeki64( file, 0, SEEK_SET ); // pøesun zpìt na zaèátek
		fclose( file );
		file = NULL;

		return file_size;
	}

	return 0;	
}

void PrintTime( double t, char * buffer )
{
	// rozklad èasu
	int days = ( int )( t / ( 24.0 * 60.0 * 60.0 ) );
	int hours = ( int )( ( t - days * 24.0 * 60.0 * 60.0 ) / ( 60.0 * 60.0 ) );
	int minutes = ( int )( ( t - days * 24.0 * 60.0 * 60.0 - hours * 60.0 * 60.0 ) / 60.0 );
	double seconds = t - days * 24.0 * 60.0 * 60.0 - hours * 60.0 * 60.0 - minutes * 60.0;

	// ošetøení chybných stavù jako napø. 1m60s a pøevedení na korektní zápis 2m00s
	if ( seconds >= 59.5 )
	{
		seconds = 0.0;
		minutes++;
		if ( minutes == 60 )
		{
			minutes = 0;
			hours++;
			if ( hours == 24 )
			{
				hours = 0;
				days++;
			}
		}
	}

	// tisk èasu s odfiltrováním nulových poèáteèních hodnot 0d0h10m14s => 10m14s
	if ( days == 0 )
	{
		if ( hours == 0 )
		{
			if ( minutes == 0 )
			{
				if ( seconds < 1.0 )
				{
					sprintf( buffer, "%0.0f ms", seconds*1e+3 );
				}
				else
				{
					sprintf( buffer, "%0.1f s", seconds );
				}
			}
			else
			{
				sprintf( buffer, "%d m%02.0f s", minutes, seconds );
			}
		}
		else
		{
			sprintf( buffer, "%d h%02d m%02.0f s", hours, minutes, seconds );
		}
	}
	else
	{
		sprintf( buffer, "%d d%02d h%02d m%02.0f s", days, hours, minutes, seconds );
	}
}

std::string TimeToString( const double t )
{
	// rozklad èasu
	int days = static_cast<int>( t / ( 24.0 * 60.0 * 60.0 ) );
	int hours = static_cast<int>( ( t - days * 24.0 * 60.0 * 60.0 ) / ( 60.0 * 60.0 ) );
	int minutes = static_cast<int>( ( t - days * 24.0 * 60.0 * 60.0 - hours * 60.0 * 60.0 ) / 60.0 );
	double seconds = t - days * 24.0 * 60.0 * 60.0 - hours * 60.0 * 60.0 - minutes * 60.0;

	// ošetøení chybných stavù jako napø. 1m60s a pøevedení na korektní zápis 2m00s
	if ( seconds >= 59.5 )
	{
		seconds = 0.0;
		++minutes;

		if ( minutes == 60 )
		{
			minutes = 0;
			++hours;

			if ( hours == 24 )
			{
				hours = 0;
				++days;
			}
		}
	}

	char buffer[32] = { 0 };

	// tisk èasu s odfiltrováním nulových poèáteèních hodnot 0d0h10m14s => 10m14s
	if ( days == 0 )
	{
		if ( hours == 0 )
		{
			if ( minutes == 0 )
			{
				if ( seconds < 10 )
				{
					if ( seconds < 1 )
					{
						sprintf( buffer, "%0.1fms", seconds * 1e+3 );						
					}
					else
					{
						sprintf( buffer, "%0.1fs", seconds );
					}
				}
				else
				{
					sprintf( buffer, "%0.0fs", seconds );
				}
			}
			else
			{
				sprintf( buffer, "%dm%02.0fs", minutes, seconds );
			}
		}
		else
		{
			sprintf( buffer, "%dh%02dm%02.0fs", hours, minutes, seconds );
		}
	}
	else
	{
		sprintf( buffer, "%dd%02dh%02dm%02.0fs", days, hours, minutes, seconds );
	}

	return std::string( buffer );
}

char * LTrim( char * s )
{
    while ( isspace( *s ) || ( *s == 0 ) )
	{
		++s;
	};

    return s;
}

char * RTrim( char * s )
{
    char * back = s + strlen( s );

    while ( isspace( *--back ) );

    *( back + 1 ) = '\0';

    return s;
}

char * Trim( char * s )
{
	return RTrim( LTrim( s ) );
}

bool check_gl(const GLenum error)
{
	if (error != GL_NO_ERROR)
	{
		//const GLubyte * error_str;
		//error_str = gluErrorString( error );
		//printf( "OpenGL error: %s\n", error_str );

		return false;
	}

	return true;
}

/* glfw callback */
void glfw_callback(const int error, const char * description)
{
	printf("GLFW Error (%d): %s\n", error, description);
}

/* OpenGL messaging callback */
void GLAPIENTRY gl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * user_param)
{
	printf("GL %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "Error" : "Message"),
		type, severity, message);
}

/* invoked when window is resized */
void framebuffer_resize_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/* load shader code from text file */
char * LoadShader(const char * file_name)
{
	FILE * file = fopen(file_name, "rt");

	if (file == NULL)
	{
		printf("IO error: File '%s' not found.\n", file_name);

		return NULL;
	}

	size_t file_size = static_cast<size_t>(GetFileSize64(file_name));
	char * shader = NULL;

	if (file_size < 1)
	{
		printf("Shader error: File '%s' is empty.\n", file_name);
	}
	else
	{
		/* v glShaderSource nezadáváme v posledním parametru délku,
		takže øetìzec musí být null terminated, proto +1 a reset na 0*/
		shader = new char[file_size + 1];
		memset(shader, 0, sizeof(*shader) * (file_size + 1));

		size_t bytes = 0; // poèet již naètených bytù

		do
		{
			bytes += fread(shader, sizeof(char), file_size, file);
		} while (!feof(file) && (bytes < file_size));

		if (!feof(file) && (bytes != file_size))
		{
			printf("IO error: Unexpected end of file '%s' encountered.\n", file_name);
		}
	}

	fclose(file);
	file = NULL;

	return shader;
}

/* check shader for completeness */
GLint CheckShader(const GLenum shader)
{
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	printf("Shader compilation %s.\n", (status == GL_TRUE) ? "was successful" : "FAILED");

	if (status == GL_FALSE)
	{
		int info_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		char * info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetShaderInfoLog(shader, info_length, &info_length, info_log);

		printf("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}
