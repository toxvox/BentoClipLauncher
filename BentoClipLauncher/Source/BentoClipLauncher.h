/*
 ==============================================================================
 
 BentoClipLauncher.h
 Created: 03 March 2024 10:58:42am
 Author:  Halil Kleinmann
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

#include "common/Utilities.h"
//#include "common/BinaryData.h"

using namespace std::literals;
using namespace tracktion::literals;

namespace bento
{


//==============================================================================
struct ClipLauncher   : public juce::Component
{
    //==============================================================================
    ClipLauncher (std::shared_ptr<tracktion::Edit> edit_)
    {
        edit = edit_;
        
        addAndMakeVisible(playPauseButton);
        addAndMakeVisible(clipLaunchButton1);
        addAndMakeVisible(clipLaunchButton2);
        addAndMakeVisible(clipLaunchButton3);
        addAndMakeVisible(clipLaunchButton4);
        
        //setup();
        
        playPauseButton.onClick = [this] { EngineHelpers::togglePlay (*edit); };
        

        audioTrack1 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 0);
        audioTrack2 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 1);
        audioTrack3 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 2);
        audioTrack4 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 3);
                
        edit->getSceneList().insertScene(0);
        edit->getSceneList().insertScene(1);
        edit->getSceneList().insertScene(2);
        edit->getSceneList().insertScene(3);

        stepClip1 = ClipHelpers::createStepClip(audioTrack1);
        stepClip2 = ClipHelpers::createStepClip(audioTrack1);
        stepClip3 = ClipHelpers::createStepClip(audioTrack1);
        stepClip4 = ClipHelpers::createStepClip(audioTrack1);
        
        audioClip1 = ClipHelpers::createWaveAudioClip(audioTrack1);
        
        
        /*
        for ( auto clipSlot : audioTrack1->getClipSlotList().getClipSlots() )
        {
            
            if( clipSlot )
            {
                DBG("SlotList - " + clipSlot->getName());
                //clipSlot->setClip(stepClip1);
                clipSlot->setClip(audioClip1);
            }
            else
            {
                DBG("No SlotList");
            }
        }
        */
        
        audioTrack1->getClipSlotList().getClipSlots()[0]->setClip(audioClip1.get());
        audioTrack1->getClipSlotList().getClipSlots()[0]->getClip()->getLaunchHandle()->play( tracktion::MonotonicBeat({}) );
        
        
        /*
        
        for( auto clipSlot : audioTrack1->getClipSlotList().getClipSlots() )
        {
            if(clipSlot)
            {
                for( auto clip : clipSlot->getClips() )
                {
                    if(clip)
                        clip->getLaunchHandle()->play({});
                    else
                        DBG("No clip");
                }
            }
            else
            {
                DBG("No clipSlot");
            }
        }
        
        */
        

        /*
                
        audioTrack1->getClipSlotList().getClipSlots()[0]->setClip(stepClip1);
        audioTrack1->getClipSlotList().getClipSlots()[1]->setClip(stepClip2);
        audioTrack1->getClipSlotList().getClipSlots()[2]->setClip(stepClip3);
        audioTrack1->getClipSlotList().getClipSlots()[3]->setClip(stepClip4);
        
        */
        
        //auto defaultSampleFile = PluginHelpers::createSampleFiles(edit.get());
        //PluginHelpers::createSamplerPlugin(edit.get(), stepClip1, defaultSampleFile);
        
        
        //stepClip1->setLoopRangeBeats( { 0_bp, 1_bd } );
        

        
        //PluginHelpers::setupInputs(*edit, audioTrack1, audioTrack2, audioTrack3, audioTrack4);
        //PluginHelpers::setupOutputs(*edit, audioTrack1, audioTrack2, audioTrack3, audioTrack4);

        //PluginHelpers::createSynthPlugin(edit.get(), stepClip1);
        //PluginHelpers::createSamplerPlugin(edit.get(), stepClip3, defaultSampleFile);
        //PluginHelpers::createSamplerPlugin(edit.get(), stepClip4, defaultSampleFile);
        
        //clipSlot_A1->setClip(stepClip1);
        //clipSlot_A2->setClip(stepClip2);
        //clipSlot_A3->setClip(stepClip3);
        //clipSlot_A4->setClip(stepClip4);
  
        //stepClip1->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        //stepClip2->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 4_bp } ) );
        //stepClip3->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 8_bp } ) );
        //stepClip4->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 12_bp } ) );
        

        clipLaunchButton1.onClick = [this]
        {
            for( auto clipSlot : audioTrack1->getClipSlotList().getClipSlots() )
            {
                if(clipSlot)
                {
                    if(auto clip = clipSlot->getClip())
                    {
                        if(auto launchHandle = clip->getLaunchHandle())
                        {
                            launchHandle->play( tracktion::MonotonicBeat(  { } ) );
                        }
                        else
                        {
                            DBG("No launchHandle");
                        }
                    }
                    else
                    {
                        DBG("No clip");
                    }
                }
                else
                {
                    DBG("No clipSlot");
                }
            }
            
            //auto launchHandle = clip1->getLaunchHandle();
            //launchHandle->play( tracktion::MonotonicBeat(  { 0_bp } ) );
            //audioTrack1->getClipSlotList().getClipSlots()[0]->getClip()->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
            //stepClip1->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        };
        
        clipLaunchButton2.onClick = [this]
        {
            for( auto clipSlot : audioTrack1->getClipSlotList().getClipSlots() )
            {
                if(clipSlot)
                {
                    for( auto clip : clipSlot->getClips() )
                    {
                        if(clip)
                            clip->getLaunchHandle()->play({});
                        else
                            DBG("No clip");
                    }
                }
                else
                {
                    DBG("No clipSlot");
                }
            }
            
            //stepClip2->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        };
        
        clipLaunchButton3.onClick = [this]
        {
            stepClip3->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        };
        
        clipLaunchButton4.onClick = [this]
        {
            stepClip4->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        };
        
        
        
    }
    
    ~ClipLauncher()
    {
    }
    
    void paint (Graphics&) override
    {
    }
    
    void resized() override
    {
        auto r = getLocalBounds();
        
        r.setTop(60);
        
        auto topR = r.removeFromTop (60);

        playPauseButton.setBounds (topR.removeFromLeft (getWidth()/4).reduced (2));
        
        topR = r.removeFromTop (60);
        clipLaunchButton1.setBounds (topR.removeFromLeft (getWidth()/4).reduced (2));
        clipLaunchButton2.setBounds (topR.removeFromLeft (getWidth()/4).reduced (2));
        clipLaunchButton3.setBounds (topR.removeFromLeft (getWidth()/4).reduced (2));
        clipLaunchButton4.setBounds (topR.removeFromLeft (getWidth()/4).reduced (2));
    }
    
    
private:
    std::shared_ptr<tracktion::Edit> edit;
    
    // tracktion::Engine& engine;
    
    tracktion::ExternalControllerManager* controllerManager;
    
    TextButton playPauseButton { "Play" };
    TextButton clipLaunchButton1 { "Clip 1" }, clipLaunchButton2 { "Clip 2" }, clipLaunchButton3 { "Clip 3" }, clipLaunchButton4 { "Clip 4" };
    
    //tracktion::LaunchHandle launchHandle;

    
    tracktion::AudioTrack* audioTrack1;
    tracktion::AudioTrack* audioTrack2;
    tracktion::AudioTrack* audioTrack3;
    tracktion::AudioTrack* audioTrack4;
    
    //tracktion::ClipSlotList clipSlotList1;
    /*
    tracktion::ClipSlot* clipSlot_A1;
    tracktion::ClipSlot* clipSlot_A2;
    tracktion::ClipSlot* clipSlot_A3;
    tracktion::ClipSlot* clipSlot_A4;
    
    
    tracktion::ClipSlot* clipSlot2;
    tracktion::ClipSlot* clipSlot3;
    tracktion::ClipSlot* clipSlot4;
    */
     
     
    tracktion::StepClip* stepClip1;
    tracktion::StepClip* stepClip2;
    tracktion::StepClip* stepClip3;
    tracktion::StepClip* stepClip4;
    
    juce::ReferenceCountedObjectPtr<tracktion::WaveAudioClip> audioClip1;
    
    
    
    void setup()
    {
        
        
        playPauseButton.onClick = [this] { EngineHelpers::togglePlay (*edit); };
        

        audioTrack1 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 0);
        audioTrack2 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 1);
        audioTrack3 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 2);
        audioTrack4 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 3);
        
        
        edit->getSceneList().ensureNumberOfScenes(4);
        
        /*

        clipSlot_A1 = audioTrack1->getClipSlotList().insertSlot(0);
        clipSlot_A2 = audioTrack1->getClipSlotList().insertSlot(1);
        clipSlot_A3 = audioTrack1->getClipSlotList().insertSlot(2);
        clipSlot_A4 = audioTrack1->getClipSlotList().insertSlot(3);
         
         */
        
        //clipSlot2 = audioTrack2->getClipSlotList().insertSlot(0);
        //clipSlot3 = audioTrack3->getClipSlotList().insertSlot(0);
        //clipSlot4 = audioTrack4->getClipSlotList().insertSlot(0);
        
        stepClip1 = ClipHelpers::createStepClip(audioTrack1);
        stepClip2 = ClipHelpers::createStepClip(audioTrack1);
        stepClip3 = ClipHelpers::createStepClip(audioTrack1);
        stepClip4 = ClipHelpers::createStepClip(audioTrack1);
        
        
        audioTrack1->getClipSlotList().getClipSlots()[0]->setClip(stepClip1);
        audioTrack1->getClipSlotList().getClipSlots()[1]->setClip(stepClip2);
        audioTrack1->getClipSlotList().getClipSlots()[2]->setClip(stepClip3);
        audioTrack1->getClipSlotList().getClipSlots()[3]->setClip(stepClip4);
        
        
        //stepClip1->setLoopRangeBeats( { 0_bp, 1_bd } );
        

        
        //PluginHelpers::setupInputs(*edit, audioTrack1, audioTrack2, audioTrack3, audioTrack4);
        
        //PluginHelpers::setupOutputs(*edit, audioTrack1, audioTrack2, audioTrack3, audioTrack4);
        
        auto defaultSampleFile = PluginHelpers::createSampleFiles(edit.get());
        PluginHelpers::createSamplerPlugin(edit.get(), stepClip1, defaultSampleFile);
        //PluginHelpers::createSynthPlugin(edit.get(), stepClip1);
        //PluginHelpers::createSamplerPlugin(edit.get(), stepClip3, defaultSampleFile);
        //PluginHelpers::createSamplerPlugin(edit.get(), stepClip4, defaultSampleFile);
        
        //clipSlot_A1->setClip(stepClip1);
        //clipSlot_A2->setClip(stepClip2);
        //clipSlot_A3->setClip(stepClip3);
        //clipSlot_A4->setClip(stepClip4);
  
        //stepClip1->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 0_bp } ) );
        //stepClip2->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 4_bp } ) );
        //stepClip3->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 8_bp } ) );
        //stepClip4->getLaunchHandle()->play( tracktion::MonotonicBeat(  { 12_bp } ) );


        
        
    }
    
};

}  // namespace bento

