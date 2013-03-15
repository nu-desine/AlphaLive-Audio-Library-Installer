/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "BinaryData.h"


//==============================================================================
MainContentComponent::MainContentComponent (JUCEApplication *juceApplication_) 
                                            :   Thread ("installerThread"),
                                                juceApplication (juceApplication_)
{
    //load binary data into Image
    backgroundImage = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    
    addAndMakeVisible (infoLabel = new Label());
    infoLabel->setJustificationType(Justification::centred);
    infoLabel->setText(translate("This application will install the AlphaLive Audio Library and Demo Project onto your computer. Please make sure that you have installed AlphaLive before running this installer. Press 'Install' to begin."), 
                       dontSendNotification);
    
    addAndMakeVisible (installButton = new TextButton(translate("Install")));
    installButton->addListener(this);
    
    addAndMakeVisible (closeButton = new TextButton(translate("Close")));
    closeButton->addListener(this);
    
    addChildComponent (cancelButton = new TextButton(translate("Cancel")));
    cancelButton->addListener(this);
    
    setSize (backgroundImage.getWidth(), backgroundImage.getHeight());
    LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(true);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.drawImage(backgroundImage, 0, 0, getWidth(), getHeight(), 0, 0, backgroundImage.getWidth(), backgroundImage.getHeight());
}

void MainContentComponent::resized()
{
    infoLabel->setBounds(0, 0, getWidth(), getHeight()-40);
    installButton->setBounds((getWidth()/2)-25, getHeight()-30, 50, 20);
    closeButton->setBounds((getWidth()/2)-75, getHeight()-30, 50, 20);
    cancelButton->setBounds((getWidth()/2)-75, getHeight()-30, 50, 20);
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
        juceApplication->quit();
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
            
            // With the audio library, don't just copy the entire directories (like done below with the Demo Project directory),
            // incase the user has already installed it and added their own files. Simply copying the directory would replace
            // any new files, so must copy each file individually.
            
            Array<File> filesToCopy;
            File audioDir (dataToCopy.getFullPathName() + File::separatorString + "Audio Library");
            audioDir.findChildFiles(filesToCopy, 2, true);
            
            for (int i = 0; i < filesToCopy.size(); i++)
            {           
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
    }
    else
    {
        const MessageManagerLock mmLock;
        infoLabel->setText(translate("Installation complete!"), dontSendNotification);
        
        closeButton->setVisible(true);
        installButton->setVisible(false);
        cancelButton->setVisible(false);
        
    }
    
}
