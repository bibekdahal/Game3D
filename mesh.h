#ifndef MESH_H
#define MESH_H

#include "animation.h"
class model;
class graphics;
class mesh
{
    public:
        mesh();
        virtual ~mesh();

        void newMesh(vertex* vertices, unsigned int numVerts, unsigned int * indices, unsigned int numIndices);
        void ready();
        void render(graphics* gp, glm::mat4 matModel);
        void free();

        void makeSkinned(unsigned int numVerts, weightInfo * weights);
        void setTextureId(unsigned int texture);

        friend model;
    protected:
    private:
        unsigned int numBones;
        bone* bones;

        GLuint texId;

        GLuint vbo_data;
        GLuint ibo_data;
        GLuint wtbo_data;
        GLuint vao_data;

        bool skinned;
};

#endif // MESH_H
