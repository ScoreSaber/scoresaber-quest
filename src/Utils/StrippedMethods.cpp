#include "Utils/StrippedMethods.hpp"

namespace StrippedMethods {
    namespace UnityEngine {
        namespace Application {
            void OpenURL(StringW url) {
                using OpenURL_funcptr = void (*)(StringW);
                static auto OpenURL_func = (OpenURL_funcptr)il2cpp_functions::resolve_icall("UnityEngine.Application::OpenURL");
                OpenURL_func(url);
            }
        } // namespace Application
        
        namespace Networking {
            namespace UnityWebRequest {
                ::UnityEngine::Networking::UnityWebRequest::UnityWebRequestError SetRequestHeader(::UnityEngine::Networking::UnityWebRequest* request, StringW header, StringW value) {
                    using SetRequestHeader_funcptr = ::UnityEngine::Networking::UnityWebRequest::UnityWebRequestError (*)(::UnityEngine::Networking::UnityWebRequest*, StringW, StringW);
                    static auto SetRequestHeader_func = (SetRequestHeader_funcptr)il2cpp_functions::resolve_icall("UnityEngine.Networking.UnityWebRequest::InternalSetRequestHeader");
                    return SetRequestHeader_func(request, header, value);
                }
            } // namespace UnityWebRequest
        } // namespace Networking
    } // namespace UnityEngine
    namespace System {
        namespace IO {
            namespace Path {
                ArrayW<char16_t> GetInvalidFileNameChars() {
                    return ArrayW<char16_t> {
                        u'"',
                        u'<',
                        u'>',
                        u'|',
                        u'\x0000',
                        u'\x0001',
                        u'\x0002',
                        u'\x0003',
                        u'\x0004',
                        u'\x0005',
                        u'\x0006',
                        u'\x0007',
                        u'\x0008',
                        u'\x0009',
                        u'\x000A',
                        u'\x000B',
                        u'\x000C',
                        u'\x000D',
                        u'\x000E',
                        u'\x000F',
                        u'\x0010',
                        u'\x0011',
                        u'\x0012',
                        u'\x0013',
                        u'\x0014',
                        u'\x0015',
                        u'\x0016',
                        u'\x0017',
                        u'\x0018',
                        u'\x0019',
                        u'\x001A',
                        u'\x001B',
                        u'\x001C',
                        u'\x001D',
                        u'\x001E',
                        u'\x001F',
                        u':',
                        u'*',
                        u'?',
                        u'\\',
                        u'/'
                    };
                }
            }
        }
    }
} // namespace StrippedMethods