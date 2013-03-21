/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

//
//  MainComponent.cpp
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

#include "MainComponent.h"
#include "BinaryData.h"

//Dimensions for centre text box
#define BOX_X getWidth()/10
#define BOX_Y getHeight()/6
#define BOX_W (getWidth()/10)*8
#define BOX_H (getHeight()/6)*3


//==============================================================================
MainContentComponent::MainContentComponent () : Thread ("installerThread")
{
    //load binary data into Image
    backgroundImage = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    
    addAndMakeVisible (infoLabel = new Label());
    infoLabel->setJustificationType(Justification::centred);
    infoLabel->setText(translate("This application will install the AlphaLive Audio Library and Demo Project onto your computer. Please make sure that you have installed AlphaLive before running this installer. Press 'Install' to begin."), 
                       dontSendNotification);
    
    addAndMakeVisible (installButton = new TextButton(translate("Install")));
    installButton->addListener(this);
    installButton->setColour(TextButton::buttonColourId, AlphaColours::verydarkgrey.withAlpha(0.7f));
    
    addAndMakeVisible (closeButton = new TextButton(translate("Close")));
    closeButton->addListener(this);
    closeButton->setColour(TextButton::buttonColourId, AlphaColours::verydarkgrey.withAlpha(0.7f));
    
    addChildComponent (cancelButton = new TextButton(translate("Cancel")));
    cancelButton->addListener(this);
    cancelButton->setColour(TextButton::buttonColourId, AlphaColours::verydarkgrey.withAlpha(0.7f));
    
    progress = 0;
    addChildComponent (progressBar = new ProgressBar (progress));
    progressBar->setColour (ProgressBar::backgroundColourId, AlphaColours::verydarkgrey.withAlpha(1.0f));
    progressBar->setColour (ProgressBar::foregroundColourId, Colours::grey);
    
    setSize (backgroundImage.getWidth(), backgroundImage.getHeight());
    
    //Look-and-feel stuff
    LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);
    LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(true);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.drawImage(backgroundImage, 0, 0, getWidth(), getHeight(), 0, 0, backgroundImage.getWidth(), backgroundImage.getHeight());
    
    g.setColour(Colours::darkgrey.withAlpha(0.7f));
    g.fillRoundedRectangle(BOX_X, BOX_Y, BOX_W, BOX_H, 10);
    
    g.setColour(Colours::grey.withAlpha(0.6f));
    int border = 5;
    g.fillRoundedRectangle(BOX_X+border, 
                           BOX_Y+border, 
                           BOX_W-(border*2), 
                           BOX_H-(border*2), 
                           10);
}

void MainContentComponent::resized()
{
    infoLabel->setBounds(BOX_X, BOX_Y, BOX_W, BOX_H);
    installButton->setBounds((BOX_X + BOX_W) - 105,
                            (BOX_Y + BOX_H) - 55,
                            45,
                            45);
    closeButton->setBounds((BOX_X + BOX_W) - 55,
                           (BOX_Y + BOX_H) - 55,
                           45,
                           45);
    cancelButton->setBounds((BOX_X + BOX_W) - 55,
                           (BOX_Y + BOX_H) - 55,
                           45,
                           45);
    
    progressBar->setBounds((getWidth()/2)-75, (BOX_Y + BOX_H) - 40, 150, 20);
    
}

void MainContentComponent::buttonClicked (Button *button)
{
    if (button == installButton)
    {
        // === Locate the AlphaLive directory ===
        
        alphaLiveDirectory = File::getSpecialLocation (File::globalApplicationsDirectory).getFullPathName() +
                                 File::separatorString +
                                 "AlphaLive";
        
        bool continueInstallation = true;
        
        if (! alphaLiveDirectory.isDirectory())
        {
            //If can't be found, ask user to manually locate it
            
            bool userSelection = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, 
                                                              translate("AlphaLive not found!"), 
                                                              translate("The AlphaLive folder can not be found on your computer. Please manually locate it."));
            if (userSelection == true)
            {
                FileChooser myChooser (translate("Please select the AlphaLive directory..."),
                                       File::getSpecialLocation (File::globalApplicationsDirectory));
                
                if (myChooser.browseForDirectory() == true)
                {
                    // Perform some basic check to see if this is the AlphaLive directory
                    
                    #if JUCE_MAC || JUCE_LINUX
                    File alphaLiveApp (myChooser.getResult().getFullPathName() + File::separatorString + "AlphaLive.app");
                    #endif
                    #if JUCE_WINDOWS
                    File alphaLiveApp (myChooser.getResult().getFullPathName() + File::separatorString + "AlphaLive.exe");
                    #endif
                    
                    if (alphaLiveApp.exists())
                    {
                        // Presume that this is the AlphaLive directory
                        alphaLiveDirectory = myChooser.getResult();
                    }
                    else
                    {
                        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                                          translate("Error!"),
                                                          translate("The selected directory does not contain the AlphaLive application. Please try again."));
                        continueInstallation = false;
                    }
                }
                else
                    continueInstallation = false;
            }
            else
                continueInstallation = false;
        }
        
        
        if (continueInstallation == true)
        {
            startThread();
        }
    }
    
    else if (button == closeButton)
    {
        JUCEApplication::quit();
    }
    
    else if (button == cancelButton)
    {
        bool userSelection = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, 
                                                          translate("Are you sure?"), 
                                                          translate("Are you sure you want to cancel? The audio library will not be fully installed if so."));
        if (userSelection == true)
        {
            signalThreadShouldExit();
        }
    }
}

