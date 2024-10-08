/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Vibrator.h"
#include "VibratorSettings.h"

#include <android-base/logging.h>
#include <thread>

namespace aidl {
namespace android {
namespace hardware {
namespace vibrator {

ndk::ScopedAStatus Vibrator::getCapabilities(int32_t* _aidl_return) {
    LOG(VERBOSE) << "Vibrator reporting capabilities";

    *_aidl_return = IVibrator::CAP_ON_CALLBACK;
    *_aidl_return |= IVibrator::CAP_PERFORM_CALLBACK;

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::off() {
    LOG(VERBOSE) << "Vibrator off";

    ndk::ScopedAStatus status = setNodes(STOP_VIBRATIONS);
    if (!status.isOk()) return status;

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::on(int32_t timeoutMs, const std::shared_ptr<IVibratorCallback>& callback) {
    ndk::ScopedAStatus status;

    LOG(VERBOSE) << "Vibrator on for timeoutMs: " << timeoutMs;

    if (timeoutMs < 1) return off();

    // if timeoutMs is bigger than 102, we do a regular vibration
    if (timeoutMs < 103) {
        // begin haptic
        status = setNodes(SETUP_CLICK_HAPTIC);
        if (!status.isOk()) return status;

        std::string seqValue = "0x00 0x00";

#ifdef VIBRATOR_ALT_SEQ_TYPE
        if (timeoutMs < 3) seqValue = "0x00 0x07";
        else if (timeoutMs < 10) seqValue = "0x00 0x0e";
        else if (timeoutMs < 20) seqValue = "0x00 0x01";
        else if (timeoutMs < 40) seqValue = "0x00 0x0d";
        else if (timeoutMs < 60) seqValue = "0x00 0x04";
        else if (timeoutMs < 80) seqValue = "0x00 0x06";
        else seqValue = "0x00 0x0a";
#else
        if (timeoutMs < 10) seqValue = "0x00 0x02";
        else if (timeoutMs < 20) seqValue = "0x00 0x06";
        else if (timeoutMs < 40) seqValue = "0x00 0x07";
        else if (timeoutMs < 60) seqValue = "0x00 0x09";
        else if (timeoutMs < 80) seqValue = "0x00 0x09";
        else seqValue= "0x00 0x01";
#endif

        // set the seq value
        status = setNode(SEQ_PATH, seqValue);
        if (!status.isOk()) return status;

        // execute haptic
        status = setNodes(EXECUTE_CLICK_HAPTIC);
        if (!status.isOk()) return status;
    } else {
        // setup vibration
        status = setNodes(SETUP_NORMAL_VIBRATION);
        if (!status.isOk()) return status;

        status = setNode(DURATION_PATH, std::to_string(timeoutMs));
        if (!status.isOk()) return status;

        // execute vibration
        status = setNodes(EXECUTE_NORMAL_VIBRATION);
        if (!status.isOk()) return status;
    }

    if (callback != nullptr) {
        std::thread([=] {
            LOG(VERBOSE) << "Starting on on another thread";
            usleep(timeoutMs * 1000);
            LOG(VERBOSE) << "Notifying on complete";
            if (!callback->onComplete().isOk()) {
                LOG(ERROR) << "Failed to call onComplete";
            }
        }).detach();
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::perform(Effect, EffectStrength, const std::shared_ptr<IVibratorCallback>& callback, int32_t*) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getSupportedEffects(std::vector<Effect>* /* _aidl_return */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setAmplitude(float amplitude __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::setExternalControl(bool enabled __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getCompositionDelayMax(int32_t* maxDelayMs __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getCompositionSizeMax(int32_t* maxSize __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getSupportedPrimitives(std::vector<CompositePrimitive>* supported __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getPrimitiveDuration(CompositePrimitive primitive __unused, int32_t* durationMs __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::compose(const std::vector<CompositeEffect>& composite __unused, const std::shared_ptr<IVibratorCallback>& callback __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getSupportedAlwaysOnEffects(std::vector<Effect>* _aidl_return __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::alwaysOnEnable(int32_t id __unused, Effect effect __unused, EffectStrength strength __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::alwaysOnDisable(int32_t id __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getResonantFrequency(float* resonantFreqHz __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getQFactor(float* qFactor __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getFrequencyResolution(float* freqResolutionHz __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getFrequencyMinimum(float* freqMinimumHz __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getBandwidthAmplitudeMap(std::vector<float>* _aidl_return __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getPwlePrimitiveDurationMax(int32_t* durationMs __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getPwleCompositionSizeMax(int32_t* maxSize __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::getSupportedBraking(std::vector<Braking>* supported __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

ndk::ScopedAStatus Vibrator::composePwle(const std::vector<PrimitivePwle>& composite __unused, const std::shared_ptr<IVibratorCallback>& callback __unused) {
    return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
}

} // namespace vibrator
} // namespace hardware
} // namespace android
} // namespace aidl
