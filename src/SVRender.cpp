#include <SVRender.hpp>

#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>

SVRender::SVRender(const int32 wnd_width_, const int32 wnd_height_) :
    wnd_width(wnd_width_), wnd_height(wnd_height_), aspect_ratio(0.f), texReady(false),
    white_luminance(1.0), tonemap_luminance(1.0)
{

}

//! --------------------------------------  初始化  --------------------------------------
bool SVRender::init(const ConfigBowl& cbowl, const std::string& shadersurroundvert, const std::string& shadersurroundfrag,
                    const std::string& shaderscreenvert, const std::string& shaderscreenfrag,
                    const std::string shaderblackrectvert, const std::string shaderblackrectfrag)
{
    if (isInit)
        return isInit;

    aspect_ratio = static_cast<float>(wnd_width) / wnd_height;  // 窗口宽高比

    if (!initBowl(cbowl, shadersurroundvert, shadersurroundfrag))
        return false;

    if (!shaderblackrectvert.empty() && !shaderblackrectfrag.empty()){
        if (!initbowlBlackRect(shaderblackrectvert, shaderblackrectfrag))
          return false;
    }

    if (!initQuadRender(shaderscreenvert, shaderscreenfrag))
        return false;

    isInit = true;
    return isInit;
}

// 初始化 OGLbowl 对象
bool SVRender::initBowl(const ConfigBowl& cbowl, const std::string& shadersurroundvert, const std::string& shadersurroundfrag)
{
    if (!OGLbowl.OGLShader.initShader(shadersurroundvert.c_str(), shadersurroundfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender::initBowl", "OGLbowl.OGLShader.initShader() Error !");
        return false;
    }
        
    glGenVertexArrays(1, &OGLbowl.VAO);  // 生成顶点数组对象（VAO）
    glGenBuffers(1, &OGLbowl.VBO);  // 生成顶点缓冲对象（VBO）
    glGenBuffers(1, &OGLbowl.EBO);  // 生成元素缓冲对象（EBO）

    bowlmodel = cbowl;  // ConfigBowl 结构体赋值
    std::vector<float> data;
    std::vector<uint> idxs;

    Bowl bowl(bowlmodel);
    if (!bowl.generate_mesh_uv_hole(cbowl.vertices_num, cbowl.hole_radius, data, idxs)) {  // 生成网格数据，存储在 data 和 idxs 向量中
        LOG_ERROR("SVRender::initBowl", "bowl.generate_mesh_uv_hole Error !");
        return false;
    }

    OGLbowl.indexBuffer = idxs.size();

    constexpr auto stride = (3 + 2) * sizeof(float);  // 顶点属性之间的间隔，包含3个浮点数的位置数据和2个浮点数的纹理坐标数据

    glBindVertexArray(OGLbowl.VAO);  // 绑定之前创建的顶点数组对象（VAO），之后对VBO和EBO的所有操作都会被记录在这个VAO中
    glBindBuffer(GL_ARRAY_BUFFER, OGLbowl.VBO);  // 绑定VBO，以便将顶点数据（data）上传到GPU
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);  // 将顶点数据从CPU内存复制到GPU的内存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OGLbowl.EBO);  // 绑定EBO，以便将索引数据（idxs）上传到GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, OGLbowl.indexBuffer * sizeof(uint), &idxs[0], GL_STATIC_DRAW);  // 将索引数据从CPU内存复制到GPU的内存
    glEnableVertexAttribArray(0);  // 启用顶点属性数组0（通常用于位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);  // 定义顶点属性数组0的数据格式，指定位置数据由3个浮点数组成，不需要标准化，每个顶点数据集合之间的间隔是 stride
    glEnableVertexAttribArray(1);  // 启用顶点属性数组1（通常用于纹理坐标数据）
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));  // 定义顶点属性数组1的数据格式，指定纹理坐标由2个浮点数组成，每个顶点数据集合之间的间隔是 stride，纹理坐标数据在每个顶点数据集合中紧随位置数据之后

    return true;
}

