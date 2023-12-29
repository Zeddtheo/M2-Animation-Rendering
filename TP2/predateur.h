#ifndef PREDATEUR_H
#define PREDATEUR_H


#include "poisson.h"
#include "qmatrix4x4.h"
#include "qopenglshaderprogram.h"
#include "qvectornd.h"
class Predateur
{
public:
    Predateur();
    Predateur(QVector3D position, float rayon, QVector3D vitesse, float maxVitesse);
    QVector3D getPosition() const;
    float getRayon() const;

    QVector3D position;
    float rayon;
    QVector3D vitesse;
    float maxVitesse;

    void anime(float dt, const std::vector<Poisson>& poissons);
    void affiche(QOpenGLShaderProgram *sp, int start, int nbVertices);
    QMatrix4x4 modelMatrix;
};

#endif // PREDATEUR_H
