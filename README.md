# Pano-View

* `GLFW`和 `GLUT`都用于OpenGL的窗口和事件管理，但GLFW是更现代和灵活的选择。
* `GLAD`和 `GLEW`都用于管理OpenGL的扩展，但GLAD提供了更现代和灵活的生成方式，更适合处理多样化的OpenGL环境。
* `GLU`（OpenGL Utility Library）是OpenGL的一个辅助库，它提供了一些高级的图形绘制功能。随着OpenGL本身和现代图形库（如GLFW、GLEW、GLAD）的发展，很多原本需要 `GLU`来实现的功能现在可以直接用更现代的方式实现。

本套代码使用 `glut` 和固定渲染管线进行实现，不建议继续维护：
`glPushMatrix`、`glPopMatrix` 和 `glFlush` 这些函数是OpenGL旧版本（即固定功能管线）中的一部分，而在现代OpenGL（即可编程管线）中，它们的使用方式有所不同，一些任务（如矩阵管理）通常由着色器和应用程序代码直接处理。