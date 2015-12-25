#include "stdinc.h"

#include "graphics.h"
#include "sprite.h"
#include "animation.h"
#include "simpleMesh.h"
#include "mesh.h"
#include "model.h"
#include "terrain.h"
#include "skybox.h"
#include "win.h"

#define BOT_RADIUS 18.75*2.0


skybox sky;
///////////////////// bot collision info
// radius = 18.75
// center = 13.0
////////////////////

bool GunShot=false;
double guntm =0;
//double tm=0.0;
double btm=0.0;

int FPS=200;
float speedFactor=1.0f;

bool started=false;

model gun;
//model astroBoy;
model mineBot;
//simpleMesh msh;
DWORD startTime;
int frames=0;

simpleMesh flash;

struct bots_s
{
    glm::vec3 pos;
    float yangle,xangle,zangle;
    bool dead;
    int shots;

    double anim_tm;
    float expl_tm;
};

bots_s bots[5];


sprite spr;
sprite crosshair;

float camYAngle = 0;
float camXAngle = 0;

terrain terr;

void printVec(glm::vec3 vec)
{
    cout << endl << vec.x << " " << vec.y << " " << vec.z;
}
void UpdateCam()
{

    float ht = terr.getHeight(gp.cam_eye.x, gp.cam_eye.z);
    if (ht>=0/* && fabs(ht+50 - gp.cam_eye.y) > 10*/) gp.cam_eye.y = ht+50;


    if (camXAngle <= -0.35) camXAngle = -0.35;
    if (camXAngle >= 0.78) camXAngle = 0.78;
    gp.cam_target.x = gp.cam_eye.x + 20*sinf(camYAngle)*cos(camXAngle) ;
    gp.cam_target.y = gp.cam_eye.y - 20*sinf(camXAngle);// - 2.0f;
    gp.cam_target.z = gp.cam_eye.z - 20*cosf(camYAngle)*cos(camXAngle);

    glm::vec3 right = glm::normalize(glm::cross(gp.cam_target-gp.cam_eye, glm::vec3(0.0f,1.0f,0.0f)));
    gp.cam_up = glm::normalize(glm::cross(right,gp.cam_target-gp.cam_eye));
    //printVec(gp.cam_target);
    //cout << glm::length(gp.cam_target-gp.cam_eye) << endl;



    gun.rotationY = 180 -glm::degrees(camYAngle);
    gun.rotationX = glm::degrees(camXAngle) - 100;

    flash.rotationY = 180-glm::degrees(camYAngle);
    flash.rotationX = glm::degrees(camXAngle)-45;


    /*gun.position.x = gp.cam_eye.x;
    gun.position.y = gp.cam_eye.y;
    gun.position.z = gp.cam_eye.z;*/

    glm::vec3 tocam = gp.cam_eye-gp.cam_target;

    gun.position = gp.cam_eye;
    flash.position = gp.cam_target + 2.5f*glm::normalize(tocam);
    flash.position.x = flash.position.x-2.5*sinf(camXAngle)*sinf(camYAngle);
    flash.position.y = flash.position.y-2.5*cosf(camXAngle);
    flash.position.z = flash.position.z+2.5*sinf(camXAngle)*cosf(camYAngle);

    crosshair.position = glm::vec3(0.0f,0.0f,-20.0f);//gp.cam_target;
    //crosshair.rotationX = glm::degrees(atanf(-tocam.y/tocam.z));
    //crosshair.rotationY = glm::degrees(atan2f(tocam.x,tocam.z));


    //printVec(tocam);
    //cout << crosshair.rotationY << endl;

}
//simpleMesh box;
GLuint box_tex;

