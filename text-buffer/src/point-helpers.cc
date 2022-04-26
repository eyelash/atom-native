#include "point-helpers.h"

static int compareNumbers(double a, double b) {
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  } else {
    return 0;
  }
}

int compare(const Point &a, const Point &b) {
  if (a.row == b.row) {
    return compareNumbers(a.column, b.column);
  } else {
    return compareNumbers(a.row, b.row);
  }
}

bool isEqual(const Point &a, const Point &b) {
  return a.row == b.row && a.column == b.column;
}

Point traverse(const Point &start, const Point &distance) {
  if (distance.row == 0) {
    return Point(start.row, start.column + distance.column);
  } else {
    return Point(start.row + distance.row, distance.column);
  }
}

Point traversal(const Point &end, const Point &start) {
  if (end.row == start.row) {
    return Point(0, end.column - start.column);
  } else {
    return Point(end.row - start.row, end.column);
  }
};

Regex NEWLINE_REG_EXP(u"\\n", nullptr);

/*exports.characterIndexForPoint = function(text, point) {
  var column, row;
  row = point.row;
  column = point.column;
  NEWLINE_REG_EXP.lastIndex = 0;
  while (row-- > 0) {
    if (!NEWLINE_REG_EXP.exec(text)) {
      return text.length;
    }
  }
  return NEWLINE_REG_EXP.lastIndex + column;
};*/

Point clipNegativePoint(const Point &point) {
  if (point.row < 0) {
    return Point(0, 0);
  } else if (point.column < 0) {
    return Point(point.row, 0);
  } else {
    return point;
  }
}

Point max(const Point &a, const Point &b) {
  if (compare(a, b) >= 0) {
    return a;
  } else {
    return b;
  }
}

Point min(const Point &a, const Point &b) {
  if (compare(a, b) <= 0) {
    return a;
  } else {
    return b;
  }
}
