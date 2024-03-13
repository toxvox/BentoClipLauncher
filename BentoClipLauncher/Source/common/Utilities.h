/*
    ,--.                     ,--.     ,--.  ,--.
  ,-'  '-.,--.--.,--,--.,---.|  |,-.,-'  '-.`--' ,---. ,--,--,      Copyright 2018
  '-.  .-'|  .--' ,-.  | .--'|     /'-.  .-',--.| .-. ||      \   Tracktion Software
    |  |  |  |  \ '-'  \ `--.|  \  \  |  |  |  |' '-' '|  ||  |       Corporation
    `---' `--'   `--`--'`---'`--'`--' `---' `--' `---' `--''--'    www.tracktion.com
*/

#pragma once

using namespace std::literals;
using namespace tracktion::literals;

#include "BinaryData.h"

//==============================================================================
namespace Helpers
{
    static inline void addAndMakeVisible (Component& parent, const Array<Component*>& children)
    {
        for (auto c : children)
            parent.addAndMakeVisible (c);
    }

    static inline String getStringOrDefault (const String& stringToTest, const String& stringToReturnIfEmpty)
    {
        return stringToTest.isEmpty() ? stringToReturnIfEmpty : stringToTest;
    }
    
    static inline File findRecentEdit (const File& dir)
    {
        auto files = dir.findChildFiles (File::findFiles, false, "*.tracktionedit");
        
        if (files.size() > 0)
        {
            files.sort();
            return files.getLast();
        }
        
        return {};
    }
}

//==============================================================================
namespace PlayHeadHelpers
{
    // Quick-and-dirty function to format a timecode string
    static inline String timeToTimecodeString (double seconds)
    {
        auto millisecs = roundToInt (seconds * 1000.0);
        auto absMillisecs = std::abs (millisecs);

        return String::formatted ("%02d:%02d:%02d.%03d",
                                  millisecs / 3600000,
                                  (absMillisecs / 60000) % 60,
                                  (absMillisecs / 1000)  % 60,
                                  absMillisecs % 1000);
    }

