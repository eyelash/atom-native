#include "display-marker-layer.h"
#include "marker-layer.h"

DisplayMarkerLayer::DisplayMarkerLayer(DisplayLayer *displayLayer, MarkerLayer *bufferMarkerLayer, bool ownsBufferMarkerLayer) :
  displayLayer{displayLayer}, id{bufferMarkerLayer->id} {}

DisplayMarkerLayer::~DisplayMarkerLayer() {}
