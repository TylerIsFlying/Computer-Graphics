#include "context.h"
#include "render.h"
#include "glm/ext.hpp"
int main()
{
	context engine;
	engine.init(512, 512, "Tyler Engine");
	
	// vertices (remember that work in unity?)
	vertex triVerts[] =
	{
		{ { -.5f, -.5f, 0, 1 }, { 0.0f, 0.0f, 1.0f, 1.0f }, {0,   0}, {0.0f, 0.0f, -1.0f, 0.0f} }, // bottom-left
		{ {  .5f, -.5f, 0, 1 }, { 0.0f, 1.0f, 0.0f, 1.0f }, {1,   0}, {0.0f, 0.0f, -1.0f, 0.0f} }, // bottom-right
		{ {   0.f, .5f, 0, 1 }, { 1.0f, 0.0f, 0.0f, 1.0f }, {0.5, 1}, {0.0f, 0.0f, -1.0f, 0.0f} }  // top-center
	};

	vertex quadVerts[] =
	{
		{ { -1.f, -1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {0, 0}, {0.0f, 0.0f, -1.0f, 0.0f} }, // bottom-left
		{ {   1.f,-1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1, 0}, {0.0f, 0.0f, -1.0f, 0.0f} }, // bottom-right
		{ {  -1.f, 1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {0, 1}, {0.0f, 0.0f, -1.0f, 0.0f} }, // top-left
		{ {   1.f, 1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1, 1}, {0.0f, 0.0f, -1.0f, 0.0f} }  // top-right
	};

	// indices (remember that work in unity)
	unsigned int triIndices[] = { 2,0,1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// make geometry
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry triObj = loadGeometry("res\\soulspear.obj");
	geometry cubeObj = loadGeometry("res\\cube.obj");
	geometry quad = makeGeometry(quadVerts, 4, quadIndices, 6);

	// load textures 
	texture texImage = loadTexture("res\\terry.png");

	//make the shader
	shader basicShader = loadShader("res\\basic.vert", "res\\basic.frag");
	shader mvpShader = loadShader("res\\mvp.vert", "res\\basic.frag");
	shader lightShader = loadShader("res\\light.vert", "res\\light.frag");

	light sun = { {-1,0,0}, {1,1,1} };

	// set up our matrices
	glm::mat4 camProj = glm::perspective(glm::radians(80.0f), 512.f / 512.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 triModel = glm::identity<glm::mat4>();

	//setUniform(mvpShader,0,camProj);
	//setUniform(mvpShader,1, camView);
	//setUniform(mvpShader, 2, triModel);

	setUniform(basicShader, 3, texImage, 0);

	setUniform(lightShader, 6, sun.color);
	setUniform(lightShader, 7, sun.direction);
	setUniform(lightShader, 0, camProj);
	setUniform(lightShader, 1, camView);
	setUniform(lightShader, 2, triModel);

	// game loop
	while(!engine.shouldClose())
	{
		engine.tick();

		// implement game logic here
		triModel = glm::rotate(triModel, glm::radians(1.0f), glm::vec3(0, 1, 0));

		engine.clear();

		// implement render logic here
		setUniform(lightShader, 2, triModel);
		setUniform(lightShader, 4, engine.time());

		//draw(mvpShader, cubeObj);
		draw(lightShader, cubeObj);
		//draw(mvpShader, triObj);
	}
	freeGeometry(triangle);
	freeGeometry(cubeObj);
	freeGeometry(triObj);
	freeGeometry(quad);

	freeShader(mvpShader);
	freeShader(basicShader);
	engine.term();
	return 0; 
}