/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MakoBiteAudioProcessorEditor::MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //R1.00 Create SLIDER ATTACHMENTS so our parameter vars get adjusted automatically for Get/Set states.
    ParAtt_GainL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "gainl", jsP1_GainL);
    ParAtt_GainR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "gainr", jsP1_GainR);
    ParAtt_SizeL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "sizel", jsP1_SizeL);
    ParAtt_SizeR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "sizer", jsP1_SizeR);
    ParAtt_MixL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mixl", jsP1_MixL);
    ParAtt_MixR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mixr", jsP1_MixR);
    ParAtt_ThumpL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "thumpl", jsP1_ThumpL);
    ParAtt_ThumpR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "thumpr", jsP1_ThumpR);
    ParAtt_ThumpFL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "thumpfl", jsP1_ThumpFL);
    ParAtt_ThumpFR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "thumpfr", jsP1_ThumpFR);
    ParAtt_Mono = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mono", jsP1_Mono);
    ParAtt_LoCutL = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "locutl", jsP1_LoCutL);
    ParAtt_LoCutR = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "locutr", jsP1_LoCutR);
    ParAtt_Trim = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "trim", jsP1_Trim);
    ParAtt_Gate = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "cpugate", jsP1_Gate);
    

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
   
    //****************************************************************************************
    //R1.00 Add GUI CONTROLS
    //****************************************************************************************
    //jsP1_Low.setTextValueSuffix(" Hz");

    //R1.00 Background and Log graph image. 
    imgBack = juce::ImageCache::getFromMemory(BinaryData::makoir_back_01_png, BinaryData::makoir_back_01_pngSize);

    //R1.00 Setup our LARGE slider objects.
    GUI_Init_Large_Slider(&jsP1_GainL, audioProcessor.Pedal_Gain[0], 0.0f,2.0f, .01f, "");    
    GUI_Init_Large_Slider(&jsP1_GainR, audioProcessor.Pedal_Gain[1], 0.0f, 2.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_SizeL, audioProcessor.Pedal_Size[0], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_SizeR, audioProcessor.Pedal_Size[1], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_MixL, audioProcessor.Pedal_Mix[0], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_MixR, audioProcessor.Pedal_Mix[1], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_ThumpL, audioProcessor.Pedal_Thump[0], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_ThumpR, audioProcessor.Pedal_Thump[1], 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_ThumpFL, audioProcessor.Pedal_ThumpF[0], 80, 330, 10, " Hz");
    GUI_Init_Large_Slider(&jsP1_ThumpFR, audioProcessor.Pedal_ThumpF[1], 80, 330, 10, " Hz");
    GUI_Init_Large_Slider(&jsP1_Trim, audioProcessor.Pedal_Trim, 0.0f, 1.0f, .01f, "");
    GUI_Init_Large_Slider(&jsP1_Gate, audioProcessor.Pedal_Gate, 0.0f, 1.0f, .01f, "");

    //R1.00 MONO
    GUI_Init_Small_Slider(&jsP1_Mono);
    jsP1_Mono.setTextBoxStyle(juce::Slider::NoTextBox, true, 70, 18);
    jsP1_Mono.setSliderStyle(juce::Slider::LinearHorizontal);
    jsP1_Mono.setRange(0, 1, 1);
    jsP1_Mono.setValue(audioProcessor.Pedal_Mono);
    jsP1_Mono.addListener(this);
    addAndMakeVisible(jsP1_Mono);

    GUI_Init_Button(&butLoadIRL1, "Load IR L1", false);
    GUI_Init_Button(&butLoadIRL2, "Load IR L2", false);
    GUI_Init_Button(&butX_IRL1, "X", false);
    GUI_Init_Button(&butX_IRL2, "X", false);

    GUI_Init_Button(&butLoadIRR1, "Load IR R1", false);
    GUI_Init_Button(&butLoadIRR2, "Load IR R2", false);
    GUI_Init_Button(&butX_IRR1, "X", false);
    GUI_Init_Button(&butX_IRR2, "X", false);

    GUI_Init_Button(&butLoadPreset, "Load Preset", false);
    GUI_Init_Button(&butSavePreset, "Save Preset", false);

    //R1.00 LOW CUT
    GUI_Init_Small_Slider(&jsP1_LoCutL);
    jsP1_LoCutL.setSliderStyle(juce::Slider::LinearVertical);
    jsP1_LoCutL.setRange(20, 150, 10);
    jsP1_LoCutL.setValue(audioProcessor.Pedal_LoCut[0]);
    jsP1_LoCutL.addListener(this);
    addAndMakeVisible(jsP1_LoCutL);

    GUI_Init_Small_Slider(&jsP1_LoCutR);
    jsP1_LoCutR.setSliderStyle(juce::Slider::LinearVertical);
    jsP1_LoCutR.setRange(20, 150, 10);
    jsP1_LoCutR.setValue(audioProcessor.Pedal_LoCut[1]);
    jsP1_LoCutR.addListener(this);
    addAndMakeVisible(jsP1_LoCutR);

   
 
    //R1.00 HELP Text! Must be LAST defined object to be blank at the start.
    labHelp.setText("", juce::dontSendNotification);
    labHelp.setFont(12.0f);
    labHelp.setJustificationType(juce::Justification::centred);
    labHelp.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF101010));
    labHelp.setColour(juce::Label::textColourId, juce::Colour(0xFFA0A0A0));
    labHelp.setColour(juce::Label::outlineColourId, juce::Colour(0xFF404040));
    addAndMakeVisible(labHelp);

    //R1.00 Update the Look and Feel (Global colors). Most are not used. 
    getLookAndFeel().setColour(juce::DocumentWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::DocumentWindow::textColourId, juce::Colour(255, 255, 255));
    getLookAndFeel().setColour(juce::DialogWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(192, 0, 0));
    getLookAndFeel().setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF5085E8));
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ListBox::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::Label::backgroundColourId, juce::Colour(32, 32, 32));

    getLookAndFeel().setDefaultSansSerifTypefaceName("Trebuchet MS");    

    //R1.00 Set the window size.
    setSize(580, 370);
}