    // Quick-and-dirty function to format a bars/beats string
    static inline String quarterNotePositionToBarsBeatsString (double quarterNotes, int numerator, int denominator)
    {
        if (numerator == 0 || denominator == 0)
            return "1|1|000";

        auto quarterNotesPerBar = ((double) numerator * 4.0 / (double) denominator);
        auto beats  = (fmod (quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

        auto bar    = ((int) quarterNotes) / quarterNotesPerBar + 1;
        auto beat   = ((int) beats) + 1;
        auto ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

        return String::formatted ("%d|%d|%03d", bar, beat, ticks);
    }

    // Returns a textual description of a CurrentPositionInfo
    static inline String getTimecodeDisplay (const AudioPlayHead::CurrentPositionInfo& pos)
    {
        MemoryOutputStream displayText;

        displayText << String (pos.bpm, 2) << " bpm, "
                    << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                    << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                    << "  -  " << quarterNotePositionToBarsBeatsString (pos.ppqPosition,
                                                                        pos.timeSigNumerator,
                                                                        pos.timeSigDenominator);

        if (pos.isRecording)
            displayText << "  (recording)";
        else if (pos.isPlaying)
            displayText << "  (playing)";
        else
            displayText << "  (stopped)";

        return displayText.toString();
    }
}

//==============================================================================
namespace EngineHelpers
{
    static tracktion::Project::Ptr createTempProject (tracktion::Engine& engine)
    {
        auto file = engine.getTemporaryFileManager().getTempDirectory().getChildFile ("temp_project").withFileExtension (tracktion::projectFileSuffix);
        tracktion::ProjectManager::TempProject tempProject (engine.getProjectManager(), file, true);
        return tempProject.project;
    }

    static void showAudioDeviceSettings (tracktion::Engine& engine)
    {
        DialogWindow::LaunchOptions o;
        o.dialogTitle = TRANS("Audio Settings");
        o.dialogBackgroundColour = LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId);
        o.content.setOwned (new AudioDeviceSelectorComponent (engine.getDeviceManager().deviceManager,
                                                              0, 512, 1, 512, false, false, true, true));
        o.content->setSize (400, 600);
        o.launchAsync();
    }

    static void browseForAudioFile (tracktion::Engine& engine, std::function<void (const File&)> fileChosenCallback)
    {
        auto fc = std::make_shared<FileChooser> ("Please select an audio file to load...",
                                                 engine.getPropertyStorage().getDefaultLoadSaveDirectory ("pitchAndTimeExample"),
                                                 engine.getAudioFileFormatManager().readFormatManager.getWildcardForAllFormats());

        fc->launchAsync (FileBrowserComponent::openMode + FileBrowserComponent::canSelectFiles,
                         [fc, &engine, callback = std::move (fileChosenCallback)] (const FileChooser&)
                         {
                             const auto f = fc->getResult();

                             if (f.existsAsFile())
                                 engine.getPropertyStorage().setDefaultLoadSaveDirectory ("pitchAndTimeExample", f.getParentDirectory());

                             callback (f);
                         });
    }

    static void removeAllClips (tracktion::AudioTrack& track)
    {
        auto clips = track.getClips();

        for (int i = clips.size(); --i >= 0;)
            clips.getUnchecked (i)->removeFromParent(); // Old: clips.getUnchecked (i)->removeFromParentTrack();
    }
    
    static tracktion::AudioTrack* getOrInsertAudioTrackAt (tracktion::Edit& edit, int index)
    {
        edit.ensureNumberOfAudioTracks (index + 1);
        return tracktion::getAudioTracks (edit)[index];
    }

    static tracktion::WaveAudioClip::Ptr loadAudioFileAsClip (tracktion::Edit& edit, const File& file)
    {
        // Find the first track and delete all clips from it
        if (auto track = getOrInsertAudioTrackAt (edit, 0))
        {
            removeAllClips (*track);

            // Add a new clip to this track
            tracktion::AudioFile audioFile (edit.engine, file);
            /*
            if (audioFile.isValid())
                if (auto newClip = track->insertWaveClip (file.getFileNameWithoutExtension(), file, { { 0.0, audioFile.getLength() }, 0.0 }, false))
                    return newClip;
            */
        }

        return {};
    }

    static void togglePlay (tracktion::Edit& edit)
    {
        auto& transport = edit.getTransport();

        if (transport.isPlaying())
            transport.stop (false, false);
        else
            transport.play (false);
    }
    
    static void toggleRecord (tracktion::Edit& edit)
    {
        auto& transport = edit.getTransport();
        
        if (transport.isRecording())
            transport.stop (true, false);
        else
            transport.record (false);
    }
    
    static void armTrack (tracktion::AudioTrack& t, bool arm, int position = 0)
    {
        /*
         @todo
        auto& edit = t.edit;
        for (auto instance : edit.getAllInputDevices())
            if (instance->isOnTargetTrack (t, position))
                instance->setRecordingEnabled (t, arm);
        */
    }
    
    static bool isTrackArmed (tracktion::AudioTrack& t, int position = 0)
    {
        //auto& edit = t.edit;
        /*
         @todo
        for (auto instance : edit.getAllInputDevices())
            if (instance->isOnTargetTrack (t, position))
                return instance->isRecordingEnabled (t.itemID);
         */
        return false;
    }
    
    static bool isInputMonitoringEnabled (tracktion::AudioTrack& t, int position = 0)
    {
        /*
         @todo
        auto& edit = t.edit;
        for (auto instance : edit.getAllInputDevices())
            if (instance->isOnTargetTrack (t, position))
                return instance->getInputDevice().isEndToEndEnabled();
        */
        return false;
    }
    
    static void enableInputMonitoring (tracktion::AudioTrack& t, bool im, int position = 0)
    {
        /*
         @todo
        if (isInputMonitoringEnabled (t, position) != im)
        {
            auto& edit = t.edit;
            for (auto instance : edit.getAllInputDevices())
                if (instance->isOnTargetTrack (t, position))
                    instance->getInputDevice().flipEndToEnd();
        }
         */
    }
    
    static bool trackHasInput (tracktion::AudioTrack& t, int position = 0)
    {
        /*
         @todo
         
        auto& edit = t.edit;
        for (auto instance : edit.getAllInputDevices())
            if (instance->isOnTargetTrack (t, position))
                return true;
        */
        return false;
    }

    static inline std::unique_ptr<juce::KnownPluginList::PluginTree> createPluginTree (tracktion::Engine& engine)
    {
        auto& list = engine.getPluginManager().knownPluginList;

        if (auto tree = list.createTree (list.getTypes(), KnownPluginList::sortByManufacturer))
            return tree;

        return {};
    }



}


//--------------------------------------------------------

namespace TestHelpers
{

