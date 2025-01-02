#ifndef MCU_TIMER_H
#define MCU_TIMER_H

#include "bits.h"
#include "concepts.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu {

template <
    unsigned_type Size,
    is_data_reg<uint8_t> TCCRxA,
    is_data_reg<uint8_t> TCCRxB,
    is_data_reg<Size> TCNTx,
    is_data_reg<Size> OCRxA,
    is_data_reg<Size> OCRxB,
    is_data_reg<uint8_t> TIMSKx,
    is_data_reg<uint8_t> TIFRx>
struct TimerInfo {
    using size_t                 = Size;
    static constexpr auto MAX    = static_cast<size_t>(-1);
    static constexpr auto BOTTOM = 0;

    using TCCRnA = TCCRxA;
    using TCCRnB = TCCRxB;

    using TCNTn = TCNTx;

    using OCRnA = OCRxA;
    using OCRnB = OCRxB;

    using TIMSKn = TIMSKx;
    using TIFRn  = TIFRx;
};

template <
    unsigned_type Size,
    is_data_reg<uint8_t> TCCRxA,
    is_data_reg<uint8_t> TCCRxB,
    is_data_reg<uint8_t> TCCRxC,
    is_data_reg<Size> TCNTx,
    is_data_reg<Size> OCRxA,
    is_data_reg<Size> OCRxB,
    is_data_reg<Size> ICRx,
    is_data_reg<uint8_t> TIMSKx,
    is_data_reg<uint8_t> TIFRx>
struct TimerInfoEx : TimerInfo<Size, TCCRxA, TCCRxB, TCNTx, OCRxA, OCRxB, TIMSKx, TIFRx> {
    using TCCRnC = TCCRxC;
    using ICRn   = ICRx;
};

template <
    unsigned_type Size,
    is_data_reg<uint8_t> TCCRxA,
    is_data_reg<uint8_t> TCCRxB,
    is_data_reg<Size> TCNTx,
    is_data_reg<Size> OCRxA,
    is_data_reg<Size> OCRxB,
    is_data_reg<uint8_t> TIMSKx,
    is_data_reg<uint8_t> TIFRx,
    is_data_reg<uint8_t> ASSRx>
struct AsyncTimerInfo : TimerInfo<Size, TCCRxA, TCCRxB, TCNTx, OCRxA, OCRxB, TIMSKx, TIFRx> {
    using ASSR = ASSRx;
};

template <typename T>
concept timer_info = requires {
    typename T::size_t;
    requires unsigned_type<typename T::size_t>;

    T::MAX;
    T::BOTTOM;

    typename T::TCCRnA;
    typename T::TCCRnB;
    requires is_data_reg<typename T::TCCRnA, uint8_t>;
    requires is_data_reg<typename T::TCCRnB, uint8_t>;

    typename T::TCNTn;
    requires is_data_reg<typename T::TCNTn, typename T::size_t>;

    typename T::OCRnA;
    typename T::OCRnB;
    requires is_data_reg<typename T::OCRnA, typename T::size_t>;
    requires is_data_reg<typename T::OCRnB, typename T::size_t>;

    typename T::TIMSKn;
    typename T::TIFRn;
    requires is_data_reg<typename T::TIMSKn, uint8_t>;
    requires is_data_reg<typename T::TIFRn, uint8_t>;
};

template <typename T>
concept timer_info_extended = requires {
    requires timer_info<T>;

    typename T::TCCRnC;
    typename T::ICRn;
    requires is_data_reg<typename T::TCCRnC, uint8_t>;
    requires is_data_reg<typename T::ICRn, typename T::size_t>;
};

template <typename T>
concept async_timer_info = requires {
    requires timer_info<T>;

    T::ASSR;
    requires is_data_reg<typename T::ASSR, uint8_t>;
};

enum class TimerClockSource {
    NO_SOURCE = 0,
    CLK       = 1,
    CLK_8     = 8,
    CLK_64    = 64,
    CLK_256   = 256,
    CLK_1024  = 1024,
};

}

#endif
