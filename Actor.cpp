#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor stuff
Actor::Actor(int imageId, double x, double y, int dir, double size, unsigned int depth, bool isCollision, StudentWorld* stuPtr, double vertSpeed, double horizSpeed, bool isAlive): GraphObject(imageId, x, y, dir, size, depth), m_isCollisionAvoidWorthy(isCollision), m_studentPtr(stuPtr), m_verticalSpeed(vertSpeed),
m_horizontalSpeed(horizSpeed), m_isAlive(isAlive)
{}

Actor::~Actor()
{}

bool Actor::isAlive() const
{
    return m_isAlive;
}

double Actor::getVertSpeed() const
{
    return m_verticalSpeed;
}

double Actor::getHorizSpeed() const
{
    return m_horizontalSpeed;
}

void Actor::setDie()
{
    m_isAlive = false;
}

StudentWorld* Actor::getStuPtr() const
{
    return m_studentPtr;
}

void Actor::setVertSpeed(double vSpeed)
{
    m_verticalSpeed = vSpeed;
}

void Actor::setHorizSpeed(double hSpeed)
{
    m_horizontalSpeed = hSpeed;
}

bool Actor::isCollisionAvoidWorthy() const
{
    return m_isCollisionAvoidWorthy;
}

void Actor::goneOffScreen()
{
    //checking whether the Actor has gone off the bounds of the screen
    if(getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDie();
        return;
    }
}

void Actor::moveWithGhost()
{
    //determining the relative movement of an Actor with respect to the Ghost Racer
    double vert_speed = getVertSpeed() - ((getStuPtr() -> getGhostPtr()) -> getVertSpeed());
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    goneOffScreen();
}

bool Actor::beSprayedIfAppropriate()
{
    return false;
}

//Agent stuff
Agent::Agent(int imageId, double x, double y, int dir, double size, unsigned int depth, bool isCollision, StudentWorld* stuPtr, double vertSpeed, double horizSpeed, bool isAlive, int health, int movePlan): Actor(imageId, x, y, dir, size, depth, isCollision, stuPtr, vertSpeed, horizSpeed, isAlive), m_health(health), m_movePlan(movePlan)
{}

Agent::~Agent()
{}

int Agent::getHealth() const
{
    return m_health;
}

void Agent::setHealth(int currHealth)
{
    m_health = currHealth;
    //if the health goes below 0, set the Actor's state to dead
    if(currHealth <= 0)
    {
        m_health = 0;
        setDie();
    }
}

int Agent::soundWhenHurt() const
{
    return SOUND_VEHICLE_HURT;
}
int Agent::soundWhenDie() const
{
    return SOUND_VEHICLE_DIE;
}

int Agent::getMovePlan() const
{
    return m_movePlan;
}

void Agent::setMovePlan(int movePlan)
{
    m_movePlan = movePlan;
}

//GhostRacer stuff

GhostRacer::GhostRacer(StudentWorld* stuPtr, double x, double y): Agent(IID_GHOST_RACER, x, y, 90, 4.0, 0,true, stuPtr,0, 0, true, 100,0)
{
    m_numHolyWaters = 10;
}

GhostRacer::~GhostRacer()
{}

int GhostRacer::getNumHolyWaters() const
{
    return m_numHolyWaters;
}

void GhostRacer::setNumHolyWaters(int num)
{
    m_numHolyWaters = num;
}

void GhostRacer::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    if(getX() <= ROAD_CENTER - ROAD_WIDTH/2)
    {
        if(getDirection() > 90)
        {
            setHealth(getHealth() - 10);
            if(!isAlive())
            {
                return;
            }
        }
        setDirection(82);
        getStuPtr() -> playSound(SOUND_VEHICLE_CRASH);
    }
    if(getX() >= ROAD_CENTER + ROAD_WIDTH/2)
    {
        if(getDirection() < 90)
        {
            setHealth(getHealth() - 10);
            if(!isAlive())
            {
                return;
            }
        }
        setDirection(98);
        getStuPtr() -> playSound(SOUND_VEHICLE_CRASH);
    }
    int ch;
    if(getStuPtr() -> getKey(ch))
    {
        int dir = getDirection();
        switch(ch)
        {
            case KEY_PRESS_SPACE:
                if(m_numHolyWaters >= 1)
                {
                    //adding a new holy water projectile to the screen
                    getStuPtr() -> AddActor(new Spray(getStuPtr(), getX() + (SPRITE_HEIGHT *  cos((getDirection()) * ((M_PI)/180))), getY() + (SPRITE_HEIGHT *  sin((getDirection()) * ((M_PI)/180))), getDirection()));
                    getStuPtr() -> playSound(SOUND_PLAYER_SPRAY);
                    m_numHolyWaters--;
                }
                break;
            case KEY_PRESS_LEFT:
                if(dir < 114)
                {
                    setDirection(dir + 8);
                }
                break;
            case KEY_PRESS_RIGHT:
                if(dir > 66)
                {
                    setDirection(dir - 8);
                }
                break;
            case KEY_PRESS_UP:
                if(getVertSpeed() < 5)
                {
                    setVertSpeed(getVertSpeed() + 1);
                }
                break;
            case KEY_PRESS_DOWN:
                if(getVertSpeed() > -1)
                {
                    setVertSpeed(getVertSpeed() - 1);
                }
                break;
        }
    }
    
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double delta_x = cos((direction) * ((M_PI)/180)) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::setHealth(int currHealth)
{
    //you cannot heal greater than possible, 100 is the maximum
    if(currHealth > 100)
    {
        currHealth = 100;
    }
    Agent::setHealth(currHealth);
    if(getHealth() <= 0)
    {
        getStuPtr() -> playSound(SOUND_PLAYER_DIE);
    }
}