    /** Writes an audio buffer to a file. */
    template<typename AudioFormatType>
    static void writeToFile (juce::File file, choc::buffer::ChannelArrayView<float> block, double sampleRate, int qualityOptionIndex)
    {
        AudioFormatType type;

        if (auto writer = std::unique_ptr<juce::AudioFormatWriter> (type.createWriterFor (file.createOutputStream().release(),
                                                                                          sampleRate,
                                                                                          block.getNumChannels(),
                                                                                          16, {}, qualityOptionIndex)))
        {
            writer->writeFromAudioSampleBuffer (tracktion::toAudioBuffer (block), 0, (int) block.getNumFrames());
        }
    }

    
    static inline auto createSineBuffer (int numChannels, int numFrames, double phaseIncrement)
    {
        return choc::buffer::createChannelArrayBuffer (numChannels, numFrames,
                                                       [=] (auto, auto frame) { return std::sin ((float) (frame * phaseIncrement)); });
    }

    static inline auto createSquareBuffer (int numChannels, int numFrames, double phaseIncrement)
    {
        return choc::buffer::createChannelArrayBuffer (numChannels, numFrames,
                                                       [=] (auto, auto frame)
                                                       {
                                                            const auto sinValue = std::sin ((float) (frame * phaseIncrement));
                                                            
                                                            if (sinValue > 0.0f) return 1.0f;
                                                            if (sinValue < 0.0f) return -1.0f;
                                                            
                                                            return 0.0f;
                                                        });
        
    }


    static inline float getPhaseIncrement (float frequency, double sampleRate)
    {
        return (float) ((frequency * juce::MathConstants<double>::pi * 2.0) / sampleRate);
    }
        

    template<typename AudioFormatType>
    std::unique_ptr<juce::TemporaryFile> getSquareFile (double sampleRate, double durationInSeconds,
                                                        int numChannels = 1, float frequency = 220.0f,
                                                        int qualityOptionIndex = -1)
    {
        auto buffer = createSquareBuffer (numChannels, (int) (sampleRate * durationInSeconds),
                                          getPhaseIncrement (frequency, sampleRate));
        
        AudioFormatType format;
        auto f = std::make_unique<juce::TemporaryFile> (format.getFileExtensions()[0]);
        writeToFile<AudioFormatType> (f->getFile(), buffer, sampleRate, qualityOptionIndex);
        return f;
    }

}

//--------------------------------------------------------


namespace ClipHelpers
{

    template<typename ClipType>
    typename ClipType::Ptr loopAroundClip (ClipType& clip)
    {
        auto& transport = clip.edit.getTransport();
        transport.setLoopRange (clip.getEditTimeRange());
        transport.looping = true;
        transport.setPosition (0s);
        transport.play (false);

        return clip;
    }

    static tracktion::StepClip* createStepClip(tracktion::AudioTrack* audioTrack, int newBarNum = 1)
    {
        // tracktion::StepClip::Ptr
        if(audioTrack)
        {
            const tracktion::TimeRange editTimeRange (0s, audioTrack->edit.tempoSequence.toTime ({ newBarNum,tracktion::BeatDuration() }));
            if(auto clip = audioTrack->insertNewClip (tracktion::TrackItem::Type::step, "StepClip", editTimeRange, nullptr))
            {
                auto stepClip = static_cast<tracktion::StepClip*>(clip);
                
                stepClip->setLoopRange(editTimeRange);

                return stepClip;
            }
        }
        return {};
    }

