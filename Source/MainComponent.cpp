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

#if JUCE_MAC || JUCE_LINUX
#include <sys/stat.h>
#endif

#if JUCE_WINDOWS
#include <Windows.h>
#endif

//Dimensions for centre text box
#define BOX_X getWidth()/10
#define BOX_Y getHeight()/6
#define BOX_W (getWidth()/10)*8
#define BOX_H (getHeight()/6)*3


//==============================================================================
MainContentComponent::MainContentComponent () : Thread ("installerThread")
{
    setLocalisation();
    
    //load binary data into Image
    backgroundImage = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    
    addAndMakeVisible (infoLabel = new Label());
    infoLabel->setJustificationType(Justification::centredTop);
    Font newFont(labelFontSize);
    infoLabel->setFont (newFont);
    
    String infoString = String();
    infoString += translate("IMPORTANT NOTICE:\n\n");
    infoString += translate("In order to install the AlphaLive Audio Library and demo projects you need to select the 'Data' folder from within the downloaded 'Audio Library Installer v1_1_0' folder after pressing the 'Install' button below.\n\n");
    infoString += translate("On pressing 'Install' you will see a file browser window - please navigate to the 'Audio Library Installer v1_1_0' folder that you have downloaded.\n\n");
    infoString += translate("Once you have selected the 'Data' folder, just press 'Open' on the bottom right of the file browser window and the Audio Library installation should run.");
    
    infoLabel->setText(infoString, dontSendNotification);
    
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
    int border = 5;
    infoLabel->setBounds(BOX_X, BOX_Y + border, BOX_W, BOX_H);
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
                            File::getSeparatorString() +
                            "AlphaLive";
        
        bool continueInstallation = true;
        
        if (! alphaLiveDirectory.isDirectory())
        {
            //cancel installation
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                        "AlphaLive not found!",
                                        "The AlphaLive folder can not be found on your computer. Please make sure that you have installed AlphaLive.");
            
            continueInstallation = false;
        
        } //if (! alphaLiveDirectory.isDirectory())
        
        if (continueInstallation && manuallyLocateDataDir)
        {
            FileChooser myChooser (translate("Please select the 'Data' directory from this Audio Library Installer package..."), File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getParentDirectory());
            
            if (myChooser.browseForDirectory() == true)
            {
                //check if the user has selected the correct directory....
                
                File audioLibDir (myChooser.getResult().getFullPathName() + File::getSeparatorString() + "Audio Library");
            
                if (myChooser.getResult().getFileName() == "Data" && audioLibDir.exists())
                {
                    // Presume that this is the Data directory
                    dataDirectory = myChooser.getResult();
                    continueInstallation = true;
                }
                else
                {
                    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                                      translate("Error!"),
                                                      translate("The selected directory is not the 'Data' directory from this Audio Library Installer package."));
                    continueInstallation = false;
                }
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
        // Get all needed file paths
        
        const File audioLibraryDir (alphaLiveDirectory.getFullPathName() + 
                              File::getSeparatorString() +
                              "Library" +
                              File::getSeparatorString() +
                              "Audio Library");
        if (! audioLibraryDir.isDirectory())
            audioLibraryDir.createDirectory();
        
        const File projectsDir (File::getSpecialLocation (File::userDocumentsDirectory).getFullPathName() + 
                          File::getSeparatorString() +
                          "AlphaLive Projects");
        if (! projectsDir.isDirectory())
            projectsDir.createDirectory();
        
        const File dataToCopy = dataDirectory;
        
        if (dataToCopy.exists())
        {
            // Work out the total size of the files being copied,
            // so we can work out a percentage for the progress bar.
            int totalSize = 0;
            int extractedSize = 0;
            
            Array<File> allFiles;
            dataToCopy.findChildFiles(allFiles, 3, true);
            for (int i = 0; i < allFiles.size(); i++)
                totalSize += allFiles[i].getSize();
            allFiles.clear();
            
            // For the audio library don't just copy the entire directories,
            // incase the user has already installed them and added their own files.
            // Simply copying the directories would replace
            // any new or modified files, so we must copy each file individually.
            
            //TO DO:
            //The following section contains 3 very similiar sub-sections, which each install a different part.
            //This could probably be consolidated into a single algorithm that runs 3 times, but has different
            //filepaths each time.
            //You would need the following File objects: contentToInstallDir and locationToInstallDir.
            
            //====================================================================================================
            //Install the audio library directory into the AlphaLive/Library directory
            
            Array<File> filesToCopy;
            File audioDir (dataToCopy.getFullPathName() + File::getSeparatorString() + "Audio Library");
            audioDir.findChildFiles(filesToCopy, 2, true);
            
            for (int i = 0; i < filesToCopy.size(); i++)
            {
                // Increase the extracted size to we can work out the current progress bar value
                extractedSize += filesToCopy[i].getSize();
                // Get progress to a value between 0 and 1 (NOT 0 and 100) to update the progress bar correctly
                progress = ((extractedSize * 0.0001)/(totalSize * 0.0001));
                
                File newFile (audioLibraryDir.getFullPathName() + 
                              File::getSeparatorString() +
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
            
            //====================================================================================================
            // Install the demo project into the AlphaLive Projects directory
            // NEW to version 1.0.2 - always replace the existing demo project
            // However still need to copy files one by one so that the progress bar
            // can update in realtime.
            
            filesToCopy.clear();
            File demoProjDir (dataToCopy.getFullPathName() + File::getSeparatorString() + "Demo Project");
            demoProjDir.findChildFiles(filesToCopy, 2, true);
            File newDemoProjDir (projectsDir.getFullPathName() + File::getSeparatorString() + "Demo Project");
            
            if (newDemoProjDir.exists())
                newDemoProjDir.deleteRecursively();
            
            for (int i = 0; i < filesToCopy.size(); i++)
            {
                // Increase the extracted size to we can work out the current progress bar value
                extractedSize += filesToCopy[i].getSize();
                // Get progress to a value between 0 and 1 (NOT 0 and 100) to update the progress bar correctly
                progress = ((extractedSize * 0.0001)/(totalSize * 0.0001));
                
                File newFile (newDemoProjDir.getFullPathName() + 
                              File::getSeparatorString() +
                              filesToCopy[i].getRelativePathFrom(demoProjDir));
                
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
        
            if (threadShouldExit() == true)
            {
                installationCancelled = true;
                break;
            }
            
            //====================================================================================================
            // Install the tutorial project into the AlphaLive Projects directory
            // NEW to version 1.0.2 - always replace the existing tutorial project.
            // However still need to copy files one by one so that the progress bar
            // can update in realtime.
            
            filesToCopy.clear();
            File tutorialProjDir (dataToCopy.getFullPathName() + File::getSeparatorString() + "Tutorial Project");
            tutorialProjDir.findChildFiles(filesToCopy, 2, true);
            File newTutorialProjDir (projectsDir.getFullPathName() + File::getSeparatorString() + "Tutorial Project");
            
            if (newTutorialProjDir.exists())
                newTutorialProjDir.deleteRecursively();
            
            for (int i = 0; i < filesToCopy.size(); i++)
            {
                // Increase the extracted size to we can work out the current progress bar value
                extractedSize += filesToCopy[i].getSize();
                // Get progress to a value between 0 and 1 (NOT 0 and 100) to update the progress bar correctly
                progress = ((extractedSize * 0.0001)/(totalSize * 0.0001));
                
                File newFile (newTutorialProjDir.getFullPathName() + 
                              File::getSeparatorString() +
                              filesToCopy[i].getRelativePathFrom(tutorialProjDir));
                
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
            
            //====================================================================================================
            //====================================================================================================
            //NEW - set the installed Demo and Tutorial project files to read-only.

			File demoProjFile (newDemoProjDir.getFullPathName() + File::getSeparatorString() + "Demo Project.alphalive");
            File tutorialProjFile (newTutorialProjDir.getFullPathName() + File::getSeparatorString() + "Tutorial Project.alphalive");
            
            #if JUCE_MAC || JUCE_LINUX
            
            //To set the file permissions to read only, you can use the command chmod 555 in terminal
            //or use th chmod() function as documented here:
            //http://www.manpagez.com/man/2/chmod/osx-10.4.php
            
            chmod (demoProjFile.getFullPathName().toUTF8(), S_IRUSR | S_IRGRP | S_IROTH);
            chmod (tutorialProjFile.getFullPathName().toUTF8(), S_IRUSR | S_IRGRP | S_IROTH);
            
            #endif
            
            #if JUCE_WINDOWS

			//Use the SetFileAttributes() function here like so:
			//http://msdn.microsoft.com/en-us/library/windows/desktop/aa365535(v=vs.85).aspx
			//to get more control over editting file permissions, you can use cacls

            SetFileAttributes(demoProjFile.getFullPathName().toUTF8(), FILE_ATTRIBUTE_READONLY);
			SetFileAttributes(tutorialProjFile.getFullPathName().toUTF8(), FILE_ATTRIBUTE_READONLY);

            #endif   
        }
        else
        {
            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              translate("Error!"),
                                              translate("The files that need installing appear to be missing. If this installer was supplied to you on a CD, please run it directly from the CD. If you downloaded this installer, please run it directly from the downloaded folder."));

			installationCancelled = true;
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
        LookAndFeel::getDefaultLookAndFeel().playAlertSound();
        
        closeButton->setVisible(true);
        installButton->setVisible(false);
        cancelButton->setVisible(false);
        progressBar->setVisible(false);
        
    }
    
}

void MainContentComponent::setLocalisation()
{
    static String countryCode = SystemStats::getDisplayLanguage();
    std::cout << "Language: " << countryCode << std::endl;
    
    File dataDir = dataDirectory;
    
    //We may need to find suitable fonst that exists on the current system
    //for languages such as Chinese, Japanese, and Korean.
    //http://en.wikipedia.org/wiki/List_of_CJK_fonts
    
    StringArray availableFonts = Font::findAllTypefaceNames();
    
    //countryCode will equal ISO 639-1 or ISO 639-2 codes as listed here:
    //http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
    
    if (countryCode == "ja" || countryCode == "jpn") //japanese
    {
        File transFile (dataDir.getFullPathName() + File::getSeparatorString() + "trans_ja");
        
        if (transFile.exists())
        {
            trans = new LocalisedStrings (transFile, false);
            LocalisedStrings::setCurrentMappings(trans);
            
            String fontToUse = "Arial Unicode MS"; // available on OSX 10.5 and above
            
            if (availableFonts.contains(fontToUse) == false)
            {
                fontToUse = "Meiryo"; // available on Windows Vista and above
                
                if (availableFonts.contains(fontToUse) == false)
                {
                    fontToUse = "MS PGothic"; // available on Windows XP
                    
                    //what about on Linux?
                }
            }
            
            lookAndFeel.setDefaultSansSerifTypefaceName(fontToUse);
            
            currentLanguage = "Japanese";
            labelFontSize = 14;
        }
        
    }
    else if (countryCode == "zh" || countryCode == "zho" || countryCode == "zh-Hant" || countryCode == "zh-Hans") //chinese. do i need the first two?
    {
        File transFile (dataDir.getFullPathName() + File::getSeparatorString() + "trans_zh");
        
        if (transFile.exists())
        {
            trans = new LocalisedStrings (transFile, false);
            LocalisedStrings::setCurrentMappings(trans);
            
            String fontToUse = "Arial Unicode MS"; // available on OSX 10.5 and above
            
            if (availableFonts.contains(fontToUse) == false)
            {
                fontToUse = "Microsoft JhengHei"; // available on Windows Vista and above
                
                if (availableFonts.contains(fontToUse) == false)
                {
                    fontToUse = "SimHei"; // available on Windows XP
                    
                    //do we neeed to consider simplified vs traditional?
                    //what about on Linux?
                }
            }
            
            lookAndFeel.setDefaultSansSerifTypefaceName(fontToUse);
            
            currentLanguage = "Chinese";
            labelFontSize = 14;
        }
    }
    else if (countryCode == "ko" || countryCode == "kor") //Korean
    {
        File transFile (dataDir.getFullPathName() + File::getSeparatorString() + "trans_ko");
        
        if (transFile.exists())
        {
            trans = new LocalisedStrings (transFile, false);
            LocalisedStrings::setCurrentMappings(trans);
            
            String fontToUse = "AppleMyungjo"; // available on OSX 10.5 and above
            
            if (availableFonts.contains(fontToUse) == false)
            {
                fontToUse = "Batang"; // available on Windows.. XP and above?
            }
            
            lookAndFeel.setDefaultSansSerifTypefaceName(fontToUse);
            
            currentLanguage = "Korean";
            labelFontSize = 14;
        }
    }
    else //english
    {
        LocalisedStrings::setCurrentMappings(0);
        
        currentLanguage = "English";
        labelFontSize = 12;
    }
    
}
