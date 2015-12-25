#include "stdinc.h"
#include "graphics.h"
#include "simpleMesh.h"

simpleMesh::simpleMesh()
{
    texId = vbo_data = ibo_data = vao_data = 0;
    offset = position = glm::vec3(0.0f,0.0f,0.0f);
    scale = glm::vec3(1.0f,1.0f,1.0f);
    rotationX = rotationY = rotationZ = 0.0f;
}

simpleMesh::~simpleMesh()
{
    free();
}

void simpleMesh::newMesh(vertex* vertices, unsigned int numVerts, unsigned int * indices, unsigned int numIndices)
{
    if (vao_data!=0 || ibo_data!=0 || vbo_data!=0) free();
    vbo_data = CreateBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertex)*numVerts, GL_STATIC_DRAW);
    ibo_data = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, indices,sizeof(unsigned int)*numIndices,GL_STATIC_DRAW);

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

void simpleMesh::setTextureId(unsigned int texture)
{
    texId = texture;
}

void simpleMesh::free()
{
    if (vbo_data) glDeleteBuffers(1, &vbo_data);
    if (ibo_data) glDeleteBuffers(1, &ibo_data);
    if (vao_data) glDeleteVertexArrays(1, &vao_data);
    texId = vbo_data = ibo_data = vao_data = 0;

}

void simpleMesh::render(graphics* gp, int NotLit)
{
    render(gp, glm::mat4(), NotLit);
}
void simpleMesh::render(graphics* gp, glm::mat4 matModel, int NotLit)
{
    if (!gp) return;
    if (!gp->initialized) return;
    if (vao_data == 0 || ibo_data == 0) return;


    glUseProgram(gp->program);

    glBindVertexArray(vao_data);

    glm::mat4 matOffset;

    matOffset = glm::translate(glm::mat4(), position)
                * glm::rotate(glm::mat4(), rotationZ, glm::vec3(0.0f,0.0f,1.0f))
                * glm::rotate(glm::mat4(), rotationY, glm::vec3(0.0f,1.0f,0.0f))
                * glm::rotate(glm::mat4(), rotationX, glm::vec3(1.0f,0.0f,0.0f))
                * glm::scale(glm::mat4(), scale)
                * glm::translate(glm::mat4(), -offset);
    glm::mat4 mvp = gp->projection * gp->view * matModel * matOffset;
    glUniformMatrix4fv(gp->uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));

    glm::mat3 mit = glm::transpose(glm::inverse(glm::mat3(matModel * matOffset)));
    glUniformMatrix3fv(gp->uniform_mat_mit, 1, GL_FALSE, glm::value_ptr(mit));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(gp->uniform_tex, 0); //XX where GL_TEXTUREXX in glActiveTexture

    glUniform2f(gp->uniform_uvdisp, 0.0f, 0.0f);
    glUniform1i(gp->uniform_notlit,NotLit);

    glUniform3f(gp->uniform_sundir, gp->sundir.x, gp->sundir.y, gp->sundir.z);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); // Get the size of buffer
    glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}


void simpleMesh::load(string src, graphics * gp, string dir, unsigned int meshId)
{
    fstream file;
    file.open(src.c_str(), fstream::in | fstream::binary);
    if (file.fail())
    {
        cout << "FAILED TO OPEN FILE";
        return;
    }
    unsigned i,j;


    unsigned int numTextures, numMeshes;
    file.read((char*)&numTextures, sizeof(numTextures));

    vector<string> textures;
    string str;
    if (numTextures>0)
    {
        for (i=0;i<numTextures;i++)
        {
            file >> str;
            if (str=="$NULL$") continue;
            if (dir!="")str = dir + "\\" + str;
            //textures[i] = LoadTexture(str.c_str());
            //cout << str;
            textures.push_back(str);
        }
        file.ignore();
    }
    file.read((char*)&numMeshes, sizeof(numMeshes));

    if (numMeshes>0)
    {
        unsigned int numVertices, numIndices;
        if (meshId < numMeshes)
        for (j=0;j<meshId;j++)
        {
            unsigned int tmp;
            file.read((char*)&tmp, sizeof(tmp));

            file.read((char*)&numVertices, sizeof(numVertices));
            glm::vec3 * poses = new glm::vec3[numVertices];
            glm::vec3 * normals = new glm::vec3[numVertices];
            glm::vec2 * tcoords = new glm::vec2[numVertices];
            vertex * vertices = new vertex[numVertices];


            file.read((char*)poses, sizeof(glm::vec3)*numVertices);
            file.read((char*)normals, sizeof(glm::vec3)*numVertices);
            file.read((char*)tcoords, sizeof(glm::vec2)*numVertices);

            file.read((char*)&numIndices, sizeof(numIndices));
            unsigned int * indices = new unsigned int[numIndices];
            file.read((char*)indices, sizeof(unsigned int)*numIndices);


            delete[] vertices;
            delete[] poses;
            delete[] normals;
            delete[] tcoords;
            delete[] indices;
        }

        file.read((char*)&texId, sizeof(texId));

        if (numTextures>0 && texId<numTextures)
            texId = gp->newTexture(textures[texId]);

        file.read((char*)&numVertices, sizeof(numVertices));
        glm::vec3 * poses = new glm::vec3[numVertices];
        glm::vec3 * normals = new glm::vec3[numVertices];
        glm::vec2 * tcoords = new glm::vec2[numVertices];
        vertex * vertices = new vertex[numVertices];


        file.read((char*)poses, sizeof(glm::vec3)*numVertices);
        file.read((char*)normals, sizeof(glm::vec3)*numVertices);
        file.read((char*)tcoords, sizeof(glm::vec2)*numVertices);


        for (j=0;j<numVertices;j++)
        {
            vertices[j].pos = poses[j];
            vertices[j].normal = normals[j];
            vertices[j].tcoords = tcoords[j];
        }

        file.read((char*)&numIndices, sizeof(numIndices));
        unsigned int * indices = new unsigned int[numIndices];
        file.read((char*)indices, sizeof(unsigned int)*numIndices);

        newMesh(vertices, numVertices, indices, numIndices);
        delete[] vertices;
        delete[] poses;
        delete[] normals;
        delete[] tcoords;
        delete[] indices;
    }

    file.close();
}
