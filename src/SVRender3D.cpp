#include <SVRender3D.hpp>

#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>


//! --------------------------------------  初始化  --------------------------------------
bool SVRender3D::init(const ConfigProjModel& proj_cfg_, const std::string& shadersurroundvert, const std::string& shadersurroundfrag,
                    const std::string& shaderscreenvert, const std::string& shaderscreenfrag,
                    const std::string shaderblackrectvert, const std::string shaderblackrectfrag)
{
    if (isInit)
        return isInit;

    aspect_ratio = static_cast<float>(wnd_width) / wnd_height;  // 窗口宽高比
    proj_cfg = proj_cfg_;  // ConfigProjModel 结构体赋值

    if (!initProjModel(shadersurroundvert, shadersurroundfrag))
        return false;

    if (!initBlackRect(shaderblackrectvert, shaderblackrectfrag))
        return false;

    if (!initQuadRender(shaderscreenvert, shaderscreenfrag))
        return false;

    isInit = true;
    return isInit;
}

// 初始化 OGLProjModel 对象
bool SVRender3D::initProjModel(const std::string& shadersurroundvert, const std::string& shadersurroundfrag)
{
    if (!OGLProjModel.OGLShader.initShader(shadersurroundvert.c_str(), shadersurroundfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender3D::initProjModel", "OGLProjModel.OGLShader.initShader() Error !");
        return false;
    }
        
    glGenVertexArrays(1, &OGLProjModel.VAO);  // 生成顶点数组对象（VAO）
    glGenBuffers(1, &OGLProjModel.VBO);  // 生成顶点缓冲对象（VBO）
    glGenBuffers(1, &OGLProjModel.EBO);  // 生成元素缓冲对象（EBO）

    std::vector<float> data;
    std::vector<uint> idxs;

    // 初始化投影模型
    if (proj_type == "bowl_parab")
        proj_model = std::make_shared<BowlParabModel>(proj_cfg);
    else if (proj_type == "drop")
        proj_model = std::make_shared<DropModel>(proj_cfg);
    else if (proj_type == "burger")
        proj_model = std::make_shared<BurgerModel>(proj_cfg);
    else if (proj_type == "hemi_sphere")
        proj_model = std::make_shared<HemiSphereModel>(proj_cfg);
    else if (proj_type == "sphere")
        proj_model = std::make_shared<SphereModel>(proj_cfg);
    else if (proj_type == "cylinder")
        proj_model = std::make_shared<CylinderModel>(proj_cfg);
    else {
        LOG_ERROR("SVRender3D::initProjModel", "Unknown projection type !");
        return false;
    }

    // 生成模型网格数据，存储在 data 和 idxs 向量中
    if (!proj_model->generate_mesh_uv(proj_cfg.vertices_num, data, idxs)) {  
        LOG_ERROR("SVRender3D::initProjModel", "bowl.generate_mesh_uv_hole Error !");
        return false;
    }

    OGLProjModel.indexBuffer = idxs.size();

    constexpr auto stride = (3 + 2) * sizeof(float);  // 顶点属性之间的间隔，包含3个浮点数的位置数据和2个浮点数的纹理坐标数据

    glBindVertexArray(OGLProjModel.VAO);  // 绑定之前创建的顶点数组对象（VAO），之后对VBO和EBO的所有操作都会被记录在这个VAO中
    glBindBuffer(GL_ARRAY_BUFFER, OGLProjModel.VBO);  // 绑定VBO，以便将顶点数据（data）上传到GPU
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);  // 将顶点数据从CPU内存复制到GPU的内存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OGLProjModel.EBO);  // 绑定EBO，以便将索引数据（idxs）上传到GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, OGLProjModel.indexBuffer * sizeof(uint), &idxs[0], GL_STATIC_DRAW);  // 将索引数据从CPU内存复制到GPU的内存
    glEnableVertexAttribArray(0);  // 启用顶点属性数组0（通常用于位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);  // 定义顶点属性数组0的数据格式，指定位置数据由3个浮点数组成，不需要标准化，每个顶点数据集合之间的间隔是 stride
    glEnableVertexAttribArray(1);  // 启用顶点属性数组1（通常用于纹理坐标数据）
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));  // 定义顶点属性数组1的数据格式，指定纹理坐标由2个浮点数组成，每个顶点数据集合之间的间隔是 stride，纹理坐标数据在每个顶点数据集合中紧随位置数据之后

    return true;
}

