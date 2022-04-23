
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <boost/process.hpp>
#include "forkmem/windows/Bridge.hxx"

#include <Windows.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")
extern "C" {
__declspec(dllimport) LONG NTAPI NtResumeProcess(HANDLE ProcessHandle);
__declspec(dllimport) LONG NTAPI NtSuspendProcess(HANDLE ProcessHandle);
}

namespace frkm {

namespace bp = boost::process;
struct Bridge::Control {
    bp::child proc;

    Control(const Executable& exec) {}
};

void Bridge::start(const Executable& exec, void* user_data) {
    control = frkm::new_object(get_allocator<Control>(), exec);

    control->proc = bp::child(
        bp::env["MAPVIEW_HANDLE"] = std::to_string(reinterpret_cast<std::size_t>(memory.map_handle)),
        bp::env["MAPVIEW_SIZE"] = std::to_string(memory.mapping_size),
        bp::env["MAPVIEW_PTR_BASE"] = std::to_string(reinterpret_cast<std::size_t>(memory.mmap_buffer)),
        bp::env["USERDATA_PTR"] = std::to_string(reinterpret_cast<std::size_t>(user_data)), exec.path);
}

void Bridge::suspend() { ::NtSuspendProcess(control->proc.native_handle()); }
void Bridge::resume() { ::NtResumeProcess(control->proc.native_handle()); }

void Bridge::stop() {
    control->proc.terminate();
    control->proc.wait();
    control->proc = {};
}

Bridge::~Bridge() {
    stop();
    frkm::delete_object(get_allocator<Control>(), control);
}
} // namespace frkm

#endif