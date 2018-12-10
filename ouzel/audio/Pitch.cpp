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

/*
 FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
 Fills fftBuffer[0...2 * fftFrameSize - 1] with the Fourier transform of the
 time domain data in fftBuffer[0...2 * fftFrameSize - 1]. The FFT array takes
 and returns the cosine and sine parts in an interleaved manner, ie.
 fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
 must be a power of 2. It expects a complex input signal (see footnote 2),
 ie. when working with 'common' audio signals our input signal has to be
 passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
 of the frequencies of interest is in fftBuffer[0...fftFrameSize].
 */
static void smbFft(float* fftBuffer, unsigned long fftFrameSize, long sign)
{
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;

    for (unsigned long i = 2; i < 2 * fftFrameSize - 2; i += 2)
    {
        unsigned long bitm, j;

        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1)
        {
            if (i & bitm) j++;
            j <<= 1;
        }
        if (i < j)
        {
            p1 = fftBuffer + i; p2 = fftBuffer + j;
            temp = *p1; *(p1++) = *p2;
            *(p2++) = temp; temp = *p1;
            *p1 = *p2; *p2 = temp;
        }
    }
    for (unsigned long k = 0, le = 2; k < (unsigned long)(log(fftFrameSize) / log(2.) + .5); k++)
    {
        le <<= 1;
        unsigned long le2 = le >> 1;
        ur = 1.0;
        ui = 0.0;
        arg = ouzel::PI / (le2 >> 1);
        wr = cos(arg);
        wi = sign * sin(arg);
        for (unsigned long j = 0; j < le2; j += 2)
        {
            p1r = fftBuffer + j; p1i = p1r + 1;
            p2r = p1r + le2; p2i = p2r + 1;
            for (unsigned long i = j; i < 2 * fftFrameSize; i += le)
            {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr; *p2i = *p1i - ti;
                *p1r += tr; *p1i += ti;
                p1r += le; p1i += le;
                p2r += le; p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

namespace ouzel
{
    namespace audio
    {
        class PitchProcessor final: public Processor
        {
        public:
            PitchProcessor(float initPitch):
                pitch(initPitch)
            {
                memset(gInFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(gOutFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(gFFTworksp, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
                memset(gLastPhase, 0, (MAX_FRAME_LENGTH / 2 + 1)*sizeof(float));
                memset(gSumPhase, 0, (MAX_FRAME_LENGTH / 2 + 1)*sizeof(float));
                memset(gOutputAccum, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
                memset(gAnaFreq, 0, MAX_FRAME_LENGTH * sizeof(float));
                memset(gAnaMagn, 0, MAX_FRAME_LENGTH * sizeof(float));
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
                channelSamples.resize(frames);

                for (uint16_t channel = 0; channel < channels; ++channel)
                {
                    for (uint32_t sample = 0; sample < frames; ++sample)
                        channelSamples[sample] = samples[sample * channels + channel];

                    smbPitchShift(pitch, frames, 256, 16, sampleRate, channelSamples.data(), channelSamples.data());

                    for (uint32_t sample = 0; sample < frames; ++sample)
                        samples[sample * channels + channel] = channelSamples[sample];
                }
            }

            void setPitch(float newPitch)
            {
                pitch = newPitch;
            }

        private:
            // TODO: handle each channel separately
            void smbPitchShift(float pitchShift, unsigned long numSampsToProcess, unsigned long fftFrameSize, unsigned long osamp, float sampleRate, float* indata, float* outdata)
            {
                double magn, phase, tmp, window, real, imag;

                // set up some handy variables
                unsigned long fftFrameSize2 = fftFrameSize / 2;
                unsigned long stepSize = fftFrameSize / osamp;
                double freqPerBin = sampleRate / (double)fftFrameSize;
                double expct = 2. * PI * (double)stepSize / (double)fftFrameSize;
                unsigned long inFifoLatency = fftFrameSize - stepSize;
                if (gRover == 0) gRover = inFifoLatency;

                // main processing loop
                for (unsigned long i = 0; i < numSampsToProcess; i++)
                {
                    // As long as we have not yet collected enough data just read in
                    gInFIFO[gRover] = indata[i];
                    outdata[i] = gOutFIFO[gRover - inFifoLatency];
                    gRover++;

                    // now we have enough data for processing
                    if (gRover >= fftFrameSize)
                    {
                        gRover = inFifoLatency;

                        /* do windowing and re,im interleave */
                        for (unsigned long k = 0; k < fftFrameSize; k++)
                        {
                            window = -.5 * cos(2. * PI * (double)k / (double)fftFrameSize) + .5;
                            gFFTworksp[2 * k] = gInFIFO[k] * window;
                            gFFTworksp[2 * k + 1] = 0.;
                        }


                        /* ***************** ANALYSIS ******************* */
                        // do transform
                        smbFft(gFFTworksp, fftFrameSize, -1);

                        // this is the analysis step
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            /* de-interlace FFT buffer */
                            real = gFFTworksp[2 * k];
                            imag = gFFTworksp[2 * k + 1];

                            // compute magnitude and phase
                            magn = 2. * sqrt(real * real + imag * imag);

                            // avoid domain error
                            double signx = (imag > 0.) ? 1. : -1.;
                            if (imag == 0.) phase = 0.;
                            else if (real == 0.) phase = signx * PI / 2.;
                            else phase = atan2(imag, real);

                            // compute phase difference
                            tmp = phase - gLastPhase[k];
                            gLastPhase[k] = phase;

                            // subtract expected phase difference
                            tmp -= (double)k * expct;

                            /* map delta phase into +/- Pi interval */
                            unsigned long qpd = tmp / PI;
                            if (qpd >= 0) qpd += qpd&1;
                            else qpd -= qpd&1;
                            tmp -= PI * (double)qpd;

                            /* get deviation from bin frequency from the +/- Pi interval */
                            tmp = osamp * tmp / (2. * PI);

                            /* compute the k-th partials' true frequency */
                            tmp = (double)k * freqPerBin + tmp * freqPerBin;

                            // store magnitude and true frequency in analysis arrays
                            gAnaMagn[k] = magn;
                            gAnaFreq[k] = tmp;

                        }

                        /* ***************** PROCESSING ******************* */
                        // this does the actual pitch shifting
                        memset(gSynMagn, 0, fftFrameSize * sizeof(float));
                        memset(gSynFreq, 0, fftFrameSize * sizeof(float));
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            unsigned long index = k * pitchShift;
                            if (index <= fftFrameSize2)
                            {
                                gSynMagn[index] += gAnaMagn[k];
                                gSynFreq[index] = gAnaFreq[k] * pitchShift;
                            }
                        }

                        /* ***************** SYNTHESIS ******************* */
                        // this is the synthesis step
                        for (unsigned long k = 0; k <= fftFrameSize2; k++)
                        {
                            // get magnitude and true frequency from synthesis arrays
                            magn = gSynMagn[k];
                            tmp = gSynFreq[k];

                            // subtract bin mid frequency
                            tmp -= (double)k * freqPerBin;

                            // get bin deviation from freq deviation
                            tmp /= freqPerBin;

                            // take osamp into account
                            tmp = 2. * PI * tmp / osamp;

                            // add the overlap phase advance back in
                            tmp += (double)k * expct;

                            // accumulate delta phase to get bin phase
                            gSumPhase[k] += tmp;
                            phase = gSumPhase[k];

                            /* get real and imag part and re-interleave */
                            gFFTworksp[2 * k] = magn * cos(phase);
                            gFFTworksp[2 * k + 1] = magn * sin(phase);
                        }

                        // zero negative frequencies
                        for (unsigned long k = fftFrameSize + 2; k < 2 * fftFrameSize; k++) gFFTworksp[k] = 0.;

                        // do inverse transform
                        smbFft(gFFTworksp, fftFrameSize, 1);

                        // do windowing and add to output accumulator
                        for (unsigned long k = 0; k < fftFrameSize; k++)
                        {
                            window = -.5 * cos(2. * PI * (double)k / (double)fftFrameSize) + .5;
                            gOutputAccum[k] += 2. * window * gFFTworksp[2 * k] / (fftFrameSize2 * osamp);
                        }
                        for (unsigned long k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

                        // shift accumulator
                        memmove(gOutputAccum, gOutputAccum + stepSize, fftFrameSize * sizeof(float));

                        // move input FIFO
                        for (unsigned long k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k + stepSize];
                    }
                }
            }

            float pitch = 1.0f;
            std::vector<float> channelSamples;

            float gInFIFO[MAX_FRAME_LENGTH];
            float gOutFIFO[MAX_FRAME_LENGTH];
            float gFFTworksp[2 * MAX_FRAME_LENGTH];
            float gLastPhase[MAX_FRAME_LENGTH / 2 + 1];
            float gSumPhase[MAX_FRAME_LENGTH / 2 + 1];
            float gOutputAccum[2 * MAX_FRAME_LENGTH];
            float gAnaFreq[MAX_FRAME_LENGTH];
            float gAnaMagn[MAX_FRAME_LENGTH];
            float gSynFreq[MAX_FRAME_LENGTH];
            float gSynMagn[MAX_FRAME_LENGTH];
            unsigned long gRover = 0;
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
