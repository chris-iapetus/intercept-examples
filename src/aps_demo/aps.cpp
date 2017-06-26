#include "aps.hpp"

#include <sstream>

namespace aps_demo {

    std::list<aps> aps::aps_systems;

    aps::aps(types::object vehicle_) : _vehicle(vehicle_), _ammo_count(10), _last_engagement_time(0) {

    }

    aps::~aps() {
    }

    void aps::on_frame() const {
        //vector3 current_pos = sqf::get_pos_asl(_vehicle);
        auto projectiles = sqf::near_objects(_vehicle, "MissileBase", 250.0f);
        std::stringstream ss;
        ss << "Count: " << projectiles.size();
        sqf::system_chat(ss.str());
    }

    void aps::add_track(types::object & projectile_) {
        _tracking.push_back(projectile_);
    }

    void aps::engage(types::object & projectile_) {

    }
}