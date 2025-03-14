﻿/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * - 3) 绘制‘J’、‘Z’等形状的方块
 * - 4) 随机生成方块并赋上不同的颜色
 * - 5) 方块的自动向下移动
 * - 6) 方块之间、方块与边界之间的碰撞检测
 * - 7) 棋盘格中每一行填充满之后自动消除
 * - 8) R键重启游戏，重新开始
 * - 9) 积分系统 每个方块的积分=方块颜色*方块类型
 * - 10) 梯度难度设计 每消除一行，自动下落速度会加快
 * - 11) 音乐特效 在消除一行时和GameOver后右音效
 *
 */

#include "Angel.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <windows.h>
#include <bits/stdc++.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
int starttime; // 控制方块向下移动时间
int rotation = 0; // 控制当前窗口中的方块旋转
glm::vec2 tile[4]; // 表示当前窗口中的方块
bool gameover = false; // 游戏结束控制变量
int xsize = 400; // 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;
int gameCredits = 0;//@ANSWER 积分
// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num = (board_width + 1) + (board_height + 1);


//@ANSWER 一个数组表示所有可能出现的方块和方向。
glm::vec2 allRotationsLshape[7][4][4] = {
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}, //   "O"
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}, //
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}, //
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(-2, 0)}, //   "I"
        {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(0, -2)}, //
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(-2, 0)}, //
        {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(0, -2)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}, //   "S"
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, -1), glm::vec2(0, 1)}, //
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}, //
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, -1), glm::vec2(0, 1)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)}, //   "Z"
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, -1)}, //
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)}, //
        {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, -1)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(-1, -1)}, //   "L"
        {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, -1)}, //
        {glm::vec2(1, 1), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0)}, //
        {glm::vec2(-1, 1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(1, -1)}, //   "J"
        {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 1)}, //
        {glm::vec2(-1, 1), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0)}, //
        {glm::vec2(-1, -1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}
    },
    {
        {glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(0, -1)}, //   "T"
        {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0)}, //
        {glm::vec2(0, 1), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0)}, //
        {glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}
    },
};
//@ANSWER 随机生成方块的种类和颜色
int RandomShape, RandomColor;
// @ANSWER 自定义自动下落时间，初始1s，每消除一行减小0.01s
float FallingPace = 1;
// 绘制窗口的颜色变量
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);
glm::vec4 white = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 black = glm::vec4(0.0, 0.0, 0.0, 1.0);
// @ANSWER:更多颜色，用于生成方块
glm::vec4 colors[8] = {
    glm::vec4(1.0, 0.0, 0.0, 1.0), // 红色
    glm::vec4(0.0, 1.0, 0.0, 1.0), // 绿色
    glm::vec4(0.0, 0.0, 1.0, 1.0), // 蓝色
    glm::vec4(1.0, 1.0, 0.0, 1.0), // 黄色
    glm::vec4(0.0, 1.0, 1.0, 1.0), // 青色
    glm::vec4(1.0, 0.0, 1.0, 1.0), // 品红
    glm::vec4(0.5, 0.0, 0.5, 1.0), // 紫色
    glm::vec4(1.0, 0.75, 0.8, 1.0) // 粉红色
};
// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour) {
    // 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
    for (int i = 0; i < 6; i++)
        board_colours[(int) (6 * (board_width * pos.y + pos.x) + i)] = colour;

    glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

    // 计算偏移量，在适当的位置赋上颜色
    int offset = 6 * sizeof(glm::vec4) * (int) (board_width * pos.y + pos.x);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

    // 每个方块包含四个格子
    for (int i = 0; i < 4; i++) {
        // 计算格子的坐标值
        GLfloat x = tilepos.x + tile[i].x;
        GLfloat y = tilepos.y + tile[i].y;

        glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
        glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width * 2 + (y * tile_width), .4, 1);
        glm::vec4 p3 = glm::vec4(tile_width * 2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
        glm::vec4 p4 = glm::vec4(tile_width * 2 + (x * tile_width), tile_width * 2 + (y * tile_width), .4, 1);

        // 每个格子包含两个三角形，所以有6个顶点坐标
        glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
        glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(glm::vec4), 6 * sizeof(glm::vec4), newpoints);
    }
    glBindVertexArray(0);
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos) {
    // @ANSWER 判断是否与其他块重合 & 是否在边界内
    if ((cellpos.x >= 0) && (cellpos.x < board_width) && (cellpos.y >= 0) && (cellpos.y < board_height)
        && board[int(cellpos.x)][int(cellpos.y)] == false)
        return true;
    else
        return false;
}

