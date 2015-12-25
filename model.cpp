#include "stdinc.h"
#include "graphics.h"
#include "mesh.h"
#include "model.h"

model::model()
{
    numMeshes = numAnimations = numTextures = 0;
    root = NULL;

    offset = position = glm::vec3(0.0f,0.0f,0.0f);
    scale = glm::vec3(1.0f,1.0f,1.0f);
    /*rotationAxis = glm::vec3(0.0f,1.0f,0.0f);
    rotationAngle = 0.0f;*/
    rotationX = rotationY = rotationZ = 0.0f;
    loaded = false;
}

model::~model()
{
    free();
}

void model::renderNode(node* nd, glm::mat4 mat, graphics * gp)
{
    for (unsigned i =0 ;i<nd->numMeshes;i++)
        meshes[nd->meshes[i]].render(gp, mat * nd->combinedTransform);
    for (unsigned i =0 ;i<nd->numChildren;i++)
        renderNode(&nd->children[i], mat, gp);
}
void model::render(graphics * gp)
{
    if (!root) return;
    glm::mat4 matModel;
    matModel =  glm::translate(glm::mat4(), position)
                * glm::rotate(glm::mat4(), rotationZ, glm::vec3(0.0f,0.0f,1.0f))
                * glm::rotate(glm::mat4(), rotationY, glm::vec3(0.0f,1.0f,0.0f))
                * glm::rotate(glm::mat4(), rotationX, glm::vec3(1.0f,0.0f,0.0f))
                * glm::scale(glm::mat4(), scale)
                * glm::translate(glm::mat4(), -offset);

    //for (unsigned int i =0;i<numMeshes;i++)
        //meshes[i].render(gp, matModel);
    renderNode(root, matModel, gp);
}

void model::updateNode(node* nd)
{
    if (nd->parent) nd->combinedTransform = nd->parent->combinedTransform * nd->transform;
    else nd->combinedTransform = nd->transform;
    unsigned int i;
    for (i=0;i<nd->numChildren;i++)
        updateNode(&nd->children[i]);
}

void model::update(double &anim_time)
{
    unsigned int i,j;
    if (numAnimations<=0) return;

    double tm = anim_time;//*400;
    if (tm>=animations[0].duration) anim_time=tm=0;


    animation* anim = &animations[0];
    for (i=0;i<anim->numNodeAnims;i++)
    {
        nodeAnim * nam = &anim->nodeAnims[i];
        unsigned int pk=0,rk=0,sk=0;

        if (nam->numPosKeys>1)
            for (j=0;j<nam->numPosKeys-1;j++)
                if (nam->posKeys[j+1].time>tm)
                {
                    pk=j;
                    break;
                }

        if (nam->numRotKeys>1)
            for (j=0;j<nam->numRotKeys-1;j++)
                if (nam->rotKeys[j+1].time>tm)
                {
                    rk=j;
                    break;
                }

        if (nam->numScaleKeys>1)
            for (j=0;j<nam->numScaleKeys-1;j++)
                if (nam->scaleKeys[j+1].time>tm)
                {
                    sk=j;
                    break;
                }


        unsigned int npk = (pk==nam->numPosKeys-1) ? 0: pk+1;
        unsigned int nrk = (rk==nam->numRotKeys-1) ? 0: rk+1;
        unsigned int nsk = (sk==nam->numScaleKeys-1) ? 0: sk+1;

        float dft = (nam->posKeys[npk].time - nam->posKeys[pk].time);
        float drt = (nam->rotKeys[nrk].time - nam->rotKeys[rk].time);
        float dst = (nam->scaleKeys[nsk].time - nam->scaleKeys[sk].time);
        float pf =  0.0f, rf=0.0f, sf=0.0f;

        if (dft!=0) pf = (tm-nam->posKeys[pk].time)/dft;
        if (drt!=0) rf = (tm-nam->rotKeys[rk].time)/drt;
        if (dst!=0) sf = (tm-nam->scaleKeys[sk].time)/dst;


        if (pf<0) pf*=-1;
        if (sf<0) sf*=-1;
        if (rf<0) rf*=-1;
        glm::vec3 pos = nam->posKeys[pk].vec + pf*(nam->posKeys[npk].vec-nam->posKeys[pk].vec);
        glm::vec3 scale = nam->scaleKeys[sk].vec + pf*(nam->scaleKeys[nsk].vec-nam->scaleKeys[sk].vec);
        glm::quat rot = glm::slerp(nam->rotKeys[rk].rot, nam->rotKeys[nrk].rot, rf);
        nam->Node->transform =
                          glm::translate(glm::mat4(), pos)
                        * glm::mat4_cast(rot)
                        * glm::scale(glm::mat4(), scale);


    }

    updateNode(root);
    //updateData();
}

