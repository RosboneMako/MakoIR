/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "cmath"              //R1.00 Added library.

//==============================================================================
MakoBiteAudioProcessor::MakoBiteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
    ),

    //R1.00 Added for VALUE TREE.
    parameters(*this, nullptr, "PARAMETERS", {
      std::make_unique<juce::AudioParameterFloat>("gainl","Gain Left", .0f, 2.0f, 1.0f),
      std::make_unique<juce::AudioParameterFloat>("gainr","Gain Right", .0f, 2.0f, 1.0f),
      std::make_unique<juce::AudioParameterFloat>("sizel","Size Left", .0f, 1.0f, .5f),
      std::make_unique<juce::AudioParameterFloat>("sizer","Size Right", .0f, 1.0f, .5f),
      std::make_unique<juce::AudioParameterFloat>("mixl","Mix Left", .0f, 1.0f, .5f),
      std::make_unique<juce::AudioParameterFloat>("mixr","Mix Right", .0f, 1.0f, .5f),
      std::make_unique<juce::AudioParameterFloat>("thumpl","Thump Left", .0f, 1.0f, 0.0f),
      std::make_unique<juce::AudioParameterFloat>("thumpr","Thump Right", .0f, 1.0f, 0.0f),
      std::make_unique<juce::AudioParameterInt>("thumpfl","Thump Freq L", 80, 330, 150),
      std::make_unique<juce::AudioParameterInt>("thumpfr","Thump Freq R", 80, 330, 150),
      std::make_unique<juce::AudioParameterInt>("mono","Mono", 0, 1, 1),
      std::make_unique<juce::AudioParameterInt>("locutl","Low Cut L", 20, 150, 20),
      std::make_unique<juce::AudioParameterInt>("locutr","Low Cut R", 20, 150, 20),
      std::make_unique<juce::AudioParameterFloat>("trim","Trim", .0f, 1.0f, 1.0f),
      std::make_unique<juce::AudioParameterFloat>("cpugate","CPU Gate", .0f, 1.0f, 0.0f),      

    }

    )

#endif
{
   
}

MakoBiteAudioProcessor::~MakoBiteAudioProcessor()
{
}

//==============================================================================
const juce::String MakoBiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MakoBiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MakoBiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MakoBiteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MakoBiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MakoBiteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MakoBiteAudioProcessor::getProgramName (int index)
{
    return {};
}

void MakoBiteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MakoBiteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //R1.00 Get our Sample Rate for filter calculations.
    SampleRate = MakoBiteAudioProcessor::getSampleRate();
    if (SampleRate < 21000) SampleRate = 48000;
    if (192000 < SampleRate) SampleRate = 48000;

    //R1.00 Force our Thump filters to be setup.
    SettingType = 5;     
    Settings_Update();

    //R1.00 Create a Full frequency IR to begin with.
    //R1.00 An IR by def is a single pulse at full volume. __|_______
    //R1.00 You can think of it as a single echo.
    IR_Final[0][0] = 1.0f;
    IR_Final[1][0] = 1.0f;

    //R1.00 Get the last known IRs since we dont save them as parameters.
    PATHS_Load();
}

void MakoBiteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MakoBiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MakoBiteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //R1.00 Our defined variables.
    float tS;

    //R1.00 Handle any changes to our Paramters.
    //R1.00 Handle any settings changes made in Editor. 
    if (0 < SettingChanged) Settings_Update();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        //R1.00 Process the AUDIO buffer data.
        if (Pedal_Mono && (channel == 1))
        {
            auto* channel0Data = buffer.getWritePointer(0);

            //R1.00 FORCE MONO - Put CHANNEL 0 data in CHANNEL 1.
            for (int samp = 0; samp < buffer.getNumSamples(); samp++) channelData[samp] = channel0Data[samp];
        }
        else

        // ..do something to the data...
        for (int samp = 0; samp < buffer.getNumSamples(); samp++)
        {

            tS = buffer.getSample(channel, samp);  //R1.00 Get the current sample and put it in tS. 
            
            //R1.00 If we have a valid signal, apply the IR.
            if (IR_SignalPresent(tS, channel))
            {
                tS = IR_ProcessAudio(tS, channel);     //R1.00 Apply our IR/CAB simulation. 
                tS = IR_Thump(tS, channel);            //R1.00 Add some thump.
                tS = IR_LoCut(tS, channel);            //R1.00 Adjust the very low freq area.
            }
            else
                tS = 0.0f;

            channelData[samp] = tS;                //R1.00 Write our modified sample back into the sample buffer.
        }
    }
}