void Initialize()
{
    flash.load("Assets\\muzzleflash.m3d", &gp, "Assets\\");
    sky.newSkyBox();
    //camYAngle = -glm::radians( 90.0f);
    terr.create("Assets\\heightmap.raw", 512,512,20.0f,20.0f, 0.20f);
    gp.cam_eye = glm::vec3(5000.0f,0.0f,-5000.0f);

    spr.create("Assets\\explosion.png",4, 5, 1.0/4,1.0/5);
    spr.scale = glm::vec3(700.0f,700.0f,700.0f);
    spr.position.y = -10;

    crosshair.create("Assets\\cross.png", 1,1,1.0f,1.0f);
   // astroBoy.load("Assets\\astroboy.m3d", "Assets\\");
   // astroBoy.position.z = -20;
   // astroBoy.offset.y = -10;

    mineBot.load("Assets\\mine_bot.m3d", "Assets\\");
    mineBot.scale = glm::vec3(0.25f,0.25f,0.25f)*2.0f;
    //mineBot.rotationX = -90;
    //mineBot.position.z = -700;
    //mineBot.position.y = -30;

    srand(time(NULL));
    for (unsigned i=0;i<5;i++)
    {
        bots[i].pos = glm::vec3(rand()%(5000*2), -30.0f, -rand()%(5700*2));
        bots[i].yangle = bots[i].xangle = bots[i].zangle = 0.0f;
        bots[i].dead = false;
        bots[i].shots = 0;
        bots[i].anim_tm = 0.0;
        bots[i].expl_tm = 0.0f;
    }

    gun.load("Assets\\gun.m3d", "Assets\\");
    gun.scale = glm::vec3(0.09f,0.09f,0.09f);

    gun.rotationX = -100;
    gun.rotationY = 180;


    gun.offset.z = 65;
    gun.offset.y = 105;


    gun.update(guntm);

    startTime = timeGetTime();

    UpdateCam();

    ShowCursor(FALSE);
    SetCursorPos(WIDTH/2, HEIGHT/2);


//#include "box.h"

    //box.position.y = -35;
    //box.scale.z = box.scale.x = 3000;
    box_tex = gp.newTexture("assets\\grass_T.bmp");
    //box.setTextureId(box_tex);

    terr.setTextureId(box_tex);


    srand(time(NULL));
}

// Only for ray from point rayA directing towards rayB, not behing rayA
bool SphereRayIntersect(glm::vec3 center, float radius, glm::vec3 rayA, glm::vec3 rayB)
{
    glm::vec3 dir = rayB-rayA;
    dir = glm::normalize(dir);

    glm::vec3 rc = center - rayA;
    glm::vec3 prc = glm::dot(rc, dir) * dir;

    float dist = glm::length(rc-prc);

    if (dist >= radius) return false;
    return true;
}

void Shoot()
{
    for (unsigned i=0;i<5;i++)
    if (!bots[i].dead)
    {
        if (SphereRayIntersect( glm::vec3(bots[i].pos.x, bots[i].pos.y+13, bots[i].pos.z), BOT_RADIUS,
                               gp.cam_eye, gp.cam_target))
        {

            bots[i].shots++;
        }
    }
}


