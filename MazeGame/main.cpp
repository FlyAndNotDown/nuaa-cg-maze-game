#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

// 定义颜色
Color white, gray, green;
// 定义地图
Map map;
// 定义灯光
Light sunLight;
// 定义方块的材质
Material cubeMaterial, playerCubeMaterial;
// 恭喜图案的材质
Material completeMaterial;
// 玩家
Player player;
// 摄像机
Camare fristPersonCamare, globalCamare;
// 起点和终点
MapPosition startPosition, endPosition;
// 视图模式
ViewMode viewMode;
// 鼠标初始位置
Vector2f mousePosition;
// 玩家人物方块
Cube playerCube;
// 墙壁贴图
Texture wall, complete;

// xyz 存入顶点结构体
void xyzToVertex(Vertex &vertex, float x, float y, float z) {
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
}

// 拷贝顶点结构体
void vertexToVertex(Vertex &det, Vertex src) {
	det.x = src.x;
	det.y = src.y;
	det.z = src.z;
}

// 四个顶点存入面结构体
void vertexesToQuad(Quad4 &quad, Vertex vertex0, Vertex vertex1, Vertex vertex2, Vertex vertex3) {
	vertexToVertex(quad.vertexes[0], vertex0);
	vertexToVertex(quad.vertexes[1], vertex1);
	vertexToVertex(quad.vertexes[2], vertex2);
	vertexToVertex(quad.vertexes[3], vertex3);
}

// 设置摄像机位置
void setCamarePosition(Camare &camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.position[0] = x;
	camare.position[1] = y;
	camare.position[2] = z;
}

// 设置摄像机朝向
void setCamareLookAt(Camare &camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.lookAt[0] = x;
	camare.lookAt[1] = y;
	camare.lookAt[2] = z;
}

// 设置摄像机旋转
void setCamareDirection(Camare &camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.direction[0] = x;
	camare.direction[1] = y;
	camare.direction[2] = z;
}

// 根据既有信息修改第一人称摄像机
void setFristPersonCamareByPlayer() {
	// 更新摄像机位置
	setCamarePosition(
		fristPersonCamare,
		player.y * MAP_BLOCK_LENGTH + MAP_BLOCK_LENGTH / 2,
		map.height * MAP_BLOCK_LENGTH - player.x * MAP_BLOCK_LENGTH - MAP_BLOCK_LENGTH / 2,
		MAP_BLOCK_LENGTH / 2
	);
	// 根据玩家脸部朝向来更新摄像机朝向
	switch (player.face) {
		case PLAYER_FACE_UP:
			setCamareLookAt(
				fristPersonCamare,
				fristPersonCamare.position[0],
				fristPersonCamare.position[1] + MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
				fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
			);
			break;
		case PLAYER_FACE_DOWN:
			setCamareLookAt(
				fristPersonCamare,
				fristPersonCamare.position[0],
				fristPersonCamare.position[1] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
				fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
			);
			break;
		case PLAYER_FACE_LEFT:
			setCamareLookAt(
				fristPersonCamare,
				fristPersonCamare.position[0] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
				fristPersonCamare.position[1],
				fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
			);
			break;
		case PLAYER_FACE_RIGHT:
			setCamareLookAt(
				fristPersonCamare,
				fristPersonCamare.position[0] + MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
				fristPersonCamare.position[1],
				fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
			);
			break;
	}
	// 设置摄像机上方正方向
	setCamareDirection(fristPersonCamare, 0, 0, 1);
}

// 将图像数据转换成纹理贴图信息
void loadTexture(Texture &texture) {
	// 将 textureId 设置为 2d 纹理信息
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	//纹理放大缩小使用线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 将图像内存用作纹理信息
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture.img.data);
}

