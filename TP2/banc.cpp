#include "banc.h"

Banc::Banc()
    : nbPoissons(0),
    largeur(0),
    hauteur(0),
    profondeur(0),
    quadtree(0, QRect(0, 0, largeur, hauteur))
{

}

Banc::Banc(int nbPoissons, float largeur, float hauteur, float profondeur, const std::vector<Obstacle>& obstacles, const std::vector<Predateur>& predateurs)
    : nbPoissons(nbPoissons), largeur(largeur), hauteur(hauteur), profondeur(profondeur), quadtree(0, QRect(0, 0, largeur, hauteur)){

    QVector3D position;
    QVector3D vitesse;
    bool position_ok;
    float x, y, z;
    float distance = 10;

    for (int i = 0; i < nbPoissons; ++i) {
        do {
            position_ok = true;
            position = QVector3D((rand() / (float)RAND_MAX) * largeur / 2 - largeur / 4,
                                 (rand() / (float)RAND_MAX) * hauteur / 2 - hauteur / 4,
                                 (rand() / (float)RAND_MAX) * profondeur / 2 - profondeur / 4);

            for (const Obstacle& o : obstacles) {
                x = position.x() - o.getPosition().x();
                y = position.y() - o.getPosition().y();
                z = position.z() - o.getPosition().z();
                if ((x * x) + (y * y) + (z * z) <= o.getRayon() * o.getRayon()) {
                    position_ok = false;
                    break;
                }
            }

            for (const Predateur& pred : predateurs) {
                x = position.x() - pred.getPosition().x();
                y = position.y() - pred.getPosition().y();
                z = position.z() - pred.getPosition().z();
                if ((x * x) + (y * y) + (z * z) <= pred.getRayon() * pred.getRayon()) {
                    position_ok = false;
                    break;
                }
            }
        } while (!position_ok);

        vitesse = QVector3D((rand() / (float)RAND_MAX) * 2 - 1,
                            (rand() / (float)RAND_MAX) * 2 - 1,
                            (rand() / (float)RAND_MAX) * 2 - 1);
        Poisson p(position, 1, vitesse, distance);
        poissons.push_back(p);
    }
}

void Banc::anime(float dt, const std::vector<Obstacle>& obstacles, const std::vector<Predateur>& predateurs) {
    for (auto& poisson : poissons) {
        std::vector<Poisson> voisins = getVoisins(poisson);

        QVector3D v1 = calcSeparation(poisson, voisins, obstacles, predateurs);
        QVector3D v2 = calcAlignment(poisson, voisins);
        QVector3D v3 = calcCohesion(poisson, voisins);

        QVector3D v = 1.0* v1 + 1.0 * v2 + 1.0 * v3;

        QVector3D nouvelleVitesse = (1 - L) * poisson.getVitesse() + L * v * maxVitesse;
        if (nouvelleVitesse.length() > maxVitesse) {
            nouvelleVitesse.normalize();
            nouvelleVitesse *= maxVitesse;
        }

        QVector3D newPosition = poisson.getPosition() + nouvelleVitesse * dt;

        poisson.setVitesse(nouvelleVitesse);
        poisson.setPosition(newPosition);

        poisson.anime(dt);
    }
}

void Banc::affiche(QOpenGLShaderProgram *sp){
    for(auto& p:poissons){
        p.affiche(sp);
    }
}
//Voisins
std::vector<Poisson> Banc::getVoisins(Poisson& poisson) {
    std::vector<Poisson> voisins;
    for (auto& p : poissons) {
        if (poisson.dans_voisinage(p.getPosition()) && &poisson != &p) {
            voisins.push_back(p);
        }
    }
    return voisins;
}
//Vecteur v1
QVector3D Banc::calcSeparation(Poisson& poisson, const std::vector<Poisson>& voisins,
                               const std::vector<Obstacle>& obstacles,
                               const std::vector<Predateur>& predateurs) {
    QVector3D moyenne(0, 0, 0);
    int count = 0;

    for (const auto& voisin : voisins) {
        moyenne += voisin.getPosition();
        count++;
    }

    for (const auto& obstacle : obstacles) {
        float distanceToObstacle = poisson.distanceToSphere(obstacle.getPosition(), obstacle.getRayon());
        float obstacleRayon = obstacle.getRayon();
        float poissonRayon = 1.0f;
        float safeDistance = 1.0f;
        float thresholdDistance = obstacleRayon + poissonRayon + safeDistance;
        if (distanceToObstacle < thresholdDistance) {
            moyenne += obstacle.getPosition();
            count++;
        }
    }

    for (const auto& predateur : predateurs) {
        float distanceToPredateur = poisson.distanceToSphere(predateur.getPosition(), predateur.getRayon());
        float predateurRayon = predateur.getRayon();
        float poissonRayon = 1.0f;
        float safeDistance = 1.0f;
        float thresholdDistance = predateurRayon + poissonRayon + safeDistance;
        if (distanceToPredateur < thresholdDistance) {
            moyenne += predateur.getPosition();
            count++;
        }
    }

    if (count > 0) {
        moyenne /= count;
        return poisson.getPosition() - moyenne;
    }

    return QVector3D(0, 0, 0);
}
//Vecteur v2
QVector3D Banc::calcAlignment(Poisson& poisson, const std::vector<Poisson>& voisins) {
    QVector3D moyenneVitesse(0, 0, 0);
    int count = 0;

    for (const auto& voisin : voisins) {
        moyenneVitesse += voisin.getVitesse();
        count++;
    }

    if (count > 0) {
        moyenneVitesse /= count;
        return poisson.getVitesse() - moyenneVitesse;
    }

    return QVector3D(0, 0, 0);
}
//Vecteur v3
QVector3D Banc::calcCohesion(Poisson& poisson, const std::vector<Poisson>& voisins) {
    QVector3D centreDeMasse(0, 0, 0);
    int count = 0;

    for (const auto& voisin : voisins) {
        centreDeMasse += voisin.getPosition();
        count++;
    }

    if (count > 0) {
        centreDeMasse /= count;
        return centreDeMasse - poisson.getPosition();
    }

    return QVector3D(0, 0, 0);
}

void Banc::anime_quadtree(float dt, const std::vector<Obstacle>& obstacles,
                          const std::vector<Predateur>& predateurs) {

    quadtree.clear();
    qDebug("here0");
    for (auto& poisson : poissons) {
        quadtree.insert(poisson);
    }

    qDebug("here1");
    for (auto& poisson : poissons) {
        QRect searchArea(poisson.getPosition().x() - perceptionRadius,
                         poisson.getPosition().y() - perceptionRadius,
                         2 * perceptionRadius, 2 * perceptionRadius);
        std::vector<Poisson> voisins = quadtree.queryRange(searchArea);
        qDebug("here2");
        QVector3D v1 = calcSeparation(poisson, voisins, obstacles, predateurs);
        QVector3D v2 = calcAlignment(poisson, voisins);
        QVector3D v3 = calcCohesion(poisson, voisins);

        QVector3D v = 1.0* v1 + 1.0 * v2 + 1.0 * v3;

        QVector3D nouvelleVitesse = (1 - L) * poisson.getVitesse() + L * v * maxVitesse;
        if (nouvelleVitesse.length() > maxVitesse) {
            nouvelleVitesse.normalize();
            nouvelleVitesse *= maxVitesse;
        }

        QVector3D newPosition = poisson.getPosition() + nouvelleVitesse * dt;

        poisson.setVitesse(nouvelleVitesse);
        poisson.setPosition(newPosition);
//qDebug("here1");
        poisson.anime(dt);
    }
}
