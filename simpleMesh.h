#ifndef SIMPLEMESH_H
#define SIMPLEMESH_H

class graphics;
class simpleMesh
{
    public:
        simpleMesh();
        virtual ~simpleMesh();

        void newMesh(vertex* vertices, unsigned int numVerts, unsigned int * indices, unsigned int numIndices);
        void render(graphics* gp, glm::mat4 matModel, int NotLit=0);
        void render(graphics* gp, int NotLit=0);
        void free();

        void setTextureId(unsigned int texture);

        glm::vec3 position, scale;
        float rotationX, rotationY, rotationZ;
        glm::vec3 offset;

        void load(string filename, graphics * gp, string dir="", unsigned int meshId=0);

    protected:
    private:
        GLuint texId;

        GLuint vbo_data;
        GLuint ibo_data;
        GLuint vao_data;
};

#endif // SIMPLEMESH_H
