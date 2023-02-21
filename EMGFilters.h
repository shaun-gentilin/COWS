

#ifndef _EMGFILTERS_H
#define _EMGFILTERS_H

enum NOTCH_FREQUENCY { NOTCH_FREQ_50HZ = 50, NOTCH_FREQ_60HZ = 60 };

enum SAMPLE_FREQUENCY { SAMPLE_FREQ_500HZ = 500, SAMPLE_FREQ_1000HZ = 1000 };

// \brief EMGFilter provides an anti-hum notch filter to filter out 50HZ or
//        60HZ power line noise, a lowpass filter to filter out signals above
//        150HZ, and a highpass filter to filter out noise below 20HZ;
//        You can turn on or off these filters by the init function.
// \remark Input frequencies of 500HZ and 1000HZ are supported only!
class EMGFilters {
  public:
    // \brief Initializes the filter.
    // \param sampleFreq only supports SAMPLE_FREQ_500HZ &
    // SAMPLE_FREQ_1000HZ.
    // \param notchFreq only supports NOTCH_FREQ_50HZ & NOTCH_FREQ_60HZ.
    // \param enableNotchFilter enables the notch filter. True by default.
    // \param enableLowpassFilter enables the lowpass filter. True by
    // default.
    // \param enableHighpassFilter enables the highpass filter. True by
    // default
    // \remark The filter won't work and just bypass input if sampleFreq or
    //         notchFreq isn't specified properly.
    void init(SAMPLE_FREQUENCY sampleFreq,
              NOTCH_FREQUENCY  notchFreq,
              bool             enableNotchFilter    = true,
              bool             enableLowpassFilter  = true,
              bool             enableHighpassFilter = true);

    // \brief Called in the loop, input read analog value to get filtered
    // value
    int update(int inputValue);

  private:
    SAMPLE_FREQUENCY m_sampleFreq;
    NOTCH_FREQUENCY  m_notchFreq;
    bool             m_bypassEnabled;
    bool             m_notchFilterEnabled;
    bool             m_lowpassFilterEnabled;
    bool             m_highpassFilterEnabled;
};

#endif
