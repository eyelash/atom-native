#ifndef POINT_HELPERS_H_
#define POINT_HELPERS_H_

#include "point.h"
#include <regex.h>

int compare(const Point&, const Point&);
bool isEqual(const Point &, const Point &);
Point traverse(const Point &, const Point &);
Point traversal(const Point &, const Point &);
extern Regex NEWLINE_REG_EXP;
Point clipNegativePoint(const Point &);
Point max(const Point &, const Point &);
Point min(const Point &, const Point &);

#endif // POINT_HELPERS_H_
