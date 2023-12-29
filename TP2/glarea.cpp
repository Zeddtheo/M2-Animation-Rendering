// Basé sur :
// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <iostream>
#include <ostream>
#include <random>

GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);

    setEnabled(true);                   // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus);    // accepte focus
    setFocus();                         // donne le focus

    timer = new QTimer(this);
    timer->setInterval(50);           // msec
    connect (timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
    elapsedTimer.start();

    Obstacle o1 = Obstacle(QVector3D(15,0,-5),5.0f);
    Obstacle o2 = Obstacle(QVector3D(-15,0,0),5.0f);
    Obstacle o3 = Obstacle(QVector3D(0,-15,5),5.0f);

    obstacles.push_back(o1);
    obstacles.push_back(o2);
    obstacles.push_back(o3);

    Predateur p1 = Predateur(QVector3D(-1,0,0), 1.0f, QVector3D(0.5f,0.5f,0.5f), 1.0f);
    Predateur p2 = Predateur(QVector3D(1,0,0), 1.0f, QVector3D(0.5f,0.5f,0.5f), 1.0f);
    predateurs.push_back(p1);
    predateurs.push_back(p2);

    //poisson = Poisson(QVector3D(0,0,0),1,QVector3D(0,1,0),1);

    banc = Banc(100, largeurAquarium, hauteurAquarium, profondeurAquarium, obstacles, predateurs);
}


GLArea::~GLArea()
{
    delete timer;

    makeCurrent();
    tearGLObjects();
    doneCurrent();
}


void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.5f,0.5f,1.0f,1.0f);
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();
    // shader du aquarium
    program_aquarium = new QOpenGLShaderProgram(this);
    program_aquarium->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/aquarium.vsh");
    program_aquarium->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/aquarium.fsh");
    if (! program_aquarium->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_aquarium->log();
    }
    program_aquarium->setUniformValue("texture", 0);
    // shader du poisson
    program_poisson = new QOpenGLShaderProgram(this);
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/poisson.vsh");
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/poisson.fsh");
    if (! program_poisson->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_poisson->log();
    }
    program_poisson->setUniformValue("texture", 0);
    // shader de obstacle
    program_obstacle = new QOpenGLShaderProgram(this);
    program_obstacle->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/obstacle.vsh");
    program_obstacle->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/obstacle.fsh");
    if (! program_obstacle->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_obstacle->log();
    }
    program_obstacle->setUniformValue("texture", 0);
    // shader de predateur
    program_predateur = new QOpenGLShaderProgram(this);
    program_predateur->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/predateur.vsh");
    program_predateur->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/predateur.fsh");
    if (! program_predateur->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_predateur->log();
    }
    program_predateur->setUniformValue("texture", 0);
}


