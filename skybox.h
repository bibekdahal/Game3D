#ifndef SKYBOX_H
#define SKYBOX_H


class graphics;
class skybox
{
    public:
        skybox();

        void newSkyBox();
        void render(graphics * gp, glm::mat4 trans);
        void free();
        virtual ~skybox();
    protected:
    private:
        GLuint texId;

        GLuint vbo_data;
        GLuint ibo_data;
        GLuint vao_data;
};

#endif // SKYBOX_H
