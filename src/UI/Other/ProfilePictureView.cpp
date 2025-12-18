#include "UI/Other/ProfilePictureView.hpp"

#include <GlobalNamespace/ICoroutineStarter.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Networking/DownloadHandlerTexture.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/Networking/UnityWebRequestTexture.hpp>
#include <bsml/shared/Helpers/utilities.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include <paper2_scotland2/shared/string_convert.hpp>
#include <custom-types/shared/coroutine.hpp>
#include "questui/ArrayUtil.hpp"
#include "Utils/OperatorOverloads.hpp"

#include <map>
#include <queue>

using namespace System::Threading;
using namespace UnityEngine;
using namespace UnityEngine::Networking;
using namespace std;

#define BeginCoroutine(method) \
    BSML::Helpers::GetDiContainer()->Resolve<GlobalNamespace::ICoroutineStarter*>()->StartCoroutine( \
        custom_types::Helpers::CoroutineHelper::New(method));

namespace ScoreSaber::UI::Other {

    namespace SpriteCache {
        map<string, SafePtrUnity<Sprite>> cachedSprites;
        const int MaxSpriteCacheSize = 150;
        queue<string> spriteCacheQueue;

        void MaintainSpriteCache() {
            
            while (cachedSprites.size() > MaxSpriteCacheSize) {
                string oldestUrl = spriteCacheQueue.front();
                spriteCacheQueue.pop();
                cachedSprites.erase(oldestUrl);
            }

            // somehow the objects can be GCed, even when behind a SafePtrUnity
            vector<string> badSprites;
            for (auto &[key, value] : cachedSprites) {
                if (!value.isAlive()) {
                    badSprites.push_back(key);
                }
            }
            for(auto &key : badSprites) {
                cachedSprites.erase(key);
            }
        }

        void AddSpriteToCache(string url, Sprite* sprite) {
            if (cachedSprites.contains(url)) {
                return;
            }
            cachedSprites.emplace(url, sprite);
            spriteCacheQueue.push(url);
        }
    }

    bool initializedGlobals = false;
    SafePtrUnity<Sprite> nullSprite;
    SafePtrUnity<Material> mat_UINoGlowRoundEdge;

    void ProfilePictureView::OnSoftRestart() {
        SpriteCache::cachedSprites.clear();
        SpriteCache::spriteCacheQueue = queue<string>();

        initializedGlobals = false;
        nullSprite = nullptr;
        mat_UINoGlowRoundEdge = nullptr;
    }

    ProfilePictureView::ProfilePictureView(HMUI::ImageView* profileImage, UnityEngine::GameObject* loadingIndicator) : profileImage(profileImage), loadingIndicator(loadingIndicator) { }

    void ProfilePictureView::Parsed() {
        if (!initializedGlobals) {
            nullSprite = BSML::Utilities::ImageResources::GetBlankSprite();
            mat_UINoGlowRoundEdge = QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) { return Paper::StringConvert::from_utf16(x->name) == "UINoGlowRoundEdge"; });
            initializedGlobals = true;
        }

        profileImage->material = mat_UINoGlowRoundEdge.ptr();
        profileImage->sprite = nullSprite.ptr();
        profileImage->gameObject->SetActive(true);
        loadingIndicator->gameObject->SetActive(false);
    }

    custom_types::Helpers::Coroutine GetSpriteAvatar(string url, function<void(Sprite*, int, string, CancellationToken)> onSuccess, function<void(string, int, CancellationToken)> onFailure, CancellationToken cancellationToken, int pos) {
        UnityWebRequest* www = UnityWebRequestTexture::GetTexture(url);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        auto handler = il2cpp_utils::cast<DownloadHandlerTexture>(www->downloadHandler);

        while (!www->isDone) {
            if (cancellationToken.IsCancellationRequested) {
                www->Dispose();
                onFailure("Cancelled", pos, cancellationToken);
                co_return;
            }
            co_yield nullptr;
        }

        if (www->result == UnityWebRequest::Result::ProtocolError || www->result == UnityWebRequest::Result::ConnectionError) {
            onFailure(www->error, pos, cancellationToken);
            www->Dispose();
            co_return;
        }
        if (!System::String::IsNullOrEmpty(www->error)) {
            onFailure(www->error, pos, cancellationToken);
            www->Dispose();
            co_return;
        }

        Sprite* sprite = BSML::Utilities::LoadSpriteFromTexture(handler->texture);
        www->Dispose();
        onSuccess(sprite, pos, url, cancellationToken);
        co_return;
    }

    void ProfilePictureView::SetProfileImage(string url, int pos, CancellationToken cancellationToken) {
        if (!cancellationToken.IsCancellationRequested) {
            if (SpriteCache::cachedSprites.contains(url) && SpriteCache::cachedSprites[url].isAlive()) {
                profileImage->gameObject->SetActive(true);
                profileImage->sprite = SpriteCache::cachedSprites[url].ptr();
                loadingIndicator->gameObject->SetActive(false);
            } else {
                loadingIndicator->gameObject->SetActive(true);

                using namespace std::placeholders;
                BeginCoroutine(GetSpriteAvatar(url, bind(&ProfilePictureView::OnAvatarDownloadSuccess, this, _1, _2, _3, _4), bind(&ProfilePictureView::OnAvatarDownloadFailure, this, _1, _2, _3), cancellationToken, pos));
            }
        } else {
            OnAvatarDownloadFailure("Cancelled", pos, cancellationToken);
        }
        SpriteCache::MaintainSpriteCache();
    }

    void ProfilePictureView::OnAvatarDownloadSuccess(Sprite* a, int pos, string url, CancellationToken cancellationToken) {
        SpriteCache::AddSpriteToCache(url, a);
        if (cancellationToken.IsCancellationRequested) {
            return;
        }
        profileImage->gameObject->SetActive(true);
        profileImage->sprite = a;
        loadingIndicator->gameObject->SetActive(false);
    }

    void ProfilePictureView::OnAvatarDownloadFailure(string error, int pos, CancellationToken cancellationToken) {
        if (cancellationToken.IsCancellationRequested) {
            return;
        }
        ClearSprite();
    }

    void ProfilePictureView::ClearSprite() {
        if (profileImage.isAlive()) {
            profileImage->sprite = nullSprite.ptr();
        }
        if (loadingIndicator.isAlive()) {
            loadingIndicator->gameObject->SetActive(false);
        }
    }
}