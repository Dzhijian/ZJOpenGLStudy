//
//  main.cpp
//  04-DepthTest(深度测试)
//
//  Created by 邓志坚 on 2019/5/14.
//  Copyright © 2019 James. All rights reserved.
//

#include <stdio.h>
//demo演示了OpenGL背面剔除，深度测试，和多边形模式
#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

// 设置角色帧,作为相机
GLFrame                 viewFrame;
// 使用 GLFrustum 类来设置透视投影
GLFrustum               viewFrustum;
// 甜甜圈
GLTriangleBatch         torusBatch;
GLMatrixStack           modelViewMatix;
GLMatrixStack           projectionMatrix;
GLGeometryTransform     transformPipeline;
GLShaderManager         shaderManager;


// 标记: 背面剔除/深度测试
int iCull = 0;
int iDepth = 0;

// 窗口大小
void ChangeSize (int w, int h){
    //防止h变为0
    if(h == 0)
        h = 1;
    
    //设置视口窗口尺寸
    glViewport(0, 0, w, h);
    
    //setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
    // 设置透视模式，初始化其透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //把透视矩阵加载到透视矩阵对阵中
    //viewFrustum.GetProjectionMatrix() 获取投影矩阵
    //projectionMatrix.LoadMatrix(矩阵) 把矩阵加载到projectionMatrix中来
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

void SpecialhKeys (int key, int w, int h){
    if(key == GLUT_KEY_UP)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //重新刷新window
    glutPostRedisplay();
}

// 召唤场景
void RenderScene (void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (iCull) {
        // 实现背面剔除
        // 1.开启背面剔除功能
        glEnable(GL_CULL_FACE);
        // 2.指定模型
        glFrontFace(GL_CCW);
        
        // 3. 执行剔除
        glCullFace(GL_BACK);
        
    }else{
        glDisable(GL_CULL_FACE);
    }
    
    // 是否开启深度测试
    iDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    
    modelViewMatix.PushMatrix(viewFrame);
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    // 使用着色器,默认光源着色器
    /**
     参数一: GLT_SHADER_DEFAULT_LIGHT
     参数二: 模型视图矩阵
     参数三: 投影矩阵
     参数四: 颜色
     // transformPipeline
     */ shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);

    // 绘制
    torusBatch.Draw();
    
    // 出栈
    modelViewMatix.PopMatrix();
    
    glutSwapBuffers();
}

// context 绘制图形上下文
void SetUpRC () {
    // 设置清屏颜色 - 理解为设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    // 初始化固定管线
    shaderManager.InitializeStockShaders();
    
    // 观察者-相机📷
    // 表示 Z 轴的值
    viewFrame.MoveForward(7.0f);
    
    // 创建一个甜甜圈🍩
    /**
     参数一: 三角形批次类
     参数二: 外圆的半径,从圆心到外圆的距离
     参数三: 内圆半径,从圆心到内圆的距离
     参数四/五: 三角形带的数量, numMajor = 2 * numMinor
     */
    gltMakeTorus(torusBatch,  1.0f, 0.3f, 52, 26);
    
    // 设置点的大小
    glPointSize(4.0);
}

// 右击菜单栏,右击之后调用此处
void ProcessMenu(int value){
    switch (value) {
        case 1:
            iCull = !iCull;
            break;
        case 2:
            iDepth = !iDepth;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


int main(int argc,char* argv[])
{
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    /**
     GLUT_DOUBLE :  双缓存区
     GLUT_RGBA :  颜色缓存区
     GLUT_DEPTH : 深度缓存区
     GLUT_STENCIL : 模板缓存区
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    // 设置窗口大小
    glutInitWindowSize(800, 600);
    // 窗口名称
    glutCreateWindow("Geometry Test Program");
    
    glutReshapeFunc(ChangeSize);
    // 特殊健操作
    glutSpecialFunc(SpecialhKeys);
    // 渲染
    glutDisplayFunc(RenderScene);
    
    // 设置右击菜单栏
    glutCreateMenu(ProcessMenu);
    
    // 添加右击菜单栏选项
    // 参数一: 菜单栏中选项的名称
    // 参数二 : value 值 1
    glutAddMenuEntry("Toggle cull backFace", 1);
    glutAddMenuEntry("Toggle depth test", 2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error : %s\n",glewGetErrorString(err));
        return 1;
    }
    
    SetUpRC();
    glutMainLoop();
    return 0;
}
