#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp



StudentWorld::StudentWorld(string assetPath) //my constructor for StudentWorld
: GameWorld(assetPath)
{
    m_bonus = 5000;
    m_numActors = 0;
    m_yWhite = 0;
    m_ghost = nullptr;
    m_souls = 0;
    m_levelComplete = false;
}

int StudentWorld::init()
{
    m_souls = 0;
    m_yWhite = 0;
    m_levelComplete = false;
    m_bonus = 5000;
    m_numActors = 0;
    //creating the n yellow border line objects
    int n = VIEW_HEIGHT/SPRITE_HEIGHT;

    const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    for(int j = 0; j < n; j++)
    {
        //The IID is only for construction and was not stored as a data member
        //From here on out whenever the imageId is passed for a border it was not used for distinction, only construction purposes
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE,this,LEFT_EDGE, j*SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE,this,RIGHT_EDGE, j*SPRITE_HEIGHT));
    }
    m_numActors += 2*n;
    
    //creating the m white border line objects
    int m = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for(int j = 0; j < m; j++)
    {
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE,this,LEFT_EDGE + ROAD_WIDTH/3, j*(4*SPRITE_HEIGHT)));
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE,this,RIGHT_EDGE - ROAD_WIDTH/3, j*(4*SPRITE_HEIGHT)));
    }
    m_yWhite = (m-1) * (4 * SPRITE_HEIGHT);
    m_numActors += 2*m;
    
    //creating a new GhostRacer object
    m_ghost = new GhostRacer(this,128,32);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    if(m_ghost -> isAlive())
    {
        m_ghost -> doSomething();
        if(!(m_ghost -> isAlive()))
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if(m_levelComplete)
        {
            increaseScore(m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    //every actor gets a chance to do something
    list<Actor*>::iterator p = m_actors.begin();
    for(; p!=m_actors.end(); p++)
    {
        if((*p) -> isAlive())
        {
            (*p) -> doSomething();
            if(!(m_ghost -> isAlive()))
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }
    //removes the dead actors if they go offscreen or die
    list<Actor*>::iterator q = m_actors.begin();
    for(; q != m_actors.end(); q++)
    {
        if(!((*q) ->isAlive()))
        {
            Actor* temp = *q;
            q = m_actors.erase(q);
            delete temp;
            q--;
            m_numActors--;
        }
        
    }
    //add new yellow border lines
    m_yWhite = m_yWhite - (m_ghost -> getVertSpeed()) - 4;
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - m_yWhite;
    if(delta_y >= SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE,this,ROAD_CENTER - ROAD_WIDTH/2, new_border_y));
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE,this,ROAD_CENTER + ROAD_WIDTH/2, new_border_y));
        m_numActors += 2;
    }
    //add new white borderlines
    if(delta_y >= 4 * SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE,this,ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3, new_border_y));
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE,this,ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3, new_border_y));
        m_yWhite = new_border_y;
        m_numActors += 2;
    }
    
    //add new human peds
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int hPedNum = randInt(0, ChanceHumanPed - 1);
    if(hPedNum == 0)
    {
        int hPedX = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new HumanPed(hPedX, VIEW_HEIGHT, this));
    }
    m_numActors++;
    
    //add new zombie peds
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int zPedNum = randInt(0,ChanceZombiePed - 1);
    if(zPedNum == 0)
    {
        int zPedX = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new ZombiePed(zPedX, VIEW_HEIGHT, this));
    }
    m_numActors++;
    
    //add new oil slicks
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int oilNum = randInt(0, ChanceOilSlick - 1);
    if(oilNum == 0)
    {
        int oilX = randInt((ROAD_CENTER - ROAD_WIDTH/2) ,(ROAD_CENTER + ROAD_WIDTH/2) );
        int randSize = randInt(2, 5);
        m_actors.push_back(new OilSlick(oilX, VIEW_HEIGHT, randSize, this));
    }
    m_numActors++;
    
    //add new Holy Water Refill Goodies
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int holyWaterNum = randInt(0, ChanceOfHolyWater - 1);
    if(holyWaterNum == 0)
    {
        int holyX = randInt((ROAD_CENTER - ROAD_WIDTH/2) ,(ROAD_CENTER + ROAD_WIDTH/2) );
        m_actors.push_back(new HolyWaterGoodie(holyX, VIEW_HEIGHT,this));
    }
    m_numActors++;
    
    //add new souls as needed
    int ChanceOfLostSoul = 100;
    int toAdd = randInt(0, ChanceOfLostSoul - 1);
    if(toAdd == 0)
    {
        int randomX = randInt((ROAD_CENTER - ROAD_WIDTH/2) + 1,(ROAD_CENTER + ROAD_WIDTH/2) - 1);
        m_actors.push_back(new Soul(randomX, VIEW_HEIGHT, this));
    }
    m_numActors++;
    
    //add a new Zombie Cab
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int randVehicle = randInt(0, ChanceVehicle - 1);
    
    if(randVehicle == 0)
    {
        int laneNum = randInt(0,2); //0 is left, 1 is middle, and 2 is right lane
        bool lanes[3] = {false, false, false};
        lanes[laneNum] = true;
        bool added = false;
        int count = 0;
        while((count < 3) && !added)
        {
            double zombCabX = -1;
            switch(laneNum)
            {
                case 0:
                    zombCabX = ROAD_CENTER - ROAD_WIDTH/3;
                    break;
                case 1:
                    zombCabX = ROAD_CENTER;
                    break;
                case 2:
                    zombCabX = ROAD_CENTER + ROAD_WIDTH/3;
                    break;
                default:
                    break;
            }
            
            
            //checks if adding at bottom is possible
            Actor* bottom = CloseBottomTop(laneNum + 1, true);
            if((bottom == nullptr) || ((bottom -> getY()) > (VIEW_HEIGHT/3)))
            {
                int randIncSpeed = randInt(2, 4);
                double zIncSpeed = randIncSpeed + (m_ghost -> getVertSpeed());
                m_actors.push_back(new ZombieCab(this,zIncSpeed,zombCabX, SPRITE_HEIGHT/2));
                m_numActors++;
                added = true;
                break;
            }
            
            //checks if adding at top of lane is possible
            Actor* top = CloseBottomTop(laneNum + 1, false);
            if((top == nullptr) || (((top -> getY()) < (VIEW_HEIGHT * 2/3))))
            {
                int randDecSpeed = randInt(2, 4);
                double zDecSpeed = (m_ghost -> getVertSpeed()) - randDecSpeed;
                m_actors.push_back(new ZombieCab(this,zDecSpeed,zombCabX, VIEW_HEIGHT - SPRITE_HEIGHT/2));
                m_numActors++;
                added = true;
                break;
            }
            
            count++;
            if(count == 1)
            {
                int chooseInt = randInt(1, 2);
                if(laneNum == 0)
                {
                    if(chooseInt == 1)
                    {
                        laneNum = 1;
                    }else{
                        laneNum = 2;
                    }
                }
                
                if(laneNum == 1)
                {
                    if(chooseInt == 1)
                    {
                        laneNum = 0;
                    }else{
                        laneNum = 2;
                    }
                }
                
                if(laneNum == 2)
                {
                    if(chooseInt == 1)
                    {
                        laneNum = 0;
                    }else{
                        laneNum = 1;
                    }
                }
                
            }
            if(count == 2)
            {
                for(int i = 0; i < 2; i++)
                {
                    if(lanes[i] == false)
                    {
                        laneNum = i;
                    }
                }
            }
            
        }
    }
    
    
    
    
    
    
    
    
    //decreasing the bonus by one every tick
    m_bonus--;
    
    //setting the stats for the screen
    ostringstream oss;
    oss<<"Score: "<<getScore();
    oss<<"  Lvl: "<<getLevel();
    oss<<"  Souls2Save: "<< ((getLevel() * 2) + 5) - getNumSouls();
    oss<<"  Lives: "<<getLives();
    oss<<"  Health: "<<m_ghost -> getHealth();
    oss<<"  Sprays: "<<m_ghost -> getNumHolyWaters();
    oss<<"  Bonus: "<<m_bonus;
    
    string s = oss.str();
    setGameStatText(s);
    
    
    return GWSTATUS_CONTINUE_GAME;
    //decLives();
    //return GWSTATUS_PLAYER_DIED;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::cleanUp()
{
    delete m_ghost;
    
    //loops through all the Actor pointers and deletes and removes the Actors from list
    list<Actor*>::iterator p = m_actors.begin();
    for(; p != m_actors.end(); p++)
    {
        Actor* temp = *p;
        p = m_actors.erase(p);
        delete temp;
        p--;
    }
    m_actors.clear();
}