MakoBiteAudioProcessorEditor::~MakoBiteAudioProcessorEditor()
{
}


//==============================================================================
void MakoBiteAudioProcessorEditor::paint (juce::Graphics& g)
{   
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    //R1.00 Turn off unused controls if in MONO mode.
    GUI_SetMono();

    bool UseImage = true;
    if (UseImage)
    {
        //R1.00 Draw stored PNG background image.
        g.drawImageAt(imgBack, 0, 0);    
    }
    else
    {
        //R1.00 Draw a background manually.
        Gui_Draw_Background(g);
    }

    //*********************************************************************
    //R1.00 IR Filenames and Sizes
    //*********************************************************************
    g.setFont(13.0f);
    g.setColour(juce::Colours::orange);
    if (audioProcessor.IR_Loaded[0]) g.drawFittedText(std::to_string(audioProcessor.IR_FinalLen[0]), 110, 5, 40, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[2]) g.drawFittedText(std::to_string(audioProcessor.IR_Len[2]), 110, 25, 40, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[1]) g.drawFittedText(std::to_string(audioProcessor.IR_FinalLen[1]), 110, 165, 40, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[3]) g.drawFittedText(std::to_string(audioProcessor.IR_Len[3]), 110, 185, 40, 15, juce::Justification::centredLeft, 1);

    g.setColour(juce::Colours::white);
    if (audioProcessor.IR_Loaded[0]) g.drawFittedText(audioProcessor.IR_Name[0], 140, 5, 335, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[2]) g.drawFittedText(audioProcessor.IR_Name[2], 140, 25, 335, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[1]) g.drawFittedText(audioProcessor.IR_Name[1], 140, 165, 335, 15, juce::Justification::centredLeft, 1);
    if (audioProcessor.IR_Loaded[3]) g.drawFittedText(audioProcessor.IR_Name[3], 140, 185, 335, 15, juce::Justification::centredLeft, 1);
}

void MakoBiteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //R1.00 To render the background PNG, skip this function so objects do not draw on the background image.
    //return;

    //R1.00 Setup the sizes and positions of all of our control objects.
    butLoadIRL1.setBounds (  5,  5, 70,  18);
    butLoadIRL2.setBounds (  5, 25, 70,  18);
    butX_IRL1.setBounds   ( 80,  5, 20, 18);
    butX_IRL2.setBounds   ( 80, 25, 20, 18);
    jsP1_GainL.setBounds  (  0, 55, 80, 100);
    jsP1_SizeL.setBounds  (100, 55, 80, 100);
    jsP1_MixL.setBounds   (200, 55, 80, 100);
    jsP1_ThumpL.setBounds (300, 55, 80, 100);
    jsP1_ThumpFL.setBounds(400, 55, 80, 100);

    butLoadIRR1.setBounds(   5, 165, 70, 18);
    butLoadIRR2.setBounds(   5, 185, 70, 18);
    butX_IRR1.setBounds  (  80, 165, 20, 18);
    butX_IRR2.setBounds  (  80, 185, 20, 18);
    jsP1_GainR.setBounds  (  0, 215, 80, 100);
    jsP1_SizeR.setBounds  (100, 215, 80, 100);
    jsP1_MixR.setBounds   (200, 215, 80, 100);
    jsP1_ThumpR.setBounds (300, 215, 80, 100);
    jsP1_ThumpFR.setBounds(400, 215, 80, 100);

    labHelp.setBounds(0, 320, 580, 18);
    
    butLoadPreset.setBounds(5, 345, 80, 18);
    butSavePreset.setBounds(95, 345, 80, 18);
    jsP1_Mono.setBounds(185, 350, 60, 18);

    jsP1_LoCutL.setBounds(490, 10, 40, 90);
    jsP1_LoCutR.setBounds(535, 10, 40, 90);
    jsP1_Trim.setBounds(500, 115, 70, 90);
    jsP1_Gate.setBounds(500, 220, 70, 90);

}

void MakoBiteAudioProcessorEditor::GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix)
{
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setValue(Val);
    slider->addListener(this);
    slider->setTextValueSuffix(Suffix);
    addAndMakeVisible(slider);
    
    //R1.00 Setup the Slider colors.
    slider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFFFFFFF));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFF5085E8));
    slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF5085E8));
    slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(colorButton));
}

