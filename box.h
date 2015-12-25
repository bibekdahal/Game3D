

    vertex verts[] = {
    {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec2(0.0f,0.0f)},
    {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec2(1.0f,0.0f)},
    {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f,1.0f,0.0f), glm::vec2(1.0f,1.0f)},
    {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,1.0f,0.0f), glm::vec2(0.0f,1.0f)},
    // back
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec2(0.0f,1.0f)},
    {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec2(1.0f,1.0f)},
    {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f,1.0f,0.0f), glm::vec2(1.0f,0.0f)},
    {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,1.0f,0.0f), glm::vec2(0.0f,0.0f)}
    };
    unsigned int indices[] = {
    // front
    0, 1, 2,
    2, 3, 0,
    // top
    3, 2, 6,
    6, 7, 3,
    // back
    7, 6, 5,
    5, 4, 7,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // left
    4, 0, 3,
    3, 7, 4,
    // right
    1, 5, 6,
    6, 2, 1,
    };

    box.newMesh(verts, 8, indices, 3*2*6);

