#ifndef TERRAIN_H
#define TERRAIN_H


class terrain
{
    public:
        terrain();
        virtual ~terrain();

        void create(string filename, int width, int height, float xscale, float yscale, float hscale);
        void render(graphics* gp);
        void free();
        void setTextureId(unsigned int texture);

        float getHeight(float x, float y);
    protected:
    private:
        GLuint vbo_data;
        GLuint ibo_data;
        GLuint vao_data;
        GLuint texId;

        float * heightData;
        int hWidth, hHeight;
        float xScale, yScale;
};

#endif // TERRAIN_H
