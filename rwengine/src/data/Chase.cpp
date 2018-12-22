#include "data/Chase.hpp"

#include <cstdint>
#include <cstddef>
#include <fstream>

#include <rw/debug.hpp>

#include "engine/GameWorld.hpp"
#include "objects/GameObject.hpp"

#define KEYFRAMES_PER_SECOND 30

bool ChaseKeyframe::load(const std::string &filePath,
                         std::vector<ChaseKeyframe> &frames) {
    std::ifstream chaseFile(filePath, std::ios_base::binary);
    RW_CHECK(chaseFile.is_open(), "Failed to open chase file");
    if (!chaseFile.is_open()) {
        return false;
    }

    chaseFile.seekg(0, std::ios_base::end);
    size_t fileLength;
    if (auto tmp_fileLength = chaseFile.tellg(); tmp_fileLength < 0) {
        return false;
    } else {
        fileLength = static_cast<size_t>(tmp_fileLength);
    }
    chaseFile.seekg(0);

    struct ChaseEntryRecord {
        int16_t velocity[3];
        int8_t right[3];
        int8_t up[3];
        uint8_t steering;
        uint8_t driving;
        uint8_t braking;
        uint8_t handbrake;
        glm::vec3 position{};
    };

    static_assert(sizeof(ChaseEntryRecord) == 28,
                  "ChaseEntryRecord is not 28 bytes");
    RW_CHECK(fileLength % sizeof(ChaseEntryRecord) == 0,
             "File is not a mulitple of 28 byte");

    size_t recordCount = fileLength / sizeof(ChaseEntryRecord);

    for (size_t i = 0; i < recordCount; ++i) {
        ChaseEntryRecord rec;
        chaseFile.read(reinterpret_cast<char *>(&rec),
                       sizeof(ChaseEntryRecord));
        glm::vec3 velocity{
            rec.velocity[0] / 16383.5f,
            rec.velocity[1] / 16383.5f,
            rec.velocity[2] / 16383.5f,
        };
        glm::vec3 right{
            rec.right[0] / 127.5f,
            rec.right[1] / 127.5f,
            rec.right[2] / 127.5f,
        };
        glm::vec3 up{
            rec.up[0] / 127.5f,
            rec.up[1] / 127.5f,
            rec.up[2] / 127.5f,
        };
        glm::mat3 rotation(right, up, glm::cross(right, up));
        frames.emplace_back(velocity, rec.steering, rec.driving, rec.braking,
                            !!rec.handbrake, rec.position,
                            glm::quat_cast(rotation));
    }

    return true;
}

bool ChaseCoordinator::addChaseVehicle(GameObject *vehicle, int index,
                                       const std::string &pathFile) {
    std::vector<ChaseKeyframe> keyframes;
    bool result = ChaseKeyframe::load(pathFile, keyframes);
    RW_CHECK(result, "Failed to load chase keyframes: " + pathFile);
    chaseVehicles[index] = {std::move(keyframes), vehicle};
    return result;
}

GameObject *ChaseCoordinator::getChaseVehicle(int index) {
    return chaseVehicles.at(index).object;
}

void ChaseCoordinator::removeChaseVehicle(int index) {
    auto it = chaseVehicles.find(index);
    RW_CHECK(it != chaseVehicles.end(), "Vehicle not in chase");
    if (it != chaseVehicles.end()) {
        chaseVehicles.erase(it);
    }
}

void ChaseCoordinator::start() {
    chaseTime = 0.f;
}

void ChaseCoordinator::update(float dt) {
    chaseTime += dt;
    auto frameNum = static_cast<size_t>(chaseTime * KEYFRAMES_PER_SECOND);
    for (auto &[index, chase] : chaseVehicles) {
        RW_CHECK(frameNum < chase.keyframes.size(),
                 "Vehicle out of chase keyframes");
        if (frameNum >= chase.keyframes.size()) continue;

        const ChaseKeyframe &kf = chase.keyframes[frameNum];
        chase.object->setPosition(kf.position);
        chase.object->setRotation(kf.rotation);
    }
}

void ChaseCoordinator::reserve(size_t nr) {
    chaseVehicles.reserve(nr);
}

void ChaseCoordinator::cleanup() {
    for (auto &[index, chase] : chaseVehicles) {
        chase.object->engine->destroyObject(chase.object);
    }
    chaseVehicles.clear();
}
