#include "stdinc.h"
#include "graphics.h"
#include "sprite.h"

sprite::sprite()
{
    texId = vbo_data = vao_data = 0;
    position = glm::vec3(0.0f,0.0f,0.0f);
    scale = glm::vec3(1.0f,1.0f,1.0f);
    rotationX = rotationY = rotationZ = 0.0f;
}

sprite::~sprite()
{
    free();
}

void sprite::free()
{

    if (vbo_data) glDeleteBuffers(1, &vbo_data);
    if (texId) glDeleteTextures(1, &texId);
    if (vao_data) glDeleteVertexArrays(1, &vao_data);
    texId = vbo_data = ibo_data = vao_data = 0;
}

void sprite::create(string filename, unsigned int num_cols, unsigned int num_rows, float width, float height,
                    float animSpeed)
{
    if (vao_data!=0 || ibo_data!=0 || vbo_data!=0) free();


    float swidth = width/2.0f;//(imgWidth*numImgsRow + hDisp*(numImgsRow-1))/2.0f;
    float sheight = height/2.0f;//(imgHeight*numImgsCol + vDisp*(numImgsCol-1))/2.0f;

    float imgHeight = 1.0f/num_rows;
    float imgWidth = 1.0f/num_cols;

    vertex vertices[] =
    {
        {glm::vec3(-swidth, -sheight,  0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec2(0.0f,1-imgHeight)},
        {glm::vec3( swidth, -sheight,  0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec2(imgWidth,1-imgHeight)},
        {glm::vec3( swidth,  sheight,  0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec2(imgWidth,1.0f)},
        {glm::vec3(-swidth,  sheight,  0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec2(0.0f,1.0f)},
    };

    vbo_data = CreateBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertex)*4, GL_STATIC_DRAW);

    texId = LoadTexture(filename.c_str());

    numRows = num_rows;
    numCols = num_cols;
    animationSpeed = animSpeed;



    glGenVertexArrays(1, &vao_data);
    glBindVertexArray(vao_data);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_data);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, tcoords));

    glBindVertexArray(0);

}

void sprite::render(graphics* gp, float speedFactor, float &tm, bool viewTransform)
{
    if (vao_data == 0) return;


    glUseProgram(gp->program);


    glBindVertexArray(vao_data);
    //glEnableVertexAttribArray(0);//gp->attrib_coord3d);
    //glEnableVertexAttribArray(1);//gp->attrib_texcoord);

    glm::mat4 matOffset;

    matOffset = glm::translate(glm::mat4(), position)
                * glm::rotate(glm::mat4(), rotationZ, glm::vec3(0.0f,0.0f,1.0f))
                * glm::rotate(glm::mat4(), rotationY, glm::vec3(0.0f,1.0f,0.0f))
                * glm::rotate(glm::mat4(), rotationX, glm::vec3(1.0f,0.0f,0.0f))
                * glm::scale(glm::mat4(), scale);
    glm::mat4 mvp;
    if (viewTransform)
        mvp = gp->projection * gp->view * matOffset;
    else
        mvp = gp->projection * matOffset;
    glUniformMatrix4fv(gp->uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));

    glm::mat3 mit = glm::transpose(glm::inverse(glm::mat3(matOffset)));
    glUniformMatrix3fv(gp->uniform_mat_mit, 1, GL_FALSE, glm::value_ptr(mit));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(gp->uniform_tex, 0);

    float s,t;
    //s=(imgId%numImgRow)*iwidth;
    //t=(imgId/numImgRow)*iheight;
    unsigned int imgId = floor(tm);
    s= (float)(imgId%numCols) / (float) numCols;
    t= - (float)(imgId/numCols) / (float) numRows;

    tm += animationSpeed * speedFactor;
    //cout << imgId << endl;

    //cout << s << " " << t << endl;
    if (tm >= (numCols)*(numRows)) tm=0;

    glUniform2f(gp->uniform_uvdisp, s, t);
    glUniform1i(gp->uniform_notlit,1);


    /*glBindBuffer(GL_ARRAY_BUFFER, vbo_data);
    glVertexAttribPointer(
            0,//gp->attrib_coord3d,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            (GLvoid*) offsetof(vertex, pos)
    );
    glVertexAttribPointer(
            1,//gp->attrib_texcoord,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            (GLvoid*) offsetof(vertex, tcoords)
    );*/

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data);
    //int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); // Get the size of buffer
    //glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //glDisableVertexAttribArray(0);//gp->attrib_coord3d);
    //glDisableVertexAttribArray(1);//gp->attrib_texcoord);
    glBindVertexArray(0);
}
