//
// Created by Stefan Annell on 2021-02-27.
//

#ifndef CHESSGUI_SQUARE_H
#define CHESSGUI_SQUARE_H

#include "cocos2d.h"

class Square {
public:
    Square(int width, int height, int x, int y, int index, const cocos2d::Color4F&);

    cocos2d::DrawNode* CreateNode();
    cocos2d::Sprite* CreateSprite(std::string filename);

    void Select();
    void DeSelect();

    bool ContainsPoint(const cocos2d::Vec2&);
    bool TrySetSpritePosition(const cocos2d::Vec2&, cocos2d::Sprite*);
    bool SetSpriteOrigin(cocos2d::Sprite*);

    int index;
private:
    cocos2d::DrawNode* node;
    cocos2d::Vec2 corners[4];
    cocos2d::Color4F color;
    cocos2d::Color4F colorSelected;
    int width;
    int height;
    int x;
    int y;
};


#endif //CHESSGUI_SQUARE_H
