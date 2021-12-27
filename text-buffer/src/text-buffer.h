#ifndef TEXT_BUFFER_H_
#define TEXT_BUFFER_H_

#include <string>
#include <unordered_map>
#include <native-text-buffer.h>

class Marker;
class MarkerLayer;

class TextBuffer {
  NativeTextBuffer *buffer;
  unsigned nextMarkerLayerId;
  MarkerLayer *defaultMarkerLayer;
  std::unordered_map<unsigned, MarkerLayer *> markerLayers;
  unsigned nextMarkerId;

public:

  TextBuffer();
  TextBuffer(const std::u16string &text);
  ~TextBuffer();

  std::u16string getText();
  MarkerLayer *addMarkerLayer();
  MarkerLayer *getMarkerLayer(unsigned);
  MarkerLayer *getDefaultMarkerLayer();
  Marker *markRange(Range range);
  Marker *markPosition(Point position);
  std::vector<Marker *> getMarkers();
  Marker *getMarker(unsigned id);
  std::size_t getMarkerCount();
  unsigned getNextMarkerId();
};

#endif  // TEXT_BUFFER_H_
