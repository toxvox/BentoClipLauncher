#include "MainComponent.h"
//#include "common/Utilities.h"

//==============================================================================
MainComponent::MainComponent()
{
    auto editFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile( "MySong.tracktionedit" );
    edit = tracktion::createEmptyEdit(engine, editFile);
    
    clipLauncher = std::make_unique<bento::ClipLauncher>(edit);
    clipLauncher->setSize(600, 400);
    addAndMakeVisible(clipLauncher.get());

    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Clip Launcher", getLocalBounds(), juce::Justification::top, true);
}

void MainComponent::resized()
{
    clipLauncher->setBounds(0, 0, 600, 400);
}