// 初始化
void init() {
	// 白色
	white.r = 1.0f;
	white.g = 1.0f;
	white.b = 1.0f;
	// 灰色
	gray.r = 0.3f;
	gray.g = 0.3f;
	gray.b = 0.3f;
	// 绿色
	green.r = 0.4f;
	green.g = 1.0f;
	green.b = 0.6f;

	// 加载第二张地图
	map.width = MAP2_WIDTH;
	map.height = MAP2_HEIGHT;
	for (int i = 0; i < map.width; i++)
		for (int j = 0; j < map.height; j++)
			map.blocks[i][j] = MAP2_BLOCKS[i][j];

	// 设置阳光参数
	sunLight.position[0] = 0.0f;
	sunLight.position[1] = 0.0f;
	sunLight.position[2] = 0.0f;
	sunLight.position[3] = 1.0f;
	sunLight.ambient[0] = 0.0f;
	sunLight.ambient[1] = 0.0f;
	sunLight.ambient[2] = 0.0f;
	sunLight.ambient[3] = 1.0f;
	sunLight.diffuse[0] = 1.0f;
	sunLight.diffuse[1] = 1.0f;
	sunLight.diffuse[2] = 1.0f;
	sunLight.diffuse[3] = 1.0f;
	sunLight.specular[0] = 1.0f;
	sunLight.specular[1] = 1.0f;
	sunLight.specular[2] = 1.0f;
	sunLight.specular[3] = 1.0f;

	// 设置方块的材质
	cubeMaterial.ambient[0] = 0.0f;
	cubeMaterial.ambient[1] = 0.0f;
	cubeMaterial.ambient[2] = 0.5f;
	cubeMaterial.ambient[3] = 1.0f;
	cubeMaterial.diffuse[0] = 0.0f;
	cubeMaterial.diffuse[1] = 0.0f;
	cubeMaterial.diffuse[2] = 0.5f;
	cubeMaterial.diffuse[3] = 1.0f;
	cubeMaterial.specular[0] = 0.0f;
	cubeMaterial.specular[1] = 0.0f;
	cubeMaterial.specular[2] = 0.8f;
	cubeMaterial.specular[3] = 1.0f;
	cubeMaterial.emission[0] = white.r;
	cubeMaterial.emission[1] = white.g;
	cubeMaterial.emission[2] = white.b;
	cubeMaterial.emission[3] = 0;
	cubeMaterial.shininess = 20;
	playerCubeMaterial.ambient[0] = 0.0f;
	playerCubeMaterial.ambient[1] = 0.0f;
	playerCubeMaterial.ambient[2] = 0.5f;
	playerCubeMaterial.ambient[3] = 1.0f;
	playerCubeMaterial.diffuse[0] = 0.0f;
	playerCubeMaterial.diffuse[1] = 0.0f;
	playerCubeMaterial.diffuse[2] = 0.5f;
	playerCubeMaterial.diffuse[3] = 1.0f;
	playerCubeMaterial.specular[0] = 0.0f;
	playerCubeMaterial.specular[1] = 0.0f;
	playerCubeMaterial.specular[2] = 0.8f;
	playerCubeMaterial.specular[3] = 1.0f;
	playerCubeMaterial.emission[0] = green.r;
	playerCubeMaterial.emission[1] = green.g;
	playerCubeMaterial.emission[2] = green.b;
	playerCubeMaterial.emission[3] = 0;
	playerCubeMaterial.shininess = 20;
	// 完成图案材质
	completeMaterial.ambient[0] = 0.0f;
	completeMaterial.ambient[1] = 0.0f;
	completeMaterial.ambient[2] = 0.5f;
	completeMaterial.ambient[3] = 1.0f;
	completeMaterial.diffuse[0] = 0.0f;
	completeMaterial.diffuse[1] = 0.0f;
	completeMaterial.diffuse[2] = 0.5f;
	completeMaterial.diffuse[3] = 1.0f;
	completeMaterial.specular[0] = 0.0f;
	completeMaterial.specular[1] = 0.0f;
	completeMaterial.specular[2] = 0.8f;
	completeMaterial.specular[3] = 1.0f;
	completeMaterial.emission[0] = white.r;
	completeMaterial.emission[1] = white.g;
	completeMaterial.emission[2] = white.b;
	completeMaterial.emission[3] = 0;
	completeMaterial.shininess = 20;

	// 初始化贴图
	// 载入墙面贴图图片
	wall.img = imread(".\\texture.bmp");
	// 设置长宽
	wall.width = wall.img.cols;
	wall.height = wall.img.rows;
	// 载入通关图片
	complete.img = imread(".\\complete.jpg");
	complete.width = complete.img.cols;
	complete.height = complete.img.rows;

	// 寻找起点和终点
	for (int i = 0; i < map.width; i++) {
		for (int j = 0; j < map.height; j++) {
			if (map.blocks[i][j] == MAP_BLOCK_START) {
				startPosition.x = i;
				startPosition.y = j;
			}

			if (map.blocks[i][j] == MAP_BLOCK_END) {
				endPosition.x = i;
				endPosition.y = j;
			}
		}
	}

	// 设置玩家位置为起点
	player.x = startPosition.x;
	player.y = startPosition.y;
	// 设置玩家脸部朝向向上
	player.face = PLAYER_FACE_UP;
	
	// 设置视图模式
	viewMode = VIEW_MODE_FRIST_PERSON;

	// 初始化全局摄像机
	setCamarePosition(globalCamare, GLOBAL_CAMARE_POSITION_X, GLOBAL_CAMARE_POSITION_Y, GLOBAL_CAMARE_POSITION_Z);
	setCamareLookAt(globalCamare, GLOBAL_CAMARE_LOOKAT_X, GLOBAL_CAMARE_LOOKAT_Y, GLOBAL_CAMARE_LOOKAT_Z);
	setCamareDirection(globalCamare, GLOBAL_CAMARE_DIRECTION_X, GLOBAL_CAMARE_DIRECTION_Y, GLOBAL_CAMARE_DIRECTION_Z);

	// 初始化第一人称摄像机
	setFristPersonCamareByPlayer();

	// 初始化鼠标位置
	mousePosition.x = 0.0f;
	mousePosition.y = 0.0f;
}