// 初始化 OGLBlackRect 对象，用于渲染黑色矩形
bool SVRender3D::initBlackRect(const std::string& fileblackrectvert, const std::string& fileblackrectfrag)
{
    if (!OGLBlackRect.OGLShader.initShader(fileblackrectvert.c_str(), fileblackrectfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender3D::initBlackRect", "OGLBlackRect.OGLShader.initShader Error !");
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

    glGenVertexArrays(1, &OGLBlackRect.VAO);
    glGenBuffers(1, &OGLBlackRect.VBO);
    glGenBuffers(1, &OGLBlackRect.EBO);

    glBindVertexArray(OGLBlackRect.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OGLBlackRect.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectvert), &rectvert, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    return true;
}

// 初始化 OGLQuadRender 对象，用于渲染二维四边形（quad）
bool SVRender3D::initQuadRender(const std::string& shaderscreenvert, const std::string& shaderscreenfrag)
{
    if (!OGLQuadRender.OGLShader.initShader(shaderscreenvert.c_str(), shaderscreenfrag.c_str())) {  // 传入顶点和片段着色器的路径，用于初始化着色器
        LOG_ERROR("SVRender3D::initQuadRender", "OGLQuadRender.OGLShader.initShader Error !");
        return false;
    }

    // 定义一个二维四边形（quad）的顶点数据，四个顶点，每个顶点包含位置(x,y)和纹理坐标(u,v)
    constexpr float quadvert[] = {
        -1.f, 1.f, 0.f, 1.f,  // 左上角
        -1.f, -1.f, 0.f, 0.f,  // 左下角
        1.f, 1.f, 1.f, 1.f,  // 右上角
        1.f, -1.f, 1.f, 0.f  // 右下角
    };

    glGenVertexArrays(1, &OGLQuadRender.VAO);  // 生成一个新的顶点数组对象（VAO）并将其ID存储在 OGLQuadRender.VAO 中。VAO用于存储顶点属性配置
    glGenBuffers(1, &OGLQuadRender.VBO);  // 生成一个新的顶点缓冲对象（VBO）并将其ID存储在 OGLQuadRender.VBO 中。VBO用于存储顶点数据
    glBindVertexArray(OGLQuadRender.VAO);  // 绑定VAO，接下来所有关于顶点属性和顶点缓冲对象的操作都会应用到这个VAO
    glBindBuffer(GL_ARRAY_BUFFER, OGLQuadRender.VBO);  // 绑定VBO并指定它是一个GL_ARRAY_BUFFER，这意味着它包含顶点数组数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadvert), &quadvert, GL_STATIC_DRAW);  // 将顶点数据（quadvert）复制到当前绑定的缓冲区（VBO）
    glEnableVertexAttribArray(0);  // 启用顶点属性数组0，这是对顶点数据的第一部分（位置数据）的引用
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);  // 指定顶点属性数组0的格式，描述了位置数据，包含2个浮点数
    glEnableVertexAttribArray(1);  // 启用顶点属性数组1，这是对顶点数据的第二部分（纹理坐标）的引用
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));  // 指定顶点属性数组1的格式，描述了纹理坐标，包含2个浮点数

    glGenFramebuffers(1, &OGLQuadRender.framebuffer);  // 生成一个新的帧缓冲对象，并将其ID存储在 OGLQuadRender.framebuffer 中
    glBindFramebuffer(GL_FRAMEBUFFER, OGLQuadRender.framebuffer);  // 将之前创建的帧缓冲对象绑定为当前活动的帧缓冲对象

    glGenTextures(1, &OGLQuadRender.framebuffer_tex);  // 生成一个新的纹理对象用于存储图像数据，并将其ID存储在 OGLQuadRender.framebuffer_tex 中
    glBindTexture(GL_TEXTURE_2D, OGLQuadRender.framebuffer_tex);  // 将新创建的纹理对象绑定为当前活动的2D纹理对象，之后对2D纹理的操作都会应用到这个纹理对象上
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wnd_width, wnd_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);  // 使用 glTexImage2D 分配纹理图像的内存，此处纹理暂时没有数据
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 设置纹理过滤参数，当纹理被缩小时使用线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // 设置纹理过滤参数，当纹理被放大时使用线性过滤
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OGLQuadRender.framebuffer_tex, 0);  // 将纹理附加到帧缓冲对象的颜色附件上，意味着任何渲染到这个帧缓冲对象的内容都会被写入到这个纹理中

    glGenRenderbuffers(1, &OGLQuadRender.renderbuffer);  // 生成一个新的渲染缓冲对象，并将其ID存储在 OGLQuadRender.renderbuffer 中
    glBindRenderbuffer(GL_RENDERBUFFER, OGLQuadRender.renderbuffer);  // 将新创建的渲染缓冲对象绑定为当前活动的渲染缓冲对象，之后对渲染缓冲对象的操作都会应用到这个对象上
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wnd_width, wnd_height);  // 为渲染缓冲对象分配存储空间，指定了存储格式为 GL_DEPTH24_STENCIL8，同时包含了24位的深度信息和8位的模板信息
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OGLQuadRender.renderbuffer);  // 将渲染缓冲对象作为深度和模板附件连接到帧缓冲对象上，使帧缓冲对象具备了存储深度和模板信息的能力

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)  // 检查当前绑定的帧缓冲对象的状态
      return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  //将当前绑定的帧缓冲对象解绑，回到默认的帧缓冲对象

    return true;
}

