#ifndef LOGIC_HPP_
# define LOGIC_HPP_

# include <random>
# include <algorithm>
# include <vector>
# include <memory>
# include <chrono>
# include <mutex>
# include "Input.hpp"
# include "EntityManager.hpp"
# include "Laser.hpp"

class Display;

class Logic
{
public:
  using Clock = std::conditional<std::chrono::high_resolution_clock::is_steady,
                                 std::chrono::high_resolution_clock,
                                 std::chrono::steady_clock>::type;

  decltype(Clock::now()) lastUpdate;

  static constexpr std::chrono::microseconds const getTickTime()
  {
    return std::chrono::microseconds(1000000 / 60);
  };
private:
  EntityManager entityManager;

  unsigned int timer;
  unsigned long int score;
  double multiplier;

  bool restart;
  bool gameOver;
  bool tutoPage;
  bool startPage;
  bool running;
  std::vector<Laser> lasers;

  // input variables
  claws::Vect<2u, double> mousePos;
  claws::Vect<2u, double> dragOrigin;
  // mouse variables
  bool leftClick;
  bool rightClick;

  std::size_t updatesSinceLastFrame;


  void handleKey(GLFWwindow *window, Key key);
  void handleMouse(Display const &, GLFWwindow *window, Mouse mouse);
  void handleButton(GLFWwindow *window, Button button);
  void selectBots();
  void selectRect(claws::Vect<2u, double> start, claws::Vect<2u, double> end);

public:
  Logic(bool animation = true);

  void handleEvent(Display const &, Event const& event);
  void checkEvents(Display const &);
  void selectRect(claws::Vect<2u, double>, claws::Vect<2u, double>, claws::Vect<4u, bool>);
  void update();
  void moveSelection(claws::Vect<2u, double> target);
  void tick(std::mutex &lock);
  void addToScore(int);
  void addToTimer(unsigned int);

  template<class... Args>
  void addLaser(Args &&...args)
  {
    lasers.emplace_back(std::forward<Args>(args)...);
  }

  EntityManager	getEntityManager(void) const;
  claws::Vect<2, double> getPlayerPos(void) const;
  claws::Vect<2u, double> getMouse(Display const &) const;
  std::string	getTimer(void) const;
  std::string	getScore(void) const;
  bool          getGameOver(void) const;
  bool          getRestart(void) const;
  bool          getStartPage(void) const;
  bool          getTutoPage(void) const;

  bool		isRunning() const
  {
    return running;
  }

  bool		&isRunning()
  {
    return running;
  }
};

#endif // !LOGIC_HPP_
