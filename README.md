# Pano-View

* `GLFW`和 `GLUT`都用于OpenGL的窗口和事件管理，但GLFW是更现代和灵活的选择。
* `GLAD`和 `GLEW`都用于管理OpenGL的扩展，但GLAD提供了更现代和灵活的生成方式，更适合处理多样化的OpenGL环境。
* `GLU`（OpenGL Utility Library）是OpenGL的一个辅助库，它提供了一些高级的图形绘制功能。随着OpenGL本身和现代图形库（如GLFW、GLEW、GLAD）的发展，很多原本需要 `GLU`来实现的功能现在可以直接用更现代的方式实现。



- `glGenVertexArrays`：生成顶点数组对象（Vertex Array Object, VAO）。VAO用于存储顶点属性配置（如顶点坐标、纹理坐标、法线等）以及顶点缓冲对象（VBO）的绑定信息。简而言之，VAO记住了如何解释顶点数据。

- `glGenBuffers`：生成顶点缓冲对象（Vertex Buffer Object, VBO）或其他类型的缓冲对象。VBO主要用于存储顶点数据，如顶点坐标、颜色、纹理坐标、法线等。此外，也用于生成索引缓冲对象（Element Buffer Object, EBO）来存储索引数据。

- `glGenFramebuffers`：生成帧缓冲对象（Framebuffer Object, FBO）。FBO是一种复杂的对象，用于离屏渲染，即直接渲染到不同于默认帧缓冲的地方。它可以包含多种附件，如颜色附件、深度附件和模板附件等。

- `glGenTextures`：生成纹理对象。纹理对象用于存储纹理数据，如2D纹理、3D纹理或立方体贴图。纹理可以用于多种目的，如给物体添加细节、存储帧缓冲的输出等。

- `glGenRenderbuffers`：生成渲染缓冲对象（Renderbuffer Object, RBO）。RBO通常用于存储帧缓冲对象的深度和模板数据。与纹理不同，渲染缓冲对象通常用于不需要采样的数据，如深度测试和模板测试。