#define checkSpeed (speedFactor <= 0.5 && started)
float sunangle = 0.0f;
void Update()
{


    frames++;
    if (frames>=200)
    {
        FPS = (200 * 1000)/ (timeGetTime()-startTime);
        startTime = timeGetTime();
        frames = 0;
    }
    speedFactor = 10.0f/FPS;

    //system("cls");
    //cout << FPS << endl;
    if (GunShot)
    {
        guntm += 40.0*speedFactor;
        gun.update(guntm);
        if (guntm==0.0f) {
            GunShot=false;
            Shoot();
        }
    }
    //tm+=0.08*speedFactor;
    //astroBoy.update(tm);

    //btm+= 0.06*speedFactor;
    //mineBot.update(btm);

    //mineBot.position.z += 0.1;




    /*if (keys[VK_LEFT]) if (gun.rotationY <270-35) gun.rotationY += 5 * speedFactor;
    if (keys[VK_RIGHT]) if (gun.rotationY >90+35) gun.rotationY -= 5 * speedFactor;
    if (keys[VK_UP]) if (gun.rotationX > -180+35) gun.rotationX -= 5 * speedFactor;
    if (keys[VK_DOWN]) if (gun.rotationX < -90+4) gun.rotationX += 5 * speedFactor;*/


    if (!checkSpeed) return;

    if (GetAsyncKeyState('S')) sunangle+=0.01;
    if (GetAsyncKeyState('D')) sunangle-=0.01;

    gp.sundir.x = -cosf(sunangle);
    gp.sundir.y = -sinf(sunangle);

    for (unsigned i=0;i<5;i++)
         if (bots[i].shots==-100) return;

    POINT point;
    GetCursorPos(&point);
    //if (keys[VK_RIGHT])
    //if (GetAsyncKeyState(VK_RIGHT))
    if (point.x>WIDTH/2)
    {
        camYAngle += 0.1f*speedFactor * (point.x-WIDTH/2)/2.0f;
        UpdateCam();
    }
    //if (keys[VK_LEFT])
    //if (GetAsyncKeyState(VK_LEFT))
    if (point.x<WIDTH/2)
    {
        camYAngle -= 0.1f*speedFactor * (WIDTH/2-point.x)/2.0f;
        UpdateCam();
    }

     //if (keys[vka('T')])
    //if (GetAsyncKeyState(vka('T')))
    if (point.y<HEIGHT/2)
    {
        camXAngle -= 0.1f*speedFactor;
        //gp.cam_eye.y += mov_units/10;
        UpdateCam();
    }
    //if (keys[vka('G')])
    //if (GetAsyncKeyState(vka('G')))
    if (point.y>HEIGHT/2)
    {
        camXAngle += 0.1f*speedFactor;
        //gp.cam_eye.y -= mov_units/10;
        UpdateCam();
    }

    SetCursorPos(WIDTH/2, HEIGHT/2);

    #define mov_units (20.0f * speedFactor)
    //if (keys[VK_UP])
    if (GetAsyncKeyState(VK_UP))
    {

        gp.cam_eye.x += mov_units * sinf(camYAngle);
        gp.cam_eye.z -= mov_units * cosf(camYAngle);
        UpdateCam();
    }
    //if (keys[VK_DOWN])
    if (GetAsyncKeyState(VK_DOWN))
    {

        gp.cam_eye.x -= mov_units * sinf(camYAngle);
        gp.cam_eye.z += mov_units * cosf(camYAngle);
        UpdateCam();
    }

    //if (keys[vka('D')])
    if (GetAsyncKeyState(VK_RIGHT))
    {

        gp.cam_eye.x += mov_units * cosf(camYAngle);
        gp.cam_eye.z += mov_units * sinf(camYAngle);
        UpdateCam();
    }
    //if (keys[vka('A')])
    if (GetAsyncKeyState(VK_LEFT))
    {

        gp.cam_eye.x -= mov_units * cosf(camYAngle);
        gp.cam_eye.z -= mov_units * sinf(camYAngle);
        UpdateCam();
    }






    //gun.position = gp.cam_eye;
    //mdl.rotationAngle += 0.1;
    //if (keys[VK_SPACE] /*&& !GunShot*/)
    if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_LBUTTON))
    {
        GunShot=true;
        //guntm = 0;

    }



}

