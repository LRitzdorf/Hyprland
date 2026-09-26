#include "ITrackpadGesture.hpp"

// scale the pinch "scale" to match our imaginary delta units
constexpr const float PINCH_DELTA_SCALE         = 400.F;
constexpr const float PINCH_DELTA_SCALE_OUT_ADD = 1.6F;

//
void ITrackpadGesture::begin(const STrackpadGestureBegin& e) {
    m_lastPinchScale = 1.F;
    m_scale          = e.scale;
    m_natural        = e.natural;
}

float ITrackpadGesture::distance(const STrackpadGestureBegin& e) {
    switch (e.direction) {
        case TRACKPAD_GESTURE_DIR_LEFT:
            return m_scale * -e.swipe->delta.x;
        case TRACKPAD_GESTURE_DIR_RIGHT:
            return m_scale * e.swipe->delta.x;
        case TRACKPAD_GESTURE_DIR_HORIZONTAL:
            return m_scale * e.swipe->delta.x * (m_natural ? -1 : 1);
        case TRACKPAD_GESTURE_DIR_UP:
            return m_scale * -e.swipe->delta.y;
        case TRACKPAD_GESTURE_DIR_DOWN:
            return m_scale * e.swipe->delta.y;
        case TRACKPAD_GESTURE_DIR_VERTICAL:
            return m_scale * e.swipe->delta.y * (m_natural ? -1 : 1);
        case TRACKPAD_GESTURE_DIR_SWIPE:
            return m_scale * e.swipe->delta.size() * (m_natural ? -1 : 1);
        case TRACKPAD_GESTURE_DIR_PINCH:
        case TRACKPAD_GESTURE_DIR_PINCH_OUT:
            {
                const auto Δ     = m_lastPinchScale - e.pinch->scale;
                m_lastPinchScale = e.pinch->scale;
                return m_scale * Δ * PINCH_DELTA_SCALE * PINCH_DELTA_SCALE_OUT_ADD;
            }
        case TRACKPAD_GESTURE_DIR_PINCH_IN:
            {
                const auto Δ     = e.pinch->scale - m_lastPinchScale;
                m_lastPinchScale = e.pinch->scale;
                return m_scale * Δ * PINCH_DELTA_SCALE;
            }
        default:
            return m_scale * (e.swipe ? e.swipe->delta.size() : e.pinch->delta.size());
    }
}

float ITrackpadGesture::distance(const STrackpadGestureUpdate& e) {
    return ITrackpadGesture::distance(STrackpadGestureBegin{
        .swipe     = e.swipe,
        .pinch     = e.pinch,
        .direction = e.direction,
        .scale     = e.scale,
    });
}
