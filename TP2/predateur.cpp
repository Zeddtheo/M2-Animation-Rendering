#include "predateur.h"

Predateur::Predateur()
{

}

Predateur::Predateur(QVector3D position, float rayon, QVector3D vitesse, float maxVitesse)
    :position(position), rayon(rayon), vitesse(vitesse), maxVitesse(maxVitesse){

}

void Predateur::anime(float dt, const std::vector<Poisson>& poissons){
    if (poissons.empty()) return;

    const Poisson* closestFish = nullptr;
    float minDistanceSq = std::numeric_limits<float>::max();
    for (const auto& fish : poissons) {
        float distanceSq = (fish.getPosition() - position).lengthSquared();
        if (distanceSq < minDistanceSq) {
            minDistanceSq = distanceSq;
            closestFish = &fish;
        }
    }

    if (closestFish) {
        QVector3D direction = (closestFish->getPosition() - position).normalized();

        vitesse = direction * maxVitesse;

        position += vitesse * dt;

    }
}

void Predateur::affiche(QOpenGLShaderProgram *sp, int start, int nbVertices){
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(position);

    sp->setUniformValue("modelMatrix", this->modelMatrix);

    glDrawArrays(GL_TRIANGLE_STRIP, start, nbVertices);
}

QVector3D Predateur::getPosition() const {
    return position;
}

float Predateur::getRayon() const {
    return rayon;
}


