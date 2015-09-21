#include <vector>

class LifeGame {
public:
  LifeGame(int width, int height);

  void nextStep();
  int getWidth();
  int getHeight();
  bool unitAliveAt(int x, int y);

private:
  typedef std::vector<std::vector<bool> > field_t;
  field_t field;
  int width;
  int height;
};
