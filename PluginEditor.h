/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

//R1.00 Add SLIDER listener. BUTTON or TIMER listeners also go here if needed. Must add ValueChanged overrides!
class MakoBiteAudioProcessorEditor  : public juce::AudioProcessorEditor , public juce::Slider::Listener , public juce::Button::Listener //, public juce::Timer
{
public:
    MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor&);
    ~MakoBiteAudioProcessorEditor() override;

    //R1.00 OUR override functions.
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* butt) override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MakoBiteAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakoBiteAudioProcessorEditor)

    void MAKO_IR_Update(int idx);
    void FILE_Mako_IR_Load_Wave(juce::String tFile, int idx);
    void FILE_Mako_IR_PreLoad_Wave(juce::String tFile, int idx);
    std::unique_ptr<juce::FileChooser> dlgFileOpen;

    void FILE_MakoIRPreset_Save();
    void FILE_MakoIRPreset_Load();
    void File_UpdateControls();

    int FILE_ClipInt(int val, int min, int max);
    float FILE_ClipFloat(float val, float min, float max);

    void GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix);
    void GUI_Init_Small_Slider(juce::Slider* slider);
    void GUI_Init_Button(juce::Button* button, juce::String text, bool toggle);
    void GUI_SetMono();
    void Gui_Draw_Background(juce::Graphics& g);

    juce::uint32 colorButton = 0xFFE0E0E8;
    juce::Image imgBack;

    //R1.00 Define our UI Juce Slider controls.
    juce::Slider jsP1_GainL; 
    juce::Slider jsP1_GainR;
    juce::Slider jsP1_SizeL;
    juce::Slider jsP1_SizeR;
    juce::Slider jsP1_MixL;
    juce::Slider jsP1_MixR;
    juce::Slider jsP1_ThumpL;
    juce::Slider jsP1_ThumpR;
    juce::Slider jsP1_ThumpFL;
    juce::Slider jsP1_ThumpFR;
    juce::Slider jsP1_Mono;
    juce::Slider jsP1_LoCutL;
    juce::Slider jsP1_LoCutR;
    juce::Slider jsP1_Trim;
    juce::Slider jsP1_Gate;
    
    juce::TextButton butLoadIRL1;
    juce::TextButton butLoadIRR1;
    juce::TextButton butLoadIRL2;
    juce::TextButton butLoadIRR2;

    juce::TextButton butX_IRL1;
    juce::TextButton butX_IRL2;
    juce::TextButton butX_IRR1;
    juce::TextButton butX_IRR2;

    juce::TextButton butLoadPreset;
    juce::TextButton butSavePreset;
    
    juce::Label labHelp;
    
    //R1.00 Create some easy to read flags for settings changes.
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
    

public:
    
    //R1.00 Define our SLIDER attachment variables.
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_GainL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_GainR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_SizeL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_SizeR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_MixL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_MixR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_ThumpL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_ThumpR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_ThumpFL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_ThumpFR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Mono;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_LoCutL;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_LoCutR;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Trim;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Gate;
    
};
