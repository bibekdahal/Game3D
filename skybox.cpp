#include "stdinc.h"
#include "graphics.h"
#include "skybox.h"
#include "soil.h"

skybox::skybox()
{
    texId = vbo_data = ibo_data = vao_data = 0;
}

skybox::~skybox()
{
    free();
}

void skybox::newSkyBox()
{
    if (vao_data!=0 || ibo_data!=0 || vbo_data!=0) free();

    GLfloat vertices[] = {
      -1.0,  1.0,  1.0,
      -1.0, -1.0,  1.0,
       1.0, -1.0,  1.0,
       1.0,  1.0,  1.0,
      -1.0,  1.0, -1.0,
      -1.0, -1.0, -1.0,
       1.0, -1.0, -1.0,
       1.0,  1.0, -1.0,
    };
    GLushort indices[] = {
      0, 1, 2, 3,
      3, 2, 6, 7,
      7, 6, 5, 4,
      4, 5, 1, 0,
      0, 3, 7, 4,
      1, 2, 6, 5,
    };

    texId = SOIL_load_OGL_cubemap("assets\\skybox_zn.jpg", "assets\\skybox_zp.jpg",
                                  "assets\\skybox_yp.jpg", "assets\\skybox_yn.jpg",
                                  "assets\\skybox_xn.jpg", "assets\\skybox_xp.jpg",
                                  SOIL_LOAD_AUTO,
                                  SOIL_CREATE_NEW_ID,
                                  SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
                                    );
    vbo_data = CreateBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertices), GL_STATIC_DRAW);
    ibo_data = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, indices,sizeof(indices),GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_data);
    glBindVertexArray(vao_data);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_data);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}
void skybox::free()
{
    if (vbo_data) glDeleteBuffers(1, &vbo_data);
    if (ibo_data) glDeleteBuffers(1, &ibo_data);
    if (vao_data) glDeleteVertexArrays(1, &vao_data);
    if (texId) glDeleteTextures(1, &texId);
    texId = vbo_data = ibo_data = vao_data = 0;
}

void skybox::render(graphics * gp, glm::mat4 trans)
{
    if (!gp) return;
    if (!gp->initialized) return;
    if (vao_data == 0 || ibo_data == 0) return;

    glDisable(GL_CULL_FACE);
    glUseProgram(gp->skyProgram);

    glBindVertexArray(vao_data);

    glm::mat4 mvp = gp->projection * gp->view * trans;
    glUniformMatrix4fv(gp->sky_uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(gp->sky_uniform_tex, 0);

    glUniform3f(gp->sky_uniform_sundir, gp->sundir.x, gp->sundir.y, gp->sundir.z);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); // Get the size of buffer
    glDrawElements(GL_QUADS, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
}
