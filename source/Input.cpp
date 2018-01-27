#include <functional>
#include "Input.hpp"

Input Input::instance = Input();

void Input::setWindow(GLFWwindow *window)
{
  // set glfw callback
  glfwSetKeyCallback(window, [] (GLFWwindow *window, int key, int scancode, int action, int mode) {
      Event ev;
      ev.hasEvent = true;
      ev.window = window;
      ev.type = Event::KEY;
      ev.val.key = { key, scancode, action, mode };
      Input::instance.events.push(ev);
    });
  glfwSetCursorPosCallback(window, [] (GLFWwindow *window, double x, double y) {
      Event ev;
      ev.hasEvent = true;
      ev.window = window;
      ev.type = Event::MOUSE;
      ev.val.key = { static_cast<int>(x), static_cast<int>(y), 0, 0 };
      Input::instance.events.push(ev);
    });
  glfwSetMouseButtonCallback(window, [] (GLFWwindow *window, int button, int action, int mods) {
      Event ev;
      ev.hasEvent = true;
      ev.window = window;
      ev.type = Event::BUTTON;
      ev.val.button = { button, action, mods };
      Input::instance.events.push(ev);
    });
}

Event Input::pollEvent()
{
  if (Input::instance.events.empty())
    {
      Event ev;
      ev.hasEvent = false;
      return ev;
    }

  Event event = Input::instance.events.front();

  Input::instance.events.pop();

  return event;
}
