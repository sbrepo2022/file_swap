#ifndef MOVETRACKER_H
#define MOVETRACKER_H

class MoveTracker {
private:
    int x1, y1, x2, y2;
    int last_x, last_y;
    bool isMove;
    int blocks;

public:
    MoveTracker() {this->isMove = false; this->blocks = 0;}
    MoveTracker(int x1, int y1, int x2, int y2) {this->x1 = x1; this->y1 = y1; this->x2 = x2; this->y2 = y2; this->isMove = false; this->blocks = 0;}

    void init(int x1, int y1, int x2, int y2) {this->x1 = x1; this->y1 = y1; this->x2 = x2; this->y2 = y2;}
    bool start(int xpos, int ypos, int w_xpos, int w_ypos);
    bool tracking(int xpos, int ypos, int &xoffset, int &yoffset);
    void end();
    void block() {blocks++;}
    void unblock() {blocks--; if (blocks < 0) blocks = 0;}
};

#endif // MOVETRACKER_H
