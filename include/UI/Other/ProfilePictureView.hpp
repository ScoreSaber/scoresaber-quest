#pragma once
#include <HMUI/ImageView.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Sprite.hpp>
#include <System/Threading/CancellationToken.hpp>
#include <beatsaber-hook/shared/utils/typedefs-wrappers.hpp>
#include <string>
#include "Utils/SafePtr.hpp"

namespace ScoreSaber::UI::Other {
    // shouldn't need to be a C# class until we switch over to BSML
    class ProfilePictureView {
        FixedSafePtrUnity<HMUI::ImageView> profileImage;
        FixedSafePtrUnity<UnityEngine::GameObject> loadingIndicator;
        void OnAvatarDownloadSuccess(UnityEngine::Sprite* a, int pos, std::string url, System::Threading::CancellationToken cancellationToken);
        void OnAvatarDownloadFailure(std::string error, int pos, System::Threading::CancellationToken cancellationToken);
    public:
        ProfilePictureView(HMUI::ImageView* profileImage, UnityEngine::GameObject* loadingIndicator);
        void Parsed(); // we are not using BSML here yet, but keep names analogous
        void SetProfileImage(std::string url, int pos, System::Threading::CancellationToken cancellationToken);
        void ClearSprite();

        static void OnSoftRestart();
    };
}