void GhostRacer::spinSelf()
{
    int initialSpin = randInt(5, 20);
    int direction = randInt(1,2);
    
    if(direction == 1)
    {
        initialSpin = initialSpin * (-1);
        if(initialSpin + getDirection() < 60)
        {
            initialSpin = initialSpin * (-1);
            if(initialSpin + getDirection() > 120)
            {
                initialSpin = 0;
            }
        }
    }
    setDirection(getDirection() + initialSpin);
    
}

//Zombie Cab Stuff
ZombieCab::ZombieCab(StudentWorld* stuPtr,double vertSpeed, double x, double y): Agent(IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, true, stuPtr, vertSpeed, 0, true, 3, 0)
{
    m_damagedGhostRacerYet = false;
}

ZombieCab::~ZombieCab()
{}

void ZombieCab::setHealth(int currHealth)
{
    if(currHealth <= 0)
    {
        Agent::setHealth(currHealth);
        setDie();
        //play sound when the zombie cab dies
        getStuPtr() -> playSound(soundWhenDie());
        //add an oil slick by chance
        int chanceOilSlick = randInt(1,5);
        if(chanceOilSlick == 1)
        {
            int randSize = randInt(2, 5);
            getStuPtr() -> AddActor(new OilSlick(getX(), getY(),randSize, getStuPtr()));
        }
        //don't forget to increase the score
        getStuPtr() -> increaseScore(200);
        return;
    }
    if(currHealth > 0)
    {
        Agent::setHealth(currHealth);
        getStuPtr() -> playSound(soundWhenHurt());
    }
}

bool ZombieCab::beSprayedIfAppropriate()
{
    setHealth(getHealth() - 1);
    return true;
}

void ZombieCab::doSomething()
{
    
    if(!isAlive())
    {
        return;
    }
    StudentWorld* temp = getStuPtr();
    GhostRacer* ghost = temp -> getGhostPtr();
    bool didStep = false;
    if(temp -> isOverlap(this))
    {
        if(m_damagedGhostRacerYet)
        {
            moveWithGhost();
            didStep = true;
        }else{
            temp -> playSound(SOUND_VEHICLE_CRASH);
            ghost-> setHealth(ghost ->getHealth() - 20);
            if(getX() <= (ghost -> getX()))
            {
                setHorizSpeed(-5);
                int randInc = randInt(0, 19);
                setDirection(120 + randInc);
            }else if(getX() > (ghost -> getX()))
            {
                setHorizSpeed(5);
                int randDec = randInt(0, 19);
                setDirection(60 - randDec);
            }
            m_damagedGhostRacerYet = true;
        }
    }
    if(!didStep)
    {
        moveWithGhost();
    }
    
    //check for the nearest possible actor in front of the zombie cab
    if((getVertSpeed() > ghost -> getVertSpeed()) && (temp -> isActorFrontBack(this, true)))
    {
        if(temp -> nearbyYCoord(true, this))
        {
            setVertSpeed(getVertSpeed() - 0.5);
            return;
        }
    }else if((getVertSpeed() <= ghost -> getVertSpeed()) && (temp -> isActorFrontBack(this, false)))
    {
        //checks for the nearest possible actor in the back of the zombie cab
        if(temp -> nearbyYCoord(false, this))
        {
            setVertSpeed(getVertSpeed() + 0.5);
            return;
        }
    }
    
    
    
    setMovePlan(getMovePlan() - 1);
    if(getMovePlan() > 0)
    {
        return;
    }
    int randLength = randInt(4,32);
    setMovePlan(randLength);
    int speedIncrement = randInt(-2, 2);
    setVertSpeed(getVertSpeed() + speedIncrement);
    
    
}



