#include "orz/sync/cartridge.h"

namespace orz {

    Cartridge::Cartridge()
            : dry(true), bullet(nullptr), shell(nullptr), powder(&Cartridge::operating, this) {
    }

    Cartridge::~Cartridge() {
        dry = false;
        fire_cond.notify_all();
        powder.join();
    }

    void Cartridge::fire(int signet, const Cartridge::bullet_type &bullet, const Cartridge::shell_type &shell) {
        std::unique_lock <std::mutex> locker(fire_mutex);
        this->signet = signet;
        this->bullet = bullet;
        this->shell = shell;
        fire_cond.notify_all();
    }

    bool Cartridge::busy() {
        std::unique_lock <std::mutex> _(fire_mutex);
        return bullet != nullptr;
    }

    void Cartridge::join() {
        std::unique_lock <std::mutex> locker(fire_mutex);
        while (bullet) fire_cond.wait(locker);
    }

    void Cartridge::operating() {
        std::unique_lock <std::mutex> locker(fire_mutex);
        while (dry) {
            while (dry && !bullet) fire_cond.wait(locker);
            if (!dry) break;
            bullet(signet);
            if (shell) shell(signet);
            bullet = nullptr;
            shell = nullptr;
            fire_cond.notify_all();
        }
    }

}