#include "ContactListener.hpp"
#include "Box.hpp"
#include "Ground.hpp"

void ContactListener::BeginContact(b2Contact* contact){
    
    void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if(bodyUserData){
        static_cast<Box*>(bodyUserData)->startContact();
    }
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if(bodyUserData){
        static_cast<Ground*>(bodyUserData)->startContact();
    }
    // void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    // if ( (int)*fixtureUserData == 3 )
    //           number++;
    // fixtureUserData = contact->GetFixtureB()->GetUserData();
    // if ( (int)*fixtureUserData == 3 )
    //           number++;
}
void ContactListener::EndContact(b2Contact* contact){
    void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if(bodyUserData){
        static_cast<Box*>(bodyUserData)->endContact();
    }
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if(bodyUserData){
        static_cast<Ground*>(bodyUserData)->endContact();
    }
    // void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    //       if ( (int)fixtureUserData == 3 )
    //           number--;
    //       fixtureUserData = contact->GetFixtureB()->GetUserData();
    //       if ( (int)fixtureUserData == 3 )
    //           number--;
}
void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){};
void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){};