//Borderline Stuff

BorderLine::BorderLine(int imageId,StudentWorld* stuPtr, double x, double y):Actor(imageId, x, y, 0, 2.0, 2,false, stuPtr,-4, 0, true)
{}

BorderLine::~BorderLine()
{}

void BorderLine::doSomething()
{
    moveWithGhost();
    
}

//Pedestrian stuff
Pedestrian::Pedestrian(int imageId, double x, double y, StudentWorld* stuPtr, double size):Agent(imageId, x, y, 0, size, 0, true, stuPtr, -4, 0, true, 2, 0)
{}

Pedestrian::~Pedestrian()
{}

int Pedestrian::soundWhenHurt() const
{
    return SOUND_PED_HURT;
}

int Pedestrian::soundWhenDie() const
{
    return SOUND_PED_DIE;
}

void Pedestrian::moveAndPickPlan()
{
    //general algorithm for moving and picking plan, same for both types of pedestrians
    int num1 = randInt(1, 3);
    int num2 = randInt(1, 2);
    if(num2 == 1)
    {
        num1 = num1 * (-1);
    }
    setHorizSpeed(num1);
    int newMovePlan = randInt(4,32);
    setMovePlan(newMovePlan);
    
    if(getHorizSpeed() < 0)
    {
        setDirection(180);
    }else if(getHorizSpeed() > 0){
        setDirection(0);
    }
}


//HumanPed stuff
HumanPed::HumanPed(double x, double y, StudentWorld* stuPtr): Pedestrian(IID_HUMAN_PED, x, y, stuPtr, 2.0)
{}

HumanPed::~HumanPed()
{}

void HumanPed::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    StudentWorld* temp = getStuPtr();
    
    //check this once
    if(temp ->  isOverlap(this))
    {
        temp -> getGhostPtr() -> setDie();
        return;
    }
    moveWithGhost();
    setMovePlan(getMovePlan() - 1);
    if(getMovePlan() > 0)
    {
        return;
    }
    moveAndPickPlan();
    
}

bool HumanPed::beSprayedIfAppropriate()
{
    setHorizSpeed(-1 * getHorizSpeed());
    setDirection(-1 * getDirection());
    getStuPtr() -> playSound(soundWhenDie());
    return true;
}

//ZombiePed stuff
ZombiePed::ZombiePed(double x, double y, StudentWorld* stuPtr): Pedestrian(IID_ZOMBIE_PED, x, y, stuPtr, 3.0)
{
    m_ticksTillGrunt = 0;
}

ZombiePed::~ZombiePed()
{}

bool ZombiePed::beSprayedIfAppropriate()
{
    setHealth(getHealth() - 1);
    
    if(!isAlive())
    {
        int chanceHealGoodie = randInt(1, 5);
        if(chanceHealGoodie == 1)
        {
            getStuPtr() -> AddActor(new HealingGoodie(getX(), getY(), getStuPtr()));
        }
        getStuPtr() -> increaseScore(150);
    }
    return true;
}

void ZombiePed::setHealth(int currHealth)
{
    if(currHealth <= 0)
    {
        Agent::setHealth(currHealth);
        setDie();
        getStuPtr() -> playSound(soundWhenDie());
    }else if(currHealth < getHealth())
    {
        Agent::setHealth(currHealth);
        soundWhenHurt();
    }
}

void ZombiePed::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    StudentWorld* temp = getStuPtr();
    
    if(temp -> isOverlap(this))
    {
        temp -> getGhostPtr() -> setHealth((temp -> getGhostPtr() -> getHealth()) - 5);
        setHealth(getHealth() - 2);
        temp ->increaseScore(150);
        return;
    }
    
    int currX = getX();
    int compareX = temp -> getGhostPtr() -> getX();
    //in case of a collision with the ghost racer
    if(currX >= (compareX - 30) && currX <= (compareX + 30) && getY() > (temp -> getGhostPtr() -> getY()))
    {
        setDirection(270);
        if(currX < compareX)
        {
            setHorizSpeed(1);
        }else if(currX > compareX)
        {
            setHorizSpeed(-1);
        }else{
            setHorizSpeed(0);
        }
        m_ticksTillGrunt--;
        if(m_ticksTillGrunt <= 0)
        {
            temp -> playSound(SOUND_ZOMBIE_ATTACK);
            m_ticksTillGrunt = 20;
        }
    }else{
        //don't forget to decrement ticks
        m_ticksTillGrunt--;
    }
    
    moveWithGhost();
    if(getMovePlan() > 0)
    {
        setMovePlan(getMovePlan() - 1);
        return;
    }
    moveAndPickPlan();
    
}

