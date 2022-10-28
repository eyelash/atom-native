#include "decoration-manager.h"
#include "text-editor.h"
#include "display-marker.h"
#include "display-marker-layer.h"
#include "layer-decoration.h"

DecorationManager::DecorationManager(TextEditor *editor) {
  this->editor = editor;
  this->displayLayer = this->editor->displayLayer;
}

DecorationManager::~DecorationManager() {
  for (auto &decorations : this->decorationsByMarker) {
    for (Decoration *decoration : decorations.second) {
      delete decoration;
    }
  }
  for (auto &layerDecorations : this->layerDecorationsByMarkerLayer) {
    for (LayerDecoration *layerDecoration : layerDecorations.second) {
      delete layerDecoration;
    }
  }
}

std::unordered_map<DisplayMarker *, std::vector<Decoration::Properties>> DecorationManager::decorationPropertiesByMarkerForScreenRowRange(double startScreenRow, double endScreenRow) {
  std::unordered_map<DisplayMarker *, std::vector<Decoration::Properties>> decorationPropertiesByMarker;

  for (auto &iter : this->decorationCountsByLayer) {
    DisplayMarkerLayer *markerLayer = iter.first;
    auto markers = markerLayer->findMarkers({
      intersectsScreenRowRange(std::make_pair(startScreenRow, endScreenRow - 1))
    });
    const auto &layerDecorations = this->layerDecorationsByMarkerLayer[
      markerLayer
    ];
    const bool hasMarkerDecorations =
      this->markerDecorationCountsByLayer[markerLayer] > 0;

    for (size_t i = 0; i < markers.size(); i++) {
      DisplayMarker *marker = markers[i];
      //if (!marker.isValid()) continue;

      auto &decorationPropertiesForMarker = decorationPropertiesByMarker[
        marker
      ];

      //if (layerDecorations) {
        for (LayerDecoration *layerDecoration : layerDecorations) {
          auto properties =
            /* layerDecoration.getPropertiesForMarker(marker) || */
            layerDecoration->getProperties();
          decorationPropertiesForMarker.push_back(properties);
        }
      //}

      if (hasMarkerDecorations) {
        auto decorationsForMarker = this->decorationsByMarker.find(marker);
        if (decorationsForMarker != this->decorationsByMarker.end()) {
          for (Decoration *decoration : decorationsForMarker->second) {
            decorationPropertiesForMarker.push_back(decoration->getProperties());
          }
        }
      }
    }
  }

  return decorationPropertiesByMarker;
}

Decoration *DecorationManager::decorateMarker(DisplayMarker *marker, Decoration::Properties decorationParams) {
  marker = this->displayLayer
    ->getMarkerLayer(marker->layer->id)
    ->getMarker(marker->id);
  Decoration *decoration = new Decoration(marker, this, decorationParams);
  this->decorationsByMarker[marker].insert(decoration);
  //if (decoration.isType('overlay')) this.overlayDecorations.add(decoration);
  this->observeDecoratedLayer(marker->layer, true);
  //this.editor.didAddDecoration(decoration);
  //this.emitDidUpdateDecorations();
  //this.emitter.emit('did-add-decoration', decoration);
  return decoration;
}

LayerDecoration *DecorationManager::decorateMarkerLayer(DisplayMarkerLayer *markerLayer, Decoration::Properties decorationParams) {
  markerLayer = this->displayLayer->getMarkerLayer(markerLayer->id);
  LayerDecoration *decoration = new LayerDecoration(markerLayer, this, decorationParams);
  this->layerDecorationsByMarkerLayer[markerLayer].insert(decoration);
  this->observeDecoratedLayer(markerLayer, false);
  //this.emitDidUpdateDecorations();
  return decoration;
}

void DecorationManager::observeDecoratedLayer(DisplayMarkerLayer *layer, bool isMarkerDecoration) {
  const double newCount = (this->decorationCountsByLayer[layer] /* || 0 */ ) + 1;
  this->decorationCountsByLayer[layer] = newCount;
  if (newCount == 1) {
    /*this.layerUpdateDisposablesByLayer.set(
      layer,
      layer.onDidUpdate(this.emitDidUpdateDecorations.bind(this))
    );*/
  }
  if (isMarkerDecoration) {
    this->markerDecorationCountsByLayer[
      layer] =
      (this->markerDecorationCountsByLayer[layer] /* || 0 */ ) + 1
    ;
  }
}