void MakoBiteAudioProcessorEditor::GUI_Init_Small_Slider(juce::Slider* slider)
{
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFFFFFFF));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF202020));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFF5085E8));
    slider->setColour(juce::Slider::trackColourId, juce::Colour(0xFF5085E8));
    slider->setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(colorButton));
}

void MakoBiteAudioProcessorEditor::GUI_Init_Button(juce::Button* button, juce::String text, bool toggle)
{
    button->setButtonText(text);
    button->setClickingTogglesState(toggle);
    button->setColour(juce::TextButton::textColourOffId, juce::Colour(32, 32, 32));
    button->setColour(juce::TextButton::buttonColourId, juce::Colour(colorButton));
    button->addListener(this);
    addAndMakeVisible(button);
}

void MakoBiteAudioProcessorEditor::GUI_SetMono()
{
    if (audioProcessor.Pedal_Mono)
    {
        butLoadIRR1.setEnabled(false);
        butLoadIRR2.setEnabled(false);
        butX_IRR1.setEnabled(false);
        butX_IRR2.setEnabled(false);
        
        jsP1_GainR.setEnabled(false);
        jsP1_GainR.setVisible(false);
        jsP1_SizeR.setEnabled(false);
        jsP1_SizeR.setVisible(false);
        jsP1_MixR.setEnabled(false);
        jsP1_MixR.setVisible(false);
        jsP1_ThumpR.setEnabled(false);
        jsP1_ThumpR.setVisible(false);
        jsP1_ThumpFR.setEnabled(false);
        jsP1_ThumpFR.setVisible(false);
        jsP1_LoCutR.setEnabled(false);
        
        //R1.00 Adjust Slider colors to show enabled/disabled.
        jsP1_LoCutR.setColour(juce::Slider::trackColourId, juce::Colour(0xFF303030));
        jsP1_LoCutR.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF303030));        
    }
    else
    {
        butLoadIRR1.setEnabled(true);
        butLoadIRR2.setEnabled(true);
        butX_IRR1.setEnabled(true);
        butX_IRR2.setEnabled(true);

        jsP1_GainR.setEnabled(true);
        jsP1_GainR.setVisible(true);
        jsP1_SizeR.setEnabled(true);
        jsP1_SizeR.setVisible(true);
        jsP1_MixR.setEnabled(true);
        jsP1_MixR.setVisible(true);
        jsP1_ThumpR.setEnabled(true);
        jsP1_ThumpR.setVisible(true);
        jsP1_ThumpFR.setEnabled(true);
        jsP1_ThumpFR.setVisible(true);
        jsP1_LoCutR.setEnabled(true);
        
        //R1.00 Adjust Slider colors to show enabled/disabled.
        jsP1_LoCutR.setColour(juce::Slider::trackColourId, juce::Colour(0xFF5085E8));
        jsP1_LoCutR.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF101010));        
    }

}


void MakoBiteAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    //R1.00 These are put in order they are likely to be adjusted by the User/DAW automation.

    //R1.00 Vol / Gain
    if (slider == &jsP1_GainL)
    {
        labHelp.setText("Adjust the final volume.", juce::dontSendNotification);
        audioProcessor.Pedal_Gain[0] = float(jsP1_GainL.getValue());
        return;
    }
    if (slider == &jsP1_GainR)
    {
        labHelp.setText("Adjust the final volume.", juce::dontSendNotification);
        audioProcessor.Pedal_Gain[1] = float(jsP1_GainR.getValue());
        return;
    }

    //R1.00 Speaker Size.
    if (slider == &jsP1_SizeL)
    {
        labHelp.setText("Adjust the apparent speaker size by IR resampling.", juce::dontSendNotification);
        audioProcessor.Pedal_Size[0] = float(jsP1_SizeL.getValue());
        audioProcessor.SettingType = Adj_IRL1;
        audioProcessor.SettingChanged = true;
        return;
    }
    if (slider == &jsP1_SizeR)
    {
        labHelp.setText("Adjust the apparent speaker size by IR resampling.", juce::dontSendNotification);
        audioProcessor.Pedal_Size[1] = float(jsP1_SizeR.getValue());
        audioProcessor.SettingType = Adj_IRR1;
        audioProcessor.SettingChanged = true;
        return;
    }

    //R1.00 Mix
    if (slider == &jsP1_MixL)
    {
        labHelp.setText("Adjust the mix between IRs. No time alignment is done.", juce::dontSendNotification);
        audioProcessor.Pedal_Mix[0] = float(jsP1_MixL.getValue());
        audioProcessor.SettingType = Adj_IRL1;
        audioProcessor.SettingChanged = true;
        return;
    }
    if (slider == &jsP1_MixR)
    {
        labHelp.setText("Adjust the mix between IRs. No time alignment is done.", juce::dontSendNotification);
        audioProcessor.Pedal_Mix[1] = float(jsP1_MixR.getValue());
        audioProcessor.SettingType = Adj_IRR1;
        audioProcessor.SettingChanged = true;
        return;
    }

    //R1.00 Thump
    if (slider == &jsP1_ThumpL)
    {
        labHelp.setText("Adjust the Bass Boost.", juce::dontSendNotification);
        audioProcessor.Pedal_Thump[0] = float(jsP1_ThumpL.getValue());
        return;
    }
    if (slider == &jsP1_ThumpR)
    {
        labHelp.setText("Adjust the Bass Boost.", juce::dontSendNotification);
        audioProcessor.Pedal_Thump[1] = float(jsP1_ThumpR.getValue());
        return;
    }

    //R1.00 Thump Frequency
    if (slider == &jsP1_ThumpFL)
    {
        labHelp.setText("Adjust the Boost Frequency.", juce::dontSendNotification);
        audioProcessor.Pedal_ThumpF[0] = int(jsP1_ThumpFL.getValue());
        audioProcessor.SettingType = Adj_Filters;
        audioProcessor.SettingChanged = true;
        return;
    }
    if (slider == &jsP1_ThumpFR)
    {
        labHelp.setText("Adjust the Boost Frequency.", juce::dontSendNotification);
        audioProcessor.Pedal_ThumpF[1] = int(jsP1_ThumpFR.getValue());
        audioProcessor.SettingType = Adj_Filters;
        audioProcessor.SettingChanged = true;
        return;
    }

    //R1.00 Thump Frequency
    if (slider == &jsP1_LoCutL)
    {
        labHelp.setText("Reduce boominess by cutting very low freqs.", juce::dontSendNotification);
        audioProcessor.Pedal_LoCut[0] = int(jsP1_LoCutL.getValue());
        audioProcessor.SettingType = Adj_Filters;
        audioProcessor.SettingChanged = true;
        return;
    }
    if (slider == &jsP1_LoCutR)
    {        
        labHelp.setText("Reduce boominess by cutting very low freqs.", juce::dontSendNotification);
        audioProcessor.Pedal_LoCut[1] = int(jsP1_LoCutR.getValue());
        audioProcessor.SettingType = Adj_Filters;
        audioProcessor.SettingChanged = true;
        return;
    }

    //R1.00 toggle MONO state for processor.
    if (slider == &jsP1_Mono)
    {
        labHelp.setText("IR calcs are very CPU heavy. Always use mono if possible.", juce::dontSendNotification);
        if (bool(jsP1_Mono.getValue()))
            audioProcessor.Pedal_Mono = true;
        else
            audioProcessor.Pedal_Mono = false;

        return;
    }

    //R1.00 TRIM
    if (slider == &jsP1_Trim)
    {
        audioProcessor.Pedal_Trim = float(jsP1_Trim.getValue());        
        labHelp.setText("Reduce IR size to save CPU cycles. Will lose low Freq accuracy. Do not use for final renders.", juce::dontSendNotification);
        return;
    }
    
    //R1.00 GREEN
    if (slider == &jsP1_Gate)
    {
        audioProcessor.Pedal_Gate = float(jsP1_Gate.getValue());
        labHelp.setText("CPU Noise Gate. Skips IR calcs when no signal is present. Do not use for final renders.", juce::dontSendNotification);
        return;
    }
    
    return;
}


void MakoBiteAudioProcessorEditor::buttonClicked(juce::Button* butt)
{    
    auto buttstate = butt->getState();

    //R1.00 Load a new IR.
    if (butt == &butLoadIRL1)  MAKO_IR_Update(0);
    if (butt == &butLoadIRR1)  MAKO_IR_Update(1);
    if (butt == &butLoadIRL2)  MAKO_IR_Update(2);
    if (butt == &butLoadIRR2)  MAKO_IR_Update(3);    

    //R1.00 Tell Processor to delete this IR.
    if (butt == &butX_IRL1) audioProcessor.SettingType = Del_IRL1;
    if (butt == &butX_IRR1) audioProcessor.SettingType = Del_IRR1;
    if (butt == &butX_IRL2) audioProcessor.SettingType = Del_IRL2;
    if (butt == &butX_IRR2) audioProcessor.SettingType = Del_IRR2;
    if ((5 < audioProcessor.SettingType) && (audioProcessor.SettingType < 10))
    {
        int idx = audioProcessor.SettingType - 6;
        audioProcessor.IR_Name[idx] = "";
        audioProcessor.IR_Path[idx] = "";
        audioProcessor.SettingChanged = true;  //R1.00 We need to update settings in processor.
        repaint();
    }
    
    //R1.00 Load/Save our stand alone custom presets. These are not part of any DAW/VST operation. 
    if (butt == &butLoadPreset) FILE_MakoIRPreset_Load();          
    if (butt == &butSavePreset) FILE_MakoIRPreset_Save();    

}


void MakoBiteAudioProcessorEditor::MAKO_IR_Update(int idx)
{
    //dlgFileOpen = std::make_unique<juce::FileChooser>("Load Mako Distortion 2 IR file", juce::File{}, "*.txt");
    dlgFileOpen = std::make_unique<juce::FileChooser>("Load an IR file (1024 or 2048 @ 48kHz)", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    //R1.00 Load IR file.
    dlgFileOpen->launchAsync(chooserFlags, [this,idx](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file != juce::File{})
        {
            FILE_Mako_IR_Load_Wave(file.getFullPathName(), idx);
        }
    });
}


