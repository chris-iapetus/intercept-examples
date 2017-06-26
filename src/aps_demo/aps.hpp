#pragma once

#include "intercept.hpp"

using namespace intercept;

namespace aps_demo {
    class aps {
    public:
        explicit aps(types::object vehicle_);
        ~aps();

        void on_frame() const;
        void add_track(types::object &projectile_);

        void engage(types::object &projectile_);

        static std::list<aps> aps_systems;
    protected:
        types::object _vehicle;
        std::list<types::object> _tracking; // the list of projectiles currently being tracked

        uint32_t _ammo_count;
        float _last_engagement_time;


    };
}