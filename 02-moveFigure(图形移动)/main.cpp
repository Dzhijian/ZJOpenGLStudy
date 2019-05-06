//
//  main.cpp
//  02-MoveFigure(图形移动)
//
//  Created by 邓志坚 on 2019/5/6.
//  Copyright © 2019 James. All rights reserved.
//
#include <stdio.h>
#include "GLShaderManager.h"

#include "GLTools.h"

#include <glut/glut.h>
// 简单的批次容器,是GLTool的一个简单容器
GLBatch triangleBatch;
// 定义一个,着色管理器
GLShaderManager shaderManager;

// blockSize 1/2 边长
GLfloat blockSize = 0.1f;

// 四边形的四个顶点
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0f
};

// X 轴上移动的距离
GLfloat xPos = 0.0f;
// Y 轴上移动的距离
GLfloat yPos = 0.0f;

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
void ChangeSize(int w,int h)

{
    
    glViewport(0,0, w, h);
    
}

// 移动方法一
void SpeacialkeysFunOne(int key,int x, int y){
    // 步长
    GLfloat stepSize = 0.025f;
    // 相对点
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[10];
    
    // 上移
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    
    // 下移
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    
    // 左移
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    
    // 右移
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }
    
    //触碰到边界（4个边界）的处理
    //当正方形移动超过最左边的时候
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    //当正方形移动到最右边时
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (blockX > (1.0 - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    
    //当正方形移动到最下面时
    //-1.0 - blockSize * 2 = Y（负轴边界） - 正方形边长 = 最下面点的位置
    if (blockY < -1.0f + blockSize * 2 ) {
        
        blockY = -1.0f + blockSize * 2;
    }
    //当正方形移动到最上面时
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }
    // 计算参考点移动,借助参考点,帮助实现 4 个顶点移动
    // A点
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;
    
    // B 点
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;
    
    // C 点
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;
    
    // D 点
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    
    triangleBatch.CopyVertexData3f(vVerts);
    
    glutPostRedisplay();
}
// 操作键盘移动
void Speacialkeys(int key,int x, int y){
    SpeacialkeysFunOne(key, x, y);
}
//为程序作一次性的设置
void SetupRC()

{
    
    //设置背影颜色
    glClearColor(0.3f,0.7f,1.0f,1.0f);
    
    //没有着色器，在OpenGL 核心框架中是无法进行任何渲染的。初始化一个渲染管理器。
    //在前面的课程，我们会采用固管线渲染，后面会学着用OpenGL着色语言来写着色器
    shaderManager.InitializeStockShaders();

    
    //修改为GL_TRIANGLE_FAN ，4个顶点
    triangleBatch.Begin(GL_TRIANGLE_FAN,4);
    
    triangleBatch.CopyVertexData3f(vVerts);
    
    triangleBatch.End();
    
}

//开始渲染
void RenderScene(void)

{
    
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

int main(int argc,char* argv[])

{
    
    //设置当前工作目录，针对MAC OS X
    /*
     `GLTools`函数`glSetWorkingDrectory`用来设置当前工作目录。实际上在Windows中是不必要的，因为工作目录默认就是与程序可执行执行程序相同的目录。但是在Mac OS X中，这个程序将当前工作文件夹改为应用程序捆绑包中的`/Resource`文件夹。`GLUT`的优先设定自动进行了这个中设置，但是这样中方法更加安全。
     */
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库,这个函数只是传说命令参数并且初始化glut库
    glutInit(&argc, argv);
    
    /*
     初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区
     
     --GLUT_DOUBLE`：双缓存窗口，是指绘图命令实际上是离屏缓存区执行的，然后迅速转换成窗口视图，这种方式，经常用来生成动画效果；
     --GLUT_DEPTH`：标志将一个深度缓存区分配为显示的一部分，因此我们能够执行深度测试；
     --GLUT_STENCIL`：确保我们也会有一个可用的模板缓存区。
     深度、模板测试后面会细致讲到
     */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    glutInitWindowSize(800,600);
    
    glutCreateWindow("Triangle");
    
    
    /*
     GLUT 内部运行一个本地消息循环，拦截适当的消息。然后调用我们不同时间注册的回调函数。我们一共注册2个回调函数：
     1）为窗口改变大小而设置的一个回调函数
     2）包含OpenGL 渲染的回调函数
     */
    //注册重塑函数
    glutReshapeFunc(ChangeSize);
    
    //注册显示函数
    glutDisplayFunc(RenderScene);
    
    //注册特殊函数
    glutSpecialFunc(Speacialkeys);
    
    //驱动程序的初始化中没有出现任何问题。
    /*
     初始化一个GLEW库,确保OpenGL API对程序完全可用。
     在试图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题
     */
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        
        return 1;
        
    }
    
    //调用SetupRC,设置我们的渲染环境
    SetupRC();
    
    glutMainLoop();
    
    return 0;
    
}
