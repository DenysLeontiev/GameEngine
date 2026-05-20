#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <windows.h>
#include <mmsystem.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "winmm.lib")

namespace fs = std::filesystem;

class AudioPlayer
{

public:
    static std::vector<fs::path> EnumerateWavFilesInDirectory(const fs::path &directory)
    {
        std::vector<fs::path> wavFiles;

        if (fs::exists(directory) && fs::is_directory(directory))
        {
            for (const auto &entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file() &&
                    entry.path().extension() == ".wav")
                {

                    wavFiles.push_back(entry.path());
                }
            }
        }

        return wavFiles;
    }

    std::wstring GetCurrentTrackPath() const
    {
        return currentTrackPath;
    }

    void SetCurrentTrack(const std::wstring &path)
    {
        currentTrackPath = path;
    }

    void PlayCurrentSoundTrack() const
    {

        if (currentTrackPath.empty()) {
            return;
        }

        CloseCurrentTrack();

        int flags = SND_FILENAME | SND_ASYNC | SND_LOOP;
        PlaySoundW(currentTrackPath.c_str(), NULL, flags);
    }

    void PauseCurrentSoundTrack() const
    {

        PlaySoundW(NULL, NULL, 0);
    }

    void ResumeCurrentSoundTrack() const
    {

        PlayCurrentSoundTrack();
    }

    void StopCurrentSoundTrack() const
    {

        PlaySoundW(NULL, NULL, 0);
    }

    void CloseCurrentTrack() const
    {

        PlaySoundW(NULL, NULL, 0);
    }

    ~AudioPlayer()
    {
        CloseCurrentTrack();
    }

private:
    std::wstring currentTrackPath;
};

#endif