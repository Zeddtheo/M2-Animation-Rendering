#ifndef PUFF_H
#define PUFF_H
#include "qopenglshaderprogram.h"
#include "qvectornd.h"
#include <QOpenGLFunctions>

class Puff
{
public:
    Puff(QVector3D position, float size, QVector3D vitesse, float life);

    void animate(float dt);
    void display(QOpenGLShaderProgram *program_particule);

    QVector3D position;
    QVector3D vitesse;
    QVector3D color;

    float size;
    float life;
    float alpha;
    float maxLifeTime = 5.0;

    float indexTex;

    QMatrix4x4 modelMatrix;
};

#endif // PUFF_H