GhostRacer* StudentWorld::getGhostPtr() const
{
    return m_ghost;
}

int StudentWorld::getNumSouls() const
{
    return m_souls;
}

void StudentWorld::setNumSouls(int souls)
{
    m_souls = souls;
}

void StudentWorld::setLevelComplete(bool completion)
{
    m_levelComplete = completion;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* a)
{
    list<Actor*>::iterator temp = m_actors.begin();
    for(; temp != m_actors.end(); temp++)
    {
        if((a != (*temp)) && determineOverlap(a, *temp) && ((*temp) -> beSprayedIfAppropriate()))
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isOverlap(Actor* anActor) const
{
    return determineOverlap(m_ghost, anActor);
}

bool StudentWorld::determineOverlap(Actor* a1, Actor* a2) const
{
    double delta_x = (a1 -> getX()) - (a2 -> getX());
    if(delta_x < 0)
    {
        delta_x = -1 * delta_x;
    }
    
    double delta_y = (a1 -> getY()) - (a2 -> getY());
    if(delta_y < 0)
    {
        delta_y = -1 * delta_y;
    }
    
    double radius_sum = a1 -> getRadius() + a2 -> getRadius();
    if((delta_x < radius_sum * 0.25) && (delta_y < radius_sum * 0.6))
    {
        return true;
    }
    return false;
}


int StudentWorld::determineWhichLane(Actor* actor) const
{
    double currentX = actor -> getX();
    //checks the first lane
    if(currentX >= EDGE_ONE && currentX < EDGE_TWO)
    {
        return 1;
    }
    //checks the second lane
    if(currentX >= EDGE_TWO && currentX < EDGE_THREE)
    {
        return 2;
    }
    //checks the third lane
    if(currentX >= EDGE_THREE && currentX < EDGE_FOUR)
    {
        return 3;
    }
    return -1;
}


bool StudentWorld::areInSameLane(Actor* actor1, Actor* actor2) const
{
    int laneActor1 = determineWhichLane(actor1);
    int laneActor2 = determineWhichLane(actor2);
    //if both Actors in same lane, return true
    if(laneActor1 == laneActor2)
    {
        return true;
    }
    return false;
}


bool StudentWorld::nearbyYCoord(bool isTop, Actor* actorPtr)
{
    //if actor above present actor, check to see if within 96 pixels of vertical distance
    //don't forget ghost racer
    if(isTop)
    {
        double minDist = VIEW_HEIGHT + 1;
        double currY = actorPtr -> getY();
        if(areInSameLane(actorPtr, m_ghost) && ((m_ghost -> getY())> currY))
        {
            double tempDist = (m_ghost -> getY()) - currY;
            if(tempDist < minDist)
            {
                minDist = tempDist;
            }
        }
        //looping through to see if any actor meets the 96 pixel rule
        list<Actor*>::iterator temp = m_actors.begin();
        for(; temp != m_actors.end(); temp++)
        {
            if( ((*temp) != actorPtr) && ((*temp) -> isCollisionAvoidWorthy()) && (((*temp) -> getY()) > currY ))
            {
                double temp1dist = ((*temp) -> getY()) - currY;
                if(areInSameLane(actorPtr, *temp))
                {
                    if(temp1dist < minDist)
                    {
                        minDist = temp1dist;
                    }
                }
            }
        }
        return (minDist < 96);
    }else{
        //if actor below present actor, check to see if within 96 pixels of vertical distance
        //exclude ghost racer
        double maxDist = VIEW_HEIGHT - 1;
        double currY = actorPtr -> getY();
        //looping through to see if any actor meets the 96 pixel rule
        list<Actor*>::iterator temp = m_actors.begin();
        for(; temp != m_actors.end(); temp++)
        {
            if( ((*temp) != actorPtr) && ((*temp) -> isCollisionAvoidWorthy()) && (((*temp) -> getY()) < currY ))
            {
                double temp2dist = currY - ((*temp) -> getY());
                if(areInSameLane(actorPtr, *temp))
                {
                    if(temp2dist < maxDist)
                    {
                        maxDist = temp2dist;
                    }
                }
            }
        }
        
        return (maxDist < 96);
    }

}

bool StudentWorld::isActorFrontBack(Actor* anActor, bool isFront) const
{
    bool isForward = false;
    bool isBack = false;
    
    double currY = anActor -> getY();
    double ghostY = m_ghost -> getY();
    
    //checks if a actor is behind ghost racer
    if( areInSameLane((m_ghost), anActor)  && (ghostY > currY))
    {
        isForward = true;
    }
    
    //checks if a actor is behind ghost racer
    if( areInSameLane((m_ghost), anActor)  && (ghostY < currY))
    {
        isBack = true;
    }
    
    list<Actor*>::const_iterator temp1 = m_actors.begin();
    for(; temp1 != m_actors.end(); temp1++)
    {
        //checks for actors behind argument actor
        double tempY = (*temp1) -> getY();
        if( areInSameLane((*temp1), anActor)  && ((*temp1) -> isCollisionAvoidWorthy()) && (tempY > currY))
        {
            isForward = true;
        }
        
        //checks for actors behind argument actor
        if( areInSameLane((*temp1), anActor)  &&  ((*temp1) -> isCollisionAvoidWorthy()) && (tempY < currY))
        {
            isBack = true;
        }
    }
    
    if(isFront)
    {
        return isForward;
    }else{
        return isBack;
    }
}


Actor* StudentWorld::CloseBottomTop(int laneNum,bool isBottom) const
{
    
        Actor* bottom = nullptr;
        Actor* top = nullptr;
        double closestYBottom;
        double closestYTop;
    
    //checks if ghost racer is closest to actor
        if(determineWhichLane(m_ghost) == laneNum)
        {
            closestYBottom = m_ghost -> getY();
            closestYTop = m_ghost -> getY();
            bottom = m_ghost;
            top = m_ghost;
            
        }else{
            closestYBottom = VIEW_HEIGHT + 1;
            closestYTop = -1;
        }
        list<Actor*>::const_iterator temp = m_actors.begin();
        for(; temp != m_actors.end(); temp++)
        {
            
            if((determineWhichLane(*temp) == laneNum) && ((*temp) -> isCollisionAvoidWorthy()))
            {
                double currY = (*temp) -> getY();
                //checks the actor closest behind the argument actor
                if(currY < closestYBottom)
                {
                    bottom = (*temp);
                    closestYBottom = currY;
                }
                //checks the actor closest above the argument actor
                if(currY > closestYTop)
                {
                    top = (*temp);
                    closestYTop = currY;
                }
            }
        }
    if(isBottom)
    {
        return bottom;
    }else{
        return top;
    }
}

void StudentWorld::AddActor(Actor* anActor)
{
    //adding pointer to actor to the actor pointer container
    m_actors.push_back(anActor);
    m_numActors++;
}



