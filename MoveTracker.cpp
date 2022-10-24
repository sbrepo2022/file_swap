#include "MoveTracker.h"

bool MoveTracker::start(int xpos, int ypos, int w_xpos, int w_ypos) {
    if (blocks)
        return false;

    this->last_x = xpos;
    this->last_y = ypos;
    if (xpos > x1 + w_xpos && xpos < x2 + w_xpos && ypos > y1 + w_ypos && ypos < y2 + w_ypos) {
        this->isMove = true;
    }
    return this->isMove;
}

bool MoveTracker::tracking(int xpos, int ypos, int &xoffset, int &yoffset) {
    xoffset = xpos - this->last_x;
    yoffset = ypos - this->last_y;
    this->last_x = xpos;
    this->last_y = ypos;
    return this->isMove;
}

void MoveTracker::end() {
    this->isMove = false;
}