void model::freenodes(node* bn)
{
    if (!bn) return;
    for (unsigned i=0;i<bn->numChildren;i++)
        freenodes(&bn->children[i]);
    if (bn->children) delete[] bn->children;

    if (bn->numMeshes>0)delete[] bn->meshes;
}

void model::free()
{
    unsigned i,j;

    loaded = false;

    for (i=0;i<numMeshes;i++)
        meshes[i].free();
    for (i=0;i<numAnimations;i++)
    {
        animation * anim = &animations[i];
        for (j=0;j<anim->numNodeAnims;j++)
        {
            nodeAnim * na = &anim->nodeAnims[j];
            delete[] na->posKeys;
            delete[] na->rotKeys;
            delete[] na->scaleKeys;
        }
        if (anim->numNodeAnims>0) delete[] anim->nodeAnims;
    }

    freenodes(root);
    if (root) delete root;
    if (numMeshes>0) delete[] meshes;
    if (numAnimations>0) delete[] animations;
    if (numTextures>0)
    {
        glDeleteTextures(numTextures, textures);
        delete[] textures;
    }

    numMeshes = numAnimations = numTextures = 0;
    root = NULL;
}



node* model::getNodeFromId(node* nd, int id)
{
    if (!nd) return 0;
    if (nd->id==id) return nd;
    for(unsigned i=0;i<nd->numChildren;i++)
    {
        node* ndd = getNodeFromId(&nd->children[i],id);
        if (ndd) return ndd;
    }
    return 0;
}
void model::readNodes(node* bn, node* parent)
{
    unsigned int info[3];
	file.read((char*)info, sizeof(unsigned int)*3);
    bn->id = info[0];
    bn->numChildren = info[1];
    bn->numMeshes = info[2];
    if (info[2] > 0) {
            bn->meshes = new unsigned int [info[2]];
            file.read((char*)bn->meshes, sizeof(unsigned int)*info[2]);
    }

    bn->parent = parent;

    if (bn->numChildren>0) bn->children = new node[bn->numChildren];//(bone**)malloc(sizeof(bone*)*bn->numChildren);
    else bn->children=0;
    for (unsigned i=0;i<bn->numChildren;i++)
        readNodes(&bn->children[i], bn);


}
void model::load(string src, string dir)
{
    free();
    file.open(src.c_str(), fstream::in | fstream::binary);
    if (file.fail())
    {
        cout << "FAILED TO OPEN FILE";
        return;
    }
    unsigned i,j,k;

    root = new node;
    readNodes(root, NULL);

    file.read((char*)&numTextures, sizeof(numTextures));
    if (numTextures>0) textures = new unsigned int[numTextures];
    string str;
    if (numTextures>0)
    {
        for (i=0;i<numTextures;i++)
        {
            file >> str;
            if (str!="$NULL$")
            {
                if (dir!="")str = dir + "\\" + str;
                textures[i] = LoadTexture(str.c_str());
            }
        }
        file.ignore();
    }
    file.read((char*)&numMeshes, sizeof(numMeshes));
    if (numMeshes>0) meshes = new mesh[numMeshes];

    for (i=0;i<numMeshes;i++)
    {
        mesh * msh = &meshes[i];

        unsigned int numVertices, numIndices;

        /*unsigned int mnd;
        file.read((char*)&mnd, sizeof(mnd));
        if (mnd==1)
        {
            file.read((char*)&mnd, sizeof(mnd));
            msh->Node = getNodeFromId(root,mnd);
        }*/

        file.read((char*)&msh->texId, sizeof(msh->texId));
        msh->texId = textures[msh->texId];

        file.read((char*)&numVertices, sizeof(numVertices));
        glm::vec3 * poses = new glm::vec3[numVertices];
        glm::vec3 * normals = new glm::vec3[numVertices];
        glm::vec2 * tcoords = new glm::vec2[numVertices];
        weightInfo * weights = new weightInfo[numVertices];
        vertex * vertices = new vertex[numVertices];


        file.read((char*)poses, sizeof(glm::vec3)*numVertices);
        file.read((char*)normals, sizeof(glm::vec3)*numVertices);
        file.read((char*)tcoords, sizeof(glm::vec2)*numVertices);


        for (j=0;j<numVertices;j++)
        {
            vertices[j].pos = poses[j];
            vertices[j].normal = normals[j];
            vertices[j].tcoords = tcoords[j];
            //vertices[j].color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
        }

        file.read((char*)&numIndices, sizeof(numIndices));
        unsigned int * indices = new unsigned int[numIndices];
        file.read((char*)indices, sizeof(unsigned int)*numIndices);

        msh->newMesh(vertices, numVertices, indices, numIndices);
        delete[] vertices;
        delete[] poses;
        delete[] normals;
        delete[] tcoords;
        delete[] indices;

        file.read((char*)&msh->numBones, sizeof(msh->numBones));
        if (msh->numBones>0) msh->bones = new bone[msh->numBones];

        int* vert_wtN = new int[numVertices];
        for (j=0;j<numVertices;j++)
            vert_wtN[j]=0;

        for (j=0;j<msh->numBones;j++)
        {
            bone* bn = &msh->bones[j];
            unsigned int itmp;
            file.read((char*)&itmp, sizeof(itmp));
            bn->Node = getNodeFromId(root, itmp);

            struct trans_info
            {
                glm::vec3 pos;
                glm::quat rot;
                glm::vec3 scale;
            } trans;
            file.read((char*)&trans, sizeof(float)*(3+4+3));
            bn->offset = glm::translate(glm::mat4(), trans.pos) * glm::mat4_cast(trans.rot)
                        * glm::scale(glm::mat4(), trans.scale);
            file.read((char*)&itmp, sizeof(itmp));
            struct wtinfo_s
            {
                unsigned int id;
                float weight;
            };
            wtinfo_s * wtinfo = new wtinfo_s[itmp];
            file.read((char*)wtinfo, sizeof(wtinfo_s)*itmp);
            for (k=0;k<itmp;k++)
            {
                if (vert_wtN[wtinfo[k].id]>=4) continue;
                weights[wtinfo[k].id].weights[vert_wtN[wtinfo[k].id]] = wtinfo[k].weight;
                weights[wtinfo[k].id].boneIds[vert_wtN[wtinfo[k].id]] = j;

                vert_wtN[wtinfo[k].id]++;
            }
            delete[] wtinfo;

        }

        for (j=0;j<numVertices;j++)
            if (vert_wtN[j]<4)
                for (k=vert_wtN[j];k<4;k++)
                {
                    weights[j].weights[k]=0.0f;
                    weights[j].boneIds[k]=0;
                }
        delete[] vert_wtN;

        if (msh->numBones >0) msh->makeSkinned(numVertices,weights);
        delete[] weights;

        msh->ready();
    }

    file.read((char*)&numAnimations,sizeof(numAnimations));
    if (numAnimations>0) animations = new animation[numAnimations];
    for (i=0;i<numAnimations;i++)
    {
        animation * anim = &animations[i];
        file.read((char*)&anim->duration,sizeof(anim->duration));
        file.read((char*)&anim->numNodeAnims,sizeof(anim->numNodeAnims));
        if (anim->numNodeAnims>0) anim->nodeAnims = new nodeAnim[anim->numNodeAnims];

        for (j=0;j<anim->numNodeAnims;j++)
        {
            nodeAnim * na = &anim->nodeAnims[j];
            unsigned int tmp;
            file.read((char*)&tmp, sizeof(tmp));
            na->Node = getNodeFromId(root, tmp);

            unsigned int knums[3];
            file.read((char*)knums, sizeof(unsigned int)*3);
            na->numPosKeys = knums[0];
            na->numRotKeys = knums[1];
            na->numScaleKeys = knums[2];

            na->posKeys = new vecKey[na->numPosKeys];
            na->rotKeys = new rotKey[na->numRotKeys];
            na->scaleKeys = new vecKey[na->numScaleKeys];

            file.read((char*)na->posKeys, sizeof(vecKey)*na->numPosKeys);
            file.read((char*)na->rotKeys, sizeof(rotKey)*na->numRotKeys);
            file.read((char*)na->scaleKeys, sizeof(vecKey)*na->numScaleKeys);

        }
    }
    file.close();
    loaded = true;
}

bool model::IsLoaded()
{
    return loaded;
}
