/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the callbacks from `SubMac` layer into `Mac` or `LinkRaw`.
 */

#include "sub_mac.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator-getters.hpp"
#include "mac/mac.hpp"

namespace ot {
namespace Mac {

SubMac::Callbacks::Callbacks(Instance &aInstance)
    : InstanceLocator(aInstance)
{
}

#if OPENTHREAD_FTD || OPENTHREAD_MTD

void SubMac::Callbacks::ReceiveDone(Frame *aFrame, otError aError)
{
#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    if (Get<LinkRaw>().IsEnabled())
    {
        Get<LinkRaw>().InvokeReceiveDone(aFrame, aError);
    }
    else
#endif
    {
        Get<Mac>().HandleReceivedFrame(aFrame, aError);
    }
}

void SubMac::Callbacks::RecordCcaStatus(bool aCcaSuccess, uint8_t aChannel)
{
    Get<Mac>().RecordCcaStatus(aCcaSuccess, aChannel);
}

void SubMac::Callbacks::RecordFrameTransmitStatus(const Frame &aFrame,
                                                  const Frame *aAckFrame,
                                                  otError      aError,
                                                  uint8_t      aRetryCount,
                                                  bool         aWillRetx)
{
    Get<Mac>().RecordFrameTransmitStatus(aFrame, aAckFrame, aError, aRetryCount, aWillRetx);
}

void SubMac::Callbacks::TransmitDone(Frame &aFrame, Frame *aAckFrame, otError aError)
{
#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    if (Get<LinkRaw>().IsEnabled())
    {
        Get<LinkRaw>().InvokeTransmitDone(aFrame, aAckFrame, aError);
    }
    else
#endif
    {
        Get<Mac>().HandleTransmitDone(aFrame, aAckFrame, aError);
    }
}

void SubMac::Callbacks::EnergyScanDone(int8_t aMaxRssi)
{
#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    if (Get<LinkRaw>().IsEnabled())
    {
        Get<LinkRaw>().InvokeEnergyScanDone(aMaxRssi);
    }
    else
#endif
    {
        Get<Mac>().EnergyScanDone(aMaxRssi);
    }
}

#if OPENTHREAD_CONFIG_HEADER_IE_SUPPORT
void SubMac::Callbacks::FrameUpdated(Frame &aFrame)
{
    /**
     * This function will be called from interrupt context, it should only read/write data passed in
     * via @p aFrame, but should not read/write any state within OpenThread.
     *
     */

    if (aFrame.GetSecurityEnabled())
    {
        Get<Mac>().ProcessTransmitAesCcm(aFrame, &Get<Mac>().GetExtAddress());
    }
}
#endif

#elif OPENTHREAD_RADIO

void SubMac::Callbacks::ReceiveDone(Frame *aFrame, otError aError)
{
    Get<LinkRaw>().InvokeReceiveDone(aFrame, aError);
}

void SubMac::Callbacks::RecordCcaStatus(bool, uint8_t)
{
}

void SubMac::Callbacks::RecordFrameTransmitStatus(const Frame &aFrame,
                                                  const Frame *aAckFrame,
                                                  otError      aError,
                                                  uint8_t      aRetryCount,
                                                  bool         aWillRetx)
{
    Get<LinkRaw>().RecordFrameTransmitStatus(aFrame, aAckFrame, aError, aRetryCount, aWillRetx);
}

void SubMac::Callbacks::TransmitDone(Frame &aFrame, Frame *aAckFrame, otError aError)
{
    Get<LinkRaw>().InvokeTransmitDone(aFrame, aAckFrame, aError);
}

void SubMac::Callbacks::EnergyScanDone(int8_t aMaxRssi)
{
    Get<LinkRaw>().InvokeEnergyScanDone(aMaxRssi);
}

#if OPENTHREAD_CONFIG_HEADER_IE_SUPPORT
void SubMac::Callbacks::FrameUpdated(Frame &)
{
    /**
     * This function will be called from interrupt context, it should only read/write data passed in
     * via @p aFrame, but should not read/write any state within OpenThread.
     *
     */

    // For now this functionality is not supported in Radio Only mode.
}
#endif

#endif // OPENTHREAD_RADIO

} // namespace Mac
} // namespace ot