void MakoBiteAudioProcessorEditor::FILE_Mako_IR_Load_Wave(juce::String tFile, int idx)
{
    float max = 0.0f;
    int maxIdx = 0;
    int LIS = 0;      //R1.00 Actual # of samples in file.
    int LISI = 1024;  //R1.00 Ideal number of samples. 

    labHelp.setText("IR Loading: Opening WAVE file.", juce::dontSendNotification);
    repaint();

    //R1.00 Track the filename of this IR.
    audioProcessor.IR_Path[idx] = tFile;
    audioProcessor.IR_Name[idx] = audioProcessor.IR_Path[idx].fromLastOccurrenceOf("\\", false, true);
    repaint();

    //R1.00 Setup JUCE variables to read the WAVE file.
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File jFile = juce::File::File(tFile);
    formatManager.registerBasicFormats();
    auto fileBuffer = juce::AudioBuffer < float >::AudioBuffer();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(jFile));

    //R1.00 Read the WAVE file.
    if (reader.get() != nullptr)
    {
        auto SampCount = (float)reader->lengthInSamples;
        if (SampCount < 3000) //R1.00 At some point we may want to know the sample rate. && (reader->sampleRate < 50000))
        {
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

            //Clip to a valid IR size.
            LIS = (int)reader->lengthInSamples;
            if (LIS < 1200) LISI = 1024;         //R1.00 Most likely a 1024 sample size. Arbitrary cutoff.
            if (1199 < LIS) LISI = LIS;          //R1.00 Get the File LIS. 
            if (2048 < LIS) LIS = 2048;          //R1.00 Do not exceed 2048 samples!!! May want to if we go looking for a valid pulse.
            if (2048 < LISI) LISI = 2048;        //R1.00 Our prog handles 1024 or Less than 2048.
            
            //R1.00 Get the max val in case we need to normalize the IR down to 1.0 max.
            float tV;
            float VScale = 1.0f;
            for (int t = 0; t < LIS; t++)
            {
                tV = fileBuffer.getSample(0, t);
                if (max < abs(tV))
                {
                    max = abs(tV);
                    maxIdx = t;
                }
            }
            
            //R1.00 Check for IR scale requirements.
            if (1.0f < max)  VScale = 1.0f / max;

            //R1.00 Store the WAVE file data into our Temporary IR buffer. LIS may exceed 2048 so clamp it to 2048.
            for (int t = 0; t < LIS; t++) audioProcessor.IR[idx][t] = VScale * fileBuffer.getSample(0, t);
            for (int t = LIS; t < LISI; t++) audioProcessor.IR[idx][t] = 0.0f;
            
            //R1.00 Force IR to update in the processor.
            audioProcessor.IR_SavePaths = true;
            audioProcessor.IR_Loaded[idx] = true;   //R1.00 Store we have a valid IR loaded.
            if (LISI == 1024)                       //R1.00 Store the Temp IRs length.
                audioProcessor.IR_Len[idx] = 1024;    
            else
                audioProcessor.IR_Len[idx] = 2048; 
            audioProcessor.SettingType = idx + 1;   //R1.00 Flag we have a new IR loaded. 
            audioProcessor.SettingChanged = true;   //R1.00 Flag processor we need to update values
            
            labHelp.setText("IR Loaded successfully.", juce::dontSendNotification);
            repaint();
        }
        else
        {
            //R2.15 Handle File size issues.
            labHelp.setText("ERROR:Invalid IR (1024 or 2048 @ 48kHz).", juce::dontSendNotification);
            repaint();
        }


    }

}

void MakoBiteAudioProcessorEditor::FILE_Mako_IR_PreLoad_Wave(juce::String tFile, int idx)
{
    float max = 0.0f;
    int maxIdx = 0;
    int LIS = 0;      //R1.00 Actual # of samples in file.
    int LISI = 1024;  //R1.00 Ideal number of samples. 

    labHelp.setText("IR Loading: Opening WAVE file.", juce::dontSendNotification);
    repaint();

    //R1.00 Track the filename of this IR.
    audioProcessor.IR_Path[idx] = tFile;
    audioProcessor.IR_Name[idx] = audioProcessor.IR_Path[idx].fromLastOccurrenceOf("\\", false, true);
    repaint();

    //R1.00 Setup JUCE variables to read the WAVE file.
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File jFile = juce::File::File(tFile);
    formatManager.registerBasicFormats();
    auto fileBuffer = juce::AudioBuffer < float >::AudioBuffer();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(jFile));

    //R1.00 Read the WAVE file.
    if (reader.get() != nullptr)
    {
        auto SampCount = (float)reader->lengthInSamples;
        if (SampCount < 3000) //R1.00 At some point we may want to know the sample rate. && (reader->sampleRate < 50000))
        {
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

            //R1.00 Clip to a valid IR size.
            LIS = (int)reader->lengthInSamples;
            if (LIS < 1200) LISI = 1024;         //R1.00 Most likely a 1024 sample size. Arbitrary cutoff.
            if (1199 < LIS) LISI = LIS;          //R1.00 Get the File LIS. 
            if (2048 < LIS) LIS = 2048;          //R1.00 Do not exceed 2048 samples!!! May want to if we go looking for a valid pulse.
            if (2048 < LISI) LISI = 2048;        //R1.00 Our prog handles 1024 or Less than 2048.

            //R1.00 Get the max val in case we need to normalize the IR down to 1.0 max.
            float tV;
            float VScale = 1.0f;
            for (int t = 0; t < LIS; t++)
            {
                tV = fileBuffer.getSample(0, t);
                if (max < abs(tV))
                {
                    max = abs(tV);
                    maxIdx = t;
                }
            }

            //R1.00 Check for IR scale requirements.
            if (1.0f < max)  VScale = 1.0f / max;

            //R1.00 Store the WAVE file data into our Temporary IR buffer. LIS may exceed 2048 so clamp it to 2048.
            for (int t = 0; t < LIS; t++) audioProcessor.IR[idx][t] = VScale * fileBuffer.getSample(0, t);
            for (int t = LIS; t < LISI; t++) audioProcessor.IR[idx][t] = 0.0f;

            audioProcessor.IR_Loaded[idx] = true;   //R1.00 Store we have a valid IR loaded.
            if (LISI == 1024)                       //R1.00 Store the Temp IRs length.
                audioProcessor.IR_Len[idx] = 1024;
            else
                audioProcessor.IR_Len[idx] = 2048;

            labHelp.setText("IR " + std::to_string(idx) +  " Loaded successfully.", juce::dontSendNotification);
            repaint();
        }
        else
        {
            //R1.00 Handle File size issues.
            labHelp.setText("IR " + std::to_string(idx) + " failed to load.", juce::dontSendNotification);
            repaint();
        }

    }

}