//GhostRacerActivatedObject stuff
GhostRacerActivatedObject::GhostRacerActivatedObject(int imageId, double x, double y, int dir, double size, StudentWorld* stuPtr): Actor(imageId, x, y, dir, size, 2.0, false, stuPtr, -4, 0, true)
{}

GhostRacerActivatedObject::~GhostRacerActivatedObject()
{}

void GhostRacerActivatedObject::doSomething()
{
    moveWithGhost();
    doActivity();
}

int GhostRacerActivatedObject::getSound() const
{
    return SOUND_GOT_GOODIE;
}

bool GhostRacerActivatedObject::isSprayable() const
{
    return false;
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate()
{
    if(isSprayable())
    {
        setDie();
        return true;
    }
    return false;
}

//Oil Slick stuff
OilSlick::OilSlick(double x, double y,int size, StudentWorld* stuPtr): GhostRacerActivatedObject(IID_OIL_SLICK, x, y, 0, size, stuPtr)
{}

OilSlick::~OilSlick()
{}

void OilSlick::doActivity()
{
    StudentWorld* temp = getStuPtr();
    if(temp -> isOverlap(this))
    {
        temp -> playSound(getSound());
        temp -> getGhostPtr() -> spinSelf();
    }
    
}

int OilSlick::getScoreIncrease() const
{
    return 0;
}

int OilSlick::getSound() const
{
    return SOUND_OIL_SLICK;
}

//healing gooodie stuff

HealingGoodie::HealingGoodie(double x, double y, StudentWorld* stuPtr): GhostRacerActivatedObject(IID_HEAL_GOODIE, x, y, 0, 1.0, stuPtr)
{}

HealingGoodie::~HealingGoodie()
{}

int HealingGoodie::getScoreIncrease() const
{
    return 250;
}

void HealingGoodie::doActivity()
{
    StudentWorld* temp = getStuPtr();
    GhostRacer* ghost = temp -> getGhostPtr();
    if(temp -> isOverlap(this))
    {
        ghost -> setHealth((ghost -> getHealth()) + 10);
        setDie();
        temp -> playSound(GhostRacerActivatedObject::getSound());
        temp -> increaseScore(getScoreIncrease());
    }
}

bool HealingGoodie::isSprayable() const
{
    return true;
}



//Holy Water Goodie
HolyWaterGoodie::HolyWaterGoodie(double x, double y, StudentWorld* stuPtr): GhostRacerActivatedObject(IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, stuPtr)
{}

HolyWaterGoodie::~HolyWaterGoodie()
{}

int HolyWaterGoodie::getScoreIncrease() const
{
    return 50;
}


void HolyWaterGoodie::doActivity()
{
    StudentWorld* temp = getStuPtr();
    GhostRacer* ghost = temp -> getGhostPtr();
    if(temp -> isOverlap(this))
    {
        ghost -> setNumHolyWaters(ghost -> getNumHolyWaters() + 10);
        setDie();
        temp -> playSound(GhostRacerActivatedObject::getSound());
        temp -> increaseScore(getScoreIncrease());
    }
    
}

bool HolyWaterGoodie::isSprayable() const
{
    return true;
}


//Soul goodie stuff
Soul::Soul(double x, double y, StudentWorld* stuPtr): GhostRacerActivatedObject(IID_SOUL_GOODIE, x, y, 0, 4.0,stuPtr)
{}

Soul::~Soul()
{}

int Soul::getScoreIncrease() const
{
    return 100;
}

int Soul::getSound() const
{
    return SOUND_GOT_SOUL;
}

void Soul::doActivity()
{
    StudentWorld* myStu = getStuPtr();
    if(myStu -> isOverlap(this))
    {
        myStu -> setNumSouls((myStu -> getNumSouls()) + 1);
        
        if(myStu -> getNumSouls() == ((myStu -> getLevel() * 2) + 5))
        {
            myStu -> setLevelComplete(true);
        }
        setDie();
        myStu -> playSound(getSound());
        myStu -> increaseScore(getScoreIncrease());
        
    }
    
    setDirection(getDirection() - 10);
}

//Spray stuff

Spray::Spray(StudentWorld* stuPtr, double x, double y, int dir): Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, false, stuPtr, 0, 0)
{
    m_maxTravelDist = 160;
}

Spray::~Spray()
{}

void Spray::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    
    if(getStuPtr() -> sprayFirstAppropriateActor(this))
    {
        setDie();
        return;
    }
    
    moveForward(SPRITE_HEIGHT);
    m_maxTravelDist -= SPRITE_HEIGHT;
    goneOffScreen();
    if(m_maxTravelDist <= 0)
    {
        setDie();
    }
    
}
