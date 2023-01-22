#ifndef WIND_FIELD_HPP
#define WIND_FIELD_HPP
#include "types.h"
#include "mathlib.h"

/**
   A Windfield represents a field of vectors in a rectangular region.
   The Width and Height are given in units of CellDimenions. The CellDimension
   is given in pixels.
 */
struct WindField {
    f32 ttl = 0.f;
    Vector2 origin;
    Vector2 end;

    /*
      Granularity of each cell in pixels.
     */
    const f32 cellDimension = CellDimension;

    /*
      Width of the vector field in CellDimensions.
     */
    const f32 width = Width;

    /*
      Height of the vector vield in CellDimensions.
     */
    const f32 height = Height;

    Vector2** field;

    void load(f32 cellSizePixels, i32 fieldWithCells, i32 fieldHeightCells, f32 ttl, Vector2 origin);
    bool addVector(i32 x, i32 y, Vector2& v);
    Vector2 getWindFactor(Vector2& v);
};

#endif
