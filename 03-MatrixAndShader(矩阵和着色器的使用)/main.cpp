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
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();
    
    //三角形条带，一个小环或圆柱段
    //顶点下标
    int iCounter = 0;
    //半径
    GLfloat radius = 3.0f;
    //从0度~360度，以0.3弧度为步长
    for(GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f)
    {
        //或许圆形的顶点的X,Y
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);
        
        //绘制2个三角形（他们的x,y顶点一样，只是z点不一样）
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = -0.5;
        iCounter++;
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0.5;
        iCounter++;
    }
    
    // 关闭循环
    printf("三角形带的顶点数：%d\n",iCounter);
    //结束循环，在循环位置生成2个三角形
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = -0.5;
    iCounter++;
    
    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = 0.5;
    iCounter++;
    
    // GL_TRIANGLE_STRIP 共用一个条带（strip）上的顶点的一组三角形
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vPoints);
    triangleStripBatch.End();
    
}

// 修改窗口大小
void ChangeSize(int w, int h){
    glViewport(0, 0, w, h);
    // 创建透视投影
    viewFrustum.SetPerspective(35.0, w/h, 1.0f, 500.0f);
    // viewFrustum.GetProjectionMatrix() 获取投影矩阵
    // 把投影矩阵加载到投影堆栈projectionMatrix
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
}

// 点击空格时,调用函数
// x,y: 光标的位置
void KeyPressFunc(unsigned char key, int x,int y){
    // key == 32 32的 ASC2码,是空格.
    
    if (key == 32) {
        nStep++;
        if (nStep > 6) {
            nStep = 0;
        }
    }
    // 设置窗口名称
    switch (nStep) {
        case 0:
            glutSetWindowTitle("GL_POINTS");
            break;
        case 1:
            glutSetWindowTitle("GL_LINES");
            break;
        case 2:
            glutSetWindowTitle("GL_LINE_STEIP");
            break;
        case 3:
            glutSetWindowTitle("GL_LINE_LOOP");
            break;
        case 4:
            glutSetWindowTitle("GL_TRIANGLES");
            break;
        case 5:
            glutSetWindowTitle("GL_TRIANGLE_STRIP");
            break;
        case 6:
            glutSetWindowTitle("GL_TRIANGLE_FAN");
            break;
            
        default:
            break;
    }
    // 调用RenderScene 重新渲染
    glutPostRedisplay();
    
}

// 特殊键位键盘(上下左右)
void Speacialkeys(int key, int x, int y){
    // 通过移动世界坐标来产生物体旋转的功能.
    if (key == GLUT_KEY_UP) {
        // 围绕 X 轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        // 围绕 X 轴旋转
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        // 围绕 Y 轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        // 围绕 Y 轴旋转
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    // 重新渲染
    glutPostRedisplay();
}

// 鼠标点击事件
// button : 左键(GLUT_LEFT_BUTTON)或者右键(GLUT_RIGHT_BUTTON)
// state : 按下(GLUT_UP),抬起(GLUT_DOWN)
// x,y : 光标位置
void MouseKey(int button, int state, int x, int y){
    
}

// 深度测试
void DrawWireFramedBatch(GLBatch* pBatch)
{
    /*------------画绿色部分----------------*/
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
     --transformPipeline 变换管线（指定了2个矩阵堆栈）
     参数3：颜色值
     */
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    
    
    /*-----------边框部分-------------------*/
    /*
     glEnable(GLenum mode); 用于启用各种功能。功能由参数决定
     参数列表：http://blog.csdn.net/augusdi/article/details/23747081
     注意：glEnable() 不能写在glBegin() 和 glEnd()中间
     GL_POLYGON_OFFSET_LINE  根据函数glPolygonOffset的设置，启用线的深度偏移
     GL_LINE_SMOOTH          执行后，过虑线点的锯齿
     GL_BLEND                启用颜色混合。例如实现半透明效果
     GL_DEPTH_TEST           启用深度测试 根据坐标的远近自动隐藏被遮住的图形（材料
     
     
     glDisable(GLenum mode); 用于关闭指定的功能 功能由参数决定
     
     */
    
    //画黑色边框
    glPolygonOffset(-1.0f, -1.0f);// 偏移深度，在同一位置要绘制填充和边线，会产生z冲突，所以要偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 画反锯齿，让黑边好看些
    glEnable(GL_LINE_SMOOTH);
    //开启混合
    glEnable(GL_BLEND);
    //指定混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绘制线框几何黑色版 三种模式，实心，边框，点，可以作用在正面，背面，或者两面
    //通过调用glPolygonMode将多边形正面或者背面设为线框模式，实现线框渲染
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //设置线条宽度
    glLineWidth(2.5f);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
     --transformPipeline.GetModelViewProjectionMatrix() 获取的
     GetMatrix函数就可以获得矩阵堆栈顶部的值
     参数3：颜色值（黑色）
     */
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    //绘制
    pBatch->Draw();
    
    // 复原原本的设置
    ////通过调用glPolygonMode将多边形正面或者背面设为全部填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //关闭GL_POLYGON_OFFSET_LINE 模型
    glDisable(GL_POLYGON_OFFSET_LINE);
    //设置线条宽度
    glLineWidth(1.0f);
    //关闭混合
    glDisable(GL_BLEND);
    //关闭线条光滑功能
    glDisable(GL_LINE_SMOOTH);
    
}
// 开始渲染
void RenderScene(void)
{
    // 清除屏幕缓存区、深度缓冲区、模板缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //压栈
    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    
    //矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后简存储在堆栈的顶部
    modelViewMatrix.MultMatrix(mCamera);
    
    M3DMatrix44f mObjectFrame;
    //只要使用 GetMatrix 函数就可以获取矩阵堆栈顶部的值，这个函数可以进行2次重载。用来使用GLShaderManager 的使用。或者是获取顶部矩阵的顶点副本数据
    objectFrame.GetMatrix(mObjectFrame);
    
    //矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后简存储在堆栈的顶部
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
     --transformPipeline.GetModelViewProjectionMatrix() 获取的
     GetMatrix函数就可以获得矩阵堆栈顶部的值
     参数3：颜色值（黑色）
     */
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    switch(nStep) {
        case 0:
            //设置点的大小
            glPointSize(4.0f);
            pointBatch.Draw();
            glPointSize(1.0f);
            break;
        case 1:
            //设置线的宽度
            glLineWidth(2.0f);
            lineBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 2:
            glLineWidth(2.0f);
            lineStripBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 3:
            glLineWidth(2.0f);
            lineLoopBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 4:
            DrawWireFramedBatch(&triangleBatch);
            break;
        case 5:
            DrawWireFramedBatch(&triangleStripBatch);
            break;
        case 6:
            DrawWireFramedBatch(&triangleFanBatch);
            break;
    }
    
    //还原到以前的模型视图矩阵（单位矩阵）
    modelViewMatrix.PopMatrix();
    
    // 进行缓冲区交换
    glutSwapBuffers();
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