    static juce::ReferenceCountedObjectPtr<tracktion::WaveAudioClip> createWaveAudioClip(tracktion::AudioTrack* audioTrack)
    {
        auto squareFile = TestHelpers::getSquareFile<juce::WavAudioFormat> (44100.0, 1.0, 1, 220.0f);
        auto cc = dynamic_cast<tracktion::ContainerClip*> (insertNewClip (*audioTrack, tracktion::TrackItem::Type::container, { 0_tp, 5_tp }));
        auto clip1 = insertWaveClip (*cc, {}, squareFile->getFile(), {{ 1_tp, 2_tp }}, tracktion::DeleteExistingClips::no);
        clip1->setUsesProxy (false);
        clip1->setAutoTempo (true);
        
        return clip1;
    }



} // namespace ClipHelpers


//--------------------------------------------------------


namespace PluginHelpers
{

    static void setupInputs (tracktion::Edit& edit, tracktion::AudioTrack* audioTrack1, tracktion::AudioTrack* audioTrack2, tracktion::AudioTrack* audioTrack3, tracktion::AudioTrack* audioTrack4)
    {
        auto& dm = edit.engine.getDeviceManager();

        for (int i = 0; i < dm.getNumMidiInDevices(); i++)
        {
            auto dev = dm.getMidiInDevice (i);
            dev->setEnabled (true);
            dev->setMonitorMode (tracktion::InputDevice::MonitorMode::automatic);
            dev->recordingEnabled = true;
        }

        edit.playInStopEnabled = true;
        edit.getTransport().ensureContextAllocated (true);

        // Add the midi input to track 1
        if (auto t = audioTrack1)
            if (auto dev = dm.getMidiInDevice (0))
                for (auto instance : edit.getAllInputDevices())
                    if (&instance->getInputDevice() == dev)
                        if (auto destination = instance->setTarget (t->itemID, true, &edit.getUndoManager(), 0))
                            (*destination)->recordEnabled = true;

        // Also add the same midi input to track 2
        if (auto t = audioTrack2)
            if (auto dev = dm.getMidiInDevice (0))
                for (auto instance : edit.getAllInputDevices())
                    if (&instance->getInputDevice() == dev)
                        [[ maybe_unused ]] auto res = instance->setTarget (t->itemID, false, &edit.getUndoManager(), 0);
        
        // Also add the same midi input to track 2
        if (auto t = audioTrack3)
            if (auto dev = dm.getMidiInDevice (0))
                for (auto instance : edit.getAllInputDevices())
                    if (&instance->getInputDevice() == dev)
                        [[ maybe_unused ]] auto res = instance->setTarget (t->itemID, false, &edit.getUndoManager(), 0);
        
        // Also add the same midi input to track 2
        if (auto t = audioTrack4)
            if (auto dev = dm.getMidiInDevice (0))
                for (auto instance : edit.getAllInputDevices())
                    if (&instance->getInputDevice() == dev)
                        [[ maybe_unused ]] auto res = instance->setTarget (t->itemID, false, &edit.getUndoManager(), 0);

        edit.restartPlayback();
    }



