#include "puff.h"



Puff::Puff(QVector3D position, float size, QVector3D vitesse, float life)
    : position(position), size(size), vitesse(vitesse), life(life)
{

}

void Puff::animate(float dt)
{
    position += vitesse * dt;
    life -= dt;
    size += dt * 0.5f;
    if(indexTex< 16){
        indexTex ++;
    }else{
        indexTex = 0;
    }
}

void Puff::display(QOpenGLShaderProgram *program_particule)
{
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(position);

    alpha = life/maxLifeTime;

    program_particule->setUniformValue("modelMatrix", modelMatrix);
    program_particule->setUniformValue("particleSize",size);
    program_particule->setUniformValue("lifeTimeSize",(float) 1+5*(maxLifeTime-life)/maxLifeTime);
    program_particule->setUniformValue("elapsed", life);
    program_particule->setUniformValue("alpha", alpha);
    program_particule->setUniformValue("indexTex",indexTex);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
