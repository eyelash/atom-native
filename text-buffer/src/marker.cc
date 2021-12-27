#include "marker.h"
#include "marker-layer.h"

Marker::Marker(unsigned id, MarkerLayer *layer, Range range) :
  id{id}, layer{layer} {}

Marker::~Marker() {}