void GLArea::makeGLObjects()
{
    // Création du aquarium
    const float w = largeurAquarium / 2.0f;
    const float h = hauteurAquarium / 2.0f;
    const float d = profondeurAquarium / 2.0f;

    QVector<GLfloat> vertices = {
        -w, -h, d,  w, -h, d,
        w, -h, d,  w,  h, d,
        w,  h, d, -w,  h, d,
        -w,  h, d, -w, -h, d,

        -w, -h, -d,  w, -h, -d,
        w, -h, -d,  w,  h, -d,
        w,  h, -d, -w,  h, -d,
        -w,  h, -d, -w, -h, -d,

        -w, -h, d, -w, -h, -d,
        w, -h, d,  w, -h, -d,
        w,  h, d,  w,  h, -d,
        -w,  h, d, -w,  h, -d
    };

    vbo_aquarium.create();
    vbo_aquarium.bind();
    vbo_aquarium.allocate(vertices.constData(), vertices.count() * sizeof(GLfloat));

    // Création du poisson
    GLfloat vertices_poisson[] = {
        -1.0f,-1.0f, 0.0f,
         1.0f,-1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
    };
    QVector<GLfloat> vp;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            vp.append(vertices_poisson[i*3+j]);
        }
    }
    vbo_poisson.create();
    vbo_poisson.bind();
    vbo_poisson.allocate(vp.constData(), vp.count()* int(sizeof(GLfloat)));

    // Création des obstacles
    const int latSegments = 40;
    const int longSegments = 40;
    const int totalVertices = latSegments * longSegments * 6;

    QVector<GLfloat> vertData_obstacle;
    vertData_obstacle.reserve(totalVertices * obstacles.size() * 6);

    for (const Obstacle& obstacle : obstacles) {
        float rayon = obstacle.getRayon();
        QVector3D couleur = generateRandomColor();

        for (int i = 0; i <= latSegments; i++) {
            float lat0 = M_PI * (-0.5f + (i - 1) / float(latSegments));
            float z0 = rayon * sin(lat0);
            float zr0 = rayon * cos(lat0);

            float lat1 = M_PI * (-0.5f + i / float(latSegments));
            float z1 = rayon * sin(lat1);
            float zr1 = rayon * cos(lat1);

            for (int j = 0; j <= longSegments; j++) {
                float lng = 2 * M_PI * (j - 1) / float(longSegments);
                float x = cos(lng);
                float y = sin(lng);

                QVector3D position = obstacle.getPosition();
                vertData_obstacle << x * zr0 + position.x() << y * zr0 + position.y() << z0 + position.z();
                vertData_obstacle << couleur.x() << couleur.y() << couleur.z();
                vertData_obstacle << x * zr1 + position.x() << y * zr1 + position.y() << z1 + position.z();
                vertData_obstacle << couleur.x() << couleur.y() << couleur.z();
            }
        }
    }
    vbo_obstacle.create();
    vbo_obstacle.bind();
    vbo_obstacle.allocate(vertData_obstacle.constData(), vertData_obstacle.count() * int(sizeof(GLfloat)));
    // Création des predateurs
    const int totalVerticesPredateur = latSegments * longSegments * 6;
    QVector<GLfloat> vertData_predateur;
    vertData_predateur.reserve(totalVerticesPredateur * predateurs.size() * 6);

    for (const Predateur& predateur : predateurs) {
        float rayon = predateur.getRayon();

        for (int i = 0; i <= latSegments; i++) {
            float lat0 = M_PI * (-0.5f + (i - 1) / float(latSegments));
            float z0 = rayon * sin(lat0);
            float zr0 = rayon * cos(lat0);

            float lat1 = M_PI * (-0.5f + i / float(latSegments));
            float z1 = rayon * sin(lat1);
            float zr1 = rayon * cos(lat1);

            for (int j = 0; j <= longSegments; j++) {
                float lng = 2 * M_PI * (j - 1) / float(longSegments);
                float x = cos(lng);
                float y = sin(lng);

                QVector3D position = predateur.getPosition();
                vertData_predateur << x * zr0 + position.x() << y * zr0 + position.y() << z0 + position.z();
                vertData_predateur << 1.0f << 0.0f << 0.0f;
                vertData_predateur << x * zr1 + position.x() << y * zr1 + position.y() << z1 + position.z();
                vertData_predateur << 1.0f << 0.0f << 0.0f;
            }
        }
    }
    vbo_predateur.create();
    vbo_predateur.bind();
    vbo_predateur.allocate(vertData_predateur.constData(), vertData_predateur.count() * sizeof(GLfloat));
}


void GLArea::tearGLObjects()
{
    vbo_poisson.destroy();
    vbo_obstacle.destroy();
    vbo_predateur.destroy();
    vbo_aquarium.destroy();

    for (int i = 0; i < 2; i++)
        delete textures[i];
}


void GLArea::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    windowRatio = float(w) / h;
}


