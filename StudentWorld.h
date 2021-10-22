#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class GhostRacer;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGhostPtr() const; //returns pointer to GhostRacer
    int getNumSouls() const; //returns the number of souls that the player has collected in current level
    void setNumSouls(int souls); //sets the number of souls player collected
    void setLevelComplete(bool completion); //sets level completion
    bool nearbyYCoord(bool isTop, Actor* actorPtr); //this is primarily for zombie cab and checking for the closest such actor
    bool sprayFirstAppropriateActor(Actor* a); //checks for first sprayable actor and sprays them
    bool isOverlap(Actor* anActor) const; //checks overlap with ghostracer for now
    bool isActorFrontBack(Actor* anActor, bool isFront) const;
    void AddActor(Actor* anActor); //adds an actor pointer to the container of actor pointers
    

private:
    int m_bonus;
    int m_numActors; //number of actors that are currently in the game
    std::list<Actor*> m_actors; //my Actor pointers stored in a list
    GhostRacer* m_ghost; //pointer to the GhostRacer
    int m_yWhite; //y coordinate of the last white borderline added
    int m_souls; //number of souls cuurently saved
    bool m_levelComplete; //whether the level is complete or not
    bool areInSameLane(Actor* actor1, Actor* actor2) const; //checks whether both actors are in the same lane or not
    int determineWhichLane(Actor* actor) const;
    Actor* CloseBottomTop(int laneNum, bool isBottom) const; //takes in isBottom param to check for closest Actor near bottom or top, if none, returns the nullptr, takes in 1, 2, or 3 for the lane number
    bool determineOverlap(Actor* a1, Actor* a2) const;
    
};

#endif // STUDENTWORLD_H_
