#ifndef DECORATION_H_
#define DECORATION_H_

struct DisplayMarker;
struct DecorationManager;

struct Decoration {
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

  DisplayMarker *marker;
  DecorationManager *decorationManager;
  Properties properties;

  Decoration(DisplayMarker *, DecorationManager *, Properties);
  ~Decoration();

  void destroy();

  Properties getProperties();
  void setProperties(Properties);
};

#endif // DECORATION_H_
