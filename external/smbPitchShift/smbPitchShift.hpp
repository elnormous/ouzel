/****************************************************************************
*
* NAME: smbPitchShift.cpp
* VERSION: 1.2
* HOME URL: http://blogs.zynaptiq.com/bernsee
* KNOWN BUGS: none
*
* SYNOPSIS: Routine for doing pitch shifting while maintaining
* duration using the Short Time Fourier Transform.
*
* DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
* (one octave down) and 2. (one octave up). A value of exactly 1 does not change
* the pitch. numSampsToProcess tells the routine how many samples in indata[0...
* numSampsToProcess-1] should be pitch shifted and moved to outdata[0 ...
* numSampsToProcess-1]. The two buffers can be identical (ie. it can process the
* data in-place). fftFrameSize defines the FFT frame size used for the
* processing. Typical values are 1024, 2048 and 4096. It may be any value <=
* MAX_FRAME_LENGTH but it MUST be a power of 2. oversamp is the STFT
* oversampling factor which also determines the overlap between adjacent STFT
* frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
* recommended for best quality. sampleRate takes the sample rate for the signal 
* in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in 
* indata[] should be in the range [-1.0, 1.0), which is also the output range 
* for the data, make sure you scale the data accordingly (for 16bit signed integers
* you would have to divide (and multiply) by 32768). 
*
* COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
*
* 						The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies. 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/

#ifndef SMBPITCHSHIFT_HPP
#define SMBPITCHSHIFT_HPP

#include <algorithm>
#include <cmath>
#include <cstring>

namespace smb
{
    static const float PI = 3.14159265358979323846F;
    static const unsigned long MAX_FRAME_LENGTH = 8192;

    // Use own implementation because std::complex has a poor performance
    template<class T>
    struct Complex
    {
        inline Complex<T> operator+(const Complex& other)
        {
            return Complex{real + other.real, imag + other.imag};
        }

        inline Complex<T>& operator+=(const Complex& other)
        {
            real += other.real;
            imag += other.imag;
            return *this;
        }

        inline Complex<T> operator-(const Complex& other)
        {
            return Complex{real - other.real, imag - other.imag};
        }

        inline Complex<T>& operator-=(const Complex& other)
        {
            real -= other.real;
            imag -= other.imag;
            return *this;
        }

        inline Complex<T> operator*(const Complex& other)
        {
            return Complex{real * other.real - imag * other.imag, real * other.imag + imag * other.real};
        }

        inline Complex<T>& operator*=(const Complex& other)
        {
            float tempReal = real;
            real = tempReal * other.real - imag * other.imag;
            imag = tempReal * other.imag + imag * other.real;
            return *this;
        }

        T real;
        T imag;
    };

    static void fft(Complex<float>* fftBuffer, unsigned long fftFrameSize, long sign)
    {
        for (unsigned long i = 1; i < fftFrameSize - 1; i++)
        {
            unsigned long j = 0;

            for (unsigned long bitm = 1; bitm < fftFrameSize; bitm <<= 1)
            {
                if (i & bitm) j++;
                j <<= 1;
            }
            j >>= 1;

            if (i < j)
                std::swap(fftBuffer[i], fftBuffer[j]);
        }

        unsigned long step = 2;
        for (unsigned long i = 1; i < fftFrameSize; i <<= 1, step <<= 1)
        {
            unsigned long step2 = step >> 1;
            float arg = PI / step2;

            Complex<float> w{std::cos(arg), std::sin(arg) * sign};
            Complex<float> u{1.0, 0.0};
            for (unsigned long j = 0; j < step2; j++)
            {
                for (unsigned long k = j; k < fftFrameSize; k += step)
                {
                    Complex<float> temp = fftBuffer[k + step2] * u;
                    fftBuffer[k + step2] = fftBuffer[k] - temp;
                    fftBuffer[k] += temp;
                }

                u *= w;
            }
        }
    }

    class PitchShift
    {
    public:
        PitchShift()
        {
            std::fill(std::begin(inFIFO), std::end(inFIFO), 0.0F);
            std::fill(std::begin(outFIFO), std::end(outFIFO), 0.0F);
            std::fill(std::begin(fftWorksp), std::end(fftWorksp), Complex<float>{0.0F, 0.0F});
            std::fill(std::begin(lastPhase), std::end(lastPhase), 0.0F);
            std::fill(std::begin(sumPhase), std::end(sumPhase), 0.0F);
            std::fill(std::begin(outputAccum), std::end(outputAccum), 0.0F);
            std::fill(std::begin(anaFreq), std::end(anaFreq), 0.0F);
            std::fill(std::begin(anaMagn), std::end(anaMagn), 0.0F);
        }

        /*
            Routine process(). See top of file for explanation
            Purpose: doing pitch shifting while maintaining duration using the Short
            Time Fourier Transform.
            Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
        */
        void process(float pitchShift, unsigned long numSampsToProcess, unsigned long fftFrameSize,
                     unsigned long oversamp, float sampleRate, float* indata, float* outdata)
        {
            // set up some handy variables
            unsigned long fftFrameSize2 = fftFrameSize / 2;
            unsigned long stepSize = fftFrameSize / oversamp;
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
                        float window = -0.5F * std::cos(2.0F * PI * static_cast<float>(k) / static_cast<float>(fftFrameSize)) + 0.5F;
                        fftWorksp[k].real = inFIFO[k] * window;
                        fftWorksp[k].imag = 0.0F;
                    }

                    // ***************** ANALYSIS *******************
                    // do transform
                    fft(fftWorksp, fftFrameSize, -1);

                    // this is the analysis step
                    for (unsigned long k = 0; k <= fftFrameSize2; k++)
                    {
                        // de-interlace FFT buffer
                        float real = fftWorksp[k].real;
                        float imag = fftWorksp[k].imag;

                        // compute magnitude and phase
                        float magn = 2.0F * sqrtf(real * real + imag * imag);

                        float phase;
                        float signx = (imag > 0.0F) ? 1.0F : -1.0F;
                        if (imag == 0.0F) phase = 0.0F;
                        else if (real == 0.0F) phase = signx * PI / 2.0F;
                        else phase = std::atan2(imag, real);

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
                        tmp = oversamp * tmp / (2.0F * PI);

                        // compute the k-th partials' true frequency
                        tmp = static_cast<float>(k) * freqPerBin + tmp * freqPerBin;

                        // store magnitude and true frequency in analysis arrays
                        anaMagn[k] = magn;
                        anaFreq[k] = tmp;
                    }

                    // ***************** PROCESSING *******************
                    // this does the actual pitch shifting
                    std::fill(std::begin(synMagn), std::begin(synMagn) + fftFrameSize, 0.0F);
                    std::fill(std::begin(synFreq), std::begin(synFreq) + fftFrameSize, 0.0F);
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

                        // take oversampling factor into account
                        tmp = 2.0F * PI * tmp / oversamp;

                        // add the overlap phase advance back in
                        tmp += static_cast<float>(k) * expct;

                        // accumulate delta phase to get bin phase
                        sumPhase[k] += tmp;
                        float phase = sumPhase[k];

                        // get real and imag part and re-interleave
                        fftWorksp[k].real = magn * std::cos(phase);
                        fftWorksp[k].imag = magn * std::sin(phase);
                    }

                    // zero negative frequencies
                    for (unsigned long k = fftFrameSize + 1; k < fftFrameSize; k++) fftWorksp[k] = {0.0F, 0.0F};

                    // do inverse transform
                    fft(fftWorksp, fftFrameSize, 1);

                    // do windowing and add to output accumulator
                    for (unsigned long k = 0; k < fftFrameSize; k++)
                    {
                        float window = -0.5F * cos(2.0F * PI * static_cast<float>(k) / static_cast<float>(fftFrameSize)) + 0.5F;
                        outputAccum[k] += 2.0F * window * fftWorksp[k].real / (fftFrameSize2 * oversamp);
                    }
                    unsigned long k;
                    for (k = 0 ; k < stepSize; k++) outFIFO[k] = outputAccum[k];
                    // shift accumulator
                    unsigned long j;
                    for (j = 0; k < fftFrameSize; k++, j++) outputAccum[j] = outputAccum[k];
                    for (; j < fftFrameSize; j++) outputAccum[j] = 0.0;

                    // move input FIFO
                    for (k = 0; k < inFifoLatency; k++) inFIFO[k] = inFIFO[k + stepSize];
                }
            }
        }

    private:
        float inFIFO[MAX_FRAME_LENGTH];
        float outFIFO[MAX_FRAME_LENGTH];
        Complex<float> fftWorksp[MAX_FRAME_LENGTH];
        float lastPhase[MAX_FRAME_LENGTH / 2 + 1];
        float sumPhase[MAX_FRAME_LENGTH / 2 + 1];
        float outputAccum[2 * MAX_FRAME_LENGTH];
        float anaFreq[MAX_FRAME_LENGTH];
        float anaMagn[MAX_FRAME_LENGTH];
        float synFreq[MAX_FRAME_LENGTH];
        float synMagn[MAX_FRAME_LENGTH];
        unsigned long rover = 0;
    };
}

#endif
