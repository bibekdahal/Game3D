#ifndef MODEL_H
#define MODEL_H

#include "animation.h"

class mesh;
class graphics;
class model
{
    public:
        model();
        virtual ~model();

        void free();

        void load(string filename, string dir="");
        void render(graphics*gp);

        void update(double  &anim_time);

        glm::vec3 position, scale;
        float rotationX, rotationY, rotationZ;
        glm::vec3 offset;

        bool IsLoaded();
        //glm::vec3 rotationAxis; float rotationAngle;
    protected:
    private:
        fstream file;

        bool loaded;
        unsigned int numMeshes;
        mesh * meshes;
        node * root;

        unsigned int numTextures;
        unsigned int * textures;

        unsigned int numAnimations;
        animation * animations;

        void freenodes(node* bn);

        node* getNodeFromId(node* nd, int id);
        void readNodes(node* bn, node* parent);

        void updateNode(node* nd);
        void renderNode(node* nd, glm::mat4 mat, graphics * gp);
};

#endif // MODEL_H
