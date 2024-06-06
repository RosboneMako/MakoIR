/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class MakoBiteAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    MakoBiteAudioProcessor();
    ~MakoBiteAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //R1.00 Add Parameters.
    juce::AudioProcessorValueTreeState parameters;

    //R1.00 Our public variables.
    bool SettingChanged = false;
    int  SettingType = 0;           //R1.00 0-none,1-L1,2R1,3-L2,4-R2,5-Filters,6-9 delete IR.
    bool Pedal_Mono = true;

    juce::String IR_Path[4];
    juce::String IR_Name[4];

    float Pedal_Gain[2] = { 1.0f, 1.0f };           //R1.00 Gain values to adjust final output volume.
    float Pedal_Size[2] = { .5f, .5f };             //R1.00 The apparent speaker size adjustment.
    float Pedal_Mix[2] = { .0f, .0f };              //R1.00 Mix ratio between IR 1 and IR 2.
    float Pedal_Thump[2] = { .0f, .0f };            //R1.00 How much thump to add. 
    int   Pedal_ThumpF[2] = { 150, 150 };           //R1.00 Thump Low Pass frequency. Which lows to add.
    int   Pedal_LoCut[2] = { 20, 20 };              //R1.00 Thump pushes some undesired low freqs. This reduces them.
    float Pedal_Trim = 1.0f;                        //R1.00 Reduce the IR calculations at the cost of fidelity.
    float Pedal_Gate = 0.0f;                        //R1.00 Dont do IR calcs if no signal is present.

    float IR_Buffer[2][2050] = {  };                //R1.00 We need to store incoming samples to calc the IR.
    int   IR_BufferIndex[2] = {  };                 //R1.00 Current Index being filled in the data buffer.

    float IR_Final[2][2050] = {};                   //R1.00 Actual Base IRS used to process Audio (L, R).
    bool  IR_FinalValid[2] = { false, false };
    int   IR_FinalLen[2] = { 1024, 1024 };

    float IR[4][2050] = {};                         //R1.00 Four loaded IRs to be mixed into IR_Final[2].
    int   IR_Len[4] = { 1024, 1024, 1024, 1024 };
    bool  IR_Loaded[4] = { false, false, false, false };
    
    bool IR_SavePaths = false;
    

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MakoBiteAudioProcessor)

    int makoGetParmValue_int(juce::String Pstring);
    float makoGetParmValue_float(juce::String Pstring);
    void PATHS_Load();
    void PATHS_Save();
    void IR_LoadFromPaths(int idx);

    float Signal_AVG[2] = {};

    //R1.00 Some Constants 
    const float pi = 3.14159265f;
    const float pi2 = 6.2831853f;
    const float sqrt2 = 1.4142135f;
    float SampleRate = 48000.0f;

    const int Adj_IRL1 = 1;
    const int Adj_IRR1 = 2;
    const int Adj_IRL2 = 3;
    const int Adj_IRR2 = 4;
    const int Adj_Filters = 5;
    const int Del_IRL1 = 6;
    const int Del_IRR1 = 7;
    const int Del_IRL2 = 8;
    const int Del_IRR2 = 9;
    const int Adj_All = 10;

    //R1.00 OUR FILTER VARIABLES
    struct tp_coeffs {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
    };

    struct tp_filter {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
        float xn0[2];
        float xn1[2];
        float xn2[2];
        float yn1[2];
        float yn2[2];
        float offset[2];
    };

    //R1.00 FILTER functions and variables.
    float Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn);
    void Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn);
    void Filter_LP_Coeffs(float fc, tp_filter* fn);
    void Filter_HP_Coeffs(float fc, tp_filter* fn);
    tp_filter makoF_Thump[2] = {};
    tp_filter makoF_LoCut[2] = {};

    //R1.00 Our functions that actually do some work.
    float IR_ProcessAudio(float tSample, int channel);
    float IR_Thump(float tSample, int channel);
    float IR_LoCut(float tSample, int channel);
    bool IR_SignalPresent(float tSample, int channel);
    void IR_Create_1024(int idx);
    void IR_Create_2048(int idx);
    void IR_Delete(int idx);
    int IR_GetValidIRIndex(int Idx);
    float Sample_Clip(float tSample);

    //R1.00 Handle any parameter changes.
    void Settings_Update();

};
