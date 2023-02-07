#include "SoundBuffer.hpp"
#include "AL/al.h"
#include "sndfile.h"
#include <AL/alext.h>

namespace HyperAPI {
    SoundBuffer *SoundBuffer::get() {
        static SoundBuffer *snd_device = new SoundBuffer();
        return snd_device;
    }

    ALuint SoundBuffer::AddSoundEffect(const char *filename) {
        ALenum err, format;
        ALuint buffer;
        SNDFILE *sndfile;
        SF_INFO sfinfo;
        short *membuf;
        sf_count_t num_frames;
        ALsizei num_bytes;

        sndfile = sf_open(filename, SFM_READ, &sfinfo);
        if (!sndfile) {
            fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
            return 0;
        }
        if (sfinfo.frames < 1 || sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
            fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
            sf_close(sndfile);
            return 0;
        }

        format = AL_NONE;
        if (sfinfo.channels == 1)
            format = AL_FORMAT_MONO16;
        else if (sfinfo.channels == 2)
            format = AL_FORMAT_STEREO16;
        else if (sfinfo.channels == 3) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT2D_16;
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT3D_16;
        }
        if (!format) {
            fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
            sf_close(sndfile);
            return 0;
        }

        membuf = static_cast<short *>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

        num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
        if (num_frames < 1) {
            free(membuf);
            sf_close(sndfile);
            fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
            return 0;
        }
        num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

        buffer = 0;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

        free(membuf);
        sf_close(sndfile);

        err = alGetError();
        if (err != AL_NO_ERROR) {
            fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
            if (buffer && alIsBuffer(buffer))
                alDeleteBuffers(1, &buffer);
            return 0;
        }

        buffers.push_back(buffer);

        return buffer;
    }

    bool SoundBuffer::RemoveSoundEffect(const ALuint &buffer) {
        auto it = buffers.begin();
        while (it != buffers.end()) {
            if (*it == buffer) {
                alDeleteBuffers(1, &*it);

                it = buffers.erase(it);

                return true;
            } else {
                ++it;
            }
        }
        return false;
    }

    SoundBuffer::SoundBuffer() {
        buffers.clear();
    }

    SoundBuffer::~SoundBuffer() {
        alDeleteBuffers(buffers.size(), buffers.data());
        buffers.clear();
    }
} // namespace HyperAPI