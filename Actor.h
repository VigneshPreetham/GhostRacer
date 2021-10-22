#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//edges for the lanes on the road (x-coords)
const int EDGE_ONE = ROAD_CENTER - ROAD_WIDTH/2;
const int EDGE_TWO = ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3;
const int EDGE_THREE = ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3;
const int EDGE_FOUR = ROAD_CENTER + ROAD_WIDTH/2;


//Actor is an abstract base class because u cannot just have an arbitrary actor (it wouldn't make sense)

class StudentWorld;
class Actor: public GraphObject
{
    public:
        Actor(int imageId, double x, double y, int dir, double size, unsigned int depth, bool isCollision, StudentWorld* stuPtr,double vertSpeed, double horizSpeed, bool isAlive = true);
        bool isAlive() const; //returns the alive state of the actor
        double getVertSpeed() const; //returns vertical speed of actor
        double getHorizSpeed() const; //returns horizontal speed of actor
        void setVertSpeed(double vSpeed); //set the vertical speed of the actor
        void setHorizSpeed(double hSpeed); //set the horizontal speed of the actor
        virtual ~Actor(); //virtual destructor just in case
        virtual void doSomething() = 0; //every actor does something different
        void setDie(); //for things if they go offscreen or if they die
        StudentWorld* getStuPtr() const; //returns ptr to current StudentWorld that actor is in
        bool isCollisionAvoidWorthy() const; //returns whether object is collision avoidance worthy or not
        virtual bool beSprayedIfAppropriate(); //returns whether the Actor was sprayed or not and does the particular action of spraying the Actor
    
    protected:
        void moveWithGhost(); //moving in relation to the ghost racer
        void goneOffScreen(); //checks whether Actor has gone off screen
    
    
    
    private:
        bool m_isAlive; //the is alive state of the Actor
        bool m_isCollisionAvoidWorthy; //to check if something is collision avoidance worthy
        double m_verticalSpeed; //the vertical speed of the Actor
        double m_horizontalSpeed; //the horizontal speed of the Actor
        StudentWorld* m_studentPtr; //pointer to the StudentWorld that the Actor is in
        
    
};

class Agent: public Actor
{
    public:
        Agent(int imageId, double x, double y, int dir, double size, unsigned int depth, bool isCollision, StudentWorld* stuPtr, double vertSpeed, double horizSpeed, bool isAlive, int health, int movePlan);
        virtual ~Agent();
        int getHealth() const;//returns current hit points of actor
        virtual void setHealth(int currHealth);//sets the health of an actor (remove if not needed later)
        virtual int soundWhenHurt() const; //returns sound constant when Agent is hurt
        virtual int soundWhenDie() const; //returns sound constant when Agent dies
        int getMovePlan() const; //returns current move plan of the Agent
        void setMovePlan(int movePlan); //sets the Agent's move plan
    
    private:
        int m_health; //number of hitpoints for the certain agent
        int m_movePlan; //movement plan for that particular agent
        
    
};

class Pedestrian: public Agent
{
    public:
        Pedestrian(int imageId, double x, double y, StudentWorld* stuPtr, double size);
        virtual ~Pedestrian();
        virtual int soundWhenHurt() const;
        virtual int soundWhenDie() const;
        void moveAndPickPlan(); //move pedestrian and pick new movement plan if needed
        
};

class HumanPed: public Pedestrian
{
    public:
        HumanPed(double x, double y, StudentWorld* stuPtr);
        virtual ~HumanPed();
        virtual void doSomething();
        virtual bool beSprayedIfAppropriate();
};

class ZombiePed: public Pedestrian
{
    public:
        ZombiePed(double x, double y, StudentWorld* stuPtr);
        virtual bool beSprayedIfAppropriate();
        virtual ~ZombiePed();
        virtual void doSomething();
        virtual void setHealth(int currHealth);
    private:
        int m_ticksTillGrunt;
};

class GhostRacer: public Agent
{
    public:
        GhostRacer(StudentWorld* stuPtr, double x, double y);
        virtual ~GhostRacer();
        virtual void setHealth(int currHealth);
        void spinSelf(); //spins the ghost racer (new direction) after encountering oil slick
        int getNumHolyWaters() const; //gets the number of holy water charges that the Ghost Racer has currently
        void setNumHolyWaters(int num); //sets the number of holy water charges for Ghost Racer
        virtual void doSomething();
    private:
        int m_numHolyWaters; //number of holy water charges ghost racer has
        
};

class ZombieCab: public Agent
{
    public:
        ZombieCab(StudentWorld* stuPtr,double vertSpeed, double x, double y);
        virtual ~ZombieCab();
        virtual void doSomething();
        virtual bool beSprayedIfAppropriate();
        virtual void setHealth(int currHealth);
    private:
        bool m_damagedGhostRacerYet;
};

class BorderLine: public Actor
{
    public:
        BorderLine(int imageId,StudentWorld* stuPtr, double x, double y);
        virtual ~BorderLine();
        virtual void doSomething();
        
        
};


class GhostRacerActivatedObject: public Actor
{
    public:
        GhostRacerActivatedObject(int imageId, double x, double y, int dir, double size, StudentWorld* stuPtr);
    
        virtual ~GhostRacerActivatedObject();
    
        virtual void doSomething(); //all the doSomething() for goodies are very similar
    
        virtual void doActivity() = 0; //do special activity (depends on goodie)
    
        virtual int getScoreIncrease() const = 0; //return object's increase of score when activated
    
        virtual int getSound() const; //get sound when activated
    
        
        virtual bool isSprayable() const; //determines whether goodie is sprayable
    
        virtual bool beSprayedIfAppropriate();

};

class OilSlick: public GhostRacerActivatedObject
{
    public:
        OilSlick(double x, double y, int size, StudentWorld* stuPtr);
        virtual ~OilSlick();
        virtual void doActivity();
        virtual int getScoreIncrease() const;
        virtual int getSound() const;
        
};

class HealingGoodie: public GhostRacerActivatedObject
{
    public:
        HealingGoodie(double x, double y, StudentWorld* stuPtr);
        virtual ~HealingGoodie();
        virtual void doActivity();
        virtual int getScoreIncrease() const;
        virtual bool isSprayable() const;
};

class HolyWaterGoodie: public GhostRacerActivatedObject
{
    public:
        HolyWaterGoodie(double x, double y, StudentWorld* stuPtr);
        virtual ~HolyWaterGoodie();
        virtual void doActivity();
        virtual int getScoreIncrease() const;
        virtual bool isSprayable() const;
};

class Soul: public GhostRacerActivatedObject
{
    public:
        Soul(double x, double y, StudentWorld* stuPtr);
        virtual void doActivity();
        virtual ~Soul();
        virtual int getScoreIncrease() const;
        virtual int getSound() const;
};

class Spray: public Actor
{
    public:
        Spray(StudentWorld* stuPtr, double x, double y, int dir);
        ~Spray();
        virtual void doSomething();
    private:
        double m_maxTravelDist; //maximum travel distance
        
};

#endif // ACTOR_H_