// @ANSWER 结束提示语
void GameOver() {
    std::cout << "GAME OVER!" << std::endl;
    switch (RandomShape) {
        case 0:
            printf("Sorry...Next one is 'O'!\n");
            break;
        case 1:
            printf("Sorry...Next one is 'I'!\n");
            break;
        case 2:
            printf("Sorry...Next one is 'S'!\n");
        break;
        case 3:
            printf("Sorry...Next one is 'Z'!\n");
        break;
        case 4:
            printf("Sorry...Next one is 'L'!\n");
        break;
        case 5:
            printf("Sorry...Next one is 'J'!\n");
        break;
        case 6:
            printf("Sorry...Next one is 'T'!\n");
    }
    // @ANSWER GameOver音效 播放WAV文件（循环播放）
    PlaySound(TEXT("GameOver.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    Sleep(6000);  // 6 秒
    PlaySound(NULL, NULL, 0);
    std::cout<<"Your total credits:"<<gameCredits<<std::endl;
    std::cout <<"Press R to restart!"<< std::endl;
}


// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// @ANSWER 在游戏结束的时候判断，没有足够的空间来生成新的方块。
void newtile() {
    // @ANSWER 使用随机数来随机生成方块
    RandomShape = rand() % 7;
    // 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向
    tilepos = glm::vec2(5, 19);
    rotation = 0;
    for (int i = 0; i < 4; i++)
        tile[i] = allRotationsLshape[RandomShape][0][i];
    // @ANSWER 检查没有足够的空间来生成新的方块，没有则gameover
    glm::vec2 newpos[4];
    for (int i = 0; i < 4; i++) {
        newpos[i] = tile[i] + tilepos;
    }
    if (!(checkvalid(newpos[0]) && checkvalid(newpos[1]) && checkvalid(newpos[2]) && checkvalid(newpos[3]))) {
        gameover = true;
        GameOver();
        return;
    }
    updatetile();
    RandomColor = rand() % 8;
    // 给新方块赋上颜色
    glm::vec4 newcolours[24];
    for (int i = 0; i < 24; i++)
        newcolours[i] = colors[RandomColor];

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// 游戏和OpenGL初始化
void init() {
    // 初始化棋盘格，这里用画直线的方法绘制网格
    // 包含竖线 board_width+1 条
    // 包含横线 board_height+1 条
    // 一条线2个顶点坐标，并且每个顶点一个颜色值

    glm::vec4 gridpoints[board_line_num * 2];
    glm::vec4 gridcolours[board_line_num * 2];

    // 绘制网格线
    // 纵向线
    for (int i = 0; i < (board_width + 1); i++) {
        gridpoints[2 * i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
        gridpoints[2 * i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height + 1) * tile_width, 0, 1);
    }

    // 水平线
    for (int i = 0; i < (board_height + 1); i++) {
        gridpoints[2 * (board_width + 1) + 2 * i] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
        gridpoints[2 * (board_width + 1) + 2 * i + 1] = glm::vec4((board_width + 1) * tile_width,
                                                                  (tile_width + (tile_width * i)), 0, 1);
    }

    // 将所有线赋成白色
    for (int i = 0; i < (board_line_num * 2); i++)
        gridcolours[i] = white;

    // 初始化棋盘格，并将没有被填充的格子设置成黑色
    glm::vec4 boardpoints[points_num];
    for (int i = 0; i < points_num; i++)
        board_colours[i] = black;

    // 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
    for (int i = 0; i < board_height; i++)
        for (int j = 0; j < board_width; j++) {
            glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
            glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width * 2 + (i * tile_width), .5, 1);
            glm::vec4 p3 = glm::vec4(tile_width * 2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
            glm::vec4 p4 = glm::vec4(tile_width * 2 + (j * tile_width), tile_width * 2 + (i * tile_width), .5, 1);
            boardpoints[6 * (board_width * i + j) + 0] = p1;
            boardpoints[6 * (board_width * i + j) + 1] = p2;
            boardpoints[6 * (board_width * i + j) + 2] = p3;
            boardpoints[6 * (board_width * i + j) + 3] = p2;
            boardpoints[6 * (board_width * i + j) + 4] = p3;
            boardpoints[6 * (board_width * i + j) + 5] = p4;
        }

    // 将棋盘格所有位置的填充与否都设置为false（没有被填充）
    for (int i = 0; i < board_width; i++)
        for (int j = 0; j < board_height; j++)
            board[i][j] = false;

    // 载入着色器
    std::string vshader, fshader;
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";
    GLuint program = InitShader(vshader.c_str(), fshader.c_str());
    glUseProgram(program);

    locxsize = glGetUniformLocation(program, "xsize");
    locysize = glGetUniformLocation(program, "ysize");

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vColor = glGetAttribLocation(program, "vColor");


    glGenVertexArrays(3, &vao[0]);
    glBindVertexArray(vao[0]); // 棋盘格顶点

    glGenBuffers(2, vbo);

    // 棋盘格顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 棋盘格顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours, GL_STATIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);


    glBindVertexArray(vao[1]); // 棋盘格每个格子

    glGenBuffers(2, &vbo[2]);

    // 棋盘格每个格子顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, points_num * sizeof(glm::vec4), boardpoints, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 棋盘格每个格子顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, points_num * sizeof(glm::vec4), board_colours, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);


    glBindVertexArray(vao[2]); // 当前方块

    glGenBuffers(2, &vbo[4]);

    // 当前方块顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 当前方块顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);


    glBindVertexArray(0);

    glClearColor(0, 0, 0, 0);

    // 游戏初始化
    newtile();
    starttime = glfwGetTime();
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate() {
    // 计算得到下一个旋转方向
    int nextrotation = (rotation + 1) % 4;

    // 检查当前旋转之后的位置的有效性
    if (checkvalid((allRotationsLshape[RandomShape][nextrotation][0]) + tilepos)
        && checkvalid((allRotationsLshape[RandomShape][nextrotation][1]) + tilepos)
        && checkvalid((allRotationsLshape[RandomShape][nextrotation][2]) + tilepos)
        && checkvalid((allRotationsLshape[RandomShape][nextrotation][3]) + tilepos)) {
        // 更新旋转，将当前方块设置为旋转之后的方块
        rotation = nextrotation;
        for (int i = 0; i < 4; i++)
            tile[i] = allRotationsLshape[RandomShape][rotation][i];

        updatetile();
    }
}

// @ANSWER 获取格子的颜色
glm::vec4 getCellColour(int x, int y) {
    int index = 6 * (board_width * y + x); // 计算该格子在 board_colours 中的起始索引
    return board_colours[index]; // 获取格子的颜色，所有6个顶点颜色都是一样的
}


// @ANSWER 检查row行有没有满，若满则清除
void checkfullrow(int row) {
    bool full_row = true;
    for (int x = 0; x < board_width; ++x) {
        if (board[x][row] == false) {
            full_row = false;
            return;
        }
    }
    if (full_row) {
        std::cout<<"YES! Keep Going!"<<std::endl;
        // @ANSWER 消除音效 播放WAV文件（循环播放）
        PlaySound(TEXT("eliminate.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        Sleep(1000);  // 1 秒
        PlaySound(NULL, NULL, 0);
        // 将上面的方块下移一行
        for (int y = row; y < board_height - 1; ++y) {
            for (int x = 0; x < board_width; ++x) {
                board[x][y] = board[x][y + 1];
                changecellcolour(glm::vec2(x, y), getCellColour(x, y + 1));
            }
        }
        // 清空最顶行
        for (int x = 0; x < board_width; ++x) {
            board[x][board_height - 1] = false;
            changecellcolour(glm::vec2(x, board_height - 1), black);
        }
        FallingPace -= 0.01;
    }
}


// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile() {
    // 每个格子
    for (int i = 0; i < 4; i++) {
        // 获取格子在棋盘格上的坐标
        int x = (tile[i] + tilepos).x;
        int y = (tile[i] + tilepos).y;
        // 将格子对应在棋盘格上的位置设置为填充
        board[x][y] = true;
        // 并将相应位置的颜色修改
        changecellcolour(glm::vec2(x, y), colors[RandomColor]);
    }
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction) {
    // 计算移动之后的方块的位置坐标
    glm::vec2 newtilepos[4];
    for (int i = 0; i < 4; i++)
        newtilepos[i] = tile[i] + tilepos + direction;

    // 检查移动之后的有效性
    if (checkvalid(newtilepos[0])
        && checkvalid(newtilepos[1])
        && checkvalid(newtilepos[2])
        && checkvalid(newtilepos[3])) {
        // 有效：移动该方块
        tilepos.x = tilepos.x + direction.x;
        tilepos.y = tilepos.y + direction.y;

        updatetile();

        return true;
    }

    return false;
}

//@Answer 重新启动游戏
void restart() {
    // 初始化棋盘格，并将没有被填充的格子设置成黑色
    for (int i = 0; i < points_num; i++)
        board_colours[i] = black;

    // 将棋盘格所有位置的填充与否都设置为false（没有被填充）
    for (int i = 0; i < board_width; i++)
        for (int j = 0; j < board_height; j++) {
            board[i][j] = false;
            changecellcolour(glm::vec2(i, j), black);
        }
    gameover = false;
    gameCredits = 0;
    FallingPace = 1;
    newtile();
    starttime = glfwGetTime();
}


// @ANSWER 自动积分
void AddCredits() {
    gameCredits += (1+RandomColor)*(1+RandomShape);
}
// @ANSWER 方块自动下移
void update() {
    if (!gameover) {
        int current_time = glfwGetTime(); // 使用 GLFW 提供的计时
        if (current_time - starttime >= FallingPace) {
            // 每秒移动方块
            if (!movetile(glm::vec2(0, -1))) {
                AddCredits();//@ANSWER 积分
                settile();
                newtile();
            }
            starttime = current_time;
        }
    }
}
// 游戏渲染部分
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1i(locxsize, xsize);
    glUniform1i(locysize, ysize);

    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLES, 0, 24); // 绘制当前方块 (8 个三角形)
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINES, 0, board_line_num * 2); // 绘制棋盘格的线
}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h) {
    xsize = w;
    ysize = h;
    glViewport(0, 0, w, h);
}



// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (!gameover) {
        switch (key) {
            // 控制方块的移动方向，更改形态
            case GLFW_KEY_UP: // 向上按键旋转方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    rotate();
                    break;
                } else {
                    break;
                }
            case GLFW_KEY_DOWN: // 向下按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    if (!movetile(glm::vec2(0, -1))) {
                        AddCredits();//@ANSWER 积分
                        settile();
                        newtile();
                        break;
                    } else {
                        break;
                    }
                }
            case GLFW_KEY_LEFT: // 向左按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    movetile(glm::vec2(-1, 0));
                    break;
                } else {
                    break;
                }
            case GLFW_KEY_RIGHT: // 向右按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    movetile(glm::vec2(1, 0));
                    break;
                } else {
                    break;
                }
            // 游戏设置。
            case GLFW_KEY_ESCAPE:
                if (action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS);
                    break;
                } else {
                    break;
                }
            case GLFW_KEY_Q:
                if (action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS);
                    break;
                } else {
                    break;
                }

            case GLFW_KEY_R:
                if (action == GLFW_PRESS) {
                    restart();
                    break;
                } else {
                    break;
                }
        }
    }
    if(key == GLFW_KEY_R && action == GLFW_PRESS)
        restart();

}


int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口。
    GLFWwindow *window = glfwCreateWindow(1000, 1800, "2022150057_周楚皓_期中大作业", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    init();
    while (!glfwWindowShouldClose(window)) {
        for (int j = 0; j < board_height; j++)
            checkfullrow(j); // @ANSWER 检查是否已满
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
        update();//@ANSWER 自由下落
    }
    glfwTerminate();
    return 0;
}
