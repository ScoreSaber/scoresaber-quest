
#include <beatsaber-hook/shared/utils/typedefs.h>
#include <UnityEngine/Networking/UnityWebRequest.hpp>

namespace StrippedMethods {
    namespace UnityEngine {
        namespace Application {
            void OpenURL(StringW url);
        } // namespace Application
        namespace Networking {
            namespace UnityWebRequest {
                ::UnityEngine::Networking::UnityWebRequest::UnityWebRequestError SetRequestHeader(::UnityEngine::Networking::UnityWebRequest* request, StringW header, StringW value);
            } // namespace UnityWebRequest
        } // namespace Networking
    } // namespace UnityEngine
    namespace System {
        namespace IO {
            namespace Path {
                ArrayW<char16_t> GetInvalidFileNameChars();
            }
        }
    }
} // namespace StrippedMethods