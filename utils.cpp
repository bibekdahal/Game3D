#include "stdinc.h"

#include "soil.h"

#define Error(x) std::cout << x << std::endl;//MessageBoxA(0,x,"ERROR!",0);


#define RET_FILE_NULL {fclose(input);return NULL;}
char* file_read(const char* filename)
{
    FILE* input = fopen(filename, "rb");
    if(input == NULL) return NULL;

    if(fseek(input, 0, SEEK_END) == -1) RET_FILE_NULL;
    long size = ftell(input);
    if(size == -1) RET_FILE_NULL;
    if(fseek(input, 0, SEEK_SET) == -1) RET_FILE_NULL;

    /*if using c-compiler: dont cast malloc's return value*/
    char *content = (char*) malloc( (size_t) size +1  );
    if(content == NULL) RET_FILE_NULL;

    fread(content, 1, (size_t)size, input);
    if(ferror(input)) {
        free(content);
        RET_FILE_NULL;
    }

    fclose(input);
    content[size] = '\0';
    return content;
}


GLuint CreateShader(const char* filename, GLenum type)
{
    const GLchar* source = file_read(filename);
    if (source == NULL) {
        Error("Error opening shader");
        return 0;
    }
    GLuint res = glCreateShader(type);

    glShaderSource(res, 1, &source, NULL);
    free((void*)source);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(res, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetShaderInfoLog(res, log_length, NULL, log);
        std::cout << log << std::endl;
        free(log);

        Error("Error compiling shader");
        glDeleteShader(res);
        return 0;
    }

    return res;
}

GLuint LinkShaders(const GLuint* shaders, unsigned int numOfShaders)
{
    GLint link_ok;
    GLuint program = glCreateProgram();
    for (unsigned int i=0;i<numOfShaders;i++)
        glAttachShader(program, shaders[i]);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        Error("Error linking shaders");
        return 0;
    }
    return program;
}
/*
GLint BindAttribute(GLuint program, const char* attribute_name)
{
    GLint attribute = glGetAttribLocation(program, attribute_name);
    if (attribute == -1)
    {
        std::string str(attribute_name);
        Error("Error binding attribute: " + str);
        return 0;
    }
    return attribute;
}*/

GLint BindUniform(GLuint program, const char* uniform_name)
{
    GLint uniform = glGetUniformLocation(program, uniform_name);
    if (uniform == -1)
    {
        std::string str(uniform_name);
        Error("Error binding uniform: " + str);
        return 0;
    }
    return uniform;
}


GLuint CreateBuffer(GLenum target, const GLvoid* data, GLsizeiptr sizeofdata, GLenum usage)
{
    GLuint obj;
    glGenBuffers(1, &obj);
    glBindBuffer(target, obj);
    glBufferData(target, sizeofdata, data, usage);

    glBindBuffer(target, 0);

    return obj;
}




GLuint LoadTexture(const char* filename)
{
    GLuint texture_id = SOIL_load_OGL_texture
    (
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
    );
    if(texture_id == 0)
    {
        Error("Error Loading Texture");
        cout << ": " << SOIL_last_result();
    }
    return texture_id;
}