void MoveBot(int i)
{

        glm::vec3 * pos = & bots[i].pos;

        glm::vec3 dir = glm::vec3(gp.cam_eye.x -  pos->x, 0, gp.cam_eye.z - pos->z);
        dir = glm::normalize(dir);

        pos->x += dir.x * 5 * speedFactor;
        pos->z += dir.z * 5 * speedFactor;

        float ht = terr.getHeight(pos->x,  pos->z);
        if (ht>=0) pos->y = ht;

        bots[i].yangle = glm::degrees(atan2f(dir.x, dir.z));//glm::degrees(-camYAngle);

        float htz1 = terr.getHeight(pos->x-BOT_RADIUS, pos->z);
        float htz2 = terr.getHeight(pos->x+BOT_RADIUS, pos->z);

        float htx1 = terr.getHeight(pos->x, pos->z-BOT_RADIUS);
        float htx2 = terr.getHeight(pos->x, pos->z+BOT_RADIUS);

        bots[i].zangle = glm::degrees(atan2f(htz2-htz1, BOT_RADIUS*2.0f));
        bots[i].xangle = glm::degrees(atan2f(htx1-htx2, BOT_RADIUS*2.0f));

}
void Render()
{
    //gun.render(&gp);
    //astroBoy.render(&gp);
    //spr.position.z = -20;

    //box.render(&gp);
    for (unsigned i=0;i<5;i++)
    if (!bots[i].dead)
    {

        if (bots[i].shots < 10 && bots[i].shots!=-100)
        {
            if (checkSpeed) bots[i].anim_tm += 0.06*speedFactor;

            //mineBot.position.x = bots[0].pos_xz.x;
            //mineBot.position.z = bots[0].pos_xz.y;
            mineBot.position = bots[i].pos;//glm::vec3(bots[0].pos.x, bots[0].pos.y-15, bots[0].pos.z);
            mineBot.rotationY = bots[i].yangle;
            mineBot.rotationX = bots[i].xangle - 90;
            mineBot.rotationZ = bots[i].zangle;

            float dx = mineBot.position.x - gp.cam_eye.x;
            float dy = mineBot.position.z - gp.cam_eye.z;
            float rsqr = (dx*dx) + (dy*dy);
            if (rsqr <= 38*38) bots[i].shots = -100;

            mineBot.update(bots[i].anim_tm);
            mineBot.render(&gp);
            if (checkSpeed) MoveBot(i);//bots[0].pos_xz.y += 4*speedFactor;
        }


        //if (bots[0].pos_xz.y>0) bots[0].dead = true;
    }
    //box.render(&gp);

    //mineBot.render(&gp);
    gun.render(&gp);


    //msh.render(&gp);

    terr.render(&gp);

     sky.render(&gp, glm::translate(glm::mat4(), gp.cam_eye)
               * glm::scale(glm::mat4(), glm::vec3(5000.0f,3000.0f,5000.0f)));

    if (GunShot)
    {
        glDisable(GL_CULL_FACE);

        float scale = (rand()%5);//17)/4.0f;
        //if (scale<2.0f) scale =0.0f;

        //flash.rotationZ = angle*cosf(camYAngle);
        //flash.rotationX = angle*sinf(camYAngle);

        flash.scale = glm::vec3(scale,scale,scale);

        flash.render(&gp, 2);
        glEnable(GL_CULL_FACE);
    }


    glDisable(GL_DEPTH_TEST);
    for (unsigned i=0;i<5;i++)
    if (!bots[i].dead)
    {
        if ((bots[i].shots>=10 || bots[i].shots==-100)&& checkSpeed)
        {
            if (bots[i].shots==-100)
            {
                spr.position.x = gp.cam_target.x + 10*sinf(camYAngle);
                spr.position.z = gp.cam_target.z - 10*cosf(camYAngle);
                spr.position.y = gp.cam_target.y-20;
            }
            else
                spr.position = glm::vec3(bots[i].pos.x, bots[i].pos.y+20, bots[i].pos.z);

            glm::vec3 tocam = gp.cam_eye-spr.position;
            spr.rotationY = glm::degrees(atan2f(tocam.x, tocam.z));  // atan2f(vec.y, vec.x)

            spr.render(&gp,speedFactor, bots[i].expl_tm);
            if (bots[i].expl_tm == 0.0f)
            {
                bots[i].dead = true;
                if (bots[i].shots==-100)
                {
                    MessageBox(0, "YOU ARE DEAD!!!", "Sorry, but...",0);
                    PostQuitMessage(0);
                }
            }
        }
    }


    float tmp=0.0f;
    crosshair.render(&gp, speedFactor, tmp, false);
    glEnable(GL_DEPTH_TEST);

    //spr.render(&gp, speedFactor);
    if (!started)
    {
        DWORD newtime = timeGetTime();
        if (newtime==startTime) return;
        FPS = 1000/(timeGetTime()-startTime);
        started=true;
        return;
    }


}

void End()
{
    terr.free();
    //box.free();
    spr.free();
    crosshair.free();
    //msh.free();
    gun.free();
    //astroBoy.free();
    mineBot.free();

    sky.free();
}
