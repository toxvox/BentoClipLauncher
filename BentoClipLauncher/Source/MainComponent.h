#pragma once

#include <JuceHeader.h>
#include "BentoClipLauncher.h"


namespace bento
{

class EngineBehaviour : public tracktion::EngineBehaviour
{
public:
    //bool monitorAudioInputsWithoutRecordEnable() override { return true; }
};


class UIBehaviour : public tracktion::UIBehaviour
{
public:
    tracktion::Edit* getCurrentlyFocusedEdit() override { return edit; }
    tracktion::Edit* getLastFocusedEdit() override { return edit; }
    
    void setEdit(tracktion::Edit* edit_) { edit = edit_; }
    
private:
    tracktion::Edit* edit;
};



}

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    tracktion::Engine engine { "BentoClipLauncher", std::make_unique<bento::UIBehaviour>(), std::make_unique<bento::EngineBehaviour>() };
    
    std::shared_ptr<tracktion::Edit> edit;
    std::shared_ptr<bento::ClipLauncher> clipLauncher;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
