#include "Util.h"
#include <array>

// [tdbe] Note: [tdbe] prefix means I'm the author (/ of the comment).

/* [tdbe]
 * This input class aims to handle all OpenXR inputs for all available XR controllers and headset. Using the OpenXR API, it sets up 
 * and syncs the latest action poses (position, rotation) of controllers (joints), and head, as well as the action states e.g. buttons, 
 * gestures, pressure, proximity etc. (In OpenXR terms, an action pose = a 3D joint, an action state = button/joystick/sensor value.)
 * 
 * It supports "all" device brands, and defines customizable binding paths for each of them (ie different controllers with different 
 * button types etc, mapped to the same action).
 * 
 * Triggers haptic feedback.
 * Detects user presence state.
 * 
 * The usage philosophy is that all input your game needs, is synced and gathered into InputData once per "frame" (some things happen at 
 * different rates e.g. running headset vs sleeping controller or vice versa, polling rates etc). 
 * 
 * NOTE: I currently only have an oculus rift to test on, so I didn't test what button was what on all xr controller types
 */

class Input final
{
public:
    enum class ControllerEnum
    {
        LEFT = 0,
        RIGHT = 1,
        // 
        // [tdbe] TODO: figure out what happens in openxr with multiple controllers e.g. valve vr controllers, pucks 
        COUNT = 2
    };
    enum class SideEnum
    {
        LEFT = 0,
        RIGHT = 1,
        COUNT = 2
    };

    // [tdbe] Updated every Sync tick. It should contain all the input your game uses.
    struct InputData {
        // [tdbe] Poses
        std::array<glm::mat4, (int)SideEnum::COUNT> eyePoseMatrixes;//SideEnum::COUNT
        std::array<util::posef, (int)SideEnum::COUNT> eyePoses;//SideEnum::COUNT
        glm::mat4 headPoseMatrix;
        util::posef headPose;
        std::array<glm::mat4, (int)ControllerEnum::COUNT> controllerAimPoseMatrixes;
        std::array<util::posef, (int)ControllerEnum::COUNT> controllerAimPoses;
        std::array<glm::mat4, (int)ControllerEnum::COUNT> controllerGripPoseMatrixes;
        std::array<util::posef, (int)ControllerEnum::COUNT> controllerGripPoses;

        // [tdbe] Input States. Also includes lastChangeTime, isActive, changedSinceLastSync properties.
        std::array<XrActionStateFloat, (int)ControllerEnum::COUNT> grabState{XR_TYPE_ACTION_STATE_FLOAT};
        std::array<XrActionStateVector2f, (int)ControllerEnum::COUNT> thumbStickState{XR_TYPE_ACTION_STATE_VECTOR2F};
        std::array<XrActionStateBoolean, (int)ControllerEnum::COUNT> menuClickState{XR_TYPE_ACTION_STATE_BOOLEAN};
        std::array<XrActionStateBoolean, (int)ControllerEnum::COUNT> selectClickState{XR_TYPE_ACTION_STATE_BOOLEAN};
        
        // [tdbe] Headset State. Use to detect status / user proximity / user presence / user engagement https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#session-lifecycle
        XrSessionState headsetActivityState = XR_SESSION_STATE_UNKNOWN;
    };

    Input(XrInstance xrInstance, XrSession xrSession);
    ~Input();
    bool isValid() const;

    bool Sync(XrSpace xrReferenceSpace, XrTime predictedDisplayTime, std::vector<XrView> eyePoses, XrSessionState sessionState);
    const InputData GetInputData() const;
    void ApplyHapticFeedback(ControllerEnum controllerIndex, float amplitude01, int64_t duration, float frequency);

private:
    struct ActionStateData {
        XrActionSet actionSet{XR_NULL_HANDLE};

        // [tdbe] default actions (with direct bindings)
        //{----------------------------------------------
        XrAction aimPoseAction{XR_NULL_HANDLE};// [tdbe] hand pose that allows applications to point in the world using the input source, according to the platform’s conventions for aiming with that kind of source.
        XrAction gripPoseAction{XR_NULL_HANDLE};// [tdbe] hand pose that allows applications to reliably render a virtual object held in the user’s hand, whether it is tracked directly or by a motion controller.
        // https://raw.githubusercontent.com/KhronosGroup/OpenXR-Docs/master/specification/sources/images/grip_axes_diagram.png
        // https://raw.githubusercontent.com/KhronosGroup/OpenXR-Docs/master/specification/sources/images/ext_hand_tracking_joint_convention.png

        XrAction triggerValueAction{XR_NULL_HANDLE};
        XrAction triggerClickAction{XR_NULL_HANDLE};

        XrAction squeezeValueAction{XR_NULL_HANDLE};
        XrAction squeezeForceAction{XR_NULL_HANDLE};
        XrAction squeezeClickAction{XR_NULL_HANDLE};
                
        XrAction thumbstickAction{XR_NULL_HANDLE};
        XrAction thumbstickClickAction{XR_NULL_HANDLE};
        XrAction menuClickAction{XR_NULL_HANDLE};
        XrAction selectClickAction{XR_NULL_HANDLE};

        XrAction aClickAction{XR_NULL_HANDLE};
        XrAction bClickAction{XR_NULL_HANDLE};
        XrAction xClickAction{XR_NULL_HANDLE};
        XrAction yClickAction{XR_NULL_HANDLE};

        XrAction vibrateAction{XR_NULL_HANDLE};
        //}----------------------------------------------
        
        // [tdbe] custom actions (that vary from controller to controller)
        //{----------------------------------------------
        XrAction grabAction{XR_NULL_HANDLE};
        //}----------------------------------------------

        // [tdbe] used to get targets in openxr
        std::vector<XrPath> controllerPaths;
        std::vector<XrSpace> controllerReferenceSpaces;
    };
    ActionStateData actionSetData = {};
    InputData inputData = {};

    bool valid = true;
    XrSession session = nullptr;

    XrActionStatePose GetActionPoseState(XrAction targetAction, ControllerEnum controller);
    XrActionStateBoolean GetActionBooleanState(XrAction targetAction, ControllerEnum controller);
    XrActionStateFloat GetActionFloatState(XrAction targetAction, ControllerEnum controller);
    XrActionStateVector2f GetActionVector2fState(XrAction targetAction, ControllerEnum controller);
};