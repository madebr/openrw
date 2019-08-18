#include "loaders/LoaderIFP.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <cctype>
#include <memory>

bool findKeyframes(float t, AnimationBone* bone, AnimationKeyframe& f1,
                   AnimationKeyframe& f2, float& alpha) {
    for (size_t f = 0; f < bone->frames.size(); ++f) {
        if (t <= bone->frames[f].starttime) {
            f2 = bone->frames[f];

            if (f == 0) {
                if (bone->frames.size() != 1) {
                    f1 = bone->frames.back();
                } else {
                    f1 = f2;
                }
            } else {
                f1 = bone->frames[f - 1];
            }

            float tdiff = (f2.starttime - f1.starttime);
            if (tdiff == 0.f) {
                alpha = 1.f;
            } else {
                alpha = glm::clamp((t - f1.starttime) / tdiff, 0.f, 1.f);
            }

            return true;
        }
    }
    return false;
}

AnimationKeyframe AnimationBone::getInterpolatedKeyframe(float time) {
    AnimationKeyframe f1, f2;
    float alpha;

    if (findKeyframes(time, this, f1, f2, alpha)) {
        return {glm::normalize(glm::slerp(f1.rotation, f2.rotation, alpha)),
                glm::mix(f1.position, f2.position, alpha),
                glm::mix(f1.scale, f2.scale, alpha), time,
                std::max(f1.id, f2.id)};
    }

    return frames.back();
}

AnimationKeyframe AnimationBone::getKeyframe(float time) {
    for (auto &frame : frames) {
        if (time >= frame.starttime) {
            return frame;
        }
    }
    return frames.back();
}

bool LoaderIFP::loadFromMemory(const char* data) {
    size_t data_offs = 0;
    size_t* dataI = &data_offs;

    const ANPK* fileRoot = read<ANPK>(data, dataI);
    std::string listname = readString(data, dataI);

    animations.reserve(fileRoot->info.entries);

    for (auto a = 0u; a < fileRoot->info.entries; ++a) {
        // something about a name?
        /*NAME* n =*/read<NAME>(data, dataI);
        std::string animname = readString(data, dataI);

        auto animation = std::make_shared<Animation>();
        animation->duration = 0.f;
        animation->name = animname;

        size_t animstart = data_offs + 8;
        const DGAN* animroot = read<DGAN>(data, dataI);
        std::string infoname = readString(data, dataI);

        animation->bones.reserve(animroot->info.entries);

        for (auto c = 0u; c < animroot->info.entries; ++c) {
            size_t start = data_offs;
            const CPAN* cpan = read<CPAN>(data, dataI);
            const ANIM* frames = read<ANIM>(data, dataI);

            AnimationBone boneData{};
            boneData.name = frames->name;
            boneData.frames.reserve(frames->frames);

            data_offs += ((8 + frames->base.size) - sizeof(ANIM));

            const KFRM* frame = read<KFRM>(data, dataI);
            std::string type(frame->base.magic, 4);

            float time = 0.f;

            if (type == "KR00") {
                boneData.type = AnimationBone::R00;
                for (auto d = 0u; d < frames->frames; ++d) {
                    glm::quat q = glm::conjugate(*read<glm::quat>(data, dataI));
                    time = *read<float>(data, dataI);
                    boneData.frames.emplace_back(q, glm::vec3(0.f, 0.f, 0.f),
                                                glm::vec3(1.f, 1.f, 1.f), time,
                                                d);
                }
            } else if (type == "KRT0") {
                boneData.type = AnimationBone::RT0;
                for (auto d = 0u; d < frames->frames; ++d) {
                    glm::quat q = glm::conjugate(*read<glm::quat>(data, dataI));
                    glm::vec3 p = *read<glm::vec3>(data, dataI);
                    time = *read<float>(data, dataI);
                    boneData.frames.emplace_back(
                        q, p, glm::vec3(1.f, 1.f, 1.f), time, d);
                }
            } else if (type == "KRTS") {
                boneData.type = AnimationBone::RTS;
                for (auto d = 0u; d < frames->frames; ++d) {
                    glm::quat q = glm::conjugate(*read<glm::quat>(data, dataI));
                    glm::vec3 p = *read<glm::vec3>(data, dataI);
                    glm::vec3 s = *read<glm::vec3>(data, dataI);
                    time = *read<float>(data, dataI);
                    boneData.frames.emplace_back(q, p, s, time, d);
                }
            }

            boneData.duration = time;
            animation->duration =
                std::max(boneData.duration, animation->duration);

            data_offs = start + sizeof(CPAN) + cpan->base.size;

            std::string framename(frames->name);
            std::transform(framename.begin(), framename.end(),
                           framename.begin(), ::tolower);

            animation->bones.emplace(framename, std::move(boneData));
        }

        data_offs = animstart + animroot->base.size;

        std::transform(animname.begin(), animname.end(), animname.begin(),
                       ::tolower);
        animations.emplace(animname, animation);
    }

    return true;
}

std::string LoaderIFP::readString(const char* data, size_t* ofs) {
    size_t b = *ofs;
    for (size_t o = *ofs; (o = *ofs);) {
        *ofs += 4;
        if (data[o + 0] == 0) break;
        if (data[o + 1] == 0) break;
        if (data[o + 2] == 0) break;
        if (data[o + 3] == 0) break;
    }
    return std::string(data + b);
}
