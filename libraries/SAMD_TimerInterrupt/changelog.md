# SAMD_TimerInterrupt Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_TimerInterrupt.svg?)](https://www.ardu-badge.com/SAMD_TimerInterrupt)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/SAMD_TimerInterrupt.svg)](https://github.com/khoih-prog/SAMD_TimerInterrupt/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/SAMD_TimerInterrupt/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/SAMD_TimerInterrupt.svg)](http://github.com/khoih-prog/SAMD_TimerInterrupt/issues)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.0](#releases-v140)
  * [Releases v1.3.1](#releases-v131)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.1](#releases-v111)
  * [Releases v1.0.1](#releases-v101)
  * [Releases v1.0.0](#releases-v100)


---
---

## Changelog

### Releases v1.5.0

1. Improve frequency precision by using float instead of ulong, Check PR [change variable period from unsigned long to float #7](https://github.com/khoih-prog/SAMD_TimerInterrupt/pull/7)
2. Remove compiler warnings
3. Update `Packages' Patches`
4. Add `strict` option for PIO `lib_compat_mode`
5. Split `changelog.log` from `README.md`

### Releases v1.4.0

1. Fix SAMD21 rare bug caused by not fully init Prescaler. Check [**Bug when going from a >20000us period to a <20000us period. The timer period become 4 times greater.** #3](https://github.com/khoih-prog/SAMD_TimerInterrupt/issues/3)


### Releases v1.3.1

1. Fix compile error to some SAMD21-based boards, such as ADAFRUIT_FEATHER_M0, ARDUINO_SAMD_FEATHER_M0, ADAFRUIT_METRO_M0_EXPRESS, ARDUINO_SAMD_HALLOWING_M0 and ADAFRUIT_BLM_BADGE. Check [Doesn't compile with Adafruit Feather M0 #2](https://github.com/khoih-prog/SAMD_TimerInterrupt/issues/2).


### Releases v1.3.0

1. Add support to **Sparkfun SAMD21 boards** such as **SparkFun_RedBoard_Turbo, SparkFun_Qwiic_Micro, etc.**
2. Add support to **Sparkfun SAMD51 boards** such as **SparkFun_SAMD51_Thing_Plus, SparkFun_SAMD51_MicroMod, etc.**
3. Update examples to support Sparkfun boards.

### Releases v1.2.0

1. Add better debug feature.
2. Optimize code and examples to reduce RAM usage
3. Add Table of Contents

### Releases v1.1.1

1. Add example [**Change_Interval**](examples/Change_Interval) and [**ISR_16_Timers_Array_Complex**](examples/ISR_16_Timers_Array_Complex)
2. Bump up version to sync with other TimerInterrupt Libraries. Modify Version String.

### Releases v1.0.1

1. Add complicated example [ISR_16_Timers_Array](examples/ISR_16_Timers_Array) utilizing and demonstrating the full usage of 16 independent ISR Timers.

### Releases v1.0.0

1. Permit up to 16 super-long-time, super-accurate ISR-based timers to avoid being blocked
2. Using cpp code besides Impl.h code to use if Multiple-Definition linker error.


