#include "OpenGLWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtMath>

OpenGLWidget::OpenGLWidget(QWidget* parent)
	: QOpenGLWidget(parent)
	, m_shaderProgram(nullptr)
	, m_rotationAngle(0.0f)
	, m_autoRotate(true)
	, m_wireframe(false)
	, m_xRotation(0.0f)
	, m_yRotation(0.0f)
	, m_zoomFactor(1.0f)
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &OpenGLWidget::updateRotation);
	m_timer->start(16); // ~60 FPS
}

OpenGLWidget::~OpenGLWidget()
{
	makeCurrent();
	delete m_shaderProgram;
	doneCurrent();
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	setupShaders();
	setupCube();
}

void OpenGLWidget::setupShaders()
{
	m_shaderProgram = new QOpenGLShaderProgram;

	// Vertex shader
	const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;

        out vec3 vertexColor;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";

	// Fragment shader
	const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";

	m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_shaderProgram->link();
}

void OpenGLWidget::setupCube()
{
	// 重新组织顶点数据 - 每个面直接定义三角形
	float vertices[] = {
		// 前面 (红色)
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		// 后面 (绿色)
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

		// 左面 (蓝色)
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		// 右面 (黄色)
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		 // 底面 (紫色)
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		 // 顶面 (青色)
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
	};

	m_vao.create();
	m_vao.bind();

	m_vertexBuffer.create();
	m_vertexBuffer.bind();
	m_vertexBuffer.allocate(vertices, sizeof(vertices));

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	m_vao.release();
}

void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 添加安全检查
	if (!m_shaderProgram || !m_shaderProgram->isLinked()) {
		return;
	}

	m_shaderProgram->bind();

	// 设置矩阵
	m_view.setToIdentity();
	m_view.translate(0.0f, 0.0f, -3.0f * m_zoomFactor);
	m_view.rotate(m_xRotation, 1.0f, 0.0f, 0.0f);
	m_view.rotate(m_yRotation, 0.0f, 1.0f, 0.0f);

	m_model.setToIdentity();
	if (m_autoRotate) {
		m_model.rotate(m_rotationAngle, 0.5f, 1.0f, 0.0f);
	}

	m_shaderProgram->setUniformValue("projection", m_projection);
	m_shaderProgram->setUniformValue("view", m_view);
	m_shaderProgram->setUniformValue("model", m_model);

	// 添加VAO检查
	if (m_vao.isCreated()) {
		m_vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);  // 36个顶点 = 12个三角形 = 6个面
		m_vao.release();
	}

	m_shaderProgram->release();
}

void OpenGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	m_lastMousePos = event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	int dx = event->pos().x() - m_lastMousePos.x();
	int dy = event->pos().y() - m_lastMousePos.y();

	if (event->buttons() & Qt::LeftButton) {
		m_xRotation += dy * 0.5f;
		m_yRotation += dx * 0.5f;
		update();
	}

	m_lastMousePos = event->pos();
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
	float delta = event->angleDelta().y() / 120.0f;
	m_zoomFactor -= delta * 0.1f;
	m_zoomFactor = qMax(0.1f, qMin(5.0f, m_zoomFactor));
	update();
}

void OpenGLWidget::updateRotation()
{
	if (m_autoRotate) {
		m_rotationAngle += 1.0f;
		if (m_rotationAngle >= 360.0f) {
			m_rotationAngle = 0.0f;
		}
		update();
	}
}

void OpenGLWidget::toggleRotation()
{
	m_autoRotate = !m_autoRotate;
}

void OpenGLWidget::resetCamera()
{
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zoomFactor = 1.0f;
	m_rotationAngle = 0.0f;
	update();
}

void OpenGLWidget::setWireframeMode()
{
	m_wireframe = true;
	update();
}

void OpenGLWidget::setSolidMode()
{
	m_wireframe = false;
	update();
}