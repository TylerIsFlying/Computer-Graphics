#include "render.h"

geometry loadGeometry(const char* filePath)
{
    
    // load up all of the data from the file
    tinyobj::attrib_t vertexAttributes;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;

    // double-check that everything's OK

    
    bool success = tinyobj::LoadObj(
        &vertexAttributes, 
        &shapes, 
        &materials, 
        &error, 
        filePath);

    if (!success)
    {
        fprintf(stderr, error.c_str());
        return {};  // return empty geo -- indicating failure
    }
    

    // get mesh data
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;

    // form geometry data out of the mesh data provided by tinyobj
    size_t offset = 0;
    for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); ++i)
    {
        unsigned char faceVertices = shapes[0].mesh.num_face_vertices[i];

        assert(faceVertices == 3 && "unsupported number of face vertices");

        // iterating over the vertex data to fit our own vertex format
        for (unsigned char j = 0; j < faceVertices; ++j)
        {
            tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];

            // position
            tinyobj::real_t posX = vertexAttributes.vertices[3 * idx.vertex_index + 0];
            tinyobj::real_t posY = vertexAttributes.vertices[3 * idx.vertex_index + 1];
            tinyobj::real_t posZ = vertexAttributes.vertices[3 * idx.vertex_index + 2];

            // vertex colors
            tinyobj::real_t colR = 1.0f;
            tinyobj::real_t colG = 1.0f;
            tinyobj::real_t colB = 1.0f;

            // UVs
            tinyobj::real_t texU = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
            tinyobj::real_t texV = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

            // normal
            tinyobj::real_t norX = vertexAttributes.normals[3 * idx.normal_index + 0];
            tinyobj::real_t norY = vertexAttributes.normals[3 * idx.normal_index + 1];
            tinyobj::real_t norZ = vertexAttributes.normals[3 * idx.normal_index + 2];

            vertices.push_back(
                vertex
                {
                    {posX, posY, posZ, 1.0f},   // vertex position
                    {colR, colG, colB, 1.0f},    // vertex color
                    {texU, texV},    // texture coordinates
                    {norX, norY, norZ, 0.0f} // vertex normal
                }
            );
            indices.push_back(faceVertices * i + j);
        }
        offset += faceVertices;
    }

    // return makeGeometry using the data from tinyobj
    return makeGeometry(vertices.data(), vertices.size(), indices.data(), indices.size());
    
}

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned int* indices, size_t indxCount)
{
    // make an instance geometry
    geometry newGeo = {};
    newGeo.size = indxCount;

    // generate buffers and VAO
    glGenBuffers(1, &newGeo.vbo); // VBO
    glGenBuffers(1, &newGeo.ibo); // IBO
    glGenVertexArrays(1, &newGeo.vao); // VAO

    // bind the VAO and buffers
    // note: order matters
    glBindVertexArray(newGeo.vao);
    glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

    // populate the buffers
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    auto off = offsetof(vertex, pos);

    // describe the data contained within the buffers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,        // position
        4,        // how many things?
        GL_FLOAT,     // what types of things are in that thing?
        GL_FALSE,     // normalize this?
        sizeof(vertex),       // byte offset between vertices
        (void*)0);        //byte offset within a vertex to get this data
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,        // vertex color
        4,        // how many things?
        GL_FLOAT,     // what types of things are in that thing?
        GL_FALSE,     // normalize this?
        sizeof(vertex),       // byte offset between vertices
        (void*)offsetof(vertex, col));        //byte offset within a vertex to get this data

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,        // vertex color
        2,        // how many things?
        GL_FLOAT,     // what types of things are in that thing?
        GL_FALSE,     // normalize this?
        sizeof(vertex),       // byte offset between vertices
        (void*)offsetof(vertex, uv));        //byte offset within a vertex to get this data

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,        // vertex color
        4,        // how many things?
        GL_FLOAT,     // what types of things are in that thing?
        GL_FALSE,     // normalize this?
        sizeof(vertex),       // byte offset between vertices
        (void*)offsetof(vertex, normal));        //byte offset within a vertex to get this data

    // unbind the buffers (VAO then the buffers)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // return the geometry
    return newGeo;
}

