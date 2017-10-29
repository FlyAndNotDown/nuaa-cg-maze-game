#pragma once

// 窗口常量
#define WINDOW_POSITION_X 100
#define WINDOW_POSITION_Y 100
#define WINDOW_SIZE_WIDTH 600
#define WINDOW_SIZE_HEIGHT 600


// 颜色
struct Color {
	GLfloat r;
	GLfloat g;
	GLfloat b;
};
// 灯光
struct Light {
	GLfloat position[4];
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
};
// 材质
struct Material {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat shininess;
};


// 地图
struct Map {
	GLint width;
	GLint height;
	GLint blocks[100][100];
};
// 地图 block 常量
#define MAP_BLOCK_EMPTY 0
#define MAP_BLOCK_CUBE 1
#define MAP_BLOCK_START 2
#define MAP_BLOCK_END 3
// 地图常量
const GLint MAP1_WIDTH = 20;
const GLint MAP1_HEIGHT = 20;
const GLint MAP1_BLOCKS[MAP1_WIDTH][MAP1_HEIGHT] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
// 地图位置
struct MapPosition {
	GLint x;
	GLint y;
};
// 地图方块长度
#define MAP_BLOCK_LENGTH 30


// 视角模式
typedef GLint ViewMode;
#define VIEW_MODE_GLOBAL 0
#define VIEW_MODE_FRIST_PERSON 1


// 玩家信息
struct Player {
	GLint x;
	GLint y;
	GLint face;
};
// 玩家脸朝向
#define PLAYER_FACE_UP 0
#define PLAYER_FACE_DOWN 1
#define PLAYER_FACE_LEFT 2
#define PLAYER_FACE_RIGHT 3
// 玩家方块大小
#define PLAYER_CUBE_SIZE MAP_BLOCK_LENGTH / 4


// 方块
struct Cube {
	// 左下角顶点的三维坐标
	GLfloat x;
	GLfloat y;
	GLfloat z;
	// 边长
	GLfloat size;
};
// 顶点
struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
// 面
struct Quad4 {
	Vertex vertexes[4];
};

// 摄像机
struct Camare {
	GLfloat position[3];
	GLfloat lookAt[3];
	GLfloat direction[3];
};
// 视角步长
#define VIEW_STEP_LENGTH_XY 40 / 81
#define VIEW_STEP_LENGTH_Z 1 / 15
// 全局摄像机参数
#define GLOBAL_CAMARE_POSITION_X 200
#define GLOBAL_CAMARE_POSITION_Y 0
#define GLOBAL_CAMARE_POSITION_Z 800
#define GLOBAL_CAMARE_LOOKAT_X 200
#define GLOBAL_CAMARE_LOOKAT_Y 100
#define GLOBAL_CAMARE_LOOKAT_Z 0
#define GLOBAL_CAMARE_DIRECTION_X 0
#define GLOBAL_CAMARE_DIRECTION_Y 0
#define GLOBAL_CAMARE_DIRECTION_Z 1

// 二维向量
struct Vector2f {
	GLfloat x;
	GLfloat y;
};

// 贴图
struct Texture {
	Mat img;
	GLint width;
	GLint height;
	// 图像指针
	GLubyte *pixels;
	// id
	GLuint id;
};