// 加载物体3D模型，初始化相应的着色器，并设置模型的变换
bool SVRender3D::addObjModel(const std::string& pathmodel, const std::string& pathvertshader, const std::string& pathfragshader)
{
    if (pathmodel.empty() || pathvertshader.empty() || pathfragshader.empty()) {
        LOG_ERROR("SVRender3D::addObjModel", "Empty path to model");
        return false;
    }

    // 加载车辆3D模型文件
    ObjModel m(pathmodel);
    if (!m.getModelInit()) {
        LOG_ERROR("SVRender3D::addObjModel", "Fail load model from path");
        return false;
    }

    // 初始化着色器并添加到着色器列表中
    obj_model_shaders.emplace_back(std::make_shared<Shader>());
    auto last_idx = obj_model_shaders.size() - 1;
    if (!obj_model_shaders[last_idx]->initShader(pathvertshader.c_str(), pathfragshader.c_str())) {
        LOG_ERROR("SVRender3D::addObjModel", "Fail init shaders for load model");
        return false;
    }

    // 添加模型到物体模型列表中
    obj_models.emplace_back(std::move(m));

    // 设置模型变换并添加到变换列表中
    glm::mat4 transform_car(1.f);
    // 此处y轴指天，将车模型放在碗模型的地面中央
    transform_car = glm::translate(transform_car, glm::vec3(0.f, 1.01f, 0.f));
    // 控制车身旋转，采用旋转角+旋转轴表示，此处z轴指天
    transform_car = glm::rotate(transform_car, glm::radians(-80.f), glm::vec3(0.f, 1.f, 0.f));  // 调节绕y轴旋转角度来控制车身朝向
    // 控制车身缩放，默认0.002
    transform_car = glm::scale(transform_car, glm::vec3(0.1f));
    obj_model_transformations.emplace_back(transform_car);

    return true;
}

//! --------------------------------------  渲染  --------------------------------------
void SVRender3D::render(const Camera& cam, const cv::cuda::GpuMat& frame)
{
    glEnable(GL_DEPTH_TEST);  // 绘制之前启用深度测试，以正确处理遮挡关系
    glBindFramebuffer(GL_FRAMEBUFFER, OGLQuadRender.framebuffer); // 绑定帧缓冲对象，所有渲染命令将渲染到这个帧缓冲而不是默认的帧缓冲（即屏幕）
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  // 设置清除颜色为灰色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色缓冲和深度缓冲

    drawSurroundView(cam, frame);

    // drawBlackRect(cam);

    drawObjModel(cam);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 绑定回默认的帧缓冲对象，准备将内容渲染到屏幕上
    glDisable(GL_DEPTH_TEST);  // 禁用深度测试，这通常在渲染2D内容或后期处理时进行

    drawScreen(cam);  // 将之前渲染到自定义帧缓冲的内容绘制到屏幕上

    // 解绑任何可能绑定的顶点数组和顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// 使用 cv::cuda::GpuMat 对象准备纹理
void SVRender3D::texturePrepare(const cv::cuda::GpuMat& frame)
{
    if (!texReady) {
        texReady = cuOgl.init(frame);
        glBindTexture(GL_TEXTURE_2D, 0);  // 解绑任何可能绑定的2D纹理，用于防止意外修改纹理
    }
    auto ok = cuOgl.copyFrom(frame, 0);  // 将 frame 中的数据复制到纹理
}

// 渲染一个用于显示全景视图的3D投影模型对象
void SVRender3D::drawSurroundView(const Camera& cam, const cv::cuda::GpuMat& frame)
{
    glm::mat4 proj_model_transform(1.f);  // 获取碗模型的变换矩阵
    auto view = cam.getView();  // 获取摄像机视图矩阵
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);  // 创建投影矩阵，使用了摄像机的缩放值、长宽比和近远平面距离

    proj_model_transform = glm::scale(proj_model_transform, glm::vec3(5.f, 5.f, 5.f));  // 将模型缩放为5倍

    OGLProjModel.OGLShader.useProgramm();  // 使用碗模型的着色器程序
    OGLProjModel.OGLShader.setMat4("model", proj_model_transform);  // 设置着色器中的 model、view、projection 矩阵
    OGLProjModel.OGLShader.setMat4("view", view);
    OGLProjModel.OGLShader.setMat4("projection", projection);
    OGLProjModel.OGLShader.setFloat("lum_white", white_luminance);
    OGLProjModel.OGLShader.setFloat("lum_map", tonemap_luminance);

    texturePrepare(frame);  // 准备纹理

    glBindVertexArray(OGLProjModel.VAO);  // 绑定碗模型的顶点数组对象

    glDrawElements(GL_TRIANGLE_STRIP, OGLProjModel.indexBuffer, GL_UNSIGNED_INT, 0);  // 执行绘制调用

}

