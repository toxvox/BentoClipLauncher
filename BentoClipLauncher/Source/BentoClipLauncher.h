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
        
        playPauseButton.onClick = [this] { EngineHelpers::togglePlay (*edit); };
        
        audioTrack1 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 0);
        //audioTrack2 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 1);
        //audioTrack3 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 2);
        //audioTrack4 = EngineHelpers::getOrInsertAudioTrackAt(*edit, 3);
                
        edit->getSceneList().insertScene(0);
        edit->getSceneList().insertScene(1);
        edit->getSceneList().insertScene(2);
        edit->getSceneList().insertScene(3);

        stepClip1 = ClipHelpers::createStepClip(audioTrack1);
        stepClip2 = ClipHelpers::createStepClip(audioTrack1);
        stepClip3 = ClipHelpers::createStepClip(audioTrack1);
        stepClip4 = ClipHelpers::createStepClip(audioTrack1);
        
        audioTrack1->getClipSlotList().getClipSlots()[0]->setClip(stepClip1);
        audioTrack1->getClipSlotList().getClipSlots()[1]->setClip(stepClip2);
        audioTrack1->getClipSlotList().getClipSlots()[2]->setClip(stepClip3);
        audioTrack1->getClipSlotList().getClipSlots()[3]->setClip(stepClip4);
        
        
        for( auto clipSlot : audioTrack1->getClipSlotList().getClipSlots() )
        {
            if(clipSlot)
            {
                for( auto clip : clipSlot->getClips() )
                {
                    if(clip)
                        clip->getLaunchHandle()->play( tracktion::MonotonicBeat({}) );
                    else
                        DBG("No clip");
                }
            }
            else
            {
                DBG("No clipSlot");
            }
        }
        


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

    tracktion::ExternalControllerManager* controllerManager;
    
    TextButton playPauseButton { "Play" };
    TextButton clipLaunchButton1 { "Clip 1" }, clipLaunchButton2 { "Clip 2" }, clipLaunchButton3 { "Clip 3" }, clipLaunchButton4 { "Clip 4" };
    
    tracktion::AudioTrack* audioTrack1;
    tracktion::AudioTrack* audioTrack2;
    tracktion::AudioTrack* audioTrack3;
    tracktion::AudioTrack* audioTrack4;
     
    tracktion::StepClip* stepClip1;
    tracktion::StepClip* stepClip2;
    tracktion::StepClip* stepClip3;
    tracktion::StepClip* stepClip4;
    
    juce::ReferenceCountedObjectPtr<tracktion::WaveAudioClip> audioClip1;
    
};

}  // namespace bento

