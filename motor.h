
enum Direction { FORWARD, BACKWARD };
enum Speed { FULL_SPEED, HALF_SPEED, QUARTER_SPEED, STOP };

struct Motor {
  unsigned int forwardPin;
  unsigned int backwardsPin;
  unsigned int speedPin;
};

void motor(Motor motor, enum Direction direction, enum Speed speed);
