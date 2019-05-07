//
//  main.cpp
//  03-MatrixAndShader(矩阵和着色器的使用)
//
//  Created by 邓志坚 on 2019/5/7.
//  Copyright © 2019 James. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
// 矩阵堆栈
#include "GLMatrixStack.h"
// 矩阵
#include "GLFrame.h"
// 投影矩阵
#include "GLFrustum.h"
// 三角形批次类
#include "GLBatch.h"
// 几何变化管道,用来管理矩阵堆栈
#include "GLGeometryTransform.h"

// 数据库
#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager     shaderManager;
// 模型视图矩阵堆栈
GLMatrixStack       modelViewMatrix;
// 投影视图矩阵堆栈
GLMatrixStack       projectionMatrix;
// 照相机 Frame 观察者
GLFrame             cameraFrame;
//
GLFrame             objectFrame;
// 投影矩阵
GLFrustum           viewFrustum;

// 容器类(7 中不同的图元对应 7 种容器对象)
GLBatch             pointBatch;
GLBatch             lineBatch;
GLBatch             lineStripBatch;
GLBatch             lineLoopBatch;
GLBatch             triangleBatch;
GLBatch             triangleStripBatch;
GLBatch             triangleFanBatch;

// 几何变换的管道,管理堆栈 modelViewMatrix, projectionMatrix
GLGeometryTransform transformPipeline;
M3DMatrix44f        shadowMatrix;

// 绿色
GLfloat vGreen[] = { 0.0f ,1.0f ,0.0f ,1.0f };
// 红色
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// 跟踪步骤 空格次数
int nStep = 0;

// 绘制图形顶点
void SetupRC(void) {
    
    // 定义背景颜色
    glClearColor(0.7f, 0.7f, 0.7f, 1.0);
    
    // 初始化 shaderManager
    shaderManager.InitializeStockShaders();
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 几何变换管道
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    // 往屏幕里边移
    cameraFrame.MoveForward(-15.0f);
    
    
    //定义一些点，类似佛罗里达州的形状。
    GLfloat vCoast[24][3] = {
        {2.80, 1.20, 0.0 }, {2.0,  1.20, 0.0 },
        {2.0,  1.08, 0.0 },  {2.0,  1.08, 0.0 },
        {0.0,  0.80, 0.0 },  {-.32, 0.40, 0.0 },
        {-.48, 0.2, 0.0 },   {-.40, 0.0, 0.0 },
        {-.60, -.40, 0.0 },  {-.80, -.80, 0.0 },
        {-.80, -1.4, 0.0 },  {-.40, -1.60, 0.0 },
        {0.0, -1.20, 0.0 },  { .2, -.80, 0.0 },
        {.48, -.40, 0.0 },   {.52, -.20, 0.0 },
        {.48,  .20, 0.0 },   {.80,  .40, 0.0 },
        {1.20, .80, 0.0 },   {1.60, .60, 0.0 },
        {2.0, .60, 0.0 },    {2.2, .80, 0.0 },
        {2.40, 1.0, 0.0 },   {2.80, 1.0, 0.0 }};
    
    /**
     void Begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);
     参数一: 表示使用图元模式
     参数二: 顶点数
     参数三: 纹理坐标 | 可选的
     
     // 复制顶点数据
     inline void CopyVertexData3f(GLfloat *vVerts)
     // 复制光照数据
     inline void CopyNormalDataf(GLfloat *vNorms)
     // 复制颜色数据
     inline void CopyColorData4f(GLfloat *vColors)
     
     // 结束 数据复制已经完成
     void End(void);
     */
    
    // 用点的方式表示图形
    pointBatch.Begin(GL_POINTS, 24);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();
    
    
    // 用线的方式表达图形
    lineBatch.Begin(GL_LINES, 24);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();
    
    // 用线段的方式表达图形
    lineStripBatch.Begin(GL_LINE_STRIP, 24);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();
    
    // 用线环的方式表达图形
    lineLoopBatch.Begin(GL_LINE_LOOP, 24);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();
    
    //通过三角形创建金字塔
    GLfloat vPyramid[12][3] = {
        -2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f};
    
    //GL_TRIANGLES 每3个顶点定义一个新的三角形
    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();
    
    // 三角形扇 - 六边形
    GLfloat vPoints[100][3];
    int nVerts = 0;
    
    // 半径
    GLfloat r = 3.0f;
    
    //原点(x,y,z) = (0,0,0);
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    
    //M3D_2PI 就是2Pi 的意思，就一个圆的意思。 绘制圆形
    for (GLfloat angle = 0.0f; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        //数组下标自增（每自增1次就表示一个顶点）
        nVerts++;
        /*
         弧长=半径*角度,这里的角度是弧度制,不是平时的角度制
         既然知道了cos值,那么角度=arccos,求一个反三角函数就行了
         */
        //x点坐标 cos(angle) * 半径
        vPoints[nVerts][0] = float(cos(angle)) * r;
        //y点坐标 sin(angle) * 半径
        vPoints[nVerts][1] = float(sin(angle)) * r;
        //z点的坐标
        vPoints[nVerts][2] = -0.5f;
    }
    
    //添加闭合的终点
    //课程添加演示：屏蔽177-180行代码，并把绘制节点改为7.则三角形扇形是无法闭合的。
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0;
    vPoints[nVerts][2] = 0.0f;
    
    // 6 + 2 = 8;
    // 加载！
    //GL_TRIANGLE_FAN 以一个圆心为中心呈扇形排列，共用相邻顶点的一组三角形
    triangleFanBatch.Begin(GL_TRIANGLES, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();
    
    //三角形条带，一个小环或圆柱段
    //顶点下标
//    int iCounter = 0;
    
    
    
}

// 修改窗口大小
void ChangeSize(int w, int h){
    glViewport(0, 0, w, h);
}

// 点击空格时,调用函数
// x,y: 光标的位置
void KeyPressFunc(unsigned char key, int x,int y){
    // key == 32 32的 ASC2码,是空格.
}

// 特殊键位键盘(上下左右)
void Speacialkeys(int key, int x, int y){
    
}

// 鼠标点击事件
// button : 左键(GLUT_LEFT_BUTTON)或者右键(GLUT_RIGHT_BUTTON)
// state : 按下(GLUT_UP),抬起(GLUT_DOWN)
// x,y : 光标位置
void MouseKey(int button, int state, int x, int y){
    
}

// 开始渲染
void RenderScene(void)
{
    
}


int main(int argc,char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    
    // 申请一个颜色缓存区,深度缓存区,双缓存区,模板缓存区
    // 颜色缓存区: 存储颜色值
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    // 设置 Window 的尺寸
    glutInitWindowSize(800, 600);
    
    // 创建 window 的名称
    glutCreateWindow("GL_POINTS");

    // 注册回调函数
    // 修改窗口大小
    glutReshapeFunc(ChangeSize);
    
    // 点击空格时,调用函数
    // 注册一个键盘 ASC2 码的键位事件的回调函数,当有键盘按下时,则执行回调函数
    glutKeyboardFunc(KeyPressFunc);
    
    // 特殊键位键盘(上下左右)
    glutSpecialFunc(Speacialkeys);
    
    // 注册鼠标点击事件
    glutMouseFunc(MouseKey);
    
    // 显示函数
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
        
    }
    
    //调用SetupRC,设置我们的渲染环境
    SetupRC();
    // 运行循环
    glutMainLoop();
    
    return 0;
}
