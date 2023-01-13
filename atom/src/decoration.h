#ifndef DECORATION_H_
#define DECORATION_H_

class DisplayMarker;
class DecorationManager;

class Decoration {
public:
  enum class Type {
    line,
    line_number,
    text,
    highlight,
    overlay,
    gutter,
    block,
    cursor
  };
  struct Properties {
    Type type;
    const char *class_;
    bool onlyHead;
    bool onlyEmpty;
    bool onlyNonEmpty;
    bool omitEmptyLastRow;
    Properties();
    Properties(Type, const char * = nullptr);
  };

private:
  DisplayMarker *marker;
  DecorationManager *decorationManager;
  Properties properties;

public:
  Decoration(DisplayMarker *, DecorationManager *, Properties);
  ~Decoration();

  void destroy();

  Properties getProperties();
  void setProperties(Properties);
};

#endif  // DECORATION_H_
