#ifndef BUTT_H
#define BUTT_H

#define CHECKVAL(pullup) ((pullup) ? LOW : HIGH)

class Button {

  public:
    Button(int pin);
    Button(int pin, bool pullup);
    Button(int pin, bool pullup, unsigned long min_interval);
    bool check();
  private:
    int pin;
    bool last;
    bool pullup;
    unsigned long min_interval;
    unsigned long last_bounce;
    bool check_value(int value);
    bool check_interval(unsigned long value);
};

#endif