//==============================================================================
bool MakoBiteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MakoBiteAudioProcessor::createEditor()
{
    return new MakoBiteAudioProcessorEditor (*this);
}


//==============================================================================
void MakoBiteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
        
    //R1.00 Save our parameters to file/DAW.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
   
    //R1.00 There are methods to store strings in the XML. This would be a better way
    //R1.00 to track our IR paths. DAWs do not deal with strings so it is complicated.
    //R1.00 Example: Line6 Helix uses a local database of 100 IRs so they can just pass
    //R1.00 an integer back and forth thru the XML ValueTree/DAW. This becomes a pain if
    //R1.00 you are working on more than one PC. Keeping the databases the same.
}

void MakoBiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
        
    //R1.00 Read our parameters from file/DAW.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    //R1.00 Get our updated parameters.
    Pedal_Gain[0] = makoGetParmValue_float("gainl");
    Pedal_Gain[1] = makoGetParmValue_float("gainr");
    Pedal_Size[0] = makoGetParmValue_float("sizel");
    Pedal_Size[1] = makoGetParmValue_float("sizer");
    Pedal_Mix[0] = makoGetParmValue_float("mixl");
    Pedal_Mix[1] = makoGetParmValue_float("mixr");
    Pedal_Thump[0] = makoGetParmValue_float("thumpl");
    Pedal_Thump[1] = makoGetParmValue_float("thumpr");
    Pedal_ThumpF[0] = makoGetParmValue_int("thumpfl");
    Pedal_ThumpF[1] = makoGetParmValue_int("thumpfr");
    Pedal_Mono = makoGetParmValue_int("mono");
    Pedal_LoCut[0] = makoGetParmValue_int("locutl");
    Pedal_LoCut[1] = makoGetParmValue_int("locutr");
    Pedal_Trim = makoGetParmValue_float("trim");
    Pedal_Gate = makoGetParmValue_float("cpugate");
}

int MakoBiteAudioProcessor::makoGetParmValue_int(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return int(parm->load());
    else
        return 0;
}

float MakoBiteAudioProcessor::makoGetParmValue_float(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return float(parm->load());
    else
        return 0.0f;
}

void MakoBiteAudioProcessor::PATHS_Load()
{    
    //R1.00 Try to read the last know IRs we were using.
    //R1.00 This is a hack. Should be saved in XML value tree data.
    juce::File f = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoIR\\makoirpaths.txt";
    
    if (f.existsAsFile())
    {
        bool FileValid = false;
        juce::FileInputStream Input(f);
        {
            Input.setPosition(0);
            auto b = Input.readNextLine();   //R1.00 MIRPATHS000
            if (b != "MIRPATHS000")
                FileValid = false;
            else
            {
                FileValid = true;

                b = Input.readNextLine();    //R1.00 Do not edit this file.

                b = Input.readNextLine(); IR_Path[0] = b;
                b = Input.readNextLine(); IR_Path[1] = b;
                b = Input.readNextLine(); IR_Path[2] = b;
                b = Input.readNextLine(); IR_Path[3] = b;

                b = Input.readNextLine(); IR_Name[0] = b;
                b = Input.readNextLine(); IR_Name[1] = b;
                b = Input.readNextLine(); IR_Name[2] = b;
                b = Input.readNextLine(); IR_Name[3] = b;
            }
        }

        //R1.00 We should have valid data from our file.
        if (FileValid)
        {
            for (int t = 0; t < 4; t++)
            {
                IR_LoadFromPaths(t);
                if (IR_Loaded[t] == true) IR_Create_2048(t);
            }
        }
    }

    return;
}

