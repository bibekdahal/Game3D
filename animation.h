#ifndef ANIMATION_H
#define ANIMATION_H

struct node
{
    int id;
    unsigned int numChildren;
    node* children;
    node* parent;

    glm::mat4 transform;
    glm::mat4 combinedTransform;

    unsigned int numMeshes;
    unsigned int * meshes;
};

struct vecKey
{
    double time;
    glm::vec3 vec;
};
struct rotKey
{
    double time;
    glm::quat rot;
};
struct nodeAnim
{
    node* Node;
    unsigned int numPosKeys, numRotKeys, numScaleKeys;

    vecKey * posKeys, * scaleKeys;
    rotKey * rotKeys;
};
struct animation
{
    double duration;
    unsigned int numNodeAnims;
    nodeAnim * nodeAnims;
};

struct bone
{
    node* Node;
    glm::mat4 offset;
};
struct weightInfo
{
    //bone* bones[4];
    unsigned int boneIds[4];
    float weights[4];
};

#endif

