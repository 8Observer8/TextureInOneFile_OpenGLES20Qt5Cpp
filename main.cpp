// Add this line to .pro:
// win32: LIBS += -lopengl32

#ifdef _WIN32
#include <windows.h>
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

#include <QtWidgets/QApplication>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QMatrix4x4>

class Widget : public QOpenGLWidget {
    Q_OBJECT
public:
    Widget() : m_texture(QOpenGLTexture::Target2D) {
        setWindowTitle("OpenGL ES 2.0. Qt C++");
        resize(400, 400);
    }
private:
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLBuffer m_texCoordBuffer;
    QOpenGLTexture m_texture;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMatrix;
    int m_uMvpMatrixLocation;
    const float WORLD_HEIGHT = 50;

    void initializeGL() override {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        resize(400, 400);
        const char *vertShaderSrc =
                "attribute vec3 aPosition;"
                "attribute vec2 aTexCoord;"
                "uniform mat4 uMvpMatrix;"
                "varying vec2 vTexCoord;"
                "void main()"
                "{"
                "    gl_Position = uMvpMatrix * vec4(aPosition, 1.0);"
                "    vTexCoord = aTexCoord;"
                "}";
        const char *fragShaderSrc =
                "uniform sampler2D uSampler;"
                "varying vec2 vTexCoord;"
                "void main()"
                "{"
                "    gl_FragColor = texture2D(uSampler, vTexCoord);"
                "}";
        m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertShaderSrc);
        m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragShaderSrc);
        m_program.link();
        m_program.bind();
        float vertPositions[] = {
            -0.5f, -0.5f, 0.f,
            0.5f, -0.5f, 0.f,
            -0.5f, 0.5f, 0.f,
            0.5f, 0.5f, 0.f
        };
        float texCoords[] = {
            0.f, 1.f,
            1.f, 1.f,
            0.f, 0.f,
            1.f, 0.f
        };
        initVertexBuffer(m_vertPosBuffer, vertPositions, sizeof(vertPositions), "aPosition", 0, 3);
        initVertexBuffer(m_texCoordBuffer, texCoords, sizeof(texCoords), "aTexCoord", 1, 2);
        m_texture.create();
        m_texture.setData(QImage(":/Textures/WornBrownBrickwork_256.png"));
        m_texture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
        m_program.bind();
        m_uMvpMatrixLocation = m_program.uniformLocation("uMvpMatrix");
        m_modelMatrix.scale(QVector3D(50.f, 50.f, 1.f));
        m_viewMatrix.lookAt(QVector3D(0.f, 0.f, 40.f),
                            QVector3D(0.f, 0.f, 0.f),
                            QVector3D(0.f, 1.f, 0.f));
    }
    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);
        m_mvpMatrix = m_projMatrix * m_viewMatrix * m_modelMatrix;
        m_program.bind();
        m_program.setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
        m_texture.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
        float aspect = (float) w / h;
        float worldWidth = aspect * WORLD_HEIGHT;
        m_projMatrix.setToIdentity();
        m_projMatrix.ortho(-worldWidth / 2.f, worldWidth / 2.f,
                           -WORLD_HEIGHT / 2.f, WORLD_HEIGHT / 2.f, 50.f, -50.f);
    }
    void initVertexBuffer(QOpenGLBuffer &buffer, float data[], int amount,
                          const char *locationName, int locationIndex, int tupleSize) {
        buffer.create();
        buffer.bind();
        buffer.allocate(data, amount);
        m_program.bindAttributeLocation(locationName, locationIndex);
        m_program.setAttributeBuffer(locationIndex, GL_FLOAT, 0, tupleSize);
        m_program.enableAttributeArray(locationIndex);
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
