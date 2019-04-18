#include "pch.h"
#include "Rasterizer.h"
#include "objloader.h"
#include "utils.h"
#include "matrix4x4.h"
#include "glutils.h"

Rasterizer::Rasterizer(const int width, const int height, const float fov_y, const Vector3 view_from, const Vector3 view_at)
{
	camera = Camera(width, height, fov_y, view_from, view_at);
}


Rasterizer::~Rasterizer()
{
}

void Rasterizer::loadScene(const std::string file_name) {
	const int no_surfaces = LoadOBJ("../../data/6887_allied_avenger_gi.obj", surfaces_, materials_);
	no_triangles = 0;

	for (auto surface : surfaces_)
	{
		no_triangles += surface->no_triangles();
	}

	this->initBuffers();
}

int Rasterizer::InitDevice() {
	glfwSetErrorCallback(glfw_callback);

	if (!glfwInit())
	{
		return(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	window = glfwCreateWindow(camera.width_, camera.height_, "PG2 OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		if (!gladLoadGL())
		{
			return EXIT_FAILURE;
		}
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_callback, nullptr);

	printf("OpenGL %s, ", glGetString(GL_VERSION));
	printf("%s", glGetString(GL_RENDERER));
	printf(" (%s)\n", glGetString(GL_VENDOR));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//check_gl();

	//glEnable(GL_MULTISAMPLE);

	// map from the range of NDC coordinates <-1.0, 1.0>^2 to <0, width> x <0, height>
	glViewport(0, 0, camera.width_, camera.height_);
	// GL_LOWER_LEFT (OpenGL) or GL_UPPER_LEFT (DirectX, Windows) and GL_NEGATIVE_ONE_TO_ONE or GL_ZERO_TO_ONE
	glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char * vertex_shader_source = LoadShader("basic_shader.vert");
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	SAFE_DELETE_ARRAY(vertex_shader_source);
	CheckShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char * fragment_shader_source = LoadShader("basic_shader.frag");
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	SAFE_DELETE_ARRAY(fragment_shader_source);
	CheckShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// TODO check linking
	glUseProgram(shader_program);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return S_OK;
}

int Rasterizer::initBuffers() {
	const int no_vertices = no_triangles * 3;
	Vertex* vertices = new Vertex[no_vertices];

	const int vertex_stride = sizeof(Vertex);

	int k = 0;
	for (auto surface : surfaces_)
	{
		// triangles loop
		for (int i = 0; i < surface->no_triangles(); ++i)
		{
			Triangle & triangle = surface->get_triangle(i);
			// vertices loop
			for (int j = 0; j < 3; ++j, ++k)
			{
				vertices[k] = triangle.vertex(j);
			} // end of vertices loop

		} // end of triangles loop

	} // end of surfaces loop
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo); // generate vertex buffer object (one of OpenGL objects) and get the unique ID corresponding to that buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, no_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memory
																			   // vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/*glPointSize(10.0f);
	glLineWidth(2.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
	delete[] vertices;
	return S_OK;
}

int Rasterizer::realeaseDevice() {
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shader_program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
	return S_OK;
}


int Rasterizer::RenderFrame() {
	glBindVertexArray(vao);
	while (!glfwWindowShouldClose(window))
	{
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // state using function

		Matrix4x4 model;
		model.set(0, 0, 1);
		model.set(1, 1, 1);
		model.set(2, 2, 1);
		model.set(3, 3, 1);

		Matrix4x4 mvp = camera.projectionMatrix * camera.viewMatrix * model;
		model.EuclideanInverse();
		model.Transpose();
		Matrix4x4 mvn = model;

		SetMatrix4x4(shader_program, mvp.data(), "MVP");
		SetMatrix4x4(shader_program, mvn.data(), "MVN");

		//glDrawArrays( GL_TRIANGLES, 0, vertices / );
		//glDrawArrays( GL_POINTS, 0, 3 );
		glDrawArrays(GL_TRIANGLES, 0, no_triangles * 3);
		//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0); // optional - render from an index buffer

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	realeaseDevice();
	return S_OK;
}
