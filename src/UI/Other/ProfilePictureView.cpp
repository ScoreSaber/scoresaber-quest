#include "UI/Other/ProfilePictureView.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include <map>
#include <queue>

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace System::Threading;
using namespace UnityEngine;
using namespace UnityEngine::Networking;
using namespace std;

#define BeginCoroutine(method)                                               \
    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine( \
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
            mat_UINoGlowRoundEdge = ArrayUtil::First(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) { return to_utf8(csstrtostr(x->get_name())) == "UINoGlowRoundEdge"; });
            initializedGlobals = true;
        }

        profileImage->set_material(mat_UINoGlowRoundEdge.ptr());
        profileImage->set_sprite(nullSprite.ptr());
        profileImage->get_gameObject()->SetActive(true);
        loadingIndicator->get_gameObject()->SetActive(false);
    }

    custom_types::Helpers::Coroutine GetSpriteAvatar(string url, function<void(Sprite*, int, string, CancellationToken)> onSuccess, function<void(string, int, CancellationToken)> onFailure, CancellationToken cancellationToken, int pos) {
        Networking::UnityWebRequest* www = Networking::UnityWebRequestTexture::GetTexture(url);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        auto handler = reinterpret_cast<UnityEngine::Networking::DownloadHandlerTexture*>(www->get_downloadHandler());

        while (!www->get_isDone()) {
            if (cancellationToken.get_IsCancellationRequested()) {
                onFailure("Cancelled", pos, cancellationToken);
                co_return;
            }
            co_yield nullptr;
        }

        if (www->get_isNetworkError() || www->get_isHttpError()) {
            onFailure(www->get_error(), pos, cancellationToken);
            co_return;
        }
        
        if (!Il2CppString::IsNullOrEmpty(www->get_error())) {
            onFailure(www->get_error(), pos, cancellationToken);
            co_return;
        }

        Sprite* sprite = Sprite::Create(handler->get_texture(), Rect(0, 0, handler->get_texture()->get_width(), handler->get_texture()->get_height()), Vector2::get_one() * 0.5f, 100.0f, 0u, SpriteMeshType::Tight, Vector4::get_zero(), false);
        onSuccess(sprite, pos, url, cancellationToken);
        co_return;
    }

    void ProfilePictureView::SetProfileImage(string url, int pos, CancellationToken cancellationToken) {
        if (!cancellationToken.get_IsCancellationRequested()) {
            if (SpriteCache::cachedSprites.contains(url) && SpriteCache::cachedSprites[url].isAlive()) {
                profileImage->get_gameObject()->SetActive(true);
                profileImage->set_sprite(SpriteCache::cachedSprites[url].ptr());
                loadingIndicator->get_gameObject()->set_active(false);
            } else {
                loadingIndicator->get_gameObject()->set_active(true);

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
        if (cancellationToken.get_IsCancellationRequested()) {
            return;
        }
        profileImage->get_gameObject()->set_active(true);
        profileImage->set_sprite(a);
        loadingIndicator->get_gameObject()->set_active(false);
    }

    void ProfilePictureView::OnAvatarDownloadFailure(string error, int pos, CancellationToken cancellationToken) {
        if (cancellationToken.get_IsCancellationRequested()) {
            return;
        }
        ClearSprite();
    }

    void ProfilePictureView::ClearSprite() {
        if (profileImage.isAlive()) {
            profileImage->set_sprite(nullSprite.ptr());
        }
        if (loadingIndicator.isAlive()) {
            loadingIndicator->get_gameObject()->SetActive(false);
        }
    }
}