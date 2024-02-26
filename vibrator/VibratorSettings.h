#include <string>
#include <utility>
#include <vector>

namespace aidl {
namespace android {
namespace hardware {
namespace vibrator {

static std::string ACTIVATE_PATH = "/sys/class/leds/vibrator/activate";
static std::string BRIGHTNESS_PATH = "/sys/class/leds/vibrator/brightness";
static std::string DURATION_PATH = "/sys/class/leds/vibrator/duration";
static std::string LOOP_PATH = "/sys/class/leds/vibrator/loop";
static std::string SEQ_PATH = "/sys/class/leds/vibrator/seq";

static std::vector<std::pair<std::string, std::string>> SETUP_CLICK_HAPTIC{
    // cancels previous vibrations
    {ACTIVATE_PATH, "0"},
    {BRIGHTNESS_PATH, "0"},

    {DURATION_PATH, "10"},
    {LOOP_PATH, "0x00 0x00"},
};
// after writing seq and SETUP_HAPTIC
static std::vector<std::pair<std::string, std::string>> EXECUTE_CLICK_HAPTIC{
    {BRIGHTNESS_PATH, "1"}, // start haptic
    {BRIGHTNESS_PATH, "0"}, // end haptic (trust me, this is needed)
};

static std::vector<std::pair<std::string, std::string>> STOP_VIBRATIONS{
    {BRIGHTNESS_PATH, "0"},
    {ACTIVATE_PATH, "0"},
};

static std::vector<std::pair<std::string, std::string>> SETUP_NORMAL_VIBRATION{
    {BRIGHTNESS_PATH, "0"},
    {ACTIVATE_PATH, "0"},
    {SEQ_PATH, "0x00 0x00"},
};

static std::vector<std::pair<std::string, std::string>> EXECUTE_NORMAL_VIBRATION{
    {ACTIVATE_PATH, "1"},
};

} // namespace vibrator
} // namespace hardware
} // namespace android
} // namespace aidl
