#include "poisson.h"

Poisson::Poisson()
    : position(QVector3D()),
    taille(0.0f),
    vitesse(QVector3D()),
    distance(0.0f)
{

}

Poisson::Poisson(QVector3D position, float taille, QVector3D vitesse, float distance):
    position(position), taille(taille), vitesse(vitesse), distance(distance)
{

}

void Poisson::anime(float dt){
    position += vitesse * dt;
}

bool Poisson::dans_voisinage(const QVector3D p){
    return this->position.distanceToPoint(p) <= distance;
}

void Poisson::affiche(QOpenGLShaderProgram *sp){
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(position);

    float angleY = 0.0f;
    if(vitesse.x() != 0.0f || vitesse.z() != 0.0f) {
        int signeAngle = (vitesse.z() > 0) ? -1 : 1;
        angleY = signeAngle * std::acos(vitesse.x() / std::hypot(vitesse.x(), vitesse.z())) * 180.0f / static_cast<float>(M_PI) + 180.0f;
    }

    float angleZ = 0.0f;
    if(vitesse.length() != 0.0f) {
        angleZ = std::acos(vitesse.y() / vitesse.length()) * 180.0f / static_cast<float>(M_PI);
    }

    QQuaternion quatY = QQuaternion::fromAxisAndAngle(0, 1, 0, angleY);
    QQuaternion quatZ = QQuaternion::fromAxisAndAngle(0, 0, 1, angleZ);
    QQuaternion rotationQuat = quatY * quatZ;
    modelMatrix.rotate(rotationQuat);

    sp->setUniformValue("modelMatrix", this->modelMatrix);
    sp->setUniformValue("poissonTaille", this->taille);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

QVector3D Poisson::getPosition() const{
    return position;
}

QVector3D Poisson::getVitesse() const{
    return vitesse;
}

void Poisson::setPosition(const QVector3D& nouvellePosition) {
    position = nouvellePosition;
}

void Poisson::setVitesse(const QVector3D& nouvelleVitesse) {
    vitesse = nouvelleVitesse;
}

float Poisson::distanceToSphere(const QVector3D& spherePosition, float sphereRayon) {
    float distanceCentre = this->position.distanceToPoint(spherePosition);
    return std::max(0.0f, distanceCentre - sphereRayon);
}
