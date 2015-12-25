#ifndef UTILS_HEADER
#define UTILS_HEADER

char* file_read(const char* filename);
GLuint CreateShader(const char* filename, GLenum type);
GLuint LinkShaders(const GLuint* shaders, unsigned int numOfShaders);
//GLint BindAttribute(GLuint program, const char* attribute_name);
GLint BindUniform(GLuint program, const char* uniform_name);
GLuint CreateBuffer(GLenum target, const GLvoid* data, GLsizeiptr sizeofdata, GLenum usage);



GLuint LoadTexture(const char* filename);

#endif