// 绘制方块
void drawCube(Cube cube, bool isPlayer) {
	// 计算出方块的八个顶点坐标
	Vertex vertexes[8];
	xyzToVertex(vertexes[0], cube.x, cube.y, cube.z);
	xyzToVertex(vertexes[1], cube.x + cube.size, cube.y, cube.z);
	xyzToVertex(vertexes[2], cube.x + cube.size, cube.y + cube.size, cube.z);
	xyzToVertex(vertexes[3], cube.x, cube.y + cube.size, cube.z);
	xyzToVertex(vertexes[4], cube.x, cube.y, cube.z + cube.size);
	xyzToVertex(vertexes[5], cube.x + cube.size, cube.y, cube.z + cube.size);
	xyzToVertex(vertexes[6], cube.x + cube.size, cube.y + cube.size, cube.z + cube.size);
	xyzToVertex(vertexes[7], cube.x, cube.y + cube.size, cube.z + cube.size);

	// 计算出方块的六个面
	Quad4 quads[6];
	vertexesToQuad(quads[0], vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
	vertexesToQuad(quads[1], vertexes[0], vertexes[1], vertexes[5], vertexes[4]);
	vertexesToQuad(quads[2], vertexes[2], vertexes[3], vertexes[7], vertexes[6]);
	vertexesToQuad(quads[3], vertexes[1], vertexes[2], vertexes[6], vertexes[5]);
	vertexesToQuad(quads[4], vertexes[0], vertexes[3], vertexes[7], vertexes[4]);
	vertexesToQuad(quads[5], vertexes[4], vertexes[5], vertexes[6], vertexes[7]);

	//// 使用颜色
	//glColor3f(cubeColor.r, cubeColor.g, cubeColor.b);

	//// 使用材质
	//glMaterialfv(GL_FRONT, GL_AMBIENT, material.ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, material.diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, material.specular);
	//glMaterialfv(GL_FRONT, GL_EMISSION, material.emission);
	//glMaterialfv(GL_FRONT, GL_SHININESS, &material.shininess);

	
	// 如果是玩家
	if (isPlayer) {
		// 使用绿色
		glColor3f(green.r, green.g, green.b);

		// 使用玩家材质
		glMaterialfv(GL_FRONT, GL_AMBIENT, playerCubeMaterial.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, playerCubeMaterial.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, playerCubeMaterial.specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, playerCubeMaterial.emission);
		glMaterialfv(GL_FRONT, GL_SHININESS, &playerCubeMaterial.shininess);

		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);
			for (int j = 0; j < 4; j++) {
				glVertex3f(quads[i].vertexes[j].x, quads[i].vertexes[j].y, quads[i].vertexes[j].z);
			}
			glEnd();
		}
	} else {
		// 使用白色
		glColor3f(white.r, white.g, white.b);

		// 使用墙面材质
		glMaterialfv(GL_FRONT, GL_AMBIENT, cubeMaterial.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterial.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cubeMaterial.specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, cubeMaterial.emission);
		glMaterialfv(GL_FRONT, GL_SHININESS, &cubeMaterial.shininess);

		// 使用纹理
		glEnable(GL_TEXTURE_2D);
		// 开始绘制
		for (int i = 0; i < 6; i++) {
			// 使用纹理
			glBindTexture(GL_TEXTURE_2D, wall.id);
			glBegin(GL_POLYGON);
			/* for (int j = 0; j < 4; j++) {
			glVertex3f(quads[i].vertexes[j].x, quads[i].vertexes[j].y, quads[i].vertexes[j].z);
			}*/

			// 纹理左下角
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(quads[i].vertexes[0].x, quads[i].vertexes[0].y, quads[i].vertexes[0].z);
			// 纹理右下角
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(quads[i].vertexes[1].x, quads[i].vertexes[1].y, quads[i].vertexes[1].z);
			// 纹理右上角
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(quads[i].vertexes[2].x, quads[i].vertexes[2].y, quads[i].vertexes[2].z);
			// 纹理左上角
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(quads[i].vertexes[3].x, quads[i].vertexes[3].y, quads[i].vertexes[3].z);

			glEnd();
		}
		// 关闭纹理
		glDisable(GL_TEXTURE_2D);
	}
}

