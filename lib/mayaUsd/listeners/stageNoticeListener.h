//
// Copyright 2018 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef PXRUSDMAYA_STAGE_NOTICE_LISTENER_H
#define PXRUSDMAYA_STAGE_NOTICE_LISTENER_H

#include <mayaUsd/base/api.h>

#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/stage.h>

#include <functional>

PXR_NAMESPACE_OPEN_SCOPE

/// A notice listener that can invoke callbacks in response to notices about a
/// specific USD stage.
///
/// For callbacks for a particular notice type to be invoked, the listener must
/// have been populated with a callback for notices of that type *and* a USD
/// stage.
class UsdMayaStageNoticeListener : public TfWeakBase
{
public:
    MAYAUSD_CORE_PUBLIC
    UsdMayaStageNoticeListener() = default;

    MAYAUSD_CORE_PUBLIC
    virtual ~UsdMayaStageNoticeListener();

    /// Set the USD stage for which this instance will listen for notices.
    MAYAUSD_CORE_PUBLIC
    void SetStage(const UsdStageWeakPtr& stage);

    /// Callback type for stage notices.
    using StageContentsChangedCallback
        = std::function<void(const UsdNotice::StageContentsChanged& notice)>;
    using StageObjectsChangedCallback
        = std::function<void(const UsdNotice::ObjectsChanged& notice)>;
    using StageLayerMutingChangedCallback
        = std::function<void(const UsdNotice::LayerMutingChanged& notice)>;
    using StageEditTargetChangedCallback
        = std::function<void(const UsdNotice::StageEditTargetChanged& notice)>;

    /// Sets the callback to be invoked when the listener receives a
    /// StageContentsChanged notice.
    MAYAUSD_CORE_PUBLIC
    void SetStageContentsChangedCallback(const StageContentsChangedCallback& callback);

    /// Sets the callback to be invoked when the listener receives a
    /// ObjectsChanged notice.
    MAYAUSD_CORE_PUBLIC
    void SetStageObjectsChangedCallback(const StageObjectsChangedCallback& callback);

    /// Sets the callback to be invoked when the listener receives a
    /// LayerMutingChanged notice.
    MAYAUSD_CORE_PUBLIC
    void SetStageLayerMutingChangedCallback(const StageLayerMutingChangedCallback& callback);

    /// Sets the callback to be invoked when the listener receives a
    /// EditTargetChanged notice.
    MAYAUSD_CORE_PUBLIC
    void SetStageEditTargetChangedCallback(const StageEditTargetChangedCallback& callback);

    /// We have incoming changes that USD will consider either requiring an
    /// update (meaning the render delegate needs to refresh and redraw) or
    /// a resync (meaning the scene delegate needs to fetch new datum). We
    /// want external clients to be aware of these classes of updates in case
    /// they do not use the Hydra system for refreshing and drawing the scene.
    enum class ChangeType
    {
        kIgnored, // Change does not require redraw: UI change, metadata change.
        kUpdate,  // Change requires redraw after refreshing parameter values
        kResync   // Change requires refreshing cached buffers
    };

    /// This is a stripped down copy of UsdImagingDelegate::_OnUsdObjectsChanged which is the main
    /// USD notification handler where paths to refresh and paths to update are compiled for the
    /// next Hydra refresh. We do not gather paths as there is no simple way to know when to flush
    /// these maps.
    ///
    /// This needs to stay as quick as possible since it is stuck in the middle of the notification
    /// code path.
    ///
    /// This is a work in progress. Some improvements might be necessary in the future. The
    /// following potential issues are already visible:
    ///
    ///  - Changing a parameter value for the first time creates the attribute, which is a kResync
    MAYAUSD_CORE_PUBLIC
    static ChangeType ClassifyObjectsChanged(UsdNotice::ObjectsChanged const& notice);

private:
    UsdMayaStageNoticeListener(const UsdMayaStageNoticeListener&) = delete;
    UsdMayaStageNoticeListener& operator=(const UsdMayaStageNoticeListener&) = delete;

    UsdStageWeakPtr _stage;

    /// Handling for UsdNotices
    TfNotice::Key                _stageContentsChangedKey {};
    StageContentsChangedCallback _stageContentsChangedCallback {};

    TfNotice::Key               _stageObjectsChangedKey {};
    StageObjectsChangedCallback _stageObjectsChangedCallback {};

    TfNotice::Key                   _stageLayerMutingChangedKey {};
    StageLayerMutingChangedCallback _stageLayerMutingChangedCallback {};

    TfNotice::Key                  _stageEditTargetChangedKey {};
    StageEditTargetChangedCallback _stageEditTargetChangedCallback {};

    void _UpdateStageContentsChangedRegistration();
    void _OnStageContentsChanged(const UsdNotice::StageContentsChanged& notice) const;
    void _OnStageLayerMutingChanged(const UsdNotice::LayerMutingChanged& notice) const;
    void _OnStageObjectsChanged(
        const UsdNotice::ObjectsChanged& notice,
        const UsdStageWeakPtr&           sender) const;
    void _OnStageEditTargetChanged(const UsdNotice::StageEditTargetChanged& notice) const;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
