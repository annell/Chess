//
// Created by Stefan Annell on 2021-02-27.
//

#include "Square.h"
#include <iostream>

Square::Square(int width, int height, int x, int y, int index, const cocos2d::Color4F& color)
    : node(nullptr)
    , width(width)
    , height(height)
    , x(x)
    , y(y)
    , index(index)
    , color(color) {
    colorSelected = color;
    colorSelected.r = 1;
    colorSelected.g = color.g / 2;
    colorSelected.b = color.b / 2;
    corners[0] = cocos2d::Vec2(x, y);
    corners[1] = cocos2d::Vec2(x + width, y);
    corners[2] = cocos2d::Vec2(x + width, y + height);
    corners[3] = cocos2d::Vec2(x, y + height);
}

cocos2d::DrawNode *Square::CreateNode() {
    node = cocos2d::DrawNode::create();
    node->drawPolygon(corners, 4, color, 1, color);
    return node;
}

void Square::Select() {
    node->clear();
    node->drawPolygon(corners, 4, colorSelected, 1, colorSelected);
}

void Square::DeSelect() {
    node->clear();
    node->drawPolygon(corners, 4, color, 1, color);
}

cocos2d::Sprite *Square::CreateSprite(std::string filename) {
    auto sprite = new cocos2d::Sprite;
    sprite->initWithFile(filename);
    sprite->setPosition(cocos2d::Point(x + width / 2, y + height / 2));
    sprite->setAnchorPoint(cocos2d::Point(0.5f, 0.5f));
    return sprite;
}

bool Square::ContainsPoint(const cocos2d::Vec2& point) {
    if (x < point.x && x + width > point.x && y < point.y && y + height > point.y) {
        return true;
    }
    return false;
}

bool Square::TrySetSpritePosition(const cocos2d::Vec2& point, cocos2d::Sprite* sprite) {
    if (sprite) {
        sprite->setPosition(point);
        return true;
    }
    return false;
}

bool Square::SetSpriteOrigin(cocos2d::Sprite* sprite) {
    if (sprite) {
        sprite->setPosition(cocos2d::Point(x + width / 2, y + height / 2));
        return true;
    }
}

