#ifndef GRAPHICS_H
#define GRAPHICS_H


struct vertex
{
    glm::vec3 pos;      // for buffer data : &pos[0]
    glm::vec3 normal;
    glm::vec2 tcoords;
   //glm::vec4 color;
};


class graphics
{
    public:
        graphics();
        virtual ~graphics();

        void finish();
        void resize(UINT screen_width, UINT screen_height);
        void update();
        void endupdate(HWND hwnd);
        void initialize();

        unsigned int newTexture(string filename);


        void setBackColor(float r, float g, float b, float a);
        glm::vec3 cam_eye, cam_target, cam_up;

        friend class mesh;
        friend class simpleMesh;
        friend class sprite;
        friend class terrain;
        friend class skybox;

        glm::vec3 sundir;
    protected:
    private:

        bool initialized;

        void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC);
        void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC);

        glm::mat4 view;
        glm::mat4 projection;

        /*GLint attrib_coord3d;
        GLint attrib_texcoord;*/
        //GLint attrib_color;

        GLint uniform_mat;
        GLint uniform_mat_mit;
        GLint uniform_tex;
        GLint uniform_notlit;
        GLint uniform_uvdisp;  // tex coords to be displaced;
        GLint uniform_sundir;

        /*GLint skin_attrib_coord3d;
        GLint skin_attrib_texcoord;
        GLint skin_attrib_weight;
        GLint skin_attrib_boneid;*/
        //GLint skin_attrib_color;

        GLint skin_uniform_mat;
        GLint skin_uniform_mat_mit;
        GLint skin_uniform_tex;
        GLint skin_uniform_notlit;
        GLint skin_uniform_bonemats;
        GLint skin_uniform_sundir;

        GLint sky_uniform_mat;
        GLint sky_uniform_tex;
        GLint sky_uniform_sundir;

        GLuint program;
        GLuint skinProgram;
        GLuint skyProgram;

        vector<unsigned int> textures;

};

#endif // GRAPHICS_H
