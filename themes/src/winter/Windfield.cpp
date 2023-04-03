#include "Windfield.hpp"

    
template <i32 Width, i32 Height, i32 CellDimension>
void WindField<Width, Height, CellDimension>::load(f32 cellSizePixels, i32 fieldWithCells, i32 fieldHeightCells, f32 ttl, Vector2 origin) {
    this->ttl = ttl;
    this->origin = origin;
    this->end = this->origin + Vector2(Width * CellDimension, Height * CellDimension);
}

template <i32 Width, i32 Height, i32 CellDimension>
bool WindField<Width, Height, CellDimension>::addVector(i32 x, i32 y, Vector2& v) {
    field[x][y] = v;
    return false;
}

template <i32 Width, i32 Height, i32 CellDimension>
Vector2 WindField<Width, Height, CellDimension>::getWindFactor(Vector2& v) {
    if (v.x >= origin.x && v.x <= end.x
        && v.y >= origin.y && v.y <= end.y) {
        Vector2 positionInField = v - this->origin;
        i32 cellX = static_cast<i32>(Width / positionInField.x);
        i32 cellY = static_cast<i32>(Height / positionInField.y);
        return field[cellX][cellY];
    }
    
    return Vector2();
}