void MakoBiteAudioProcessor::PATHS_Save()
{
    //R1.00 This code is a hack. Probably NOT a good idea. But fun to try.
    //R1.00 Need to write this data into the XML value tree data instead.

    //R1.00 We need to create a data directory to save our large STRING data. Strings are NOT suitable for host parameters (bool,float).
    juce::File fDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoIR";
    juce::Result tRes(fDir.createDirectory());

    //R1.00 Did our directory get created?
    if (!fDir.isDirectory()) 
        return;

    //R1.00 We have a working data directory to store data, save data.
    juce::File f = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoIR\\makoirpaths.txt";
    juce::FileOutputStream output(f);
    {        
        //R1.00 Over write any data.
        output.setPosition(0);
        output.truncate();

        //R1.00 Mako IR PATHS file, version 0.
        output.writeText("MIRPATHS000\r\n", false, false, nullptr);
        output.writeText("DO NOT EDIT THIS FILE\r\n", false, false, nullptr);
        
        //R1.00 Write the full paths to the IR files we need to load.
        output.writeText(IR_Path[0] + "\r\n", false, false, nullptr);
        output.writeText(IR_Path[1] + "\r\n", false, false, nullptr);
        output.writeText(IR_Path[2] + "\r\n", false, false, nullptr);
        output.writeText(IR_Path[3] + "\r\n", false, false, nullptr);

        //R1.00 Write the names of the IR files.
        output.writeText(IR_Name[0] + "\r\n", false, false, nullptr);
        output.writeText(IR_Name[1] + "\r\n", false, false, nullptr);
        output.writeText(IR_Name[2] + "\r\n", false, false, nullptr);
        output.writeText(IR_Name[3] + "\r\n", false, false, nullptr);
    }
}

void MakoBiteAudioProcessor::IR_LoadFromPaths(int idx)
{
    float max = 0.0f;
    int maxIdx = 0;
    int LIS = 0;                        //R1.00 Actual # of samples in file (Length In Samples).
    int LISI = 1024;                    //R1.00 Ideal number of samples. 
    juce::String tFile = IR_Path[idx];

    //R1.00 No defined IR to load. 
    if (IR_Path[idx] == "") return;

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
        //R1.00 get the sample count.
        auto SampCount = (float)reader->lengthInSamples;

        //R1.00 IR wave files should be either 1024 or 2048 samples. Having exact sizes is not expected.
        //R1.00 However, some IRs come with gigantic files. We would need to sort thru the giant file to locate the actual IR. 
        //R1.00 We are not going to do thet here. Limit to files less than 3000 samples in size. 
        //R1.00 We should also know the sample rate so we can resample the IR to our current sample rate. Skipping that here.
        if (SampCount < 3000) //&& (reader->sampleRate < 50000))
        {
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

            //R1.00 Clip to a valid IR size.
            LIS = (int)reader->lengthInSamples;
            if (LIS < 1200) LISI = 1024;         //R1.00 Most likely a 1024 sample size. Arbitrary cutoff.
            if (1199 < LIS) LISI = LIS;          //R1.00 Get the File LIS. 
            if (2048 < LIS) LIS = 2048;          //R1.00 Do not exceed 2048 samples!!! May want to if we go looking for a valid pulse.
            if (2048 < LISI) LISI = 2048;        //R1.00 Our prog handles 1024 or Less than 2048.

            //R1.00 Get the max val so we can normalize the IR to -1.0f/1.0f max.
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
            for (int t = 0; t < LIS; t++) IR[idx][t] = VScale * fileBuffer.getSample(0, t);
            for (int t = LIS; t < LISI; t++) IR[idx][t] = 0.0f;

            IR_Loaded[idx] = true;        //R1.00 Store we have a valid IR loaded.
            if (LISI == 1024)             //R1.00 Store the Temp IRs length.
                IR_Len[idx] = 1024;
            else
                IR_Len[idx] = 2048;
        }
        else
        {
            //R1.00 Handle File size issues.
            
        }

    }

}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakoBiteAudioProcessor();
}


