#ifndef EVENT_H
#define EVENT_H

#include "types.h"
#include "queue.h"

struct Simulation; // Forward declaration

// Event structure
typedef struct Event {
    double time;
    ActorType actorType;
    void *actor;
    // Function pointer for the action to perform
    void (*action)(struct Event *event, struct Simulation *sim);
} Event;

Event* create_event(double time, ActorType actorType, void *actor, void (*action)(struct Event*, struct Simulation*));
void add_event_sorted(Queue *event_queue, Event *event);

// --- Event Action Functions ---
void handle_customer_arrival(Event *event, struct Simulation *sim);
void handle_service_completion(Event *event, struct Simulation *sim);
void handle_teller_becomes_free(Event *event, struct Simulation *sim);

// --- Helper ---
void log_function_pointer_call(const char* function_name, double clock_time);


#endif // EVENT_H
