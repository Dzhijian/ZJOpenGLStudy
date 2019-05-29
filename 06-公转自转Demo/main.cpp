//
//  main.cpp
//  06-公转自转Demo
//
//  Created by 邓志坚 on 2019/5/29.
//  Copyright © 2019 James. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLFrustum.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


int main (int argc, char *argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    /**
     GLUT_DOUBLE :  双缓存区
     GLUT_RGBA :  颜色缓存区
     GLUT_DEPTH : 深度缓存区
     GLUT_STENCIL : 模板缓存区
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    // 设置窗口大小
    glutInitWindowSize(800, 600);
    // 窗口名称
    glutCreateWindow("Smoothing Out The Jaggies");
    
//    glutReshapeFunc(ChangeSize);
//    // 渲染
//    glutDisplayFunc(RenderScene);
//
//    // 设置右击菜单栏
//    glutCreateMenu(ProcessMenu);
    
    // 添加右击菜单栏选项
    // 参数一: 菜单栏中选项的名称
    // 参数二 : value 值 1
//    glutAddMenuEntry("Antialiased Rendering", 1);
//    glutAddMenuEntry("Normal Rendering", 2);
//    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error : %s\n",glewGetErrorString(err));
        return 1;
    }
    
//    SetUpRC();
    glutMainLoop();
    return 0;
}