// 绘制迷宫
void drawMaze(Map map) {
	Cube cube;
	cube.size = MAP_BLOCK_LENGTH;
	for (int i = 0; i < map.width; i++) {
		for (int j = 0; j < map.height; j++) {
			if (map.blocks[i][j] == MAP_BLOCK_CUBE) {
				cube.x = j * cube.size;
				cube.y = map.height * cube.size - (i + 1) * cube.size;
				cube.z = 0;
				drawCube(cube, false);
			}
		}
	}
}

// 绘制恭喜图案
void drawCongratulation() {
	// 视窗
	GLint viewPort[4];
	// modelView 矩阵
	GLdouble modelView[16];
	// projection 矩阵
	GLdouble projection[16];

	// 进行获取
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	// 用于存储转换前二维坐标的变量
	Vector2d pointOld[4];
	// 转换后坐标
	Vector3d pointNew[4];

	// 定义要展示通关图片的位置
	pointOld[0].x = WINDOW_SIZE_WIDTH / 4;
	pointOld[0].y = WINDOW_SIZE_HEIGHT / 4;
	pointOld[1].x = WINDOW_SIZE_WIDTH / 4;
	pointOld[1].y = WINDOW_SIZE_HEIGHT / 4 * 3;
	pointOld[2].x = WINDOW_SIZE_WIDTH / 4 * 3;
	pointOld[2].y = WINDOW_SIZE_HEIGHT / 4 * 3;
	pointOld[3].x = WINDOW_SIZE_WIDTH / 4 * 3;
	pointOld[3].y = WINDOW_SIZE_HEIGHT / 4;

	// 对每一个坐标进行转换
	for (int i = 0; i < 4; i++) {
		gluUnProject(pointOld[i].x, pointOld[i].y, 0.1, modelView, projection, viewPort, &pointNew[i].x, &pointNew[i].y, &pointNew[i].z);
	}

	// 使用白色
	glColor3f(white.r, white.g, white.b);
	// 使用特定材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, completeMaterial.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, completeMaterial.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, completeMaterial.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, completeMaterial.emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, &completeMaterial.shininess);
	
	// 开始绘制
	// 启用贴图
	glEnable(GL_TEXTURE_2D);
	// 绑定贴图
	glBindTexture(GL_TEXTURE_2D, complete.id);

	// 开始绘制
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(pointNew[0].x, pointNew[0].y, pointNew[0].z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(pointNew[1].x, pointNew[1].y, pointNew[1].z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(pointNew[2].x, pointNew[2].y, pointNew[2].z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(pointNew[3].x, pointNew[3].y, pointNew[3].z);
	glEnd();

	// 关闭贴图
	glDisable(GL_TEXTURE_2D);
}

// 渲染函数
void render() {
	// 清空颜色缓存和深度缓存
	glClearColor(gray.r, gray.g, gray.b, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 10000);
	// 设置模型视景矩阵
	switch (viewMode) {
		// 如果是全局视角的话
		case VIEW_MODE_GLOBAL:
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(
				globalCamare.position[0], globalCamare.position[1], globalCamare.position[2],
				globalCamare.lookAt[0], globalCamare.lookAt[1], globalCamare.lookAt[2],
				globalCamare.direction[0], globalCamare.direction[1], globalCamare.direction[2]);
			break;
		// 如果是第一人称视角的话
		case VIEW_MODE_FRIST_PERSON:
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(
				fristPersonCamare.position[0], fristPersonCamare.position[1], fristPersonCamare.position[2],
				fristPersonCamare.lookAt[0], fristPersonCamare.lookAt[1], fristPersonCamare.lookAt[2],
				fristPersonCamare.direction[0], fristPersonCamare.direction[1], fristPersonCamare.direction[2]);
			break;
	}

	loadTexture(wall);
	loadTexture(complete);

	// 设置光照
	glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular);
	// 使用 0 号光源
	glEnable(GL_LIGHT0);
	// 在之后的渲染中使用该光照
	glEnable(GL_LIGHTING);
	
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 绘制迷宫
	drawMaze(map);
	
	// 根据玩家位置来绘制玩家方块
	playerCube.x = player.y * MAP_BLOCK_LENGTH + MAP_BLOCK_LENGTH / 2 - PLAYER_CUBE_SIZE / 2;
	playerCube.y = map.height * MAP_BLOCK_LENGTH - player.x * MAP_BLOCK_LENGTH - MAP_BLOCK_LENGTH / 2 - PLAYER_CUBE_SIZE / 2;
	playerCube.z = 0;
	playerCube.size = PLAYER_CUBE_SIZE;
	drawCube(playerCube, true);

	// 如果玩家到达终点
	if (player.x == endPosition.x && player.y == endPosition.y) {
		drawCongratulation();
	}

	// 交换前后台缓存
	glutSwapBuffers();
}

// 键盘按键监听
void onSpecialKeyDown(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_F1:
			viewMode = VIEW_MODE_FRIST_PERSON;
			glutPostRedisplay();
			break;
		case GLUT_KEY_F2:
			viewMode = VIEW_MODE_GLOBAL;
			glutPostRedisplay();
			break;
	}

	// 在第一人称模式下才相应如下按键
	if (viewMode == VIEW_MODE_FRIST_PERSON) {
		switch (key) {
			// 前进
			case GLUT_KEY_UP:
				switch (player.face) {
					case PLAYER_FACE_UP:
						// 在最顶端则不可以再向上走
						if (player.x > 0) {
							// 如果前面不是 cube 则可以前进
							if (map.blocks[player.x - 1][player.y] != MAP_BLOCK_CUBE) {
								player.x -= 1;
							}
						}
						break;
					case PLAYER_FACE_DOWN:
						// 在最低端则不可以再往下走
						if (player.x < map.height - 1) {
							// 如果前面不是 cube 则可以前进
							if (map.blocks[player.x + 1][player.y] != MAP_BLOCK_CUBE) {
								player.x += 1;
							}
						}
						break;
					case PLAYER_FACE_LEFT:
						// 在最左边则不可以再往左走
						if (player.y > 0) {
							// 如果不是 cube 则可以继续前进
							if (map.blocks[player.x][player.y - 1] != MAP_BLOCK_CUBE) {
								player.y -= 1;
							}
						}
						break;
					case PLAYER_FACE_RIGHT:
						// 在最右边则不可以再往右走
						if (player.y < map.width - 1) {
							// 如果不是 cube 则可以继续前进
							if (map.blocks[player.x][player.y + 1] != MAP_BLOCK_CUBE) {
								player.y += 1;
							}
						}
						break;
				}
				break;
			// 后退
			case GLUT_KEY_DOWN:
				switch (player.face) {
					case PLAYER_FACE_UP:
						// 在最底端则不能再往后退
						if (player.x < map.height - 1) {
							// 如果后方不是 cube 则可以后退
							if (map.blocks[player.x + 1][player.y] != MAP_BLOCK_CUBE) {
								player.x += 1;
							}
						}
						break;
					case PLAYER_FACE_DOWN:
						// 在最顶端则不能再往后退
						if (player.x > 0) {
							// 如果后方不是 cube 则可以后退
							if (map.blocks[player.x - 1][player.y] != MAP_BLOCK_CUBE) {
								player.x -= 1;
							}
						}
						break;
					case PLAYER_FACE_LEFT:
						// 在最右边则不能再往后退
						if (player.y < map.width - 1) {
							// 如果后方不是 cube 则可以后退
							if (map.blocks[player.x][player.y + 1] != MAP_BLOCK_CUBE) {
								player.y += 1;
							}
						}
						break;
					case PLAYER_FACE_RIGHT:
						// 在最左边则不能再往后退
						if (player.y > 0) {
							// 如果后方不是 cube 则可以后退
							if (map.blocks[player.x][player.y - 1] != MAP_BLOCK_CUBE) {
								player.y -= 1;
							}
						}
						break;
				}
				break;
			// 左转
			case GLUT_KEY_LEFT:
				switch (player.face) {
					case PLAYER_FACE_UP:
						player.face = PLAYER_FACE_LEFT;
						break;
					case PLAYER_FACE_DOWN:
						player.face = PLAYER_FACE_RIGHT;
						break;
					case PLAYER_FACE_LEFT:
						player.face = PLAYER_FACE_DOWN;
						break;
					case PLAYER_FACE_RIGHT:
						player.face = PLAYER_FACE_UP;
						break;
				}
				break;
			// 右转
			case GLUT_KEY_RIGHT:
				switch (player.face) {
					case PLAYER_FACE_UP:
						player.face = PLAYER_FACE_RIGHT;
						break;
					case PLAYER_FACE_DOWN:
						player.face = PLAYER_FACE_LEFT;
						break;
					case PLAYER_FACE_LEFT:
						player.face = PLAYER_FACE_UP;
						break;
					case PLAYER_FACE_RIGHT:
						player.face = PLAYER_FACE_DOWN;
						break;
				}
				break;
		}

		// 位置更新后及时更新第一人称视角的摄像机
		setFristPersonCamareByPlayer();
		glutPostRedisplay();
	}
}

int main(int argc, char *argv[]) {
	init();
	
	// 初始化 glut
	glutInit(&argc, argv);

	// 设置显示方式为 RGB + 双缓存
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// 初始化窗口
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glutInitWindowSize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
	glutCreateWindow("迷宫游戏");

	// 设置渲染函数
	glutDisplayFunc(render);
	
	// 鼠标键盘响应
	glutSpecialFunc(onSpecialKeyDown);

	// 开启主循环
	glutMainLoop();

	return 0;
}