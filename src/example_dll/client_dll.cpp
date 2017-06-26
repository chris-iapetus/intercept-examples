#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>
#include <chrono>

using namespace std::literals::chrono_literals;

#include "intercept.hpp"
#include "logging.hpp"
#include "client/client.hpp"
#include "client/pointers.hpp"
#include "client/sqf/uncategorized.hpp"

INITIALIZE_EASYLOGGINGPP

std::thread test_thread;
std::atomic<bool> stop_thread;

std::list<object> bullets;
std::map<object, std::vector<vector3>> lines;

int __cdecl intercept::api_version() {
    return 1;
}

void __cdecl intercept::on_frame() {
    rv_color color{ 1.0f, 0.0f, 0.0f, 1.0f };
    for (auto bullet : lines) {
        auto point = bullet.second.begin();
        while (point != bullet.second.end()) {
            vector3 &pos1 = *point;
            ++point;
            if (point != bullet.second.end()) {
                vector3 &pos2 = *point;
                intercept::sqf::draw_line_3d(pos1, pos2, color);
            }
        }
    }
    object player = sqf::player();
    sqf::side_chat(player, "banana");
}

void test_thread_func() {
    LOG(DEBUG) << "START TEST THREAD";
    while (!stop_thread) {
        if (bullets.size()) {
            invoker_lock track_lock;
            auto it = bullets.begin();
            while (it != bullets.end()) {
                if (it->is_null()) {
                    lines.erase(*it);
                    bullets.erase(it++);
                    LOG(INFO) << "Null Bullet!";
                }
                else {
                    LOG(DEBUG) << "GET POS!";
                    lines[*it].push_back(intercept::sqf::get_pos(*it));
                    ++it;
                }
            }
        }
        std::this_thread::sleep_for(100ms);
    }
    LOG(DEBUG) << "SHUT DOWN TEST THREAD";
}

void __cdecl intercept::post_init() {
    stop_thread = false;
    test_thread = std::thread(&test_thread_func);
}
void __cdecl intercept::mission_stopped() {
    stop_thread = true;
    test_thread.join();
    lines.clear();
}

void __cdecl intercept::fired(
    object&  /*unit_*/,
    r_string /*weapon_*/,
    r_string /*muzzle_*/,
    r_string /*mode_*/,
    r_string /*ammo_*/,
    r_string /*magazine*/,
    object&  projectile_) {

    bullets.push_back(projectile_);
    lines[projectile_].push_back(intercept::sqf::get_pos(projectile_));
}

void Init() {
    el::Configurations conf;

    conf.setGlobally(el::ConfigurationType::Filename, "logs/intercept_example_dll.log");
    conf.setGlobally(el::ConfigurationType::MaxLogFileSize, "10240");
#ifdef _DEBUG
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%datetime] - %level - {%loc}t:%thread- %msg");
    conf.setGlobally(el::ConfigurationType::PerformanceTracking, "true");
#else
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime-{%level}- %msg");
#endif
    el::Loggers::setDefaultConfigurations(conf, true);

    LOG(INFO) << "Intercept Example DLL Loaded";
}

void Cleanup() {

}


BOOL APIENTRY DllMain(HMODULE /*hModule*/,
    DWORD  ul_reason_for_call,
    LPVOID /*lpReserved*/
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Init();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        Cleanup();
        break;
    }
    return TRUE;
}
