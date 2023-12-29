#ifndef QUADTREE_H
#define QUADTREE_H

#include "poisson.h"
class Quadtree {
public:
    static const int MAX_CAPACITY = 10;
    static const int MAX_LEVELS = 5;

    Quadtree(int level, QRect boundary) : level(level), boundary(boundary) {}

    void insert(Poisson& p);

    std::vector<Poisson> queryRange(QRect range);
    bool contains(const QRect& rect, const QVector3D& point);
    void clear();

private:
    int level;
    QRect boundary;
    std::vector<Poisson> poissons;
    std::array<std::unique_ptr<Quadtree>, 4> children;

    void split();

    int getIndex(const Poisson& p);

};

#endif // QUADTREE_H
