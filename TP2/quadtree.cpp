#include "quadtree.h"

void Quadtree::insert(Poisson& p) {
    if (poissons.size() >= MAX_CAPACITY && level < MAX_LEVELS) {
        if (!children[0]) {
            split();
        }

        auto it = poissons.begin();
        while (it != poissons.end()) {
            int index = getIndex(*it);
            if (index != -1) {
                children[index]->insert(*it);
                it = poissons.erase(it);
            } else {
                ++it;
            }
        }
    }

    int index = getIndex(p);
    if (index != -1 && children[0]) {
        children[index]->insert(p);
    } else {
        poissons.push_back(p);
    }
}

std::vector<Poisson> Quadtree::queryRange(QRect range) {
    std::vector<Poisson> rangePoissons;

    if (!boundary.intersects(range)) {
        return rangePoissons;
    }

    for (int i = 0; i < poissons.size(); i++) {
        if (contains(range, poissons[i].getPosition())) {
            rangePoissons.push_back(poissons[i]);
        }
    }

    if (!children[0]) {
        for (int i = 0; i < 4; i++) {
            std::vector<Poisson> childPoissons = children[i]->queryRange(range);
            rangePoissons.insert(rangePoissons.end(), childPoissons.begin(), childPoissons.end());
        }
    }

    return rangePoissons;
}

void Quadtree::clear() {
    poissons.clear();

    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            children[i]->clear();
            children[i].reset();
        }
    }
}

void Quadtree::split() {
    int subWidth = boundary.width() / 2;
    int subHeight = boundary.height() / 2;
    int x = boundary.x();
    int y = boundary.y();

    children[0] = std::make_unique<Quadtree>(level + 1, QRect(x + subWidth, y, subWidth, subHeight));
    children[1] = std::make_unique<Quadtree>(level + 1, QRect(x, y, subWidth, subHeight));
    children[2] = std::make_unique<Quadtree>(level + 1, QRect(x, y + subHeight, subWidth, subHeight));
    children[3] = std::make_unique<Quadtree>(level + 1, QRect(x + subWidth, y + subHeight, subWidth, subHeight));
}

int Quadtree::getIndex(const Poisson& p) {
    int index = -1;
    double verticalMidpoint = boundary.x() + (boundary.width() / 2);
    double horizontalMidpoint = boundary.y() + (boundary.height() / 2);

    bool topQuadrant = (p.getPosition().y() < horizontalMidpoint);
    bool bottomQuadrant = (p.getPosition().y() > horizontalMidpoint);

    if (p.getPosition().x() < verticalMidpoint) {
        if (topQuadrant) {
            index = 1;
        } else if (bottomQuadrant) {
            index = 2;
        }
    } else if (p.getPosition().x() > verticalMidpoint) {
        if (topQuadrant) {
            index = 0;
        } else if (bottomQuadrant) {
            index = 3;
        }
    }

    return index;
}

bool Quadtree::contains(const QRect& rect, const QVector3D& point) {
    return point.x() >= rect.left() && point.x() <= rect.right() &&
           point.y() >= rect.top() && point.y() <= rect.bottom();
}
