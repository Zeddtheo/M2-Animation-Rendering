#include "smoke.h"
#include "puff.h"
#include <QRandomGenerator>
Smoke::Smoke(QVector3D position, float timeInterval)
{
    this->position = position;
    this->timeInterval = timeInterval;
}


void Smoke::animate(float dt){
    elapsedTime += dt;
    if(elapsedTime > timeInterval){
        elapsedTime = 0;

        Puff newPuff(position, 1.0, QVector3D(0.0f,1.0f,0.0f), 5+(std::rand()%(8-5+1)));
        puffsList.push_back(newPuff);
    }

    std::list<Puff>::iterator i;
    i = puffsList.begin();
    while( i != puffsList.end() )
    {
        if( i->life <= 0 )
            i = puffsList.erase(i);
        else
            i++;
    }

    std::list<Puff>::iterator j = puffsList.begin();
    while(j != puffsList.end()){
        j->animate(dt);
        j++;
    }
}

void Smoke::display(QOpenGLShaderProgram *program_particule){
    std::list<Puff>::iterator i;
    i = puffsList.begin();
    while(i != puffsList.end()){
        i->display(program_particule);
        i++;
    }
}
