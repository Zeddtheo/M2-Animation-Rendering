#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "qopenglshaderprogram.h"
#include "qvectornd.h"

class Obstacle
{
public:
    Obstacle(const QVector3D& position, float rayon);

    void affiche(QOpenGLShaderProgram *program, int start, int nbVertices);
    QVector3D getPosition() const;
    float getRayon() const;
private:
    QVector3D position;
    float rayon;

    QMatrix4x4 modelMatrix;
};

#endif // OBSTACLE_H
