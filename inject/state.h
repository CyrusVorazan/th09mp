#include "th09address.h"

namespace th09mp
{
	void SetInputState(PlayerSide side, unsigned short keys, bool setSystemKeys = false, bool addKeys = false);

    void CopyVector2D(raw_types::Vector2D& dst, raw_types::Vector2D src);
    void CopyVector3D(raw_types::Vector3D& dst, raw_types::Vector3D src);
    void CopyRect2D(raw_types::Rect2D& dst, raw_types::Rect2D src);
    void CopyRect3D(raw_types::Rect3D& dst, raw_types::Rect3D src);
    void CopySize2D(raw_types::Size2D& dst, raw_types::Size2D src);
    void CopyBoard(raw_types::Board src, raw_types::Board& dst);
    void CopyState(address::Th9GlobalVer1_5* src, address::Th9GlobalVer1_5* dst);
}
