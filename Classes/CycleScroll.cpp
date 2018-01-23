#include "CyclyScroll.hpp"

using namespace std;
USING_NS_CC;

CycleScroll::CycleScroll():autoScrolling(false),
                            dragging(false),
                            scrollDistance(0),
                            minScale(1),
                            spaceDistance(150),
                            currentIndex(0),
                            startTime(0)
{}

CycleScroll* CycleScroll::create(Size &size, vector<Node*> nodes, float distance, float minScale /*= 1.0f*/)
{
    auto ret = CycleScroll::create();
    ret->nodes = nodes;
    ret->spaceDistance = distance;
    ret->minScale = minScale;
    ret->disSize = size;
    ret->initView();
    ret->scheduleUpdate();
    return ret;
}

void CycleScroll::initView()
{
    this->setContentSize(this->disSize);
    auto clipSize = this->disSize;
    auto stencil = LayerColor::create(Color4B::RED, clipSize.width, clipSize.height);
    clipNode = ClippingNode::create(stencil);
    this->addChild(this->clipNode);
    
    unsigned long cnt = this->nodes.size();
    for(int i = 0; i< cnt; i++){
        if(this->nodes[i] != NULL){
            this->nodes[i]->setPosition(Vec2(this->nodes[i]->getContentSize().width/2 + i * spaceDistance, disSize.height/2));// TODO
            clipNode->addChild(this->nodes[i]);
        }
    }
    
    this->initListener();
    this->scrollTo(0, 0);
}

void CycleScroll::setDisplaySize(const cocos2d::Size &size){
    LayerColor::setContentSize(size);
    clipNode->getStencil()->setContentSize(size);
}

const Size& CycleScroll::getDisplaySize() const{
    return this->disSize;
}

void CycleScroll::scrollTo(int index, float delay){
    if(index < 0 || index >= this->nodes.size()) {
        CCLOG("index invalid!");
        return;
    }
    
    Size contentSize = this->getContentSize();
    
    float distance = contentSize.width/2 - this->nodes[index]->getPositionX();
    for(auto i : this->nodes){
        i->stopAllActions();
        i->runAction(EaseSineOut::create(MoveBy::create(delay, Vec2(distance, 0))));
    }
}

bool CycleScroll::init()
{
    return LayerColor::initWithColor(Color4B(0,0,0,0));
}

void CycleScroll::initListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CycleScroll::onTouchBegin, this);
    listener->onTouchMoved = CC_CALLBACK_2(CycleScroll::onTouchMove, this);
    listener->onTouchEnded = CC_CALLBACK_2(CycleScroll::onTouchEnd, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool CycleScroll::onTouchBegin(cocos2d::Touch *touch, cocos2d::Event *unused_event){
    dragging = true;
    scrollDistance = 0;
    startTime = clock();
    startPos = touch->getLocation();
    return true;
}

void CycleScroll::onTouchMove(cocos2d::Touch *touch, cocos2d::Event *unused_event){
    float disX = touch->getDelta().x;
    this->updateNodePosX(disX);
}

void CycleScroll::onTouchEnd(cocos2d::Touch *touch, cocos2d::Event *unused_event){
//    float startDistance = touch->getLocation().distance(touch->getStartLocation());
//
//    if(startDistance < 10) // click
//    {
//
//    }
    
    // 处理减速
    dragging = false;
    auto prePos = touch->getPreviousLocation();
    auto curPos = touch->getLocation();
    scrollDistance = curPos.distance(prePos);
    int direction = curPos.x > startPos.x?1:-1;
    
    scrollDistance = scrollDistance <= MAX_SCROLL_SPEED?scrollDistance : MAX_SCROLL_SPEED;
    scrollDistance = scrollDistance * direction;
    CCLOG("end:%f, pre:%f", curPos.x, prePos.x);
    
    float longDisance = curPos.distance(startPos);
    float elapsedTime = (clock() - startTime) / 1000; //毫秒
    CCLOG("long:%f, t:%f", longDisance, elapsedTime);
    if (elapsedTime < 50 && scrollDistance < 10 && longDisance >= 10){
        scrollDistance = longDisance<=MAX_SCROLL_SPEED?longDisance:MAX_SCROLL_SPEED;
        scrollDistance *= direction;
    }
    if (fabs(scrollDistance) > MIN_SCROLL_SPEED){
        autoScrolling = true;
    }
}

void CycleScroll::updateNodePosX(float interval){
    int length = (int)nodes.size();
    for (int i=0; i<length; i++){
        auto node = this->nodes[i];
        node->setPositionX(node->getPositionX() + interval);
    }
}

void CycleScroll::deaccelerateScrolling(float dt){
    if (dragging)
    {
        autoScrolling = false;
        return;
    }
    CCLOG("sc:%f", scrollDistance);
    scrollDistance *= 0.95;
    if (fabs(scrollDistance) < MIN_SCROLL_SPEED){
        autoScrolling = false;
        startTime = 0;
    }
    this->updateNodePosX(scrollDistance);
}

void CycleScroll::update(float dt)
{
    int length = (int)nodes.size();
    float newPosX = 0;
    auto s = 0.5;
    auto mid = this->disSize.width/2;
    
    for (int i=0; i<length; i++){
        auto node = this->nodes[i];
        auto curPosX = node->getPositionX();
        
        if (curPosX < -node->getContentSize().width/2){
            int beforeIndex = i-1;
            beforeIndex = beforeIndex>=0?beforeIndex:length-1;
            newPosX = nodes[beforeIndex]->getPositionX() + this->spaceDistance;
            if (curPosX != newPosX){
                node->setPositionX(newPosX);
                curPosX = newPosX;
            }
        }else if (curPosX > this->disSize.width + node->getContentSize().width/2){
            int afterIndex = i+1;
            afterIndex = afterIndex<=length-1?afterIndex:0;
            newPosX = nodes[afterIndex]->getPositionX() - this->spaceDistance;
            if (curPosX != newPosX){
                node->setPositionX(newPosX);
                curPosX = newPosX;
            }
        }
        
        if (curPosX <= mid){
            s = curPosX / mid;
        } else{
            s = (this->disSize.width-curPosX) / mid;
        }
        s *= 1.2;
        s = s<=1?s:1;
        s = s>=this->minScale?s:this->minScale;
        node->setScale(s);
    }
    
    if (autoScrolling){
        this->deaccelerateScrolling(dt);
    }
}
