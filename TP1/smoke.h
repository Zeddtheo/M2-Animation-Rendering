#ifndef SMOKE_H
#define SMOKE_H

#include <puff.h>
#include <list>

class Smoke
{
public:
    Smoke(QVector3D position, float timeInterval);

    void animate(float dt);
    void display(QOpenGLShaderProgram *program_particule);

    QVector3D position;
    float timeInterval;
    float elapsedTime;

    std::list<Puff> puffsList;
};

#endif // SMOKE_H
