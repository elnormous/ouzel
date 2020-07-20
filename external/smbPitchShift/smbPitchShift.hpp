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
* processing. Typical values are 1024, 2048 and 4096. It MUST be a power of 2.
* oversamp is the STFT oversampling factor which also determines the overlap
* between adjacent STFT frames. It should at least be 4 for moderate scaling
* ratios. A value of 32 is recommended for best quality. sampleRate takes the
* sample rate for the signal in unit Hz, ie. 44100 for 44.1 kHz audio. The data
* passed to the routine in indata[] should be in the range [-1.0, 1.0), which is
* also the output range for the data, make sure you scale the data accordingly
* (for 16bit signed integers you would have to divide (and multiply) by 32768).
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
#include <cstdint>

namespace smb
{
    namespace
    {
        constexpr float pi = 3.14159265358979323846F;

        // Use own implementation because std::complex has a poor performance
        template <class T>
        struct Complex final
        {
            constexpr Complex<T> operator+(const Complex& other) const noexcept
            {
                return Complex{real + other.real, imag + other.imag};
            }

            constexpr Complex<T>& operator+=(const Complex& other) noexcept
            {
                real += other.real;
                imag += other.imag;
                return *this;
            }

            constexpr Complex<T> operator-(const Complex& other) const noexcept
            {
                return Complex{real - other.real, imag - other.imag};
            }

            constexpr Complex<T> operator-() const noexcept
            {
                return Complex{-real, -imag};
            }

            constexpr Complex<T>& operator-=(const Complex& other) noexcept
            {
                real -= other.real;
                imag -= other.imag;
                return *this;
            }

            constexpr Complex<T> operator*(const Complex& other) const noexcept
            {
                return Complex{real * other.real - imag * other.imag, real * other.imag + imag * other.real};
            }

            constexpr Complex<T>& operator*=(const Complex& other) noexcept
            {
                float tempReal = real;
                real = real * other.real - imag * other.imag;
                imag = tempReal * other.imag + imag * other.real;
                return *this;
            }

            T magnitude() const noexcept
            {
                return sqrt(real * real + imag * imag);
            }

            T real;
            T imag;
        };

        template <int32_t sign, std::uint32_t fftFrameSize>
        void fft(Complex<float> fftBuffer[fftFrameSize]) noexcept
        {
            // Bit-reversal permutation applied to a sequence of fftFrameSize items
            for (std::uint32_t i = 1; i < fftFrameSize - 1; ++i)
            {
                std::uint32_t j = 0;

                for (std::uint32_t bitm = 1; bitm < fftFrameSize; bitm <<= 1)
                {
                    if (i & bitm) ++j;
                    j <<= 1;
                }
                j >>= 1;

                if (i < j)
                    std::swap(fftBuffer[i], fftBuffer[j]);
            }

            // Iterative form of Danielson-Lanczos lemma
            std::uint32_t step = 2;
            for (std::uint32_t i = 1; i < fftFrameSize; i <<= 1, step <<= 1)
            {
                const std::uint32_t step2 = step >> 1;
                const float arg = pi / step2;

                const Complex<float> w{std::cos(arg), std::sin(arg) * sign};
                Complex<float> u{1.0F, 0.0F};
                for (std::uint32_t j = 0; j < step2; ++j)
                {
                    for (std::uint32_t k = j; k < fftFrameSize; k += step)
                    {
                        const Complex<float> temp = fftBuffer[k + step2] * u;
                        fftBuffer[k + step2] = fftBuffer[k] - temp;
                        fftBuffer[k] += temp;
                    }

                    u *= w;
                }
            }
        }
    }

    template <std::uint32_t fftFrameSize, std::uint32_t oversamp>
    class PitchShift final
    {
    public:
        PitchShift() noexcept
        {
            // Hann window
            for (std::uint32_t k = 0; k < fftFrameSize; ++k)
                window[k] = 0.5F * (1.0F + std::cos(2.0F * pi * static_cast<float>(k) / static_cast<float>(fftFrameSize)));
        }

