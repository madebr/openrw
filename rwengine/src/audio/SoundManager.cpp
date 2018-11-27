#include "audio/SoundManager.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
}

#include "audio/alCheck.hpp"
#include "engine/GameData.hpp"
#include "engine/GameWorld.hpp"
#include "render/ViewCamera.hpp"

#include <rw/types.hpp>

Sound& SoundManager::getSoundRef(size_t name) {
    auto ref = buffers.find(name);
    if (ref != buffers.end()) {
        return ref->second;
    }
    createSfxInstance(name);

    return buffers[name];
}

Sound& SoundManager::getSoundRef(const std::string& name) {
    return sounds[name];  // @todo reloading, how to check is it wav/mp3?
}

SoundManager::SoundManager() {
    initializeOpenAL();
    initializeAVCodec();
}

SoundManager::SoundManager(GameWorld* engine) : _engine(engine) {
    auto sdtPath = _engine->data->index.findFilePath("audio/sfx.SDT");
    auto rawPath = _engine->data->index.findFilePath("audio/sfx.RAW");
    sdt.load(sdtPath, rawPath);

    initializeOpenAL();
    initializeAVCodec();
}

SoundManager::~SoundManager() {
    deinitializeOpenAL();
}

bool SoundManager::initializeOpenAL() {
    alDevice = alcOpenDevice(nullptr);
    if (!alDevice) {
        RW_ERROR("Could not find OpenAL device!");
        return false;
    }

    alContext = alcCreateContext(alDevice, nullptr);
    if (!alContext) {
        RW_ERROR("Could not create OpenAL context!");
        return false;
    }

    if (!alcMakeContextCurrent(alContext)) {
        RW_ERROR("Unable to make OpenAL context current!");
        return false;
    }

    // Needed for max distance
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    return true;
}

void SoundManager::initializeAVCodec() {
#if defined(RW_DEBUG) && defined(RW_VERBOSE_DEBUG_MESSAGES)
    av_log_set_level(AV_LOG_WARNING);
#else
    av_log_set_level(AV_LOG_ERROR);
#endif

    // Some older versions of FFmpeg require it
    // before calling  avformat_alloc_context()
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
}

void SoundManager::deinitializeOpenAL() {
    // Buffers have to been removed before openAL is deinitialized
    sounds.clear();
    buffers.clear();

    // De-initialize OpenAL
    if (alContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(alContext);
    }
    alContext = nullptr;

    if (alDevice) {
        alcCloseDevice(alDevice);
    }
    alDevice = nullptr;
}

bool SoundManager::loadSound(const std::string& name,
                             const std::string& fileName) {
    Sound* sound = nullptr;
    auto sound_iter = sounds.find(name);

    if (sound_iter != sounds.end()) {
        sound = &sound_iter->second;
    } else {
        auto [it, emplaced] = sounds.emplace(std::piecewise_construct,
                                       std::forward_as_tuple(name),
                                       std::forward_as_tuple());
        sound = &it->second;

        sound->source = std::make_shared<SoundSource>();
        sound->buffer = std::make_unique<SoundBuffer>();

        sound->source->loadFromFile(fileName);
        sound->isLoaded = sound->buffer->bufferData(*sound->source);
    }

    return sound->isLoaded;
}

void SoundManager::loadSound(size_t index) {
    Sound* sound = nullptr;

    auto [it, emplaced] =
        sfx.emplace(std::piecewise_construct, std::forward_as_tuple(index),
                    std::forward_as_tuple());
    sound = &it->second;

    sound->source = std::make_shared<SoundSource>();
    sound->source->loadSfx(sdt, index);
}

size_t SoundManager::createSfxInstance(size_t index) {
    Sound* sound = nullptr;
    auto soundRef = sfx.find(index);

    if(soundRef == sfx.end()) {
        // Sound source is not loaded yet
        loadSound(index);
        soundRef = sfx.find(index);
    }

    // Try to reuse first available buffer
    // (aka with stopped state)
    for (auto& [id, sound] : buffers) {
        if (sound.buffer && sound.isStopped()) {
            // Let's use this buffer
            sound.buffer = std::make_unique<SoundBuffer>();
            sound.source = soundRef->second.source;
            sound.isLoaded =
                sound.buffer->bufferData(*sound.source);
            return id;
        }
    }
    // There's no available free buffer, so
    // we should create a new one.
    auto [it, emplaced] = buffers.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(bufferNr),
                                    std::forward_as_tuple());
    sound = &it->second;

    sound->id = bufferNr;
    sound->buffer = std::make_unique<SoundBuffer>();
    sound->source = soundRef->second.source;
    sound->isLoaded = sound->buffer->bufferData(*sound->source);
    bufferNr++;

    return sound->id;
}