// 初始化 OGLblackRect 对象，用于渲染黑色矩形
bool SVRender::initbowlBlackRect(const std::string& fileblackrectvert, const std::string& fileblackrectfrag)
{
    if (!OGLblackRect.OGLShader.initShader(fileblackrectvert.c_str(), fileblackrectfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender::initbowlBlackRect", "OGLblackRect.OGLShader.initShader Error !");
        return false;
    }

    // 定义一个矩形的顶点数据，包含两个三角形，每个三角形有三个顶点
    const float rectvert[] = {  
         0.4f,  0.0f,  0.525f,
        -0.4f,  0.0f,  0.525f,
        -0.4f,  0.0f, -0.525f,

         0.4f,  0.0f,  0.525f,
        -0.4f,  0.0f, -0.525f,
         0.4f,  0.0f, -0.525f,
    };

    glGenVertexArrays(1, &OGLblackRect.VAO);
    glGenBuffers(1, &OGLblackRect.VBO);
    glGenBuffers(1, &OGLblackRect.EBO);

    glBindVertexArray(OGLblackRect.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OGLblackRect.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectvert), &rectvert, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    return true;
}

// 初始化 OGLquadrender 对象，用于渲染二维四边形（quad）
bool SVRender::initQuadRender(const std::string& shaderscreenvert, const std::string& shaderscreenfrag)
{
    if (!OGLquadrender.OGLShader.initShader(shaderscreenvert.c_str(), shaderscreenfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender::initQuadRender", "OGLquadrender.OGLShader.initShader Error !");
        return false;
    }   

    // 定义一个二维四边形（quad）的顶点数据，四个顶点，每个顶点包含位置(x,y)和纹理坐标(u,v)
    constexpr float quadvert[] = {
        -1.f, 1.f, 0.f, 1.f,  // 左上角
        -1.f, -1.f, 0.f, 0.f,  // 左下角
        1.f, 1.f, 1.f, 1.f,  // 右上角
        1.f, -1.f, 1.f, 0.f  // 右下角
    };

    glGenVertexArrays(1, &OGLquadrender.VAO);  // 生成一个新的顶点数组对象（VAO）并将其ID存储在 OGLquadrender.VAO 中。VAO用于存储顶点属性配置
    glGenBuffers(1, &OGLquadrender.VBO);  // 生成一个新的顶点缓冲对象（VBO）并将其ID存储在 OGLquadrender.VBO 中。VBO用于存储顶点数据
    glBindVertexArray(OGLquadrender.VAO);  // 绑定VAO，接下来所有关于顶点属性和顶点缓冲对象的操作都会应用到这个VAO
    glBindBuffer(GL_ARRAY_BUFFER, OGLquadrender.VBO);  // 绑定VBO并指定它是一个GL_ARRAY_BUFFER，这意味着它包含顶点数组数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadvert), &quadvert, GL_STATIC_DRAW);  // 将顶点数据（quadvert）复制到当前绑定的缓冲区（VBO）
    glEnableVertexAttribArray(0);  // 启用顶点属性数组0，这是对顶点数据的第一部分（位置数据）的引用
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);  // 指定顶点属性数组0的格式，描述了位置数据，包含2个浮点数
    glEnableVertexAttribArray(1);  // 启用顶点属性数组1，这是对顶点数据的第二部分（纹理坐标）的引用
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));  // 指定顶点属性数组1的格式，描述了纹理坐标，包含2个浮点数

    glGenFramebuffers(1, &OGLquadrender.framebuffer);  // 生成一个新的帧缓冲对象，并将其ID存储在 OGLquadrender.framebuffer 中
    glBindFramebuffer(GL_FRAMEBUFFER, OGLquadrender.framebuffer);  // 将之前创建的帧缓冲对象绑定为当前活动的帧缓冲对象

    glGenTextures(1, &OGLquadrender.framebuffer_tex);
    glBindTexture(GL_TEXTURE_2D, OGLquadrender.framebuffer_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wnd_width, wnd_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OGLquadrender.framebuffer_tex, 0);

    glGenRenderbuffers(1, &OGLquadrender.renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, OGLquadrender.renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wnd_width, wnd_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OGLquadrender.renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool SVRender::addModel(const std::string& pathmodel, const std::string& pathvertshader,
              const std::string& pathfragshader)
{
    bool res = pathmodel.empty() || pathvertshader.empty() || pathfragshader.empty();
    if (res){
      std::cerr << "Error: empty path to model\n";
      return false;
    }

    Model m(pathmodel);
    res = m.getModelInit();
    if (!res){
      std::cerr << "Error: fail load model from path\n";
      return false;
    }

    modelshaders.emplace_back(std::make_shared<Shader>());
    auto last_idx = modelshaders.size() - 1;
    res = modelshaders[last_idx]->initShader(pathvertshader.c_str(), pathfragshader.c_str());
    if (!res){
      std::cerr << "Error: fail init shaders for load model\n";
      return false;
    }

    models.emplace_back(std::move(m));

    // 调整车身位姿
    glm::mat4 transform_car(1.f);
    // 此处y轴指天，将车模型放在碗模型的地面中央
    transform_car = glm::translate(transform_car, glm::vec3(0.f, 1.01f, 0.f));
    // 控制车身旋转，采用旋转角+旋转轴表示，此处z轴指天
    transform_car = glm::rotate(transform_car, glm::radians(-80.f), glm::vec3(0.f, 1.f, 0.f));  // 调节绕y轴旋转角度来控制车身朝向
    // 控制车身缩放，默认0.002
    transform_car = glm::scale(transform_car, glm::vec3(0.1f));
    modeltranformations.emplace_back(transform_car);

    return true;
}

//! --------------------------------------  渲染  --------------------------------------

void SVRender::render(const Camera& cam, const cv::cuda::GpuMat& frame)
{
    // render command
    // ...
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, OGLquadrender.framebuffer); // bind scene framebuffer
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawSurroundView(cam, frame);

    // drawBlackRect(cam);

    drawModel(cam);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer
    glDisable(GL_DEPTH_TEST);

    drawScreen(cam);

    // unbound
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SVRender::texturePrepare(const cv::cuda::GpuMat& frame)
{
    if (!texReady){
        texReady = cuOgl.init(frame);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    auto ok = cuOgl.copyFrom(frame, 0);
}


void SVRender::drawSurroundView(const Camera& cam, const cv::cuda::GpuMat& frame)
{
    glm::mat4 model = bowlmodel.transformation;
    auto view = cam.getView();
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);

#ifdef HEMISPHERE
    model = glm::scale(model, glm::vec3(3.f, 3.f, 3.f));
#else
    model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
#endif
    OGLbowl.OGLShader.useProgramm();
    OGLbowl.OGLShader.setMat4("model", model);
    OGLbowl.OGLShader.setMat4("view", view);
    OGLbowl.OGLShader.setMat4("projection", projection);
    OGLbowl.OGLShader.setFloat("lum_white", white_luminance);
    OGLbowl.OGLShader.setFloat("lum_map", tonemap_luminance);

    texturePrepare(frame);

    glBindVertexArray(OGLbowl.VAO);

    glDrawElements(GL_TRIANGLE_STRIP, OGLbowl.indexBuffer, GL_UNSIGNED_INT, 0);

}

void SVRender::drawModel(const Camera& cam)
{
    glm::mat4 model(1.f);
    auto view = cam.getView();
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);

    for(auto i = 0; i < models.size(); ++i){
        model = modeltranformations[i];
        modelshaders[i]->useProgramm();
        modelshaders[i]->setMat4("model", model);
        modelshaders[i]->setMat4("view", view);
        modelshaders[i]->setMat4("projection", projection);
        models[i].Draw(*modelshaders[i]);
    }

}


void SVRender::drawBlackRect(const Camera& cam)
{
    glm::mat4 model(1.f);

#ifdef HEMISPHERE
    const float y_min = 0.08f;
#else
    constexpr auto bias = 1e-4;
    const float y_min = bowlmodel.y_start + bias;
#endif

    model = glm::translate(model, glm::vec3(0.f, y_min, 0.f));

    auto view = cam.getView();
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);

    OGLblackRect.OGLShader.useProgramm();
    OGLblackRect.OGLShader.setMat4("model", model);
    OGLblackRect.OGLShader.setMat4("view", view);
    OGLblackRect.OGLShader.setMat4("projection", projection);

    glBindVertexArray(OGLblackRect.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void SVRender::drawScreen(const Camera& cam)
{
    OGLquadrender.OGLShader.useProgramm();


    glBindVertexArray(OGLquadrender.VAO);
    glBindTexture(GL_TEXTURE_2D, OGLquadrender.framebuffer_tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