void MainContentComponent::run()
{
    // Do the following in a seperate thread so the GUI can be updated correctly.
    
    {
        const MessageManagerLock mmLock;
        closeButton->setVisible(false);
        installButton->setVisible(false);
        cancelButton->setVisible(true);
        
        progressBar->setVisible(true);
    }
    
    bool installationCancelled = false;
    
    while (! threadShouldExit())
    {
        // === Copy the necessary files ===
        
        File audioLibraryDir (alphaLiveDirectory.getFullPathName() + 
                              File::separatorString +
                              "Library" +
                              File::separatorString +
                              "Audio Library");
        
        if (! audioLibraryDir.isDirectory())
        {
            audioLibraryDir.createDirectory();
        }
        
        File dataToCopy (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getParentDirectory().getFullPathName() + File::separatorString + "Data");
        
        if (dataToCopy.exists())
        {
            int totalSize = 0;
            int extractedSize = 0;
            
            // Work out the total size of the files being copied,
            // so we can work out a percentage for the progress bar.
            Array<File> allFiles;
            dataToCopy.findChildFiles(allFiles, 3, true);
            for (int i = 0; i < allFiles.size(); i++)
                totalSize += allFiles[i].getSize();
            allFiles.clear();
            
            // With the audio library, don't just copy the entire directories (like done below with the Demo Project directory),
            // incase the user has already installed it and added their own files. Simply copying the directory would replace
            // any new files, so must copy each file individually.
            
            Array<File> filesToCopy;
            File audioDir (dataToCopy.getFullPathName() + File::separatorString + "Audio Library");
            audioDir.findChildFiles(filesToCopy, 2, true);
            
            for (int i = 0; i < filesToCopy.size(); i++)
            {
                // Increase the extracted size to we can work out the current progress bar value
                extractedSize += filesToCopy[i].getSize();
                // Get progress to a value between 0 and 1 (NOT 0 and 100) to update the progress bar correctly
                progress = ((extractedSize * 0.0001)/(totalSize * 0.0001));
                
                File newFile (audioLibraryDir.getFullPathName() + 
                              File::separatorString + 
                              filesToCopy[i].getRelativePathFrom(audioDir));
                
                // Only copy the file if it doesn't already exist
                if (! newFile.exists())
                {
                    {
                        const MessageManagerLock mmLock;
                        String string (translate("Extracting files...") + "\n" + newFile.getFileName());
                        infoLabel->setText(string, dontSendNotification);
                    }
                    
                    // If the files parent directory doesn't exist, create it, otherwise the file won't copy.
                    
                    bool doesParentExist = newFile.getParentDirectory().isDirectory();
                    if (doesParentExist == false)
                    {
                        File parentDir (newFile.getParentDirectory());
                        parentDir.createDirectory();
                    }
                    
                    if (threadShouldExit() == true)
                    {
                        installationCancelled = true;
                        break;
                    }
                    
                    filesToCopy[i].copyFileTo(newFile);
                    
                }
            }
            
            if (threadShouldExit() == true)
            {
                installationCancelled = true;
                break;
            }
            
            File demoProjDir (dataToCopy.getFullPathName() + File::separatorString + "Demo Project");
            File newDemoProjDir (alphaLiveDirectory.getFullPathName() + File::separatorString + "Demo Project");
            
            // Only copy the Demo Project if it doesn't already exist, as if the user already has it they
            // may have edited it, and copying it would overwrite their changes.
            // Should I be copy the files individually like done with the Audio Library above?
            
            if (File (newDemoProjDir.getFullPathName() + File::separatorString + "AlphaPresets.alphalive").exists() == false)
            {
                {
                    const MessageManagerLock mmLock;
                    infoLabel->setText(translate("Extracting Demo Project files..."), dontSendNotification);
                }
                
                demoProjDir.copyDirectoryTo(newDemoProjDir);
            }
            
            
            demoProjDir.findChildFiles(allFiles, 3, true);
            for (int i = 0; i < allFiles.size(); i++)
            {
                // Increase the extracted size to we can work out the current progress bar value
                extractedSize += allFiles[i].getSize();
                // Get progress to a value between 0 and 1 (NOT 0 and 100) to update the progress bar correctly
                progress = (extractedSize * 0.0001)/(totalSize * 0.0001);
                //sleep the thread here so the progress bar GUI is updated in a noticable manner
                wait(2);
            }
            
            
            
        }
        else
        {
            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              translate("Error!"),
                                              translate("The files that need installing appear to be missing. If this installer was supplied to you on a CD, please run it directly from the CD. If you downloaded this installer, please run it directly from the downloaded folder."));
        }
        
        
        signalThreadShouldExit();
    }
    
    if (installationCancelled == true)
    {
        const MessageManagerLock mmLock;
        infoLabel->setText(translate("Installation cancelled!"), dontSendNotification);
        
        closeButton->setVisible(true);
        installButton->setVisible(true);
        cancelButton->setVisible(false);
        progressBar->setVisible(false);
    }
    else
    {
        const MessageManagerLock mmLock;
        infoLabel->setText(translate("Installation complete!"), dontSendNotification);
        
        closeButton->setVisible(true);
        installButton->setVisible(false);
        cancelButton->setVisible(false);
        progressBar->setVisible(false);
        
    }
    
}
