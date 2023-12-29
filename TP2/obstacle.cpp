#include "obstacle.h"

Obstacle::Obstacle(const QVector3D& position, float rayon)
    : position(position), rayon(rayon) {

}

void Obstacle::affiche(QOpenGLShaderProgram *sp, int start, int nbVertices){
    this->modelMatrix.setToIdentity();

    sp->setUniformValue("modelMatrix", this->modelMatrix);

    glDrawArrays(GL_TRIANGLE_STRIP, start, nbVertices);
}

QVector3D Obstacle::getPosition() const {
    return position;
}

float Obstacle::getRayon() const {
    return rayon;
}
