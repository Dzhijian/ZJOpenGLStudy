//
//  main.cpp
//

//
//  Created by 邓志坚 on 2019/5/29.
//  Copyright © 2019 James. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLFrustum.h"
#include "GLShaderManager.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"
#include <math.h>


#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

// 随机小球数量
#define NUM_SPHERES 50

// 着色器管理器
GLShaderManager             shaderManager;
//模型视图矩阵
GLMatrixStack               modelViewMatrix;
//投影矩阵
GLMatrixStack               projectionMatrix;
//视景体
GLFrustum                   viewFrustum;
// 几何图形变换管道
GLGeometryTransform         transformPipeline;
// 甜甜圈批处理
GLTriangleBatch             torusBatch;
// 地板批处理
GLBatch                     floorBatch;

// 2.地球公转批处理
GLTriangleBatch             sphereBatch;
// 3.角色帧 照相机角色帧（全局照相机实例）
GLFrame                     cameraFrame;
// 4.添加附加随机球
GLFrame spheres[NUM_SPHERES];

void SetupRC (){
    // 初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 设置清屏颜色到颜色缓存区
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 绘制 甜甜圈🍩
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    
    // 绘制球(公转自转)
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    
    // 往地板 floorBatch批处理中添加顶点数据
    floorBatch.Begin(GL_LINES, 324);
    
    for (GLfloat x = -20.0; x <= 20.0f; x += 0.5) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    
    floorBatch.End();
    
    // 在场景中随机位置对球体进行初始化
    // 随机放置球体
    for (int i = 0 ; i < NUM_SPHERES ; i++) {
        // y 轴不变,X, Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        // 在 Y 轴方向,将球体设置为 0.0 的位置,这使得它们看起来漂浮在眼睛的高度
        // 对 spherez 数组中的每一顶点,设置顶点数据
        spheres[i].SetOrigin(x, 0.0f, z);
    }
    
    
}

void ChangeSize(int w, int h){
    //防止h变为0
    if(h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    
    // 创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0, 100.0f);
    
    // //viewFrustum.GetProjectionMatrix()  获取viewFrustum投影矩阵
    //并将其加载到投影矩阵堆栈上
    
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 设置变换管道以使用两个矩阵堆栈（变换矩阵modelViewMatrix ，投影矩阵projectionMatrix）
    //初始化GLGeometryTransform 的实例transformPipeline.通过将它的内部指针设置为模型视图矩阵堆栈 和 投影矩阵堆栈实例，来完成初始化
    //当然这个操作也可以在SetupRC 函数中完成，但是在窗口大小改变时或者窗口创建时设置它们并没有坏处。而且这样可以一次性完成矩阵和管线的设置。
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void RenderScene(){
    
}

void SpeacialKeys(int kays ,int x, int y ){
    
}

int main (int argc, char *argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("OpenGL SphereWorld");
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpeacialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    return 0;
    
    
}