void MakoBiteAudioProcessorEditor::FILE_MakoIRPreset_Save()
{
    //dlgFileOpen = std::make_unique<juce::FileChooser>("Load Mako Distortion 2 IR file", juce::File{}, "*.txt");
    dlgFileOpen = std::make_unique<juce::FileChooser>("Save a MakoIR preset", juce::File{}, "*.mir");
    auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    //R1.00 Load IR file.
    dlgFileOpen->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                juce::String tFile = file.getFullPathName();
                juce::FileOutputStream output(tFile);
                {
                    //R1.00 Over write any data.
                    output.setPosition(0);
                    output.truncate();

                    //R1.00 Mako IR File, version 0.
                    output.writeText("MIR000\r\n", false, false, nullptr);
                    output.writeText("DO NOT EDIT THIS FILE\r\n", false, false, nullptr);

                    //R1.00 Write the setup variables.
                    output.writeText(std::to_string(audioProcessor.Pedal_Gain[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Gain[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Size[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Size[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Mix[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Mix[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Thump[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Thump[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_ThumpF[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_ThumpF[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Mono) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_LoCut[0]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_LoCut[1]) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Trim) + "\r\n", false, false, nullptr);
                    output.writeText(std::to_string(audioProcessor.Pedal_Gate) + "\r\n", false, false, nullptr);

                    //R1.00 Write the full paths to the IR files we need to load.
                    output.writeText(audioProcessor.IR_Path[0] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Path[1] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Path[2] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Path[3] + "\r\n", false, false, nullptr);

                    //R1.00 Write the names of the IR files.
                    output.writeText(audioProcessor.IR_Name[0] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Name[1] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Name[2] + "\r\n", false, false, nullptr);
                    output.writeText(audioProcessor.IR_Name[3] + "\r\n", false, false, nullptr);
                }
            }
        });
}

void MakoBiteAudioProcessorEditor::FILE_MakoIRPreset_Load()
{    
    //R1.00 We are using a simple text file format. A better system would be XML, etc as this can handle
    //R1.00 errors and missing fields much better.
    dlgFileOpen = std::make_unique<juce::FileChooser>("Load MakoIR Preset file", juce::File{}, "*.mir");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    dlgFileOpen->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            bool FileValid = false;
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                juce::FileInputStream Input(file);
                {
                    Input.setPosition(0);
                    auto b = Input.readNextLine(); //R1.00 MIR000
                    if (b == "MIR000")
                    {                        
                        b = Input.readNextLine();      //R1.00 DO NOT EDIT THIS FILE notice.

                        b = Input.readNextLine(); audioProcessor.Pedal_Gain[0] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Gain[1] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Size[0] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Size[1] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Mix[0] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Mix[1] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Thump[0] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Thump[1] = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_ThumpF[0] = FILE_ClipInt(b.getIntValue(), 80, 330);
                        b = Input.readNextLine(); audioProcessor.Pedal_ThumpF[1] = FILE_ClipInt(b.getIntValue(), 80, 330);
                        b = Input.readNextLine(); audioProcessor.Pedal_Mono = b.getIntValue();
                        b = Input.readNextLine(); audioProcessor.Pedal_LoCut[0] = FILE_ClipInt(b.getIntValue(), 20, 150);
                        b = Input.readNextLine(); audioProcessor.Pedal_LoCut[1] = FILE_ClipInt(b.getIntValue(), 20, 150);
                        b = Input.readNextLine(); audioProcessor.Pedal_Trim = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);
                        b = Input.readNextLine(); audioProcessor.Pedal_Gate = FILE_ClipFloat(b.getFloatValue(), 0.0f, 1.0f);

                        b = Input.readNextLine(); audioProcessor.IR_Path[0] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Path[1] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Path[2] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Path[3] = b;

                        b = Input.readNextLine(); audioProcessor.IR_Name[0] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Name[1] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Name[2] = b;
                        b = Input.readNextLine(); audioProcessor.IR_Name[3] = b;

                        FileValid = true;
                    }
                    else
                    {                        
                        labHelp.setText("ERROR: Invalid/Corrupt MIR File found. No data loaded.", juce::sendNotification);
                        repaint();
                    }

                }
            }

            if (FileValid)
            {
                File_UpdateControls();

                if (audioProcessor.IR_Path[0] != "") FILE_Mako_IR_PreLoad_Wave(audioProcessor.IR_Path[0], 0);
                if (audioProcessor.IR_Path[1] != "") FILE_Mako_IR_PreLoad_Wave(audioProcessor.IR_Path[1], 1);
                if (audioProcessor.IR_Path[2] != "") FILE_Mako_IR_PreLoad_Wave(audioProcessor.IR_Path[2], 2);
                if (audioProcessor.IR_Path[3] != "") FILE_Mako_IR_PreLoad_Wave(audioProcessor.IR_Path[3], 3);

                repaint();

                //R1.00 Tell Processor we have all new IRs to handle.
                audioProcessor.SettingType = Adj_All;
                audioProcessor.SettingChanged = true;
            }

        });


}

void MakoBiteAudioProcessorEditor::File_UpdateControls()
{
    jsP1_GainL.setValue(audioProcessor.Pedal_Gain[0], juce::sendNotification);
    jsP1_GainR.setValue(audioProcessor.Pedal_Gain[1], juce::sendNotification);
    jsP1_SizeL.setValue(audioProcessor.Pedal_Size[0], juce::sendNotification);
    jsP1_SizeR.setValue(audioProcessor.Pedal_Size[1], juce::sendNotification);
    jsP1_MixL.setValue(audioProcessor.Pedal_Mix[0], juce::sendNotification);
    jsP1_MixR.setValue(audioProcessor.Pedal_Mix[1], juce::sendNotification);
    jsP1_ThumpL.setValue(audioProcessor.Pedal_Thump[0], juce::sendNotification);
    jsP1_ThumpR.setValue(audioProcessor.Pedal_Thump[1], juce::sendNotification);
    jsP1_ThumpFL.setValue(audioProcessor.Pedal_ThumpF[0], juce::sendNotification);
    jsP1_ThumpFR.setValue(audioProcessor.Pedal_ThumpF[1], juce::sendNotification);
    jsP1_Mono.setValue(audioProcessor.Pedal_Mono, juce::sendNotification);
    jsP1_LoCutL.setValue(audioProcessor.Pedal_LoCut[0], juce::sendNotification);
    jsP1_LoCutR.setValue(audioProcessor.Pedal_LoCut[1], juce::sendNotification);
    jsP1_Trim.setValue(audioProcessor.Pedal_Trim, juce::sendNotification);
    jsP1_Gate.setValue(audioProcessor.Pedal_Gate, juce::sendNotification);
}

int MakoBiteAudioProcessorEditor::FILE_ClipInt(int val, int min, int max)
{
    int val2 = val;
    if (val < min) val2 = min;
    if (max < val) val2 = max;
    return val2;
}

float MakoBiteAudioProcessorEditor::FILE_ClipFloat(float val, float min, float max)
{
    float val2 = val;
    if (val < min) val2 = min;
    if (max < val) val2 = max;
    return val2;
}


void MakoBiteAudioProcessorEditor::Gui_Draw_Background(juce::Graphics& g)
{
    juce::ColourGradient ColGrad;

    //*********************************************************************
    // CODE TO DRAW THE BACKGROUND PNG. 
    // Load a blank texture then draw over it.
    //*********************************************************************     
    g.setColour(juce::Colour(0xFF808080));
    g.fillRect(0, 0, 580, 480);

    //*********************************************************************
    // LCD SCREENS
    //*********************************************************************
    ColGrad = juce::ColourGradient(juce::Colour(0xFF2556A6), 0.0f, 5.0f, juce::Colour(0xFF5085E8), 0.0f, 43.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(105, 5, 370, 38);
    ColGrad = juce::ColourGradient(juce::Colour(0xFF2556A6), 0.0f, 165.0f, juce::Colour(0xFF5085E8), 0.0f, 203.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(105, 165, 370, 38);

    //*********************************************************************
    // KNOB RECESSES
    //*********************************************************************
    //R1.00 Knob text backgrounds.
    g.setColour(juce::Colour(0x80303030));
    g.fillRect(5, 45, 70, 50);
    g.fillRect(105, 45, 70, 50);
    g.fillRect(205, 45, 70, 50);
    g.fillRect(305, 45, 70, 50);
    g.fillRect(405, 45, 70, 50);

    g.fillRect(5, 205, 70, 50);
    g.fillRect(105, 205, 70, 50);
    g.fillRect(205, 205, 70, 50);
    g.fillRect(305, 205, 70, 50);
    g.fillRect(405, 205, 70, 50);

    g.fillRect(505, 2, 60, 15);
    g.fillRect(505, 105, 60, 40);
    g.fillRect(505, 210, 60, 40);

    g.setColour(juce::Colour(0x80202020));
    g.fillRect(0, 338, 580, 40);

    //R1.00 Draw an inset under the sliders.
    g.setColour(juce::Colour(0xFF101010));
    g.fillEllipse(5, 60, 70, 70);
    g.fillEllipse(105, 60, 70, 70);
    g.fillEllipse(205, 60, 70, 70);
    g.fillEllipse(305, 60, 70, 70);
    g.fillEllipse(405, 60, 70, 70);
    g.fillEllipse(5, 220, 70, 70);
    g.fillEllipse(105, 220, 70, 70);
    g.fillEllipse(205, 220, 70, 70);
    g.fillEllipse(305, 220, 70, 70);
    g.fillEllipse(405, 220, 70, 70);

    g.fillEllipse(505, 120, 60, 60);
    g.fillEllipse(505, 225, 60, 60);

    //R1.00 Draw a KNOB inside the slider.
    ColGrad = juce::ColourGradient(juce::Colour(0xFFA0A0A0), 0.0f, 77.0f, juce::Colour(0xFF808080), 0.0f, 113.0f, false);
    g.setGradientFill(ColGrad);
    g.fillEllipse(22, 77, 36, 36);
    g.fillEllipse(122, 77, 36, 36);
    g.fillEllipse(222, 77, 36, 36);
    g.fillEllipse(322, 77, 36, 36);
    g.fillEllipse(422, 77, 36, 36);

    ColGrad = juce::ColourGradient(juce::Colour(0xFFC0C0C0), 0.0f, 237.0f, juce::Colour(0xFF808080), 0.0f, 273.0f, false);
    g.setGradientFill(ColGrad);
    g.fillEllipse(22, 237, 36, 36);
    g.fillEllipse(122, 237, 36, 36);
    g.fillEllipse(222, 237, 36, 36);
    g.fillEllipse(322, 237, 36, 36);
    g.fillEllipse(422, 237, 36, 36);

    ColGrad = juce::ColourGradient(juce::Colour(0xFFC0C0C0), 0.0f, 137.0f, juce::Colour(0xFF808080), 0.0f, 163.0f, false);
    g.setGradientFill(ColGrad);
    g.fillEllipse(522, 137, 26, 26);
    ColGrad = juce::ColourGradient(juce::Colour(0xFFC0C0C0), 0.0f, 242.0f, juce::Colour(0xFF808080), 0.0f, 268.0f, false);
    g.setGradientFill(ColGrad);
    g.fillEllipse(522, 242, 26, 26);

    //*********************************************************************
    // LOGO
    //*********************************************************************
    g.setColour(juce::Colours::black);
    g.setFont(16.0f);
    g.drawFittedText("MakoIR v1.0", 361, 341, 210, 15, juce::Justification::centredRight, 1);
    g.setFont(10.0f);
    g.drawFittedText("Rosbone DSP", 361, 356, 210, 15, juce::Justification::centredRight, 1);

    g.setColour(juce::Colours::orange);
    g.setFont(16.0f);
    g.drawFittedText("MakoIR v1.0", 360, 340, 210, 15, juce::Justification::centredRight, 1);
    g.setFont(10.0f);
    g.drawFittedText("Rosbone DSP", 360, 355, 210, 15, juce::Justification::centredRight, 1);

    //*********************************************************************
    // LABEL TEXT 
    //*********************************************************************
    //R1.00 Control Label text. 
    g.setColour(juce::Colours::black);
    g.setFont(12.0f);
    g.drawFittedText("Vol L", 1, 46, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Size L", 101, 46, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Mix L", 201, 46, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump L", 301, 46, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump Freq", 401, 46, 80, 15, juce::Justification::centred, 1);

    g.drawFittedText("Vol R", 1, 206, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Size R", 101, 206, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Mix R", 201, 206, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump R", 301, 206, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump Freq", 401, 206, 80, 15, juce::Justification::centred, 1);

    g.drawFittedText("Stereo/Mono", 186, 341, 60, 15, juce::Justification::centred, 1);

    g.drawFittedText("Low Cut", 494, 3, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Trim", 494, 108, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("CPU Gate", 494, 213, 80, 15, juce::Justification::centred, 1);


    //R1.00 Control Label text. 
    g.setColour(juce::Colour(0xFFC0C0C0));
    g.setFont(12.0f);
    g.drawFittedText("Vol L", 0, 45, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Size L", 100, 45, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Mix L", 200, 45, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump L", 300, 45, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump Freq", 400, 45, 80, 15, juce::Justification::centred, 1);

    g.drawFittedText("Vol R", 0, 205, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Size R", 100, 205, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Mix R", 200, 205, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump R", 300, 205, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Thump Freq", 400, 205, 80, 15, juce::Justification::centred, 1);

    g.drawFittedText("Stereo/Mono", 185, 340, 60, 15, juce::Justification::centred, 1);

    g.drawFittedText("Low Cut", 493, 2, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("Trim", 493, 107, 80, 15, juce::Justification::centred, 1);
    g.drawFittedText("CPU Gate", 493, 212, 80, 15, juce::Justification::centred, 1);

}
