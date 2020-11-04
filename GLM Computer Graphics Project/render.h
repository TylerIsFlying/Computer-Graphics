#pragma once
#include "glew/GL/glew.h" // glew
#include "glm/glm.hpp" // glm math types
#include "glm/ext.hpp" // glm::value_ptr(value);
#include <iostream>
#include "stb/stb_image.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include <fstream>

// defining our vertex structure
struct vertex
{
	glm::vec4 pos; // vertex position
	glm::vec4 col;	// vertex color
	glm::vec2 uv; // texture coordinates
	glm::vec4 normal;
};

// an object to represent our mesh
struct geometry
{
	GLuint vao, vbo, ibo; // buffers
	GLuint size; // index count
};

// an object to represent our shader
struct shader 
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

struct light
{
	glm::vec3 direction; // should be normalized
	glm::vec3 color;
};

// functions to make and unmake the above types
geometry loadGeometry(const char* filePath);

geometry makeGeometry(vertex * verts, size_t vertCount,unsigned int* indices, size_t indxCount );

texture loadTexture(const char* filePath);
texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels);
void freeTexture(texture& tex);

void freeGeometry(geometry& geo);

shader loadShader(const char* vertPath, const char* fragPath);
shader makeShader(const char* vertSource, const char* fragSource);

void freeShader(shader& shad);

void draw(const shader& shad, const geometry& geo);

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader& shad, GLuint location, float value);
void setUniform(const shader& shad, GLuint location, const glm::vec3 &value);
void setUniform(const shader& shad, GLuint location, const texture &tex, int textureSlot);

// check the shader
bool checkShader(GLuint target, const char* humanReadableName = "");


