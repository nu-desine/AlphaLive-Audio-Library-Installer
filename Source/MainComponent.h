/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

//
//  MainComponent.h
//  Audio Library Installer
//
//  Created by Liam Meredith-Lacey on 12/03/2013.
//  Copyright 2013 nu desine.
//
//  This file is part of Audio Library Installer.
//
//  Audio Library Installer is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License, version 2, 
//  as published by the Free Software Foundation.
//  
//  Audio Library Installer is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __MAINCOMPONENT_H_B87CFFDB__
#define __MAINCOMPONENT_H_B87CFFDB__

#include "../JuceLibraryCode/JuceHeader.h"
#include "AlphaLiveLookandFeel.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :  public Component,
                                public Button::Listener,
                                public Thread
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
    
    void buttonClicked (Button *button);
    void run();
    
    void setLocalisation();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    
    AlphaLiveLookandFeel lookAndFeel;
    Image backgroundImage;
    
    ScopedPointer <Label> infoLabel;
    ScopedPointer <TextButton> installButton, closeButton, cancelButton;
    
    //alphaLiveDirectory is the directory where all the supporting app files are stored
    File alphaLiveDirectory;
    //dataDirectory is the 'Data' Folder containing all the files that need installing by this app
    File dataDirectory;
    
    ScopedPointer <ProgressBar> progressBar;
    double progress;
    
    LocalisedStrings *trans;
    String currentLanguage;
    
    //It appears that, on macOS, if running an app flagged as being from an 'unidentifed' developer,
    //the app won't happily access files / folders relative from File::currentApplicationFile, which
    //we would ideally like to do here to find the 'Data' directory containing all the files
    //that need installing with this app. Therefore, in case of this situation, we need to instead ask
    //the user to manually locate the 'Data' directory for this app to always successfully work.
    const bool manuallyLocateDataDir = true;
};


#endif  // __MAINCOMPONENT_H_B87CFFDB__
