//
//  main.cpp
//  07-PyramidTexture(金字塔纹理)
//
//  Created by 邓志坚 on 2019/6/28.
//  Copyright © 2019 James. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager                 shaderManager;          // 着色器管理器
GLMatrixStack                   modelViewMatrix;        // 模型视图矩阵
GLMatrixStack                   projectionMatrix;       // 投影矩阵
GLFrame                         cameraFrame;            // 观察者 照相机
GLFrame                         objectFrame;            // 
GLFrustum                       viewFrustum;             // 透视投影

GLBatch                         pyramidBatch;            // 金字塔
GLuint                          textureID;               // 纹理变量,一般使用无符号整型

GLGeometryTransform             transformPipeline;
M3DMatrix44f                    shadowMatrix;


// 绘制金子塔
void MakePyramid (GLBatch &pyramidBatch) {
    /**
     1.pyramindBatch
     6 * 3 = 18;
     通过三角形批次类 pyramidBatch 来组件图形
     参数一: 类型
     参数二: 顶点数
     参数三: 需要使用到纹理,默认为 0 ,可以不写这个参数
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18 ,1);
    
    /**
     2.金子塔底部
     底部四边形 = 三角形X + 三角形 Y
     */
    // 三角形 X
    
    // ----------vBlackLeft------------
    // 设置法线坐标 - 光照
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    // 设置顶点对应的纹理坐标
    // MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t)
    // 参数一: 纹理层数, 参数二: s坐标, 参数三:t坐标
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    // 设置顶点坐标
    pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    
    //------vBlackRight------------
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3f(1.0f, -1.0f, -1.0f);
    
    // vFrontRight 点
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
    // 三角形 Y
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3f(-1.0f, -1.0f, 1.0f);
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
    // 塔顶
    M3DVector3f vApex = {0.0f,1.0f,0.0f};
    M3DVector3f vFrontLeft = {-1.0f,-1.0f,1.0f};
    M3DVector3f vFrontRight = {1.0f,-1.0f,1.0f};
    M3DVector3f vBackLeft = {-1.0f,-1.0f,-1.0f};
    M3DVector3f vBackRight = {1.0f,-1.0f,-1.0f};
    
    // 目的:为了临时存储法线向量
    M3DVector3f n;
    
    // 找法线
    // m3dFindNormal(float *result, const float *point1, const float *point2, const float *point3)
    // 参数一: 结果, 参数 2-4: 3 个顶点
    m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    
    //vApex
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    //vFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //vFrongRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    //金字塔的左边
    //三角形:(Apex,vBackLeft,vFrontLeft)
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    
    //vApex
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    //vBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //金字塔右边
    //三角形：（vApex, vFrontRight, vBackRight）
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //金字塔后边
    //三角形：（vApex, vBackRight, vBackLeft）
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //结束批次设置
    pyramidBatch.End();
}

bool LoadTGATexture(const char *szFileName, GLenum minFilter,GLenum magFilter, GLenum wrapMode){
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    // 1.读取纹理的像素
    /**
     
     gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat  )
     参数一: 纹理文件的名称
     参数二: 文件的宽度地址
     参数三: 文件的高度地址
     参数四: 文件的组件地址
     参数五: 文件格式地址
     返回值:pBits ,指向图像数据的指针
     */
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);

    if (pBits == NULL) {
        printf("读取纹理数据失败\n");
        return false;
    }
    
    // 2.设置纹理参数
    /**
     设置 s,t 的环绕方式
     glTexParameteri(GLenum target, GLenum pname, GLint param)
     参数一: 纹理维度
     参数二: 为S/T坐标设置环绕模式
     参数三: wrapMode,环绕模式
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //设置放大\缩小的过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    // 3.精密包装像素数据
    /**
     glPixelStorei(GLenum pname, GLint param)
     参数一: GL_UNPACK_ALIGNMENT ,OpenGL 如何从数据缓存区中解包图像数据
     参数二: 设置GL_UNPACK_ALIGNMENT的值
     */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // 4.载入纹理
    /**
     glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
     参数一: 纹理的维度
     参数二: mip贴图层次
     参数三: 加载纹理颜色成分(从读取像素图获取的)
     参数四: 加载纹理的宽
     参数五: 加载纹理的高
     参数六: 纹理的边框
     参数七: 像素数据的类型
     参数八: 指向纹理数据的指针
     */
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    
    // 使用完释放
    free(pBits);
    
    //只有minFilter 等于以下四种模式，才可以生成Mip贴图
    //GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    //GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    //GL_LINEAR_MIPMAP_LINEAR 和GL_NEAREST_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除他们之间的过滤痕迹。
    //GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图。纹理过滤的黄金准则，具有最高的精度。
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        
        //加载Mip,纹理生成所有的Mip层
        //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
        glGenerateMipmap(GL_TEXTURE_2D);
    
    
    return true;
}

void SetupRC (void) {
    glClearColor(0.7, 0.7, 0.7, 1.0);
    shaderManager.InitializeStockShaders();
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 1.纹理对象的处理
    /**
     glGenTextures(GLsizei n, GLuint *textures)
     参数一: 纹理对象的个数
     参数二: 纹理对象的指针
     */
    glGenTextures(1, &textureID);
    
    // 2.绑定纹理对象
    /**
     glBindTexture(GLenum target, GLuint texture)
     参数一:纹理的状态,GL_TEXTURE_1D\GL_TEXTURE_2D\GL_TEXTURE_3D
     */
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    
    // 3.将 TGA 文件加载成 2D 纹理
    // 函数是开发者自己设计的
    // 参数一: 纹理文件名称
    // 参数二: 缩小时过滤器使用的哪一种
    // 参数三: 放大小时过滤器使用的哪一种
    // 参数四: 纹理坐标环绕方式
    LoadTGATexture("brick.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    //4.设置渲染图像的顶点--金字塔
    MakePyramid(pyramidBatch);
    
    cameraFrame.MoveForward(-10.0);
    
    
}

// 清除 删除纹理对象
void ShutdownRC (void) {
    glDeleteTextures(1, &textureID);
}


void ChangeSize (int w, int h){
    if ( h == 0) { h = 1;}
    
    glViewport(0, 0, w, h);
    // 设置透视投影
    viewFrustum.SetPerspective(35.0, float(w)/float(h), 1.0f, 500.0f);
    
    // 将投影矩阵加载到projectionMatrix
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
}
void SpecialKeys (int key ,int x, int y){
    switch (key) {
        case GLUT_KEY_UP:
            objectFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_DOWN:
            objectFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_LEFT:
            objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            objectFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void RenderScene (void){
    // 光照位置
    static GLfloat vLightPos[] = {1.0f, 1.0f, 0.0f};
    // 关照颜色
    static GLfloat vWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    modelViewMatrix.PushMatrix();
    
    //观察者
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    
    //
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);

    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // 点光源着色器
    /**
     参数一:GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
     参数二:模型视图矩阵
     参数三:投影矩阵
     参数四:光源的位置
     参数五:光的颜色
     参数六:图形颜色(如使用了纹理,则设置为0)
     */
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vWhite,0);
    
    pyramidBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();

}


int main(int argc, char *argv[]){
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pyramid Texture 金字塔 纹理 学习");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum error = glewInit();
    if (GLEW_OK != error) {
        fprintf(stderr, "GLEW Error: %s\n",glewGetErrorString(error));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    
    ShutdownRC();
    
    return 0;
}
