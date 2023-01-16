#ifndef DECORATION_MANAGER_H_
#define DECORATION_MANAGER_H_

#include "decoration.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class TextEditor;
class DisplayLayer;
class DisplayMarker;
class DisplayMarkerLayer;
class LayerDecoration;

class DecorationManager {
  TextEditor *editor;
  DisplayLayer *displayLayer;
  std::unordered_map<DisplayMarkerLayer *, double> decorationCountsByLayer;
  std::unordered_map<DisplayMarkerLayer *, double> markerDecorationCountsByLayer;
  std::unordered_map<DisplayMarker *, std::unordered_set<Decoration *>> decorationsByMarker;
  std::unordered_map<DisplayMarkerLayer *, std::unordered_set<LayerDecoration *>> layerDecorationsByMarkerLayer;
  //this.overlayDecorations = new Set();

public:
  DecorationManager(TextEditor *);
  ~DecorationManager();

  std::unordered_map<DisplayMarker *, std::vector<Decoration::Properties>> decorationPropertiesByMarkerForScreenRowRange(double, double);
  Decoration *decorateMarker(DisplayMarker *, Decoration::Properties);
  LayerDecoration *decorateMarkerLayer(DisplayMarkerLayer *, Decoration::Properties);
  void observeDecoratedLayer(DisplayMarkerLayer *, bool);
};

#endif // DECORATION_MANAGER_H_
