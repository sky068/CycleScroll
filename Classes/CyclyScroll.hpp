//
//  ScrollCircle.hpp
//  ScrollSelector-mobile
//
//  Created by 徐家伟 on 2018/1/19.
//

#ifndef CycleScroll_hpp
#define CycleScroll_hpp

#include "cocos2d.h"
#include "cocos-ext.h"
#include <vector>

#define MIN_SCROLL_SPEED 1
#define MAX_SCROLL_SPEED 30

class CycleScroll: public cocos2d::LayerColor
{
    CycleScroll();
public:
    static CycleScroll* create(cocos2d::Size &size, std::vector<cocos2d::Node*> nodes, float distance, float minScale = 1.0f);
    void scrollTo(int index, float delay);
    void setDisplaySize(const cocos2d::Size &size);
    const cocos2d::Size& getDisplaySize() const;
    
private:
    float minScale;
    float spaceDistance;
    int currentIndex;
    cocos2d::Size disSize;
    cocos2d::ClippingNode* clipNode;
    std::vector<cocos2d::Node*> nodes;
    
    // 减速使用
    bool dragging;
    bool autoScrolling;
    float scrollDistance;
    cocos2d::Vec2 startPos;

    bool onTouchBegin(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchMove(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchEnd(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void updateNodePosX(float interval);

    virtual bool init();
    void initView();
    void initListener();
    void update(float dt);
    void deaccelerateScrolling(float dt);
    
    CREATE_FUNC(CycleScroll);
};

#endif /* ScrollCircle_hpp */
