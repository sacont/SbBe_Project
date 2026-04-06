#include "EventAction.hh"

#include "G4Event.hh"
#include "G4Threading.hh"
#include "G4ios.hh"

EventAction::EventAction() = default;

EventAction::~EventAction() = default;

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (!event) {
        return;
    }

    const auto eventId = static_cast<long>(event->GetEventID()) + 1;
    if (eventId % fProgressInterval != 0) {
        return;
    }

    G4cout << G4Threading::G4GetThreadId()
           << " progress: completed " << eventId
           << " events in this worker run" << G4endl;
}