    static void setupOutputs (tracktion::Edit& edit, tracktion::AudioTrack* audioTrack1, tracktion::AudioTrack* audioTrack2, tracktion::AudioTrack* audioTrack3, tracktion::AudioTrack* audioTrack4)
    {
        auto& dm = edit.engine.getDeviceManager();

        for (int i = 0; i < dm.getNumMidiOutDevices(); i++)
        {
            auto dev = dm.getMidiOutDevice (i);
            dev->setEnabled (true);
        }

        edit.playInStopEnabled = true;
        edit.getTransport().ensureContextAllocated (true);

        // Set track to send to midi output
        if (auto t = audioTrack1)
        {
            auto& output = t->getOutput();
            output.setOutputToDefaultDevice (true);
        }
        
        if (auto t = audioTrack2)
        {
            auto& output = t->getOutput();
            output.setOutputToDefaultDevice (true);
        }
        
        if (auto t = audioTrack3)
        {
            auto& output = t->getOutput();
            output.setOutputToDefaultDevice (true);
        }
        
        if (auto t = audioTrack4)
        {
            auto& output = t->getOutput();
            output.setOutputToDefaultDevice (true);
        }

        edit.restartPlayback();
    }



    static Array<File> createSampleFiles(tracktion::Edit* edit)
    {
        Array<File> files;
        const auto destDir = edit->getTempDirectory (true);
        jassert (destDir != File());
        
        using namespace DemoBinaryData;
        
        for (int i = 0; i < namedResourceListSize; ++i)
        {
            const auto f = destDir.getChildFile (originalFilenames[i]);
            
            int dataSizeInBytes = 0;
            const char* data = getNamedResource (namedResourceList[i], dataSizeInBytes);
            jassert (data != nullptr);
            f.replaceWithData (data, (size_t) dataSizeInBytes);
            files.add (f);
        }
        
        return files;
    }

    static void createSamplerPlugin (tracktion::Edit* edit, tracktion::StepClip* stepClip, Array<File> defaultSampleFiles)
    {
        if (stepClip)
        {
            if (auto sampler = dynamic_cast<tracktion::SamplerPlugin*> (edit->getPluginCache().createNewPlugin (tracktion::SamplerPlugin::xmlTypeName, {}).get()))
            {
                stepClip->getTrack()->pluginList.insertPlugin (*sampler, 0, nullptr);
                
                int channelCount = 0;
                
                for (auto channel : stepClip->getChannels())
                {
                    const auto error = sampler->addSound (defaultSampleFiles[channelCount++].getFullPathName(), channel->name.get(), 0.0, 0.0, 1.0f);
                    sampler->setSoundParams (sampler->getNumSounds() - 1, channel->noteNumber, channel->noteNumber, channel->noteNumber);
                    jassert (error.isEmpty());
                    
                    for (auto& pattern : stepClip->getPatterns())
                        pattern.randomiseChannel (channel->getIndex());
                }
            }
        }
        else
        {
            jassertfalse; // StepClip not been created yet?
        }
    }

    static void createSynthPlugin (tracktion::Edit* edit, tracktion::StepClip* stepClip)
    {
        String organPatch = "<PLUGIN type=\"4osc\" windowLocked=\"1\" id=\"1069\" enabled=\"1\" filterType=\"1\" presetDirty=\"0\" presetName=\"4OSC: Organ\" filterFreq=\"127.00000000000000000000\" ampAttack=\"0.60000002384185791016\" ampDecay=\"10.00000000000000000000\" ampSustain=\"100.00000000000000000000\" ampRelease=\"0.40000000596046447754\" waveShape1=\"4\" tune2=\"-24.00000000000000000000\" waveShape2=\"4\"> <MACROPARAMETERS id=\"1069\"/> <MODIFIERASSIGNMENTS/> <MODMATRIX/> </PLUGIN>";

        if (stepClip)
        {
            if (auto synth = dynamic_cast<tracktion::FourOscPlugin*> (edit->getPluginCache().createNewPlugin (tracktion::FourOscPlugin::xmlTypeName, {}).get()))
            {
                auto vt = ValueTree::fromXml (organPatch);

                if (vt.isValid())
                    synth->restorePluginStateFromValueTree (vt);
                else
                    jassertfalse; // Synth not been created yet?
                
                stepClip->getTrack()->pluginList.insertPlugin (*synth, 0, nullptr);
            }
            else
            {
                jassertfalse; // Synth not been created yet?
            }
        }
        else
        {
            jassertfalse; // StepClip not been created yet?
        }
    }

} // namespace PluginHelpers



