/*
 * File: frontend/display/Display.cpp
 */

#include "frontend/display/Display.hpp"

using namespace std;

namespace PlayToLearn {
namespace Frontend {

/** Display member functions, public */

Display::Display() :
  request_interaction_fn_(NULL),
  register_interaction_fn_(NULL),
  draw_scene_fn_(NULL)
{
  // empty body
}

Display::~Display() {
  // empty body
}

void Display::set_request_interaction_fn(InteractionPrompt::Ptr (*fn)(Util::UniqueID<Backend::Object> id)) {
  request_interaction_fn_ = fn;
}

void Display::set_register_interaction_fn(string (*fn)(Interaction::Ptr response)) {
  register_interaction_fn_ = fn;
}

void Display::set_draw_scene_fn(void (*fn)()) {
  draw_scene_fn_ = fn;
}

/** Display member functions, protected */

InteractionPrompt::Ptr Display::request_interaction(Util::UniqueID<Backend::Object> id) {
  return request_interaction_fn_(id);
}

string Display::register_interaction(Interaction::Ptr interaction) {
  return register_interaction_fn_(interaction);
}

void Display::draw_scene() {
  draw_scene_fn_();
}

} // namespace Frontend
} // namespace PlayToLearn