void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrice de projection
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, windowRatio, 1.0f, 1000.0f);

    // Matrice de vue (caméra)
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(xPos, yPos, zPos);
    viewMatrix.rotate(xRot, 1, 0, 0);
    viewMatrix.rotate(yRot, 0, 1, 0);
    viewMatrix.rotate(zRot, 0, 0, 1);

    // Affichage de aquarium
    vbo_aquarium.bind();
    program_aquarium->bind();

    QMatrix4x4 modelMatrixAquarium;
    modelMatrixAquarium.translate(0.0f, 0.0f, 0.0f);
    program_aquarium->setUniformValue("modelMatrix", modelMatrixAquarium);
    program_aquarium->setUniformValue("viewMatrix", viewMatrix);
    program_aquarium->setUniformValue("projectionMatrix", projectionMatrix);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINES, 0, 24);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    program_aquarium->release();
    vbo_aquarium.release();

    // Affichage des obstacles
    vbo_obstacle.bind();
    program_obstacle->bind();

    program_obstacle->setUniformValue("projectionMatrix", projectionMatrix);
    program_obstacle->setUniformValue("viewMatrix", viewMatrix);

    program_obstacle->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    program_obstacle->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    program_obstacle->enableAttributeArray("in_position");
    program_obstacle->enableAttributeArray("colAttr");

    int nbVertices = 3362;
    int start = 0;
    for (Obstacle& o : obstacles) {
        o.affiche(program_obstacle, start, nbVertices);
        start += nbVertices;
    }

    program_obstacle->disableAttributeArray("in_position");
    program_obstacle->disableAttributeArray("colAttr");
    program_obstacle->release();
    vbo_obstacle.release();

    // Affichage des predateurs
    vbo_predateur.bind();
    program_predateur->bind();

    program_predateur->setUniformValue("projectionMatrix", projectionMatrix);
    program_predateur->setUniformValue("viewMatrix", viewMatrix);

    program_predateur->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    program_predateur->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    program_predateur->enableAttributeArray("in_position");
    program_predateur->enableAttributeArray("colAttr");

    nbVertices = 3362;
    start = 0;
    for (Predateur& p : predateurs) {
        p.affiche(program_predateur, start, nbVertices);
        start += nbVertices;
    }

    program_predateur->disableAttributeArray("in_position");
    program_predateur->disableAttributeArray("colAttr");
    program_predateur->release();
    vbo_predateur.release();

    // Affichage du poisson
    glDepthMask(GL_FALSE);

    vbo_poisson.bind();
    program_poisson->bind();

    program_poisson->setUniformValue("projectionMatrix", projectionMatrix);
    program_poisson->setUniformValue("viewMatrix", viewMatrix);

    program_poisson->setAttributeBuffer("in_position", GL_FLOAT,0 ,3, 3*sizeof(GLfloat));
    program_poisson->enableAttributeArray("in_position");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    banc.affiche(program_poisson);
    glDisable(GL_BLEND);

    program_poisson->disableAttributeArray("in_position");
    program_poisson->release();
    vbo_poisson.release();
    glDepthMask(GL_TRUE);

}


void GLArea::keyPressEvent(QKeyEvent *ev)
{
    float da = 1.0f;

    switch(ev->key()) {
        case Qt::Key_A :
            xRot -= da;
            update();
            break;

        case Qt::Key_Q :
            xRot += da;
            update();
            break;

        case Qt::Key_Z :
            yRot -= da;
            update();
            break;

        case Qt::Key_S :
            yRot += da;
            update();
            break;

        case Qt::Key_E :
            zRot -= da;
            update();
            break;

        case Qt::Key_D :
            zRot += da;
            update();
            break;
    }
}


void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
}


void GLArea::mousePressEvent(QMouseEvent *ev)
{
    lastPos = ev->pos();
}


void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}


void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->position().x() - lastPos.x();
    int dy = ev->position().y() - lastPos.y();

    if (ev->buttons() & Qt::LeftButton) {
        xRot += dy;
        yRot += dx;
        update();
    } else if (ev->buttons() & Qt::RightButton) {
        xPos += dx/10.0f;
        yPos -= dy/10.0f;
        update();
    } else if (ev->buttons() & Qt::MiddleButton) {
        xPos += dx/10.0f;
        zPos += dy;
        update();
    }

    lastPos = ev->pos();
}


void GLArea::onTimeout()
{
    static qint64 old_chrono = elapsedTimer.elapsed(); // static : initialisation la première fois et conserve la dernière valeur
    qint64 chrono = elapsedTimer.elapsed();
    dt = (chrono - old_chrono) / 1000.0f;
    old_chrono = chrono;

    auto start = std::chrono::high_resolution_clock::now();

    banc.anime(dt, obstacles, predateurs);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Without Octree: " << elapsed.count() << " seconds" << std::endl;


    // auto start1 = std::chrono::high_resolution_clock::now();
    // banc.anime_quadtree(dt, obstacles, predateurs);
    // auto end1 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed1 = end1 - start1;
    // std::cout << "With Octree: " << elapsed1.count() << " seconds" << std::endl;

    for(Predateur &p : predateurs){
        p.anime(dt, banc.poissons);
    }

    update();
}


QVector3D GLArea::generateRandomColor()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return QVector3D(dis(gen), dis(gen), dis(gen));
}
