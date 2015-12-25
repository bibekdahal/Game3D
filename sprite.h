#ifndef SPRITE_H
#define SPRITE_H


class sprite
{
    public:
        sprite();
        virtual ~sprite();

        //void create(string filename, unsigned int numImgsRow, unsigned int numImgsCol, float imgWidth, float imgHeight,
        //            float animSpeed=1.0f, float hDisp = 0, float vDisp = 0);
        void create(string filename, unsigned int num_cols, unsigned int num_rows, float width, float height,
                    float animSpeed=1.0f);
        void render(graphics* gp, float speedFactor, float &tm, bool viewTransform=true);
        void free();

        glm::vec3 position, scale;
        float rotationX, rotationY, rotationZ;

        //glm::vec3 offset;
    protected:
    private:

        GLuint texId;
        GLuint vbo_data;
        GLuint ibo_data;
        GLuint vao_data;

        //unsigned int numImgRow, numImgCol;
        //float iwidth, iheight, hdisp, vdisp;
        unsigned int numRows, numCols;
        float animationSpeed;
};

#endif // SPRITE_H