float MakoBiteAudioProcessor::Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn)
{
    float tS = tSample;

    fn->xn0[channel] = tS;
    tS = fn->a0 * fn->xn0[channel] + fn->a1 * fn->xn1[channel] + fn->a2 * fn->xn2[channel] - fn->b1 * fn->yn1[channel] - fn->b2 * fn->yn2[channel];
    fn->xn2[channel] = fn->xn1[channel]; fn->xn1[channel] = fn->xn0[channel]; fn->yn2[channel] = fn->yn1[channel]; fn->yn1[channel] = tS;

    return tS;
}

void MakoBiteAudioProcessor::Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn)
{
    //R1.00 Second order parametric/peaking boost filter with constant-Q
    float K = pi2 * (Fc * .5f) / SampleRate;
    float K2 = K * K;
    float V0 = pow(10.0, Gain_dB / 20.0);

    float a = 1.0f + (V0 * K) / Q + K2;
    float b = 2.0f * (K2 - 1.0f);
    float g = 1.0f - (V0 * K) / Q + K2;
    float d = 1.0f - K / Q + K2;
    float dd = 1.0f / (1.0f + K / Q + K2);

    fn->a0 = a * dd;
    fn->a1 = b * dd;
    fn->a2 = g * dd;
    fn->b1 = b * dd;
    fn->b2 = d * dd;
    fn->c0 = 1.0f;
    fn->d0 = 0.0f;
}

