#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <set>
#include <chrono>
#include <atomic>
#include <thread>
#include <condition_variable>


typedef std::string                         IODDStr;
typedef const char *                        IODDConstChar;
typedef std::vector<IODDStr>                IODDStrVect;
typedef std::set<IODDStr>                   IODDStrSet;
typedef std::vector<int>                    IODDIntVect;
typedef std::pair<IODDStr, IODDStr>         IODDStrPair;
typedef std::chrono::duration<double>       IODDDuration;
typedef std::atomic<bool>                   IODDAtomicBool;
typedef std::atomic<std::string>            IODDAtomicStr;
typedef std::thread                         IODDThread;
typedef std::chrono::system_clock           IODDClock;
typedef std::chrono::seconds                IODDSeconds;
typedef std::vector<std::pair<IODDStr, int>> IODDStrIntPairVect;
typedef std::condition_variable             IODDCondVar;
typedef std::mutex                          IODDMutex;
typedef std::lock_guard<std::mutex>         IODDLock;
typedef std::unique_lock <std::mutex>       IODDUniqueLock;

typedef uint8_t                             u8;
typedef uint32_t                            u32;
typedef uint64_t                            u64;

#define RECORD_TYPE_BOOL                    "BooleanT"
#define RECORD_TYPE_UINT                    "UIntegerT"
#define RECORD_TYPE_INT                     "IntegerT"
#define RECORD_TYPE_FLOAT                   "Float32T"
#define RECORD_TYPE_STR                     "StringT"

#define TOSTR(X)                            std::to_string(X)

#ifdef DEBUG_MODE
    #define IODD_DEBUG(...)    \
    {\
        printf("DEBUG:   %s L#%d ", __func__, __LINE__);  \
        printf(__VA_ARGS__); \
        printf("\n"); \
    }
#else
    #define IODD_DEBUG(...) /* nothing */
#endif


#define IODD_INFO(...)    \
{\
    printf("INFO:   %s L#%d ", __func__, __LINE__);  \
    printf(__VA_ARGS__); \
    printf("\n"); \
}
#define IODD_WARN(...)     \
{\
    printf("WARN:   %s L#%d ", __func__, __LINE__);  \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

#define IODD_ERROR(...)    \
{\
    printf("ERROR:   %s L#%d ", __func__, __LINE__);  \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

// IODD Tag

#define SYSTEM_COMMAND                      0x02
#define PROFILE_CHARACTERISTIC              0x0D
#define PDIn_DESCRIPTOR                     0x0E
#define PDOut_DESCRIPTOR                    0x0F
#define VENDOR_NAME                         0x10
#define VARIANT_ID                          0x11
#define PRODUCT_NAME                        0x12
#define PRODUCT_ID                          0x13
#define SERIAL_NUMBER                       0x15
#define HW_REV                              0x16
#define FW_REV                              0x17
#define APP_SPECIFIC_TAG                    0x18
#define FUNC_TAG                            0x19
#define LOCATION_TAG                        0x1A
#define DEVICE_STATUS                       0x24
#define DETAILED_DEVICE_STATUS              0x25

#endif // TYPES_H