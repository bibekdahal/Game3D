#include "stdinc.h"
#include "graphics.h"
#include "terrain.h"

#define hdata(x,y) (heightData[(y)*hWidth+(x)])
terrain::terrain()
{
    texId = vbo_data = vao_data = 0;
    heightData = NULL;
}

terrain::~terrain()
{
    free();
}

float terrain::getHeight(float x, float y)
{
    if (!heightData) return -1.0f;
    /*if (x<0 || x/xScale>=hWidth) return -1.0f;
    if (y>0 || y/yScale<=-hHeight) return -1.0f;*/

    int cX = x/xScale;
    int cY = -y/yScale;
    if (cX<0 || cX+1>=hWidth || cX>=hWidth) return -1.0f;
    if (cY<0 || cY+1>=hHeight || cY>=hHeight) return -1.0f;

    float   pu = x/xScale  - cX,
            pv = -y/yScale - cY;


    float height;

    //glm::vec3 p00 = glm::vec3( cX*xScale, hdata(cX,cY), -cY*yScale);
    //glm::vec3 p10 = glm::vec3( (cX+1)*xScale, hdata(cX+1,cY), -cY*yScale);
    //glm::vec3 p01 = glm::vec3( cX*xScale, hdata(cX,cY+1), -(cY+1)*yScale);
    //glm::vec3 p11 = glm::vec3( (cX+1)*xScale, hdata(cX+1,cY+1), -(cY+1)*yScale);
    float p00 = hdata(cX,cY);
    float p10 = hdata(cX+1,cY);
    float p01 = hdata(cX,cY+1);
    float p11 = hdata(cX+1,cY+1);

    //glm::vec3 dU, dV;
    float dU,dV;
    if (pu>pv)
    {
        dU=p10-p00;
        dV=p11-p10;
    }
    else
    {
        dU = p11 - p01;
        dV = p01 - p00;
    }

    //glm::vec3 heightPos = p00 + ( dU * pu ) + ( dV * pv );
    height = p00 + ( dU * pu ) + ( dV * pv );//heightPos.y;

    //cout << dU <<  " " << dV << endl;
    return height;

    //return hdata((int)(x/xScale),(int)(-y/yScale));


}
void terrain::free()
{

    if (vbo_data) glDeleteBuffers(1, &vbo_data);
    if (ibo_data) glDeleteBuffers(1, &ibo_data);
    if (vao_data) glDeleteVertexArrays(1, &vao_data);
    if (heightData) delete[] heightData;

    texId = vbo_data = ibo_data = vao_data = 0;
    heightData = NULL;
}

void terrain::setTextureId(unsigned int texture)
{
    texId = texture;
}

void terrain::create(string filename, int width, int height, float xscale, float yscale, float hscale)
{
    if (vao_data || heightData || vbo_data) free();
    hWidth = width;
    hHeight = height;

    xScale = xscale;
    yScale = yscale;

    vertex* vertices = new vertex[width*height];

    heightData = new float[width*height];
    fstream file;
    file.open(filename.c_str(), fstream::in | fstream::binary);

    if (file.fail())
    {
        cout << "FAILED TO OPEN FILE";
        delete[] heightData;
        heightData = NULL;
        return;
    }

    //file.read(heightData, width*height);
    for (int i=0;i<width;i++)
        for (int j=0;j<height;j++)
            hdata(i,j) = (float)file.get()/hscale;
    file.close();

    int i, j;
    for (i=0;i<width;i++)
        for (j=0;j<height;j++)
    {
        vertices[j*width+i].pos = glm::vec3( i*xScale, hdata(i,j), -j*yScale);
        vertices[j*width+i].tcoords = glm::vec2( (float)i/(width-1)*20, 20.0f - (float)j/(height-1)*20 );
        vertices[j*width+i].normal = glm::vec3(0.0f,0.0f,0.0f);
    }

    unsigned int * indices = new unsigned int[(width-1)*(height-1)*6];
    for (int x=0;x< width-1;x++){
        for (int y=0; y< height-1;y++)    {
            indices[(x+y*(width-1))*6+2] = (x+1)+(y+1)*width;
            indices[(x+y*(width-1))*6+1] = (x+1)+y*width;
            indices[(x+y*(width-1))*6] = x+y*width;

            indices[(x+y*(width-1))*6+3] = x+(y+1)*width;
            indices[(x+y*(width-1))*6+4] = x+y*width;
            indices[(x+y*(width-1))*6+5] = (x+1)+(y+1)*width;
        }
    }

    for (i=0;i<(width-1)*(height-1)*6;i+=3)
    {
        glm::vec3 v0 = vertices[ indices[i + 0] ].pos;
        glm::vec3 v1 = vertices[ indices[i + 1] ].pos;
        glm::vec3 v2 = vertices[ indices[i + 2] ].pos;

        glm::vec3 normal = glm::normalize( glm::cross( v1 - v0, v2 - v0 ) );

        vertices[ indices[i + 0] ].normal += normal;
        vertices[ indices[i + 1] ].normal += normal;
        vertices[ indices[i + 2] ].normal += normal;
    }
    for (i=0;i<width*height;i++)
        vertices[i].normal = glm::normalize(vertices[i].normal);

    vbo_data = CreateBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertex)*width*height, GL_STATIC_DRAW);
    delete[] vertices;

    ibo_data = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, indices,sizeof(unsigned int)*(width-1)*(height-1)*6,GL_STATIC_DRAW);
    delete[] indices;

    glGenVertexArrays(1, &vao_data);
    glBindVertexArray(vao_data);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_data);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, tcoords));

    glBindVertexArray(0);

}

void terrain::render(graphics* gp)
{
    if (vao_data == 0) return;

    glUseProgram(gp->program);

    glBindVertexArray(vao_data);

    glm::mat4 mvp = gp->projection * gp->view;
    glUniformMatrix4fv(gp->uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));

    glm::mat3 mit = glm::mat3();
    glUniformMatrix3fv(gp->uniform_mat_mit, 1, GL_FALSE, glm::value_ptr(mit));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(gp->uniform_tex, 0);

    glUniform1i(gp->uniform_notlit,0);

    glUniform2f(gp->uniform_uvdisp, 0.0f, 0.0f);

    glUniform3f(gp->uniform_sundir, gp->sundir.x, gp->sundir.y, gp->sundir.z);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); // Get the size of buffer
    glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

}