void MakoBiteAudioProcessor::Filter_LP_Coeffs(float fc, tp_filter* fn)
{
    //R2.00 Second order LOW PASS filter. 
    float c = 1.0f / (tanf(pi * fc / SampleRate));
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = 2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * (1.0f - (c * c));
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

void MakoBiteAudioProcessor::Filter_HP_Coeffs(float fc, tp_filter* fn)
{
    //F1.00 Second order butterworth High Pass.
    float c = tanf(pi * fc / SampleRate);
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = -2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * ((c * c) - 1.0f);
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

float MakoBiteAudioProcessor::IR_ProcessAudio(float tSample, int channel)
{   
    int T1;
    float V = 0.0f;    

    //R1.00 NO CAB selected. 
    if (!IR_FinalValid[channel]) return Sample_Clip(Pedal_Gain[channel] * tSample);
    
    //R1.00 Create a Bitmask for 1024 samples (Default). 
    int BitMask = 0x3FF;
    if (1024 < IR_FinalLen[channel]) BitMask = 0x7FF;   //R1.00 Mask for 2048 samples. 
        
    T1 = IR_BufferIndex[channel];
    IR_Buffer[channel][T1] = tSample;

    //R1.00 Apply cabs based on channel 0-Left, 1-Right.
    float tLen = IR_FinalLen[channel] * Pedal_Trim;     //R1.00 Reduce calculations to save CPU cycles.
    for (int t = 0; t < tLen; t++)
    {
        V += (IR_Buffer[channel][T1] * IR_Final[channel][t]);

        //R1.00 Increment index, mask off bits past 1023 to keep our index between 0-1023.
        T1 = (T1 + 1) & BitMask;
    }   

    //R1.00 Adjust the IR gain by setting and 25% since we may gain vol in IR.
    V *= Pedal_Gain[channel] * .25f;

    //R1.00 Decrement our ring buffer index and loop around at 0. 
    IR_BufferIndex[channel]--;
    if (IR_BufferIndex[channel] < 0) IR_BufferIndex[channel] = IR_FinalLen[channel] - 1;
    
    return Sample_Clip(V);
}

float MakoBiteAudioProcessor::IR_Thump(float tSample, int channel)
{
    float tS = tSample;

    //R1.00 Enhance the Low freqs.
    if (0.0f < Pedal_Thump[channel])
    {
        float tS_Enh = Filter_Calc_BiQuad(tS, channel, &makoF_Thump[channel]);
        tS += tanhf(tS_Enh * Pedal_Thump[channel] * 4.0f);
    }

    return tS;
}

float MakoBiteAudioProcessor::IR_LoCut(float tSample, int channel)
{
    float tS = tSample;

    //R1.00 Enhance the Low freqs.
    if (20 < Pedal_LoCut[channel]) tS = Filter_Calc_BiQuad(tS, channel, &makoF_LoCut[channel]);
        
    return tS;
}

bool MakoBiteAudioProcessor::IR_SignalPresent(float tSample, int channel)
{
    //R1.00 If we have a signal return TRUE.
    //R1.00 This is used to avoid IR calcs (heavy CPU usage) when not needed.
    if (0.0f < Pedal_Gate)
    {
        //R1.00 Track our Input Signal Average (Absolute vals).
        Signal_AVG[channel] = (Signal_AVG[channel] * .999f) + (abs(tSample) * .001f);
        
        //R1.00 Slowly reduce our Average value.
        Signal_AVG[channel] *= .9999f;

        //R1.00 Apply Noise gate.
        if (Signal_AVG[channel] < (.001f * (Pedal_Gate * Pedal_Gate))) return false;        
    }

    return true;
}

void MakoBiteAudioProcessor::Settings_Update()
{
    int IR_Idx;

    //R1.00 Update our Filters.
    if ((SettingType == Adj_Filters) || (SettingType == Adj_All))
    {
        Filter_LP_Coeffs(Pedal_ThumpF[0], &makoF_Thump[0]);
        Filter_LP_Coeffs(Pedal_ThumpF[1], &makoF_Thump[1]);
        Filter_HP_Coeffs(Pedal_LoCut[0], &makoF_LoCut[0]);
        Filter_HP_Coeffs(Pedal_LoCut[1], &makoF_LoCut[1]);
    }

    //R1.00 Modify an IR (Setting Type 1,2,3,4). 
    if ((0 < SettingType) && (SettingType < 5))
    {
        IR_Idx = SettingType - 1;
        IR_Create_2048(IR_Idx);
    }

    //R1.00 Delete an IR (Setting Type 6,7,8,9)
    if ((5 < SettingType) && (SettingType < 10))
    {
        //R1.00 Delete the IR and then rebuild the IR being played.
        IR_Idx = SettingType - 6;
        IR_Delete(IR_Idx);
        PATHS_Save();

        //R1.00 Figure out which IR is valid and use that to recalc the final IR.
        int BIdx = IR_Idx % 2;
        if (IR_Loaded[BIdx] || IR_Loaded[BIdx+2])
        {
            IR_Idx = BIdx;                              //R1.00 Start with the Base Index.
            if (!IR_Loaded[BIdx]) IR_Idx = BIdx + 2;    //R1.00 Base IR does not exist, use the second IR.
            IR_Create_2048(IR_Idx);
        }
    }

    //R1.00 We loaded a preset and have all new IRs to deal with.
    if (SettingType == 10)
    {
        for (int t = 0; t < 4; t++)
        {
            if (IR_Loaded[t] == true)  IR_Create_2048(t);            
        }

        //R1.00 Save the new IR paths we just loaded.
        PATHS_Save();
    }

    //R1.00 We need to save our last used IR file paths.
    if (IR_SavePaths)
    {
        PATHS_Save();
        IR_SavePaths = false;
    }
    
    //R1.00 Reset our SETTINGS needs updated vars.
    SettingType == 0;
    SettingChanged = false;
}

void MakoBiteAudioProcessor::IR_Create_1024(int idx)
{
    //R1.00 IR0 and IR1 are the base IRs. IR2 and IR3 are the "mix" IRs.
    int Bidx = idx % 2;                     //R1.00 Get the base Index 0 or 1.
    float Max = 0.0f;
    float Mix = Pedal_Mix[Bidx];
    float MixInv = 1.0f - Pedal_Mix[Bidx];
    float TempIR[1024];

    //********************************************************************************
    // CREATE A TEMPORARY IR single or Mixed if two IRs exist.
    //******************************************************************************** 
    //R1.00 If we have two valid IRs, mix the two IRs together. 
    if ((IR_Loaded[Bidx] == true) && (IR_Loaded[Bidx + 2] == true))
    {
        for (int t = 0; t < 1024; t++) TempIR[t] = (IR[Bidx][t] * MixInv) + (IR[Bidx + 2][t] * Mix);
    }
    else
    {   //R1.00 Use the only valid IR we have.
        for (int t = 0; t < 1024; t++) TempIR[t] = IR[idx][t];
    }

    //R1.00 Set our base IR array len to the largest loaded IR. 
    IR_FinalLen[Bidx] = IR_Len[Bidx];
    if (IR_FinalLen[Bidx] < IR_Len[Bidx + 2]) IR_FinalLen[Bidx] = IR_Len[Bidx + 2];

    //********************************************************************************
    // Apply speaker size if selected and store in the final active IR array. 
    //******************************************************************************** 
    if ((Pedal_Size[Bidx] < .499f) || (.501f < Pedal_Size[Bidx]))
    {
       int t = 0;
       float t2 = 0.0f;
       int t2i = 0;
       float t2diff = 0.0f;
       float t2s = (1024.0f + ((Pedal_Size[Bidx] - .5f) * 1000.0f)) / 1024.0f;

       //R1.00 Resample the IR using new specs.
       while ((t < 1024) && (t2 < 1023.0001f))
       {
           t2i = int(t2);
           t2diff = t2 - t2i;
           IR_Final[Bidx][t] = (TempIR[t2i] * (1.0f - t2diff)) + (TempIR[t2i + 1] * t2diff);

           //R1.00 store MAX volume for Normalize code later.
           if (Max < abs(IR_Final[Bidx][t])) Max = abs(IR_Final[Bidx][t]);
           t++;
           t2 += t2s;
       }

       //R1.00 Clear the unused values or the volume may explode when Maxed/Normalized. 2048=Max buff size.
       while (t < 2048)
       {
           IR_Final[Bidx][t] = 0.0f;
           t++;
       }           
    }
    else
    {
        //R1.00 Default of .5 is selected.
        //R1.00 Do not apply Speaker Size/Time stretching.
        for (int t = 0; t < 1024; t++)
        {
            IR_Final[Bidx][t] = TempIR[t];
            if (Max < abs(IR_Final[Bidx][t])) Max = abs(IR_Final[Bidx][t]);
        }
        for (int t = 1024; t < 2048; t++) IR_Final[Bidx][t] = 0.0f;
    }

    //R1.00 Normalize.
    if ((0.0f < Max) && (Max < 1.0f))
    {
        Max = 1.0f / Max;
        for (int t = 0; t < 2048; t++) IR_Final[Bidx][t] *= Max;
    }
        
    //R1.00 Flag that our FINAL IR is ready to be used.
    IR_FinalValid[Bidx] = true;
}

void MakoBiteAudioProcessor::IR_Create_2048(int idx)
{
    //R1.00 IR0 and IR1 are the base IRs. IR2 and IR3 are the "mix" IRs.
    int Bidx = idx % 2;                     //R1.00 Get the base Index 0 or 1.
    float Max = 0.0f;
    float Mix = Pedal_Mix[Bidx];
    float MixInv = 1.0f - Pedal_Mix[Bidx];
    float TempIR[2048];

    //********************************************************************************
    // CREATE A TEMPORARY IR single or Mixed if two IRs exist.
    //******************************************************************************** 
    //R1.00 If we have two valid IRs, mix the two IRs together. 
    if ((IR_Loaded[Bidx] == true) && (IR_Loaded[Bidx + 2] == true))
    {
        for (int t = 0; t < 2048; t++) TempIR[t] = (IR[Bidx][t] * MixInv) + (IR[Bidx + 2][t] * Mix);
    }
    else
    {   //R1.00 Use the only valid IR we have.
        for (int t = 0; t < 2048; t++) TempIR[t] = IR[idx][t];
    }

    //R1.00 Set our base IR array len to the largest loaded IR. 
    IR_FinalLen[Bidx] = IR_Len[Bidx];
    if (IR_FinalLen[Bidx] < IR_Len[Bidx + 2]) IR_FinalLen[Bidx] = IR_Len[Bidx + 2];

    //********************************************************************************
    // Apply speaker size if selected and store in the final active IR array. 
    //******************************************************************************** 
    if ((Pedal_Size[Bidx] < .499f) || (.501f < Pedal_Size[Bidx]))
    {        
        int t = 0;
        float t2 = 0.0f;
        int t2i = 0;
        float t2diff = 0.0f;
        float t2s = (2048.0f + ((Pedal_Size[Bidx] - .5f) * 1000.0f)) / 2048.0f;

        //R1.00 Resample the IR using new specs.
        while ((t < 2048) && (t2 < 2047.0001f))
        {
            t2i = int(t2);
            t2diff = t2 - t2i;
            IR_Final[Bidx][t] = (TempIR[t2i] * (1.0f - t2diff)) + (TempIR[t2i + 1] * t2diff);

            //R1.00 store MAX volume for Normalize code later.
            if (Max < abs(IR_Final[Bidx][t])) Max = abs(IR_Final[Bidx][t]);
            t++;
            t2 += t2s;
        }

        //R1.00 Clear the unused values or the volume may explode when Maxed/Normalized.
        if (t < 2048)
        {
            while (t < 2048)
            {
                IR_Final[Bidx][t] = 0.0f;
                t++;
            }
        }       
    }
    else
    {
        //R1.00 Default of .5 is selected.
        //R1.00 Do not apply Speaker Size/Time stretching.
        for (int t = 0; t < 2048; t++)
        {
            IR_Final[Bidx][t] = TempIR[t];
            if (Max < abs(IR_Final[Bidx][t])) Max = abs(IR_Final[Bidx][t]);
        }
    }

    //R1.00 Normalize.
    if ((0.0f < Max) && (Max < 1.0f))
    {
        Max = 1.0f / Max;
        for (int t = 0; t < 2048; t++) IR_Final[Bidx][t] *= Max;
    }

    //R1.00 Flag that our FINAL IR is ready to be used.
    IR_FinalValid[Bidx] = true;
}

void MakoBiteAudioProcessor::IR_Delete(int idx)
{
    //R1.00 Flag we dont have an IR here anymore.
    IR_Loaded[idx] = false;
    IR_Len[idx] = 1024;
    IR_Path[idx] = "";
    IR_Name[idx] = "";

    //R1.00 Reset the IR lengths back to original if they were changed.
    if (IR_Loaded[0] && (!IR_Loaded[2])) IR_FinalLen[0] = IR_Len[0];
    if (!IR_Loaded[0] && (IR_Loaded[2])) IR_FinalLen[0] = IR_Len[2];
    if (IR_Loaded[1] && (!IR_Loaded[3])) IR_FinalLen[1] = IR_Len[1];
    if (!IR_Loaded[1] && (IR_Loaded[3])) IR_FinalLen[1] = IR_Len[3];

    //R1.00 Verify our FINAL IR arrays are valid.
    if (!IR_Loaded[0] && !IR_Loaded[2]) IR_FinalValid[0] = false;
    if (!IR_Loaded[1] && !IR_Loaded[3]) IR_FinalValid[1] = false;

    //R1.00 Create full frequency IR.
    IR[idx][0] = 1.0f;
    for (int t = 1; t < 2048; t++) IR[idx][t] = 0.0f;
}

int MakoBiteAudioProcessor::IR_GetValidIRIndex(int idx)
{
    //R1.00 Calc our base index 0 or 1.
    int Bidx = idx % 2;
    
    //R1.00 Verify our FINAL IR arrays are valid.
    if (!IR_Loaded[Bidx] && IR_Loaded[Bidx + 2]) Bidx = Bidx + 2;
    
    return Bidx;
}

float MakoBiteAudioProcessor::Sample_Clip(float tSample)
{
    float tS = tSample;

    //R1.00 Clip signal so we dont crash the audio engine.
    if (tS < -.9999f) tS = -.999f;
    if (.9999f < tS) tS = .999f;

    return tS;
}
