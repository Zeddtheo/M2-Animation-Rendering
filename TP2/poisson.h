#ifndef POISSON_H
#define POISSON_H

#include "qmatrix4x4.h"
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector3D>


class Poisson
{
public:
    Poisson();
    Poisson(QVector3D position, float taille, QVector3D vitesse, float distance);
    void anime(float dt);
    bool dans_voisinage(const QVector3D p);
    bool dans_sphere(QVector3D p, float rayon);
    void affiche(QOpenGLShaderProgram *sp);

    QVector3D getPosition() const;
    QVector3D getVitesse() const;
    void setPosition(const QVector3D& nouvellePosition);
    void setVitesse(const QVector3D& nouvelleVitesse);
    float distanceToSphere(const QVector3D& spherePosition, float sphereRayon);

private:
    QVector3D position;
    float taille;
    QVector3D vitesse;
    float distance;

    QMatrix4x4 modelMatrix;
};

#endif // POISSON_H