bool SoundManager::isLoaded(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        return sound->second.isLoaded;
    }
    return false;
}

bool SoundManager::isPlaying(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        return sound->second.isPlaying();
    }
    return false;
}

bool SoundManager::isStopped(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        return sound->second.isStopped();
    }
    return false;
}

bool SoundManager::isPaused(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        return sound->second.isPaused();
    }
    return false;
}

void SoundManager::playSound(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        auto vol = getCalculatedVolumeOfMusic();
        sound->second.setGain(vol);
        sound->second.play();
    }
}

void SoundManager::eraseSound(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        sounds.erase(sound);
    } else {
        RW_MESSAGE("Tried to erase no existing sound " << name);
    }
}

void SoundManager::playSfx(size_t name, const glm::vec3& position, bool looping,
                           int maxDist) {
    auto buffer = buffers.find(name);
    if (buffer != buffers.end()) {
        buffer->second.setPosition(position);
        if (looping) {
            buffer->second.setLooping(looping);
        }

        buffer->second.setPitch(1.f);
        buffer->second.setGain(getCalculatedVolumeOfEffects());
        if (maxDist != -1) {
            buffer->second.setMaxDistance(static_cast<float>(maxDist));
        }
        buffer->second.play();
    }
}

void SoundManager::pauseAllSounds() {
    for (auto& sound : sounds) {
        if (sound.second.isPlaying()) {
            sound.second.pause();
        }
    }
    for (auto& sound : buffers) {
        if (sound.second.isPlaying()) {
            sound.second.pause();
        }
    }
}

void SoundManager::resumeAllSounds() {
    for (auto& sound : sounds) {
        if (sound.second.isPaused()) {
            sound.second.play();
        }
    }
    for (auto& sound : buffers) {
        if (sound.second.isPaused()) {
            sound.second.play();
        }
    }
}

bool SoundManager::playBackground(const std::string& fileName) {
    if (this->loadSound(fileName, fileName)) {
        backgroundNoise = fileName;
        auto& sound = getSoundRef(fileName);
        sound.play();
        return true;
    }

    return false;
}

bool SoundManager::loadMusic(const std::string& name,
                             const std::string& fileName) {
    return loadSound(name, fileName);
}

void SoundManager::playMusic(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        sound->second.play();
    }
}

void SoundManager::stopMusic(const std::string& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        sound->second.stop();
    }
}

void SoundManager::pause(bool p) {
    if (backgroundNoise.length() > 0) {
        if (p) {
            pauseAllSounds();
        } else {
            resumeAllSounds();
        }
    }
}

void SoundManager::updateListenerTransform(const ViewCamera& cam) {
    // Orientation
    auto up = cam.rotation * glm::vec3(0.f, 0.f, 1.f);
    auto at = cam.rotation * glm::vec3(1.f, 0.f, 0.f);
    float orientation[6] = {at.x, at.y, at.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);

    // Position
    float position[3] = {cam.position.x, cam.position.y, cam.position.z};
    alListenerfv(AL_POSITION, position);

    // @todo ShFil119 it should be implemented
    // Velocity
    // float velocity[3] = ...
    // alListenerfv(AL_VELOCITY, velocity);
}

void SoundManager::setSoundPosition(const std::string& name,
                                    const glm::vec3& position) {
    if (sounds.find(name) != sounds.end()) {
        alCheck(alSource3f(sounds[name].buffer->source, AL_POSITION, position.x,
                           position.y, position.z));
    }
}


void SoundManager::setVolume(float vol) {
    _volume = vol;
}

float SoundManager::getCalculatedVolumeOfEffects() const {
    return _volume * _effectsVolume;
}

float SoundManager::getCalculatedVolumeOfMusic() const {
    return _volume * _musicVolume;
}
