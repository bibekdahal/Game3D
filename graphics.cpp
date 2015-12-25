#include "stdinc.h"
#include "graphics.h"

graphics::graphics()
{
    initialized = false;
}

graphics::~graphics()
{
    //dtor
}

void graphics::finish()
{
    if (!initialized) return;

    initialized = false;

    glDeleteTextures(textures.size(), &textures[0]);
    glDeleteProgram(program);
    glDeleteProgram(skinProgram);
}

void graphics::setBackColor(float r, float g, float b, float a)
{
    glClearColor(r,g,b,a);
}

void graphics::initialize()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    cam_eye = glm::vec3(0.0, 2.0, 20.0);
    cam_target = glm::vec3(0.0, 0.0, 0.0);
    cam_up = glm::vec3(0.0, 1.0, 0.0);

    GLuint shaders[6];
    shaders[0] = CreateShader("tri_vs.glsl", GL_VERTEX_SHADER);
    shaders[1] = CreateShader("tri_fs.glsl", GL_FRAGMENT_SHADER);
    shaders[2] = CreateShader("skin_vs.glsl", GL_VERTEX_SHADER);
    shaders[3] = shaders[1];//CreateShader("skin_fs.glsl", GL_FRAGMENT_SHADER);
    shaders[4] = CreateShader("sky_vs.glsl", GL_VERTEX_SHADER);
    shaders[5] = CreateShader("sky_fs.glsl", GL_FRAGMENT_SHADER);

    program = LinkShaders(shaders,2);
    skinProgram = LinkShaders(&shaders[2], 2);
    skyProgram = LinkShaders(&shaders[4], 2);

    /*attrib_coord3d = BindAttribute(program, "coord3d");
    attrib_texcoord = BindAttribute(program, "texcoord");*/
    uniform_mat = BindUniform(program, "m_transform");
    uniform_mat_mit = BindUniform(program, "m_inv_trans");
    uniform_tex = BindUniform(program, "texture_sample");
    uniform_uvdisp = BindUniform(program, "uv_disp");
    uniform_notlit = BindUniform(program, "notlit");
    uniform_sundir = BindUniform(program, "sundir");

    /*skin_attrib_coord3d = BindAttribute(skinProgram, "coord3d");
    skin_attrib_texcoord = BindAttribute(skinProgram, "texcoord");
    skin_attrib_weight = BindAttribute(skinProgram, "weights");
    skin_attrib_boneid = BindAttribute(skinProgram, "boneids");*/
    skin_uniform_mat = BindUniform(skinProgram, "m_transform");
    skin_uniform_mat_mit = BindUniform(skinProgram, "m_inv_trans");
    skin_uniform_bonemats = BindUniform(skinProgram, "m_bones");
    skin_uniform_tex = BindUniform(skinProgram, "texture_sample");
    skin_uniform_notlit = BindUniform(program, "notlit");
    skin_uniform_sundir = BindUniform(program, "sundir");

    sky_uniform_mat = BindUniform(skyProgram, "m_transform");
    sky_uniform_tex = BindUniform(skyProgram, "texture_sample");
    sky_uniform_sundir = BindUniform(skyProgram, "sundir");

    initialized = true;


    sundir = glm::vec3(-1.0f,0.0f,0.0f);
}

void graphics::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view = glm::lookAt(cam_eye, cam_target, cam_up);

}

void graphics::endupdate(HWND hwnd)
{
    SwapBuffers(GetDC(hwnd));
}

unsigned int graphics::newTexture(string filename)
{
    unsigned int tex = LoadTexture(filename.c_str());
    textures.push_back(tex);
    return tex;
}


void graphics::resize(UINT screen_width, UINT screen_height)
{
    //view = glm::lookAt(glm::vec3(0.0, 30.0, 100.0), glm::vec3(0.0, 30.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //view = glm::lookAt(glm::vec3(0.0, 300.0, 700.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //view = glm::lookAt(glm::vec3(0.0, 2.0, 20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //view = glm::lookAt()
    projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 20000.0f);
    glViewport(0,0,screen_width,screen_height);
}