        /*
            Routine process(). See top of file for explanation
            Purpose: doing pitch shifting while maintaining duration using the Short
            Time Fourier Transform.
            Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
        */
        void process(const float pitchShift, const std::uint32_t numSampsToProcess,
                     const std::uint32_t sampleRate, const float* indata, float* outdata) noexcept
        {
            // set up some handy variables
            constexpr auto fftFrameSizeHalf = fftFrameSize / 2;
            constexpr auto stepSize = fftFrameSize / oversamp;
            constexpr auto inFifoLatency = fftFrameSize - stepSize;
            const auto freqPerBin = static_cast<float>(sampleRate) / static_cast<float>(fftFrameSize);
            constexpr float expected = 2.0F * pi * static_cast<float>(stepSize) / static_cast<float>(fftFrameSize);
            if (rover == 0) rover = inFifoLatency;

            // main processing loop
            for (std::uint32_t i = 0; i < numSampsToProcess; ++i)
            {
                // As long as we have not yet collected enough data just read in
                inFifo[rover] = indata[i];
                outdata[i] = outFifo[rover - inFifoLatency];
                ++rover;

                // now we have enough data for processing
                if (rover >= fftFrameSize)
                {
                    rover = inFifoLatency;

                    // do windowing
                    for (std::uint32_t k = 0; k < fftFrameSize; ++k)
                        fftWorksp[k] = {inFifo[k] * window[k], 0.0F};

                    // ***************** ANALYSIS *******************
                    // do transform
                    fft<-1, fftFrameSize>(fftWorksp);

                    // this is the analysis step
                    for (std::uint32_t k = 0; k < fftFrameSizeHalf + 1; ++k)
                    {
                        const Complex<float>& current = fftWorksp[k];

                        // compute magnitude and phase
                        const float magn = 2.0F * current.magnitude();
                        const float signx = (current.imag > 0.0F) ? 1.0F : -1.0F;
                        const float phase = (current.imag == 0.0F) ? 0.0F :
                            (current.real == 0.0F) ? signx * pi / 2.0F :
                            std::atan2(current.imag, current.real);

                        // compute phase difference
                        float tmp = phase - lastPhase[k];
                        lastPhase[k] = phase;

                        // subtract expected phase difference
                        tmp -= static_cast<float>(k) * expected;

                        // map delta phase into +/- Pi interval
                        int32_t qpd = static_cast<int32_t>(tmp / pi);
                        if (qpd >= 0) qpd += qpd & 1;
                        else qpd -= qpd & 1;
                        tmp -= pi * static_cast<float>(qpd);

                        // get deviation from bin frequency from the +/- Pi interval
                        tmp = oversamp * tmp / (2.0F * pi);

                        // compute the k-th partials' true frequency
                        tmp = static_cast<float>(k) * freqPerBin + tmp * freqPerBin;

                        // store magnitude and true frequency in analysis arrays
                        anaMagn[k] = magn;
                        anaFreq[k] = tmp;
                    }

                    // ***************** PROCESSING *******************
                    // this does the actual pitch shifting
                    std::fill(std::begin(synMagn), std::end(synMagn), 0.0F);
                    for (std::uint32_t k = 0; k < fftFrameSizeHalf + 1; ++k)
                    {
                        const std::uint32_t index = static_cast<std::uint32_t>(k * pitchShift);
                        if (index > fftFrameSizeHalf) break;
                        synMagn[index] += anaMagn[k];
                        synFreq[index] = anaFreq[k] * pitchShift;
                    }

                    // ***************** SYNTHESIS *******************
                    // this is the synthesis step
                    for (std::uint32_t k = 0; k < fftFrameSizeHalf + 1; ++k)
                    {
                        // get magnitude and true frequency from synthesis arrays
                        const float magn = synMagn[k];
                        float tmp = synFreq[k];

                        // subtract bin mid frequency
                        tmp -= static_cast<float>(k) * freqPerBin;

                        // get bin deviation from freq deviation
                        tmp /= freqPerBin;

                        // take oversampling factor into account
                        tmp = 2.0F * pi * tmp / oversamp;

                        // add the overlap phase advance back in
                        tmp += static_cast<float>(k) * expected;

                        // accumulate delta phase to get bin phase
                        sumPhase[k] += tmp;
                        const float phase = sumPhase[k];

                        // get real and imag part and re-interleave
                        fftWorksp[k] = {magn * std::cos(phase), magn * std::sin(phase)};
                    }

                    // zero negative frequencies
                    //for (std::uint32_t k = fftFrameSizeHalf + 1; k < fftFrameSize; ++k)
                    //    fftWorksp[k] = {0.0F, 0.0F};

                    // do inverse transform
                    fft<1, fftFrameSize>(fftWorksp);

                    // do windowing and add to output accumulator
                    for (std::uint32_t k = 0; k < fftFrameSize; ++k)
                        outputAccum[k] += 2.0F * window[k] * fftWorksp[k].real / (fftFrameSizeHalf * oversamp);

                    std::uint32_t k;
                    for (k = 0; k < stepSize; ++k)
                        outFifo[k] = outputAccum[k];
                    // shift accumulator
                    std::uint32_t j;
                    for (j = 0; k < fftFrameSize; ++k, ++j)
                        outputAccum[j] = outputAccum[k];
                    for (; j < fftFrameSize; ++j)
                        outputAccum[j] = 0.0;

                    // move input FIFO
                    for (k = 0; k < inFifoLatency; ++k)
                        inFifo[k] = inFifo[k + stepSize];
                }
            }
        }

    private:
        float window[fftFrameSize]; // the windowing function
        float inFifo[fftFrameSize]{0.0F};
        float outFifo[fftFrameSize]{0.0F};
        Complex<float> fftWorksp[fftFrameSize]{{0.0F, 0.0F}};
        float lastPhase[fftFrameSize / 2 + 1]{0.0F};
        float sumPhase[fftFrameSize / 2 + 1]{0.0F};
        float outputAccum[2 * fftFrameSize]{0.0F};
        float anaFreq[fftFrameSize / 2 + 1]{0.0F};
        float anaMagn[fftFrameSize / 2 + 1]{0.0F};
        float synFreq[fftFrameSize / 2 + 1]{0.0F};
        float synMagn[fftFrameSize / 2 + 1]{0.0F};
        std::uint32_t rover = 0;
    };
}

#endif
