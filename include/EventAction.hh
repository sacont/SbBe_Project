#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"

class G4Event;

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    ~EventAction() override;

    void EndOfEventAction(const G4Event* event) override;

private:
    long fProgressInterval = 1000000;
};

#endif