texture loadTexture(const char* filePath)
{
    assert(filePath != nullptr && "File path was invalid.");

    // create some variables to hold onto some data
    int imageWidth, imageHeight, imageFormat;
    unsigned char* rawPixelData = nullptr; // prefer nullptr over NULL - no cost using it.

    // use stb to load up the image (and set some settings, where necessary)
    stbi_set_flip_vertically_on_load(true);
    rawPixelData = stbi_load(
        filePath, // file path to image to load
        &imageWidth, // will populate variable with width in pixels
        &imageHeight, // will populate variable with height in pixels
        &imageFormat, // will populate variable with format
        STBI_default); // import settings stb_image
    assert(rawPixelData != nullptr && "Image failed to load.");
    
    // pass the image data over to makeTexture
    texture newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
    assert(newTexture.handle != 0 && "Failed to create texture.");

    // free the image data (its already in the gpu so we dont need it on the cpu-side anymore)
    stbi_image_free(rawPixelData);

    // return texture
    return newTexture;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
    // firgure out the OGL texture format to use
    assert(channels > 0 && channels < 5); // only support 1 - 4 channels
    GLenum oglFormat = GL_RED;
    switch(channels)
    {
    case 1:
        oglFormat = GL_RED;
        break;
    case 2:
        oglFormat = GL_RG;
        break;
    case 3:
        oglFormat = GL_RGB;
        break;
    case 4:
        oglFormat = GL_RGBA;
        break;
    }

    // generate a texture
    texture retVal = { 0, width, height, channels };
    glGenTextures(1, &retVal.handle);

    // bind and buffer data to it
    glBindTexture(GL_TEXTURE_2D, retVal.handle);

    glTexImage2D(
        GL_TEXTURE_2D,  // texture to buffer data to (was previously bound)
        0, // level
        oglFormat, // format specifier
        width, // width in pixels
        height, // height in pixels
        0, // border value
        oglFormat, // final format specifier
        GL_UNSIGNED_BYTE, // type of data elements
        pixels); // pointer to actual data

    // set some filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // filtering applied when texel density is greater than display (interpolating existing data)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // filtering applied when texel density is less than display (extrapolating data)

    // return the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    return retVal;
}

void freeTexture(texture& tex)
{
    glDeleteTextures(1, &tex.handle);
    tex = {}; // default initialize
}

void freeGeometry(geometry& geo)
{
    // telling the buffers to delete vbo,ibo, and vao
    glDeleteBuffers(1, &geo.vbo);
    glDeleteBuffers(1, &geo.ibo);
    glDeleteBuffers(1, &geo.vao);

    // deleted
    geo = {};
}

shader loadShader(const char* vertPath, const char* fragPath)
{
    // extract the text for vertex shader from the file
    std::fstream vertFile(vertPath,         // path to the file
        std::ios::in);    // the mode of operation (in == reading)

    assert(vertFile.is_open() && "Failed to open vertex shader file.");
    std::string vertSource;
    for (std::string line; std::getline(vertFile, line);)
    {
        vertSource += line + "\n";
    }

    vertFile.close();

    // extract the text for vertex shader from the file
    std::fstream fragFile(fragPath,         // path to the file
        std::ios::in);    // the mode of operation (in == reading)

    assert(fragFile.is_open() && "Failed to open fragment shader file.");
    std::string fragSource;
    for (std::string line; std::getline(fragFile, line);)
    {
        fragSource += line + "\n";
    }

    fragFile.close();

    return makeShader(vertSource.c_str(), fragSource.c_str());

}

shader makeShader(const char* vertSource, const char* fragSource)
{
    // make a shader program
    shader newShad = {};
    newShad.program = glCreateProgram(); // no parameters

    // create the shaders (not the same thing as a shader program)
    GLuint vert = glCreateShader(GL_VERTEX_SHADER); // this shader is a vertex shader
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER); // this shader is a fragment shader

    // compile the shaders
    glShaderSource(vert, 1, &vertSource, 0); // sends source code for specified shader
    glShaderSource(frag, 1, &fragSource, 0); // sends source code for specified shader
    glCompileShader(vert); // actually compile the shader
    glCompileShader(frag);

    // validate the shaders
    bool shaderOK = checkShader(vert, "Vertex");
    assert(shaderOK && "Vertex shader failed to compile.");
    shaderOK = checkShader(frag, "Fragment");
    assert(shaderOK && "Fragment shader failed to compile.");

    // attach the shaders
    glAttachShader(newShad.program, vert); // associate the vertex shader w/ the shader program
    glAttachShader(newShad.program, frag); // associate the fragment shader w/ the shader program

    // link the shader program
    glLinkProgram(newShad.program); // link the shader program

    // delete the shaders
    glDeleteShader(vert); // delete the vertex shader
    glDeleteShader(frag); // delete the fragment shader

    // return the shader
    return newShad;
}

void freeShader(shader& shad)
{
    glDeleteProgram(shad.program);
    shad = {};
}

void draw(const shader& shad, const geometry& geo)
{
    // specofy whoch shader
    glUseProgram(shad.program);
    // specofy which VAO
    glBindVertexArray(geo.vao);
    // draw
    glDrawElements(GL_TRIANGLES, // primitive type
                   geo.size, // indices
                   GL_UNSIGNED_INT, // index type
                   0);
}

void setUniform(const shader& shad, GLuint location, const glm::mat4& value)
{
    // glUniform - would just effect the last bound shader program

    // we're gonna go with maximum flexibility - specify the shader program
    // that we want to modify

    glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, float value)
{
    glProgramUniform1f(shad.program, location, value);
}

void setUniform(const shader& shad, GLuint location, const glm::vec3& value)
{
    glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot)
{
    // set up this texture in a slot
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, tex.handle);

    // pass the slot number over the uniform
    glProgramUniform1i(shad.program, location, textureSlot);
}

bool checkShader(GLuint target, const char* humanReadableName)
{
    assert(glIsShader(target) && "target is not a shader -- cannot check");

    GLint shaderCompileStatus = 0;
    glGetShaderiv(target, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];
        glGetShaderInfoLog(target, 1024, &logLength, message);
        fprintf(stderr, "\n[ERROR] %s Shader \n %s", humanReadableName, message);

        // return an empty shader if it fails
        return false;
    }

    return true;
}
