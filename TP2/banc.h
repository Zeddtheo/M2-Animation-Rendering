#ifndef BANC_H
#define BANC_H

#include "obstacle.h"
#include "poisson.h"
#include "predateur.h"
#include "quadtree.h"

class Banc
{
    Quadtree quadtree;
public:
    Banc();
    Banc(int nbPoissons, float largeur, float hauteur, float profondeur, const std::vector<Obstacle>& obstacles, const std::vector<Predateur>& predateurs);
    void anime(float dt, const std::vector<Obstacle> &obstacles,const std::vector<Predateur>& predateurs);
    void affiche(QOpenGLShaderProgram *sp);

    std::vector<Poisson> getVoisins(Poisson& poisson);
    QVector3D calcSeparation(Poisson& poisson, const std::vector<Poisson>& voisins,
                             const std::vector<Obstacle>& obstacles,
                             const std::vector<Predateur>& predateurs);
    QVector3D calcAlignment(Poisson& poisson, const std::vector<Poisson>& voisins);
    QVector3D calcCohesion(Poisson& poisson, const std::vector<Poisson>& voisins);

    std::vector<Poisson> poissons;
    std::vector<Obstacle> obstacles;
    std::vector<Predateur> predateurs;

    void anime_quadtree(float dt, const std::vector<Obstacle>& obstacles,
                        const std::vector<Predateur>& predateurs);
private:

    int nbPoissons = 0;
    float largeur, hauteur, profondeur;

    float distance = 5;
    float L = 0.5f;
    float maxVitesse = 1.0f;


    float perceptionRadius = 10.0f;

    QMatrix4x4 modelMatrix;
};

#endif // BANC_H