// 在3D场景中绘制一系列物体模型
void SVRender3D::drawObjModel(const Camera& cam)
{
    glm::mat4 obj_model_transform(1.f);  // 创建一个单位模型变换矩阵
    auto view = cam.getView();  // 获取摄像机视图矩阵
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);  // 创建投影矩阵，使用了摄像机的缩放值、长宽比和近远平面距离

    // 遍历所有模型并绘制
    for(auto i = 0; i < obj_models.size(); ++i) {
        obj_model_transform = obj_model_transformations[i];  // 获取并设置当前模型的变换矩阵
        obj_model_shaders[i]->useProgramm();  // 使用与当前模型关联的着色器程序
        obj_model_shaders[i]->setMat4("model", obj_model_transform);  // 设置着色器中的 model、view、projection 矩阵
        obj_model_shaders[i]->setMat4("view", view);
        obj_model_shaders[i]->setMat4("projection", projection);
        obj_models[i].Draw(*obj_model_shaders[i]);  // 调用模型的 Draw 方法进行绘制，传递着色器作为参数
    }
}

// 在3D场景中绘制一个黑色的矩形
void SVRender3D::drawBlackRect(const Camera& cam)
{
    glm::mat4 model_transform(1.f);  // 创建一个单位模型变换矩阵

#ifdef HEMISPHERE
    const float y_min = 0.08f;
#else
    constexpr auto bias = 1e-4;
    const float y_min = proj_cfg.y_start + bias;  // 确定矩形的垂直位置
#endif

    model_transform = glm::translate(model_transform, glm::vec3(0.f, y_min, 0.f));  // 对模型变换矩阵应用平移变换，将矩形移动到指定的垂直位置

    auto view = cam.getView();
    auto projection = glm::perspective(glm::radians(cam.getCamZoom()), aspect_ratio, 0.1f, 100.f);

    OGLBlackRect.OGLShader.useProgramm();
    OGLBlackRect.OGLShader.setMat4("model", model_transform);
    OGLBlackRect.OGLShader.setMat4("view", view);
    OGLBlackRect.OGLShader.setMat4("projection", projection);

    glBindVertexArray(OGLBlackRect.VAO);  // 绑定黑色矩形的顶点数组对象
    glDrawArrays(GL_TRIANGLES, 0, 6);  // 使用 GL_TRIANGLES 模式绘制矩形
    glBindVertexArray(0);  // 解绑顶点数组对象
}

// 在屏幕上绘制四边形 Quad，使用之前渲染到帧缓冲对象 FBO 的纹理
void SVRender3D::drawScreen(const Camera& cam)
{
    OGLQuadRender.OGLShader.useProgramm();  // 使用与四边形渲染相关的着色器程序

    glBindVertexArray(OGLQuadRender.VAO);  // 绑定用于绘制四边形的顶点数组对象，包含了四边形顶点的位置和纹理坐标信息
    glBindTexture(GL_TEXTURE_2D, OGLQuadRender.framebuffer_tex);  // 绑定之前渲染到帧缓冲对象的纹理，这个纹理现在将用于在四边形上渲染
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // 使用 GL_TRIANGLE_STRIP 模式绘制四边形
}


