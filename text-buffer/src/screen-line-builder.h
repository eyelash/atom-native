#ifndef SCREEN_LINE_BUILDER_H_
#define SCREEN_LINE_BUILDER_H_

class DisplayLayer;

class ScreenLineBuilder {
  DisplayLayer *displayLayer;

public:
  ScreenLineBuilder(DisplayLayer *);
  ~ScreenLineBuilder();
};

#endif  // SCREEN_LINE_BUILDER_H_
