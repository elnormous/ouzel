// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <cstring>
#include "Pitch.hpp"
#include "Audio.hpp"
#include "math/MathUtils.hpp"

/****************************************************************************
 * COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 *
 *                         The Wide Open License (WOL)
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice and this license appear in all source copies.
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND. See http://www.dspguru.com/wol.htm for more information.
 *
 *****************************************************************************/

static const uint32_t MAX_FRAME_LENGTH = 8192;

static void smbFft(float* fftBuffer, unsigned long fftFrameSize, long sign)
{
    for (unsigned long i = 2; i < 2 * fftFrameSize - 2; i += 2)
    {
        unsigned long j = 0;

        for (unsigned long bitm = 2; bitm < 2 * fftFrameSize; bitm <<= 1)
        {
            if (i & bitm) j++;
            j <<= 1;
        }

        if (i < j)
        {
            float* p1 = fftBuffer + i;
            float* p2 = fftBuffer + j;
            float temp = *p1; *(p1++) = *p2;
            *(p2++) = temp; temp = *p1;
            *p1 = *p2; *p2 = temp;
        }
    }

    for (unsigned long k = 0, le = 2; k < (unsigned long)(log(fftFrameSize) / log(2.) + 0.5); k++)
    {
        le <<= 1;
        unsigned long le2 = le >> 1;
        float ur = 1.0;
        float ui = 0.0;
        float arg = ouzel::PI / (le2 >> 1);
        float wr = cos(arg);
        float wi = sign * sin(arg);
        for (unsigned long j = 0; j < le2; j += 2)
        {
            float* p1r = fftBuffer + j;
            float* p1i = p1r + 1;
            float* p2r = p1r + le2;
            float* p2i = p2r + 1;
            for (unsigned long i = j; i < 2 * fftFrameSize; i += le)
            {
                float tr = *p2r * ur - *p2i * ui;
                float ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }
            float tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

namespace ouzel
{
    namespace audio
    {
        static const float MIN_PITCH = 0.5F;
        static const float MAX_PITCH = 2.0F;

        class STFT
        {
        public:
            STFT()
            {
                memset(inFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(outFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(fftWorksp, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
                memset(lastPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
                memset(sumPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
                memset(outputAccum, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
                memset(anaFreq, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(anaMagn, 0, MAX_FRAME_LENGTH * sizeof(float));
            }

            void smbPitchShift(float pitchShift, unsigned long numSampsToProcess, unsigned long fftFrameSize, unsigned long osamp, float sampleRate, float* indata, float* outdata)
            {
                // set up some handy variables
                unsigned long fftFrameSize2 = fftFrameSize / 2;
                unsigned long stepSize = fftFrameSize / osamp;
                float freqPerBin = sampleRate / static_cast<float>(fftFrameSize);
                float expct = 2.0F * PI * static_cast<float>(stepSize) / static_cast<float>(fftFrameSize);
                unsigned long inFifoLatency = fftFrameSize - stepSize;
                if (rover == 0) rover = inFifoLatency;

                // main processing loop
                for (unsigned long i = 0; i < numSampsToProcess; i++)
                {
                    // As long as we have not yet collected enough data just read in
                    inFIFO[rover] = indata[i];
                    outdata[i] = outFIFO[rover - inFifoLatency];
                    rover++;

                    // now we have enough data for processing
                    if (rover >= fftFrameSize)
                    {
                        rover = inFifoLatency;

                        // do windowing and re,im interleave
                        for (unsigned long k = 0; k < fftFrameSize; k++)
                        {
                            float window = -0.5F * cosf(2.0F * PI * static_cast<float>(k) / static_cast<float>(fftFrameSize)) + 0.5F;
                            fftWorksp[2 * k] = inFIFO[k] * window;
                            fftWorksp[2 * k + 1] = 0.0F;
                        }

                        // ***************** ANALYSIS *******************
                        // do transform
                        smbFft(fftWorksp, fftFrameSize, -1);

                        // this is the analysis step
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            // de-interlace FFT buffer
                            float real = fftWorksp[2 * k];
                            float imag = fftWorksp[2 * k + 1];

                            // compute magnitude and phase
                            float magn = 2.0F * sqrtf(real * real + imag*imag);

                            float phase;
                            float signx = (imag > 0.0F) ? 1.0F : -1.0F;
                            if (imag == 0.0F) phase = 0.0F;
                            else if (real == 0.0F) phase = signx * PI / 2.0F;
                            else phase = atan2(imag, real);

                            // compute phase difference
                            float tmp = phase - lastPhase[k];
                            lastPhase[k] = phase;

                            // subtract expected phase difference
                            tmp -= static_cast<float>(k) * expct;

                            // map delta phase into +/- Pi interval
                            long qpd = static_cast<long>(tmp / PI);
                            if (qpd >= 0) qpd += qpd & 1;
                            else qpd -= qpd & 1;
                            tmp -= PI * static_cast<float>(qpd);

                            // get deviation from bin frequency from the +/- Pi interval
                            tmp = osamp * tmp / (2.0F * PI);

                            // compute the k-th partials' true frequency
                            tmp = static_cast<float>(k) * freqPerBin + tmp * freqPerBin;

                            // store magnitude and true frequency in analysis arrays
                            anaMagn[k] = magn;
                            anaFreq[k] = tmp;
                        }

                        // ***************** PROCESSING *******************
                        // this does the actual pitch shifting
                        memset(synMagn, 0, fftFrameSize * sizeof(float));
                        memset(synFreq, 0, fftFrameSize * sizeof(float));
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            unsigned long index = static_cast<unsigned long>(k * pitchShift);
                            if (index <= fftFrameSize2)
                            {
                                synMagn[index] += anaMagn[k];
                                synFreq[index] = anaFreq[k] * pitchShift;
                            }
                        }

                        // ***************** SYNTHESIS *******************
                        // this is the synthesis step
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            // get magnitude and true frequency from synthesis arrays
                            float magn = synMagn[k];
                            float tmp = synFreq[k];

                            // subtract bin mid frequency
                            tmp -= static_cast<float>(k) * freqPerBin;

                            // get bin deviation from freq deviation
                            tmp /= freqPerBin;

                            // take osamp into account
                            tmp = 2.0F * PI * tmp / osamp;

                            // add the overlap phase advance back in
                            tmp += static_cast<float>(k) * expct;

                            // accumulate delta phase to get bin phase
                            sumPhase[k] += tmp;
                            float phase = sumPhase[k];

                            // get real and imag part and re-interleave
                            fftWorksp[2 * k] = magn * cosf(phase);
                            fftWorksp[2 * k + 1] = magn * sinf(phase);
                        }

                        // zero negative frequencies
                        for (unsigned long k = fftFrameSize + 2; k < 2 * fftFrameSize; k++) fftWorksp[k] = 0.0;

                        // do inverse transform
                        smbFft(fftWorksp, fftFrameSize, 1);

                        // do windowing and add to output accumulator
                        for (unsigned long k = 0; k < fftFrameSize; k++)
                        {
                            float window = -0.5F * cos(2.0F * PI * static_cast<float>(k) / static_cast<float>(fftFrameSize)) + 0.5F;
                            outputAccum[k] += 2.0F * window * fftWorksp[2 * k] / (fftFrameSize2 * osamp);
                        }
                        for (unsigned long k = 0; k < stepSize; k++) outFIFO[k] = outputAccum[k];

                        // shift accumulator
                        memmove(outputAccum, outputAccum+stepSize, fftFrameSize * sizeof(float));

                        // move input FIFO
                        for (unsigned long k = 0; k < inFifoLatency; k++) inFIFO[k] = inFIFO[k + stepSize];
                    }
                }
            }

        private:
            float inFIFO[MAX_FRAME_LENGTH];
            float outFIFO[MAX_FRAME_LENGTH];
            float fftWorksp[2 * MAX_FRAME_LENGTH];
            float lastPhase[MAX_FRAME_LENGTH / 2 + 1];
            float sumPhase[MAX_FRAME_LENGTH / 2 + 1];
            float outputAccum[2 * MAX_FRAME_LENGTH];
            float anaFreq[MAX_FRAME_LENGTH];
            float anaMagn[MAX_FRAME_LENGTH];
            float synFreq[MAX_FRAME_LENGTH];
            float synMagn[MAX_FRAME_LENGTH];
            unsigned long rover = 0;
        };

        class PitchProcessor final: public Processor
        {
        public:
            PitchProcessor(float initPitch):
                pitch(clamp(initPitch, MIN_PITCH, MAX_PITCH))
            {
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
                channelSamples.resize(frames);
                stft.resize(channels);

                for (uint16_t channel = 0; channel < channels; ++channel)
                {
                    for (uint32_t sample = 0; sample < frames; ++sample)
                        channelSamples[sample] = samples[sample * channels + channel];

                    stft[channel].smbPitchShift(pitch, frames, 1024, 4, sampleRate, channelSamples.data(), channelSamples.data());

                    for (uint32_t sample = 0; sample < frames; ++sample)
                        samples[sample * channels + channel] = channelSamples[sample];
                }
            }

            void setPitch(float newPitch)
            {
                pitch = clamp(newPitch, MIN_PITCH, MAX_PITCH);
            }

        private:
            float pitch = 1.0f;
            std::vector<float> channelSamples;
            std::vector<STFT> stft;
        };

        Pitch::Pitch(Audio& initAudio, float initPitch):
            Filter(initAudio,
                   initAudio.initProcessor(std::unique_ptr<Processor>(new PitchProcessor(initPitch)))),
            pitch(initPitch)
        {
        }

        Pitch::~Pitch()
        {
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;

            audio.updateProcessor(processorId, [newPitch](Object* node) {
                PitchProcessor* pitchProcessor = static_cast<PitchProcessor*>(node);
                pitchProcessor->setPitch(newPitch);
            });
        }
    } // namespace audio
} // namespace ouzel
